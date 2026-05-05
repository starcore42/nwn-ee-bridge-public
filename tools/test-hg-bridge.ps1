param(
    [string]$Server = '211',
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Release',
    [switch]$Launch,
    [switch]$StopExistingClient,
    [switch]$SkipBuild,
    [switch]$SkipAssets,
    [switch]$SkipInjectTest,
    [string]$Connect,
    [switch]$DriverOnly,
    [string]$Password,
    [string]$DiamondAccount = '1',
    [string]$DiamondConfigRoot = 'C:\NWN\Config',
    [string]$SteamRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Neverwinter Nights',
    [string]$AssetBundleRoot,
    [switch]$SkipAssetBundle,
    [switch]$PrepareEeUserAssets,
    [switch]$PacketDump,
    [switch]$AutoSpeakPassword,
    [string]$AutoCharacter,
    [string]$AutoUseObjectId,
    [string]$AutoUseObjectType,
    [string]$AutoUseObjectName,
    [int]$AutoUseObjectDelaySeconds = 0,
    [string]$AutoUseObjectSecondId,
    [string]$AutoUseObjectSecondType,
    [string]$AutoUseObjectSecondName,
    [int]$AutoUseObjectSecondDelaySeconds = 0,
    [string]$AutoUseAreaId,
    [switch]$AutoUseAllowUnuseable,
    [switch]$AutoUseTransitionClick,
    [switch]$AutoTriggerWalkProbe,
    [switch]$AutoDoorOpenFirst,
    [int]$AutoDoorTransitionDelayMilliseconds = 0,
    [int]$AutoDoorTransitionSecondDelayMilliseconds = -1,
    [switch]$AutoOpenInventory,
    [int]$AutoOpenInventoryDelayMilliseconds = -1,
    [switch]$VerboseOutboundGamePackets,
    [switch]$DisableRuntimeResourceDiagnostics,
    [switch]$CdKeyPasswordCandidates,
    [switch]$RawCdKeyPasswordCandidates
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

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
Push-Location $repositoryRoot
$previousPacketDump = $env:HG_BRIDGE_PACKET_DUMP
$previousAutoSpeakPassword = $env:HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD
$previousAutoCharacter = $env:HG_BRIDGE_AUTO_CHARACTER
$previousCdKeyPasswordCandidates = $env:HG_BRIDGE_ENABLE_CDKEY_PASSWORD_CANDIDATES
$previousRawCdKeyPasswordCandidates = $env:HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES
$previousAssetBundle = $env:HG_BRIDGE_ASSET_BUNDLE
$previousDiamondRoot = $env:HG_BRIDGE_DIAMOND_ROOT
$previousHgAssetRoot = $env:HG_BRIDGE_HG_ASSET_ROOT
$previousAutoUseObjectId = $env:HG_BRIDGE_AUTO_USE_OBJECT_ID
$previousAutoUseObjectType = $env:HG_BRIDGE_AUTO_USE_OBJECT_TYPE
$previousAutoUseObjectName = $env:HG_BRIDGE_AUTO_USE_OBJECT_NAME
$previousAutoUseObjectDelaySeconds = $env:HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS
$previousAutoUseObjectSecondId = $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID
$previousAutoUseObjectSecondType = $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE
$previousAutoUseObjectSecondName = $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME
$previousAutoUseObjectSecondDelaySeconds = $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS
$previousAutoUseAreaId = $env:HG_BRIDGE_AUTO_USE_AREA_ID
$previousAutoUseAllowUnuseable = $env:HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE
$previousAutoUseTransitionClick = $env:HG_BRIDGE_AUTO_USE_TRANSITION_CLICK
$previousAutoTriggerWalkProbe = $env:HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE
$previousAutoDoorOpenFirst = $env:HG_BRIDGE_AUTO_DOOR_OPEN_FIRST
$previousAutoDoorTransitionDelay = $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS
$previousAutoDoorTransitionSecondDelay = $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS
$previousAutoOpenInventory = $env:HG_BRIDGE_AUTO_OPEN_INVENTORY
$previousAutoOpenInventoryDelay = $env:HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS
$previousVerboseOutboundGamePackets = $env:HG_BRIDGE_VERBOSE_OUTBOUND_GAME_PACKETS
$previousDisableRuntimeResourceDiagnostics = $env:HG_BRIDGE_DISABLE_RUNTIME_RESOURCE_DIAGNOSTICS

try {
    if ([string]::IsNullOrWhiteSpace($AssetBundleRoot)) {
        $AssetBundleRoot = Join-Path $SteamRoot 'hg-bridge-assets'
    }
    $AssetBundleRoot = [System.IO.Path]::GetFullPath($AssetBundleRoot)
    $env:HG_BRIDGE_ASSET_BUNDLE = $AssetBundleRoot
    $env:HG_BRIDGE_DIAMOND_ROOT = Join-Path $AssetBundleRoot 'diamond'
    $env:HG_BRIDGE_HG_ASSET_ROOT = Join-Path $AssetBundleRoot 'hg-gui'

    if ($PacketDump) {
        $env:HG_BRIDGE_PACKET_DUMP = '1'
        Write-Host 'Packet dump enabled. Avoid typing private CD keys or real passwords during this run.'
    }
    if ($AutoSpeakPassword) {
        $env:HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD = '1'
        Write-Host 'HG spoken-password auto-speak enabled for this run.'
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoCharacter)) {
        $env:HG_BRIDGE_AUTO_CHARACTER = $AutoCharacter
        Write-Host "Auto character enabled for this run: $AutoCharacter"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectId)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_ID = $AutoUseObjectId
        Write-Host "Auto-use object id enabled for this run: $AutoUseObjectId"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectType)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_TYPE = $AutoUseObjectType
        Write-Host "Auto-use object type enabled for this run: $AutoUseObjectType"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectName)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_NAME = $AutoUseObjectName
        Write-Host "Auto-use object name enabled for this run: $AutoUseObjectName"
    }
    if ($PSBoundParameters.ContainsKey('AutoUseObjectDelaySeconds') -and $AutoUseObjectDelaySeconds -ge 0) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS = [string]$AutoUseObjectDelaySeconds
        Write-Host "Auto-use object delay: $AutoUseObjectDelaySeconds second(s)"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondId)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID = $AutoUseObjectSecondId
        Write-Host "Second auto-use object id enabled for this run: $AutoUseObjectSecondId"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondType)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE = $AutoUseObjectSecondType
        Write-Host "Second auto-use object type enabled for this run: $AutoUseObjectSecondType"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondName)) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME = $AutoUseObjectSecondName
        Write-Host "Second auto-use object name enabled for this run: $AutoUseObjectSecondName"
    }
    if ($PSBoundParameters.ContainsKey('AutoUseObjectSecondDelaySeconds') -and $AutoUseObjectSecondDelaySeconds -ge 0) {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS = [string]$AutoUseObjectSecondDelaySeconds
        Write-Host "Second auto-use object delay: $AutoUseObjectSecondDelaySeconds second(s)"
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseAreaId)) {
        $env:HG_BRIDGE_AUTO_USE_AREA_ID = $AutoUseAreaId
        Write-Host "Auto-use area id override enabled for this run: $AutoUseAreaId"
    }
    if ($AutoUseAllowUnuseable) {
        $env:HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE = '1'
        Write-Host 'Auto-use may target objects marked unusable by the client.'
    }
    if ($AutoUseTransitionClick) {
        $env:HG_BRIDGE_AUTO_USE_TRANSITION_CLICK = '1'
        Write-Host 'Auto-use transition click payloads enabled for this run.'
    }
    if ($AutoTriggerWalkProbe) {
        $env:HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE = '1'
        Write-Host 'Auto trigger walk probe enabled for this run.'
    }
    if ($AutoDoorOpenFirst) {
        $env:HG_BRIDGE_AUTO_DOOR_OPEN_FIRST = '1'
        Write-Host 'Auto door transition probe enabled: open door first, then send walk-through click.'
    }
    if ($PSBoundParameters.ContainsKey('AutoDoorTransitionDelayMilliseconds') -and $AutoDoorTransitionDelayMilliseconds -ge 0) {
        $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS = [string]$AutoDoorTransitionDelayMilliseconds
        Write-Host "Auto door transition follow-up delay: $AutoDoorTransitionDelayMilliseconds ms"
    }
    if ($PSBoundParameters.ContainsKey('AutoDoorTransitionSecondDelayMilliseconds') -and $AutoDoorTransitionSecondDelayMilliseconds -ge 0) {
        $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS = [string]$AutoDoorTransitionSecondDelayMilliseconds
        Write-Host "Second auto door transition follow-up delay: $AutoDoorTransitionSecondDelayMilliseconds ms"
    }
    if ($AutoOpenInventory) {
        $env:HG_BRIDGE_AUTO_OPEN_INVENTORY = '1'
        Write-Host 'Auto inventory-open enabled for this driver run.'
    }
    if ($PSBoundParameters.ContainsKey('AutoOpenInventoryDelayMilliseconds') -and $AutoOpenInventoryDelayMilliseconds -ge 0) {
        $env:HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS = [string]$AutoOpenInventoryDelayMilliseconds
        Write-Host "Auto inventory-open delay: $AutoOpenInventoryDelayMilliseconds ms"
    }
    if ($VerboseOutboundGamePackets) {
        $env:HG_BRIDGE_VERBOSE_OUTBOUND_GAME_PACKETS = '1'
        Write-Host 'Verbose outbound game-packet logging enabled for this run.'
    }
    if ($DisableRuntimeResourceDiagnostics) {
        $env:HG_BRIDGE_DISABLE_RUNTIME_RESOURCE_DIAGNOSTICS = '1'
        Write-Host 'Runtime resource diagnostics disabled for this run.'
    }
    if ($CdKeyPasswordCandidates) {
        $env:HG_BRIDGE_ENABLE_CDKEY_PASSWORD_CANDIDATES = '1'
        Write-Host 'Public CD-key-derived spoken-password candidates enabled for this run.'
    }
    if ($RawCdKeyPasswordCandidates) {
        $env:HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES = '1'
        Write-Host 'Raw CD-key spoken-password candidates enabled. Avoid this unless deliberately testing legacy behavior.'
    }

    if (-not $SkipBuild) {
        Invoke-Step -Name 'Build bridge binaries' -Script {
            & .\tools\build.ps1 -Configuration $Configuration
        }
    }

    if (-not $SkipAssets) {
        if (-not $SkipAssetBundle) {
            Invoke-Step -Name 'Prepare Steam-local bridge asset bundle' -Script {
                & .\tools\prepare-steam-asset-bundle.ps1 -SteamRoot $SteamRoot -BundleRoot $AssetBundleRoot -Apply
            }
        }

        if ($PrepareEeUserAssets) {
            Invoke-Step -Name 'Prepare EE user assets from Steam bundle' -Script {
                & .\tools\prepare-ee-assets.ps1 -DiamondRoot (Join-Path $AssetBundleRoot 'diamond') -Mode Copy -Apply
            }
        }
    }

    $launcher = Join-Path $repositoryRoot "build\$Configuration\hgbridge_launcher.exe"
    $bridgeDll = Join-Path $repositoryRoot "build\$Configuration\nwncx_hg.dll"
    $bridgeDllForLaunch = $bridgeDll

    if (-not (Test-Path -LiteralPath $launcher -PathType Leaf)) {
        throw "Launcher not found: $launcher"
    }
    if (-not (Test-Path -LiteralPath $bridgeDll -PathType Leaf)) {
        throw "Bridge DLL not found: $bridgeDll"
    }
    if ($Launch -or -not $SkipInjectTest) {
        $driverStamp = Get-Date -Format 'yyyyMMdd-HHmmss'
        $bridgeDllForLaunch = Join-Path (Split-Path -Parent $bridgeDll) "nwncx_hg.driver-$PID-$driverStamp.dll"
        Copy-Item -LiteralPath $bridgeDll -Destination $bridgeDllForLaunch -Force
    }

    $launcherArgs = @(
        '--server', $Server,
        '--diamond-account', $DiamondAccount,
        '--diamond-config-root', $DiamondConfigRoot,
        '--dll', $bridgeDllForLaunch
    )
    if (-not [string]::IsNullOrWhiteSpace($Connect)) {
        $launcherArgs += @('--connect', $Connect)
    }
    if ($DriverOnly) {
        $launcherArgs += '--driver-only'
    }
    if ($PSBoundParameters.ContainsKey('Password')) {
        $launcherArgs += @('--password', $Password)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoCharacter)) {
        $launcherArgs += @('--auto-character', $AutoCharacter)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectId)) {
        $launcherArgs += @('--auto-use-object-id', $AutoUseObjectId)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectType)) {
        $launcherArgs += @('--auto-use-object-type', $AutoUseObjectType)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectName)) {
        $launcherArgs += @('--auto-use-object-name', $AutoUseObjectName)
    }
    if ($PSBoundParameters.ContainsKey('AutoUseObjectDelaySeconds') -and $AutoUseObjectDelaySeconds -ge 0) {
        $launcherArgs += @('--auto-use-object-delay', ([string]$AutoUseObjectDelaySeconds))
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondId)) {
        $launcherArgs += @('--auto-use-object-second-id', $AutoUseObjectSecondId)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondType)) {
        $launcherArgs += @('--auto-use-object-second-type', $AutoUseObjectSecondType)
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseObjectSecondName)) {
        $launcherArgs += @('--auto-use-object-second-name', $AutoUseObjectSecondName)
    }
    if ($PSBoundParameters.ContainsKey('AutoUseObjectSecondDelaySeconds') -and $AutoUseObjectSecondDelaySeconds -ge 0) {
        $launcherArgs += @('--auto-use-object-second-delay', ([string]$AutoUseObjectSecondDelaySeconds))
    }
    if (-not [string]::IsNullOrWhiteSpace($AutoUseAreaId)) {
        $launcherArgs += @('--auto-use-area-id', $AutoUseAreaId)
    }
    if ($AutoUseAllowUnuseable) {
        $launcherArgs += '--auto-use-allow-unuseable'
    }
    if ($AutoUseTransitionClick) {
        $launcherArgs += '--auto-use-transition-click'
    }
    if ($AutoTriggerWalkProbe) {
        $launcherArgs += '--auto-trigger-walk-probe'
    }
    if ($AutoDoorOpenFirst) {
        $launcherArgs += '--auto-door-open-first'
    }
    if ($PSBoundParameters.ContainsKey('AutoDoorTransitionDelayMilliseconds') -and $AutoDoorTransitionDelayMilliseconds -ge 0) {
        $launcherArgs += @('--auto-door-transition-delay-ms', ([string]$AutoDoorTransitionDelayMilliseconds))
    }
    if ($PSBoundParameters.ContainsKey('AutoDoorTransitionSecondDelayMilliseconds') -and $AutoDoorTransitionSecondDelayMilliseconds -ge 0) {
        $launcherArgs += @('--auto-door-transition-second-delay-ms', ([string]$AutoDoorTransitionSecondDelayMilliseconds))
    }
    if ($AutoOpenInventory) {
        $launcherArgs += '--auto-open-inventory'
    }
    if ($PSBoundParameters.ContainsKey('AutoOpenInventoryDelayMilliseconds') -and $AutoOpenInventoryDelayMilliseconds -ge 0) {
        $launcherArgs += @('--auto-open-inventory-delay-ms', ([string]$AutoOpenInventoryDelayMilliseconds))
    }

    Invoke-Step -Name 'Dry-run launch command' -Script {
        & $launcher @launcherArgs --dry-run
        if ($LASTEXITCODE -ne 0) {
            throw "Launcher dry-run failed with exit code $LASTEXITCODE"
        }
    }

    if (-not $SkipInjectTest) {
        Invoke-Step -Name 'Suspended injection test' -Script {
            & $launcher @launcherArgs --inject-test
            if ($LASTEXITCODE -ne 0) {
                throw "Injection test failed with exit code $LASTEXITCODE"
            }
        }
    }

    if ($Launch) {
        if ($StopExistingClient) {
            Invoke-Step -Name 'Stop existing EE client' -Script {
                Get-Process nwmain -ErrorAction SilentlyContinue | Stop-Process -Force
                Start-Sleep -Seconds 1
            }
        }
        Invoke-Step -Name 'Launch real EE client' -Script {
            & $launcher @launcherArgs
            if ($LASTEXITCODE -ne 0) {
                throw "Launcher failed with exit code $LASTEXITCODE"
            }
        }
    } else {
        Write-Host ''
        Write-Host 'Ready for a live test.'
        Write-Host "Asset bundle: $AssetBundleRoot"
        Write-Host "Run: .\tools\test-hg-bridge.ps1 -Server $Server -DiamondAccount $DiamondAccount -Launch"
        Write-Host "Proxy driver run: .\tools\test-hg-bridge.ps1 -Server $Server -Connect 192.168.1.103:5121 -DriverOnly -DiamondAccount $DiamondAccount -Launch"
        Write-Host "Or:  .\build\$Configuration\hgbridge_launcher.exe --server $Server --connect 192.168.1.103:5121 --driver-only --diamond-account $DiamondAccount --diamond-config-root `"$DiamondConfigRoot`""
        Write-Host 'After a live run, summarize: .\tools\summarize-hg-bridge-log.ps1'
    }
} finally {
    if ($null -eq $previousPacketDump) {
        Remove-Item Env:\HG_BRIDGE_PACKET_DUMP -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_PACKET_DUMP = $previousPacketDump
    }
    if ($null -eq $previousAutoSpeakPassword) {
        Remove-Item Env:\HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_ENABLE_AUTO_SPEAK_PASSWORD = $previousAutoSpeakPassword
    }
    if ($null -eq $previousAutoCharacter) {
        Remove-Item Env:\HG_BRIDGE_AUTO_CHARACTER -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_CHARACTER = $previousAutoCharacter
    }
    if ($null -eq $previousCdKeyPasswordCandidates) {
        Remove-Item Env:\HG_BRIDGE_ENABLE_CDKEY_PASSWORD_CANDIDATES -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_ENABLE_CDKEY_PASSWORD_CANDIDATES = $previousCdKeyPasswordCandidates
    }
    if ($null -eq $previousRawCdKeyPasswordCandidates) {
        Remove-Item Env:\HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_ENABLE_RAW_CDKEY_PASSWORD_CANDIDATES = $previousRawCdKeyPasswordCandidates
    }
    if ($null -eq $previousAssetBundle) {
        Remove-Item Env:\HG_BRIDGE_ASSET_BUNDLE -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_ASSET_BUNDLE = $previousAssetBundle
    }
    if ($null -eq $previousDiamondRoot) {
        Remove-Item Env:\HG_BRIDGE_DIAMOND_ROOT -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_DIAMOND_ROOT = $previousDiamondRoot
    }
    if ($null -eq $previousHgAssetRoot) {
        Remove-Item Env:\HG_BRIDGE_HG_ASSET_ROOT -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_HG_ASSET_ROOT = $previousHgAssetRoot
    }
    if ($null -eq $previousAutoUseObjectId) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_ID -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_ID = $previousAutoUseObjectId
    }
    if ($null -eq $previousAutoUseObjectType) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_TYPE -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_TYPE = $previousAutoUseObjectType
    }
    if ($null -eq $previousAutoUseObjectName) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_NAME -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_NAME = $previousAutoUseObjectName
    }
    if ($null -eq $previousAutoUseObjectDelaySeconds) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_DELAY_SECONDS = $previousAutoUseObjectDelaySeconds
    }
    if ($null -eq $previousAutoUseObjectSecondId) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_ID = $previousAutoUseObjectSecondId
    }
    if ($null -eq $previousAutoUseObjectSecondType) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_TYPE = $previousAutoUseObjectSecondType
    }
    if ($null -eq $previousAutoUseObjectSecondName) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_NAME = $previousAutoUseObjectSecondName
    }
    if ($null -eq $previousAutoUseObjectSecondDelaySeconds) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_OBJECT_SECOND_DELAY_SECONDS = $previousAutoUseObjectSecondDelaySeconds
    }
    if ($null -eq $previousAutoUseAreaId) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_AREA_ID -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_AREA_ID = $previousAutoUseAreaId
    }
    if ($null -eq $previousAutoUseAllowUnuseable) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_ALLOW_UNUSEABLE = $previousAutoUseAllowUnuseable
    }
    if ($null -eq $previousAutoUseTransitionClick) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_TRANSITION_CLICK -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_TRANSITION_CLICK = $previousAutoUseTransitionClick
    }
    if ($null -eq $previousAutoTriggerWalkProbe) {
        Remove-Item Env:\HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_USE_TRIGGER_WALK_PROBE = $previousAutoTriggerWalkProbe
    }
    if ($null -eq $previousAutoDoorOpenFirst) {
        Remove-Item Env:\HG_BRIDGE_AUTO_DOOR_OPEN_FIRST -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_DOOR_OPEN_FIRST = $previousAutoDoorOpenFirst
    }
    if ($null -eq $previousAutoDoorTransitionDelay) {
        Remove-Item Env:\HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_DELAY_MS = $previousAutoDoorTransitionDelay
    }
    if ($null -eq $previousAutoDoorTransitionSecondDelay) {
        Remove-Item Env:\HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_DOOR_TRANSITION_SECOND_DELAY_MS = $previousAutoDoorTransitionSecondDelay
    }
    if ($null -eq $previousAutoOpenInventory) {
        Remove-Item Env:\HG_BRIDGE_AUTO_OPEN_INVENTORY -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_OPEN_INVENTORY = $previousAutoOpenInventory
    }
    if ($null -eq $previousAutoOpenInventoryDelay) {
        Remove-Item Env:\HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_AUTO_OPEN_INVENTORY_DELAY_MS = $previousAutoOpenInventoryDelay
    }
    if ($null -eq $previousVerboseOutboundGamePackets) {
        Remove-Item Env:\HG_BRIDGE_VERBOSE_OUTBOUND_GAME_PACKETS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_VERBOSE_OUTBOUND_GAME_PACKETS = $previousVerboseOutboundGamePackets
    }
    if ($null -eq $previousDisableRuntimeResourceDiagnostics) {
        Remove-Item Env:\HG_BRIDGE_DISABLE_RUNTIME_RESOURCE_DIAGNOSTICS -ErrorAction SilentlyContinue
    } else {
        $env:HG_BRIDGE_DISABLE_RUNTIME_RESOURCE_DIAGNOSTICS = $previousDisableRuntimeResourceDiagnostics
    }
    Pop-Location
}
