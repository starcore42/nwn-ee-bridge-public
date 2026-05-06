param(
    [string]$SteamRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Neverwinter Nights',
    [string]$BundleRoot,
    [string]$DiamondRoot,
    [string]$SetupRoot,
    [string]$HgStdRoot,
    [switch]$Apply,
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Resolve-RepositoryRoot {
    return (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
}

function Resolve-ExistingDirectoryOrEmpty {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Candidates
    )

    foreach ($candidate in $Candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Container)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    return ''
}

function Resolve-ExistingFileOrEmpty {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Candidates
    )

    foreach ($candidate in $Candidates) {
        if (-not [string]::IsNullOrWhiteSpace($candidate) -and
            (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    return ''
}

function Test-AssetFileMinimumLength {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [int64]$MinimumLength
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        return $false
    }
    return (Get-Item -LiteralPath $Path).Length -ge $MinimumLength
}

function Test-HgContentRoot {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        return $false
    }

    $customHak = Join-Path $Path 'hak\cep2_custom.hak'
    $customTlk = Join-Path $Path 'tlk\cep23_v1.tlk'
    $dialogTlk = Join-Path $Path 'tlk\dialog.tlk'
    return (Test-AssetFileMinimumLength -Path $customHak -MinimumLength 1048576) -and
        (Test-AssetFileMinimumLength -Path $customTlk -MinimumLength 1048576) -and
        (Test-AssetFileMinimumLength -Path $dialogTlk -MinimumLength 1048576)
}

function Resolve-ExistingHgContentRootOrEmpty {
    param(
        [Parameter(Mandatory = $true)]
        [string[]]$Candidates
    )

    foreach ($candidate in $Candidates) {
        if ([string]::IsNullOrWhiteSpace($candidate) -or
            -not (Test-Path -LiteralPath $candidate -PathType Container)) {
            continue
        }

        $resolved = (Resolve-Path -LiteralPath $candidate).Path
        if (Test-HgContentRoot -Path $resolved) {
            return $resolved
        }
        Write-Warning "Ignoring incomplete HG content root: $resolved"
    }
    return ''
}

function Resolve-SetupChildDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SetupRoot,
        [Parameter(Mandatory = $true)]
        [string[]]$Candidates,
        [Parameter(Mandatory = $true)]
        [string]$Name
    )

    foreach ($candidate in $Candidates) {
        $path = Join-Path $SetupRoot $candidate
        if (Test-Path -LiteralPath $path -PathType Container) {
            return (Resolve-Path -LiteralPath $path).Path
        }
    }

    Write-Warning "Could not find setup directory for ${Name}: $($Candidates -join ', ') under $SetupRoot"
    return Join-Path $SetupRoot $Candidates[0]
}

function Ensure-Directory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )

    if (Test-Path -LiteralPath $Path -PathType Container) {
        return
    }
    if ($Apply) {
        New-Item -ItemType Directory -Path $Path | Out-Null
    } else {
        Write-Host "[dry-run] mkdir $Path"
    }
}

function Copy-AssetFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Source,
        [Parameter(Mandatory = $true)]
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source -PathType Leaf)) {
        Write-Warning "Skipping missing file: $Source"
        return
    }

    Ensure-Directory -Path (Split-Path -Parent $Destination)

    $shouldCopy = $true
    if (Test-Path -LiteralPath $Destination -PathType Leaf) {
        $sourceItem = Get-Item -LiteralPath $Source
        $destItem = Get-Item -LiteralPath $Destination
        $shouldCopy = $Force -or $sourceItem.Length -ne $destItem.Length -or
            $sourceItem.LastWriteTimeUtc -gt $destItem.LastWriteTimeUtc
    }

    if (-not $shouldCopy) {
        $script:UnchangedFiles += 1
        return
    }

    if ($Apply) {
        Copy-Item -LiteralPath $Source -Destination $Destination -Force
    } else {
        Write-Host "[dry-run] copy $Source -> $Destination"
    }
    $script:CopiedFiles += 1
}

function Copy-AssetDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name,
        [Parameter(Mandatory = $true)]
        [string]$Source,
        [Parameter(Mandatory = $true)]
        [string]$Destination
    )

    if (-not (Test-Path -LiteralPath $Source -PathType Container)) {
        Write-Warning "Skipping missing directory for ${Name}: $Source"
        return
    }

    Ensure-Directory -Path $Destination
    Write-Host "${Name}: $Source -> $Destination"
    if (-not $Apply) {
        return
    }

    $robocopyArgs = @(
        $Source,
        $Destination,
        '/E',
        '/COPY:DAT',
        '/DCOPY:DAT',
        '/R:2',
        '/W:1',
        '/MT:16',
        '/NP',
        '/NFL',
        '/NDL',
        '/NJH',
        '/NJS'
    )
    & robocopy @robocopyArgs | Out-Null
    $exitCode = $LASTEXITCODE
    if ($exitCode -ge 8) {
        throw "robocopy failed for ${Name} with exit code $exitCode"
    }
    $script:CopiedDirectories += 1
}

$repositoryRoot = Resolve-RepositoryRoot

if (-not (Test-Path -LiteralPath $SteamRoot -PathType Container)) {
    throw "Steam NWN:EE root not found: $SteamRoot"
}
$steamRootResolved = (Resolve-Path -LiteralPath $SteamRoot).Path

if ([string]::IsNullOrWhiteSpace($BundleRoot)) {
    $BundleRoot = Join-Path $steamRootResolved 'hg-bridge-assets'
}

if ([string]::IsNullOrWhiteSpace($DiamondRoot)) {
    $DiamondRoot = Resolve-ExistingDirectoryOrEmpty @(
        'C:\NWN\NWN Diamond',
        (Join-Path $repositoryRoot 'NWN Diamond')
    )
}
if ([string]::IsNullOrWhiteSpace($SetupRoot)) {
    $SetupRoot = Resolve-ExistingDirectoryOrEmpty @(
        'C:\NWN\Setup',
        (Join-Path $repositoryRoot 'NWN Utils\Setup')
    )
}
if ([string]::IsNullOrWhiteSpace($HgStdRoot)) {
    $HgStdRoot = Resolve-ExistingHgContentRootOrEmpty @(
        (Join-Path $env:USERPROFILE 'Downloads\hghak_std-beta28.en'),
        (Join-Path $repositoryRoot 'hghak_std-beta28.en'),
        (Join-Path $SetupRoot 'hghak_std-beta28.en')
    )
}

if ([string]::IsNullOrWhiteSpace($DiamondRoot) -or -not (Test-Path -LiteralPath $DiamondRoot -PathType Container)) {
    throw "Diamond source root not found. Pass -DiamondRoot explicitly."
}
if ([string]::IsNullOrWhiteSpace($SetupRoot) -or -not (Test-Path -LiteralPath $SetupRoot -PathType Container)) {
    throw "HG setup source root not found. Pass -SetupRoot explicitly."
}

$diamondRootResolved = (Resolve-Path -LiteralPath $DiamondRoot).Path
$setupRootResolved = (Resolve-Path -LiteralPath $SetupRoot).Path
$bundleRootResolved = [System.IO.Path]::GetFullPath($BundleRoot)

$script:CopiedFiles = 0
$script:UnchangedFiles = 0
$script:CopiedDirectories = 0

Write-Host "Steam root:   $steamRootResolved"
Write-Host "Bundle root:  $bundleRootResolved"
Write-Host "Diamond root: $diamondRootResolved"
Write-Host "Setup root:   $setupRootResolved"
if (-not [string]::IsNullOrWhiteSpace($HgStdRoot)) {
    if (-not (Test-HgContentRoot -Path $HgStdRoot)) {
        throw "HG standard source root is incomplete or contains placeholder files: $HgStdRoot"
    }
    Write-Host "HG standard:  $HgStdRoot"
}
if (-not $Apply) {
    Write-Host 'Dry run only. Add -Apply to copy files.'
}

Ensure-Directory -Path $bundleRootResolved

$diamondBundle = Join-Path $bundleRootResolved 'diamond'
Ensure-Directory -Path $diamondBundle
foreach ($fileName in @('chitin.key', 'xp1.key', 'xp2.key', 'xp2patch.key', 'xp3.key', 'dialog.tlk')) {
    Copy-AssetFile -Source (Join-Path $diamondRootResolved $fileName) -Destination (Join-Path $diamondBundle $fileName)
}

foreach ($directoryName in @('data', 'texturepacks', 'hak', 'tlk', 'override', 'portraits', 'ambient', 'music', 'erf', 'nwm')) {
    Copy-AssetDirectory `
        -Name "diamond/$directoryName" `
        -Source (Join-Path $diamondRootResolved $directoryName) `
        -Destination (Join-Path $diamondBundle $directoryName)
}

$hgGuiSource = Resolve-SetupChildDirectory `
    -SetupRoot $setupRootResolved `
    -Name 'HG GUI' `
    -Candidates @('hghak_gui-beta28.en', '01 - hghak_gui-beta28.en')
Copy-AssetDirectory -Name 'hg-gui' -Source $hgGuiSource -Destination (Join-Path $bundleRootResolved 'hg-gui')

$hgOverrideSource = Resolve-SetupChildDirectory `
    -SetupRoot $setupRootResolved `
    -Name 'HG override' `
    -Candidates @('02 - hghak_override-beta18', 'hghak_override-beta18.en', 'hghak_override-beta18')
Copy-AssetDirectory -Name 'hg-override' -Source $hgOverrideSource -Destination (Join-Path $bundleRootResolved 'hg-override')

$hgOverlaySource = Resolve-SetupChildDirectory `
    -SetupRoot $setupRootResolved `
    -Name 'HG overlay' `
    -Candidates @('0X - Overlay - hg-yal-master', 'hg-yal-master')
Copy-AssetDirectory -Name 'hg-overlay' -Source $hgOverlaySource -Destination (Join-Path $bundleRootResolved 'hg-overlay')

$cepSource = Resolve-SetupChildDirectory `
    -SetupRoot $setupRootResolved `
    -Name 'CEP 2.3' `
    -Candidates @('cep_23_full_c')
Copy-AssetDirectory -Name 'cep23' -Source $cepSource -Destination (Join-Path $bundleRootResolved 'cep23')

$mergedBaseitems = Resolve-ExistingFileOrEmpty @(
    (Join-Path $repositoryRoot 'NWN Diamond\1.72 builder resources\auto2damerger\merged\baseitems.2da'),
    (Join-Path $repositoryRoot 'NWN Diamond\1.72 builder resources\1.72 full 2dasource\baseitems.2da'),
    (Join-Path $repositoryRoot 'NWN Diamond\1.72 builder resources\1.72-hak 2das\baseitems.2da'),
    (Join-Path $repositoryRoot 'NWN Diamond\1.72 builder resources\1.72-only 2das\baseitems.2da')
)
if (-not [string]::IsNullOrWhiteSpace($mergedBaseitems)) {
    Copy-AssetFile `
        -Source $mergedBaseitems `
        -Destination (Join-Path $bundleRootResolved 'ee-fixes\2da\baseitems.2da')
} else {
    Write-Warning 'Merged baseitems.2da not found; proxy item-appearance auto-detect will fall back to slower workspace or partial HAK sources.'
}

if (-not [string]::IsNullOrWhiteSpace($HgStdRoot) -and (Test-Path -LiteralPath $HgStdRoot -PathType Container)) {
    Copy-AssetDirectory -Name 'hg-std' -Source (Resolve-Path -LiteralPath $HgStdRoot).Path -Destination (Join-Path $bundleRootResolved 'hg-std')
}

$manifest = @(
    'HG EE bridge asset bundle',
    "Generated: $(Get-Date -Format o)",
    "Steam root: $steamRootResolved",
    "Diamond source: $diamondRootResolved",
    "Setup source: $setupRootResolved",
    "HG standard source: $HgStdRoot",
    '',
    'This bundle intentionally contains resource assets only.',
    'It does not copy nwncdkey.ini or account credentials.'
)
if ($Apply) {
    Set-Content -LiteralPath (Join-Path $bundleRootResolved 'README.txt') -Value $manifest -Encoding ASCII
} else {
    Write-Host "[dry-run] write $(Join-Path $bundleRootResolved 'README.txt')"
}

Write-Host ''
Write-Host 'Summary:'
Write-Host "  copied files:       $script:CopiedFiles"
Write-Host "  unchanged files:    $script:UnchangedFiles"
Write-Host "  copied directories: $script:CopiedDirectories"
Write-Host "  bundle:             $bundleRootResolved"
