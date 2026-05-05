param(
    [string]$Server = '213',
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [string]$Listen = '0.0.0.0:5121',
    [string]$ConnectHost,
    [string]$SteamRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Neverwinter Nights',
    [string]$SteamExe = 'C:\Program Files (x86)\Steam\steam.exe',
    [string]$EeUserRoot = (Join-Path $env:USERPROFILE 'Documents\Neverwinter Nights'),
    [string]$TestUserRoot = 'C:\NWN\EEBridgeStockProxy',
    [string]$DiamondCdKey = 'C:\NWN\Config\account.nwncdkey.ini',
    [string]$NwsyncRoot,
    [string]$LogPath = 'C:\NWN\hgbridge_proxy_stock.log',
    [switch]$SkipBuild,
    [switch]$RestartProxy,
    [switch]$LaunchClient,
    [switch]$SyntheticAreaLoaded,
    [switch]$NoSyntheticAreaLoaded,
    [switch]$NoSyntheticAreaLoadBar,
    [switch]$NoGracefulDisconnect,
    [switch]$PacketDump
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name,
        [Parameter(Mandatory = $true)]
        [scriptblock]$Script
    )

    Write-Host ''
    Write-Host "== $Name =="
    & $Script
}

function Get-PrimaryIPv4 {
    $candidate = Get-NetIPAddress -AddressFamily IPv4 -ErrorAction SilentlyContinue |
        Where-Object {
            $_.IPAddress -notlike '127.*' -and
            $_.PrefixOrigin -ne 'WellKnown' -and
            $_.AddressState -eq 'Preferred'
        } |
        Sort-Object InterfaceMetric,InterfaceIndex |
        Select-Object -First 1

    if ($null -ne $candidate) {
        return $candidate.IPAddress
    }

    return '127.0.0.1'
}

function ConvertTo-CommandLineArgument {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Value
    )

    if ($Value -notmatch '[\s"]') {
        return $Value
    }

    return '"' + $Value.Replace('"', '""') + '"'
}

function Set-TmlValue {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [string]$Section,
        [Parameter(Mandatory = $true)]
        [string]$Key,
        [Parameter(Mandatory = $true)]
        [string]$Value
    )

    $lines = @()
    if (Test-Path -LiteralPath $Path -PathType Leaf) {
        $lines = @(Get-Content -LiteralPath $Path)
    }

    $sectionHeader = "[$Section]"
    $sectionIndex = -1
    for ($i = 0; $i -lt $lines.Count; $i++) {
        if ($lines[$i].Trim() -eq $sectionHeader) {
            $sectionIndex = $i
            break
        }
    }

    if ($sectionIndex -lt 0) {
        if ($lines.Count -gt 0 -and $lines[-1].Trim().Length -gt 0) {
            $lines += ''
        }
        $lines += $sectionHeader
        $lines += "`t$Key = $Value"
        [System.IO.File]::WriteAllLines($Path, [string[]]$lines, (New-Object System.Text.UTF8Encoding $false))
        return
    }

    $insertAt = $lines.Count
    $keyPattern = '^\s*' + [regex]::Escape($Key) + '\s*='
    for ($i = $sectionIndex + 1; $i -lt $lines.Count; $i++) {
        if ($lines[$i] -match '^\s*\[[^\]]+\]\s*$') {
            $insertAt = $i
            break
        }
        if ($lines[$i] -match $keyPattern) {
            $lines[$i] = "`t`t$Key = $Value"
            [System.IO.File]::WriteAllLines($Path, [string[]]$lines, (New-Object System.Text.UTF8Encoding $false))
            return
        }
    }

    $before = @()
    $after = @()
    if ($insertAt -gt 0) {
        $before = $lines[0..($insertAt - 1)]
    }
    if ($insertAt -lt $lines.Count) {
        $after = $lines[$insertAt..($lines.Count - 1)]
    }
    $lines = @($before + "`t`t$Key = $Value" + $after)
    [System.IO.File]::WriteAllLines($Path, [string[]]$lines, (New-Object System.Text.UTF8Encoding $false))
}

function Get-ProxyLogPathFromCommandLine {
    param([string]$CommandLine)

    if ([string]::IsNullOrWhiteSpace($CommandLine)) {
        return $null
    }
    $match = [regex]::Match($CommandLine, '--log\s+(?:"(?<quoted>[^"]+)"|(?<plain>\S+))')
    if (-not $match.Success) {
        return $null
    }
    if ($match.Groups['quoted'].Success) {
        return $match.Groups['quoted'].Value
    }
    return $match.Groups['plain'].Value
}

function Get-LegacyDisconnectTargetsFromLog {
    param([string[]]$Paths)

    $seen = @{}
    $targets = @()
    foreach ($path in $Paths) {
        if ([string]::IsNullOrWhiteSpace($path) -or -not (Test-Path -LiteralPath $path -PathType Leaf)) {
            continue
        }
        $matches = Select-String -LiteralPath $path -Pattern 'client session #\d+ created: client=[^: ]+:(?<clientPort>\d+) upstream_local=[^: ]+:(?<upstreamPort>\d+) server_peer=(?<serverHost>[^: ]+):(?<serverPort>\d+)' -AllMatches
        foreach ($matchInfo in $matches) {
            foreach ($match in $matchInfo.Matches) {
                $clientPort = [int]$match.Groups['clientPort'].Value
                $upstreamPort = [int]$match.Groups['upstreamPort'].Value
                $serverHost = $match.Groups['serverHost'].Value
                $serverPort = [int]$match.Groups['serverPort'].Value
                $key = "$upstreamPort|$clientPort|$serverHost|$serverPort"
                if ($seen.ContainsKey($key)) {
                    continue
                }
                $seen[$key] = $true
                $targets += [pscustomobject]@{
                    ClientPort = $clientPort
                    UpstreamPort = $upstreamPort
                    ServerHost = $serverHost
                    ServerPort = $serverPort
                    SourceLog = $path
                }
            }
        }
    }
    return $targets
}

function Send-LegacyBnds {
    param(
        [Parameter(Mandatory = $true)][int]$LocalPort,
        [Parameter(Mandatory = $true)][int]$ClientPort,
        [Parameter(Mandatory = $true)][string]$ServerHost,
        [Parameter(Mandatory = $true)][int]$ServerPort
    )

    $udp = $null
    try {
        $udp = [System.Net.Sockets.UdpClient]::new([System.Net.Sockets.AddressFamily]::InterNetwork)
        $udp.Client.Bind([System.Net.IPEndPoint]::new([System.Net.IPAddress]::Any, $LocalPort))
        $bytes = [byte[]]@(0x42, 0x4E, 0x44, 0x53, ($ClientPort -band 0xFF), (($ClientPort -shr 8) -band 0xFF))
        for ($i = 0; $i -lt 3; $i++) {
            [void]$udp.Send($bytes, $bytes.Length, $ServerHost, $ServerPort)
            Start-Sleep -Milliseconds 75
        }
        Write-Host "Sent legacy BNDS from local UDP $LocalPort for client port $ClientPort to $ServerHost`:$ServerPort"
    } catch {
        Write-Warning "Could not send legacy BNDS from local UDP $LocalPort for client port $ClientPort to $ServerHost`:${ServerPort}: $_"
    } finally {
        if ($null -ne $udp) {
            $udp.Close()
        }
    }
}

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
Push-Location $repositoryRoot

try {
    if ([string]::IsNullOrWhiteSpace($ConnectHost)) {
        $ConnectHost = Get-PrimaryIPv4
    }
    if ([string]::IsNullOrWhiteSpace($NwsyncRoot)) {
        $NwsyncRoot = Join-Path $SteamRoot 'hg-bridge-assets\nwsync'
    }

    $proxy = Join-Path $repositoryRoot "build\$Configuration\hgbridge_proxy.exe"

    if (-not $SkipBuild) {
        Invoke-Step -Name 'Build proxy' -Script {
            & .\tools\build.ps1 -Configuration $Configuration
            if ($LASTEXITCODE -ne 0) {
                throw "Build failed with exit code $LASTEXITCODE"
            }
        }
    }

    if (-not (Test-Path -LiteralPath $proxy -PathType Leaf)) {
        throw "Proxy not found: $proxy"
    }
    if (-not (Test-Path -LiteralPath $DiamondCdKey -PathType Leaf)) {
        throw "Diamond CD-key source not found for proxy verifier rewrite: $DiamondCdKey"
    }

    Invoke-Step -Name 'Prepare stock EE user profile' -Script {
        New-Item -ItemType Directory -Force -Path $TestUserRoot | Out-Null
        foreach ($dir in @('ambient','development','dmvault','hak','localvault','modules','movies','music','override','portraits','servervault','tlk')) {
            New-Item -ItemType Directory -Force -Path (Join-Path $TestUserRoot $dir) | Out-Null
        }

        foreach ($file in @('cdkey.ini','cryptographic_secret','nwnpid','nwn.ini','nwnplayer.ini','settings.tml')) {
            $source = Join-Path $EeUserRoot $file
            if (Test-Path -LiteralPath $source -PathType Leaf) {
                Copy-Item -LiteralPath $source -Destination (Join-Path $TestUserRoot $file) -Force
            }
        }

        $legacyDiamondKey = Join-Path $TestUserRoot 'nwncdkey.ini'
        if (Test-Path -LiteralPath $legacyDiamondKey -PathType Leaf) {
            Remove-Item -LiteralPath $legacyDiamondKey -Force
        }

        $settingsPath = Join-Path $TestUserRoot 'settings.tml'
        Set-TmlValue -Path $settingsPath -Section 'client.identity' -Key 'name' -Value '"EEBridgeTest"'
        Set-TmlValue -Path $settingsPath -Section 'server.net' -Key 'port' -Value '5121'
        Set-TmlValue -Path $settingsPath -Section 'ui.multiplayer.direct-connect' -Key 'last-address' -Value ('"' + $ConnectHost + '"')

        Write-Host "EE profile: $TestUserRoot"
        Write-Host 'EE identity: copied from the normal EE profile only; Diamond nwncdkey.ini is not staged.'
        Write-Host "Direct Connect address: $ConnectHost"
    }

    if ($RestartProxy) {
        Invoke-Step -Name 'Restart proxy' -Script {
            $existingProxyProcesses = @(Get-CimInstance Win32_Process -Filter "Name = 'hgbridge_proxy.exe'" -ErrorAction SilentlyContinue)
            $previousLogPaths = @($LogPath)
            foreach ($existingProxy in $existingProxyProcesses) {
                $existingLog = Get-ProxyLogPathFromCommandLine -CommandLine $existingProxy.CommandLine
                if (-not [string]::IsNullOrWhiteSpace($existingLog)) {
                    $previousLogPaths += $existingLog
                }
            }
            $disconnectTargets = @()
            if (-not $NoGracefulDisconnect) {
                $disconnectTargets = @(Get-LegacyDisconnectTargetsFromLog -Paths ($previousLogPaths | Select-Object -Unique))
            }

            Get-Process hgbridge_proxy -ErrorAction SilentlyContinue | Stop-Process -Force
            Start-Sleep -Seconds 1
            foreach ($target in $disconnectTargets) {
                Send-LegacyBnds -LocalPort $target.UpstreamPort -ClientPort $target.ClientPort -ServerHost $target.ServerHost -ServerPort $target.ServerPort
            }

            $proxyArgs = @(
                '--listen', $Listen,
                '--server', $Server,
                '--allow-remote-clients',
                '--ee-crypto',
                '--diamond-cdkey', $DiamondCdKey,
                '--nwsync-root', $NwsyncRoot,
                '--rewrite-quickbar-simple',
                '--rewrite-live-object-visual-transform-masks',
                '--rewrite-live-object-material-shader-params',
                '--rewrite-live-object-updates',
                '--rewrite-live-object-item-appearance-visual-transforms',
                '--rewrite-live-object-add-visual-transforms',
                '--rewrite-live-object-add-records',
                '--rewrite-player-list',
                '--rewrite-area-client-area',
                '--log', $LogPath
            )
            if ($PacketDump) {
                $proxyArgs += '--packet-dump'
            }
            if (-not $NoSyntheticAreaLoaded) {
                $proxyArgs += '--synthetic-area-loaded'
            }
            if ($NoSyntheticAreaLoadBar) {
                $proxyArgs += '--no-synthetic-area-loadbar'
            }

            $proxyCommandLine = ($proxyArgs | ForEach-Object { ConvertTo-CommandLineArgument $_ }) -join ' '
            $process = Start-Process -FilePath $proxy -ArgumentList $proxyCommandLine -WindowStyle Hidden -PassThru
            Write-Host "Proxy PID: $($process.Id)"
            Write-Host "Proxy log: $LogPath"
        }
    }

    if ($LaunchClient) {
        Invoke-Step -Name 'Launch stock EE through Steam' -Script {
            if (-not (Test-Path -LiteralPath $SteamExe -PathType Leaf)) {
                throw "Steam executable not found: $SteamExe"
            }
            $steamArgs = '-applaunch 704450 -userdirectory "' + $TestUserRoot + '"'
            Start-Process -FilePath $SteamExe -ArgumentList $steamArgs | Out-Null
            Write-Host 'Use Multiplayer -> Direct Connect in the EE menu.'
            Write-Host "Address: $ConnectHost"
            Write-Host 'Password: leave blank'
        }
    }

    if (-not $RestartProxy -and -not $LaunchClient) {
        Write-Host ''
        Write-Host 'Prepared stock proxy profile.'
        Write-Host "Run proxy:  .\tools\start-stock-proxy-test.ps1 -RestartProxy"
        Write-Host "Run client: .\tools\start-stock-proxy-test.ps1 -SkipBuild -LaunchClient"
    }
} finally {
    Pop-Location
}
