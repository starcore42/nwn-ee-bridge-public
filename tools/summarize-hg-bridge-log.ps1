param(
    [string]$Path,
    [int]$Top = 25,
    [switch]$LatestRun,
    [switch]$IncludeChatPreview
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Add-Count {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Table,
        [Parameter(Mandatory = $true)]
        [string]$Key
    )

    if ([string]::IsNullOrWhiteSpace($Key)) {
        $Key = '<empty>'
    }
    if ($Table.ContainsKey($Key)) {
        $Table[$Key] += 1
    } else {
        $Table[$Key] = 1
    }
}

function Test-ZeroPointerText {
    param([string]$Text)

    if ([string]::IsNullOrWhiteSpace($Text)) {
        return $false
    }
    $value = $Text.Trim()
    if ($value.StartsWith('0x', [System.StringComparison]::OrdinalIgnoreCase)) {
        $value = $value.Substring(2)
    }
    return $value -match '^0+$'
}

function Write-TopCounts {
    param(
        [string]$Title,
        [hashtable]$Table,
        [int]$Limit
    )

    Write-Host ''
    Write-Host $Title
    if ($Table.Count -eq 0) {
        Write-Host '  <none>'
        return
    }

    $Table.GetEnumerator() |
        Sort-Object -Property @{ Expression = 'Value'; Descending = $true }, @{ Expression = 'Key'; Descending = $false } |
        Select-Object -First $Limit |
        ForEach-Object {
            Write-Host ('  {0,5}  {1}' -f $_.Value, $_.Key)
        }
}

function Write-SampleLines {
    param(
        [string]$Title,
        [System.Collections.Generic.List[string]]$Lines,
        [int]$Limit
    )

    Write-Host ''
    Write-Host $Title
    if ($Lines.Count -eq 0) {
        Write-Host '  <none>'
        return
    }

    $Lines | Select-Object -Last $Limit | ForEach-Object {
        Write-Host ('  {0}' -f $_)
    }
}

$repositoryRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
if ([string]::IsNullOrWhiteSpace($Path)) {
    $latest = Get-ChildItem -Path (Join-Path $repositoryRoot 'build\Release') -Filter 'nwncx_hg*.log' -File |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 1
    if ($null -eq $latest) {
        throw 'No bridge logs found under build\Release.'
    }
    $Path = $latest.FullName
}

$resolvedPath = (Resolve-Path -LiteralPath $Path).Path
$file = Get-Item -LiteralPath $resolvedPath
$lines = @(Get-Content -LiteralPath $resolvedPath)
$latestRunStartLine = 0

if ($LatestRun) {
    for ($index = $lines.Count - 1; $index -ge 0; --$index) {
        if ($lines[$index] -match 'nwncx_hg .* loaded in pid=') {
            $latestRunStartLine = $index + 1
            $lines = @($lines[$index..($lines.Count - 1)])
            break
        }
    }
}

$lookupMissByResource = @{}
$lookupHitByResource = @{}
$lookupMissByType = @{}
$lookupHitByType = @{}
$demandMissByResource = @{}
$demandHitByResource = @{}
$demandMissByType = @{}
$demandHitByType = @{}

$moduleLines = [System.Collections.Generic.List[string]]::new()
$areaLines = [System.Collections.Generic.List[string]]::new()
$networkLines = [System.Collections.Generic.List[string]]::new()
$assetLines = [System.Collections.Generic.List[string]]::new()
$chatLines = [System.Collections.Generic.List[string]]::new()
$clientMessageLines = [System.Collections.Generic.List[string]]::new()
$writeMessageLines = [System.Collections.Generic.List[string]]::new()
$transitionLines = [System.Collections.Generic.List[string]]::new()

$passwordPromptCount = 0
$passwordIncorrectCount = 0
$welcomeCount = 0
$autoSpeakCount = 0
$autoSpeakSkippedCount = 0
$clientAreaLoadedCount = 0

foreach ($line in $lines) {
    if ($line -match "CExoResMan lookup #\d+ [^:]+:.* name='([^']*)'.* type=(\d+)(?:/([^ ]+))? result=([0-9A-Fa-fx]+)") {
        $name = $Matches[1]
        $typeNumber = $Matches[2]
        $typeName = if ($Matches.Count -gt 3 -and -not [string]::IsNullOrWhiteSpace($Matches[3])) { $Matches[3] } else { $typeNumber }
        $key = ('{0}|{1}' -f $name, $typeName).ToLowerInvariant()
        $typeKey = ('{0}/{1}' -f $typeNumber, $typeName)
        if (Test-ZeroPointerText $Matches[4]) {
            Add-Count $lookupMissByResource $key
            Add-Count $lookupMissByType $typeKey
        } else {
            Add-Count $lookupHitByResource $key
            Add-Count $lookupHitByType $typeKey
        }
    }

    if ($line -match "CExoResMan demand #\d+:.* name='([^']*)'.* type=(\d+)(?:/([^ ]+))? result=([0-9A-Fa-fx]+)") {
        $name = $Matches[1]
        $typeNumber = $Matches[2]
        $typeName = if ($Matches.Count -gt 3 -and -not [string]::IsNullOrWhiteSpace($Matches[3])) { $Matches[3] } else { $typeNumber }
        $key = ('{0}|{1}' -f $name, $typeName).ToLowerInvariant()
        $typeKey = ('{0}/{1}' -f $typeNumber, $typeName)
        if (Test-ZeroPointerText $Matches[4]) {
            Add-Count $demandMissByResource $key
            Add-Count $demandMissByType $typeKey
        } else {
            Add-Count $demandHitByResource $key
            Add-Count $demandHitByType $typeKey
        }
    }

    if ($line -match 'CNWCModule::LoadModule|CNWCModule::LoadModuleResources|legacy module hak mount #[0-9]+ (begin|complete|aborted|skipped)|legacy module-load hak-list|legacy module hak rules reload') {
        $moduleLines.Add($line)
    }
    if ($line -match 'area-load dispatch|CNWTileSet|CNWSArea::SetTileset|runtime resource diagnostics armed|legacy quickbar') {
        $areaLines.Add($line)
    }
    if ($line -match 'live-object packet summary|triggers=|legacy live trigger add|object-useable dispatch observed|placeable SetUseable observed|placeable add absent bool skip|generic-update trailing word skip|auto-use object|outbound game packet detail|client-to-server message .*major=6/0x06|Input_|loadbar dispatch observed|real LoadBar|synthetic LoadBar|server-to-player flow marker|legacy loadbar|Area_ClientArea|Area_AreaLoaded|suppressed unexpected Area_AreaLoaded|CNWCArea::LoadArea') {
        $transitionLines.Add($line)
    }
    if ($line -match 'client-to-server message|client Area_AreaLoaded marker|suppressed unexpected Area_AreaLoaded|SendPlayerToServerMessage diagnostic') {
        $clientMessageLines.Add($line)
        if ($line -match 'client Area_AreaLoaded marker|name=''Area_AreaLoaded''') {
            $clientAreaLoadedCount += 1
        }
    }
    if ($line -match 'synthetic Area_AreaLoaded') {
        $clientMessageLines.Add($line)
    }
    if ($line -match 'CNWMessage::CreateWriteMessage|CNWMessage::GetWriteMessage') {
        $writeMessageLines.Add($line)
    }
    if ($line -match "legacy BNVR response|legacy BNVS|SendBNVS|BNCS|BNCR|connection \d+ now targets|server-list target match|StartEnumerateSessions") {
        $networkLines.Add($line)
    }
    if ($line -match 'asset diagnostics:') {
        $assetLines.Add($line)
    }
    if ($line -match 'HG password-gate marker') {
        if ($line -match 'prompt=1') { $passwordPromptCount += 1 }
        if ($line -match 'incorrect=1') { $passwordIncorrectCount += 1 }
        if ($line -match 'welcome=1') { $welcomeCount += 1 }
    }
    if ($line -match 'HG password auto-speak') {
        $autoSpeakCount += 1
        if ($line -match 'skipped') {
            $autoSpeakSkippedCount += 1
        }
    }
    if ($IncludeChatPreview -and $line -match 'server chat dispatch') {
        $chatLines.Add($line)
    }
}

Write-Host ('Bridge log: {0}' -f $file.FullName)
Write-Host ('Last write: {0:yyyy-MM-dd HH:mm:ss}  Size: {1:N0} bytes  Lines: {2:N0}' -f $file.LastWriteTime, $file.Length, $lines.Count)
if ($LatestRun) {
    if ($latestRunStartLine -gt 0) {
        Write-Host ('Latest run only: starting at original line {0:N0}' -f $latestRunStartLine)
    } else {
        Write-Host 'Latest run only: no run marker found, summarized the whole file'
    }
}
Write-Host ('Password markers: prompt={0} incorrect={1} welcome={2} auto_speak_lines={3} auto_speak_skipped={4}' -f $passwordPromptCount, $passwordIncorrectCount, $welcomeCount, $autoSpeakCount, $autoSpeakSkippedCount)
Write-Host ('Client area-loaded markers: {0}' -f $clientAreaLoadedCount)

Write-SampleLines 'Network / Login Markers' $networkLines 20
Write-SampleLines 'Module Markers' $moduleLines 30
Write-SampleLines 'Area / Tileset Markers' $areaLines 30
Write-SampleLines 'Transition / Load Markers' $transitionLines 45
Write-SampleLines 'Client-To-Server Message Markers' $clientMessageLines 35
Write-SampleLines 'CNWMessage Write Markers' $writeMessageLines 35
Write-SampleLines 'Asset Diagnostics' $assetLines 35
Write-SampleLines 'Chat Preview Lines' $chatLines 20

Write-TopCounts 'Lookup Misses By Type' $lookupMissByType $Top
Write-TopCounts 'Lookup Misses By Resource' $lookupMissByResource $Top
Write-TopCounts 'Lookup Hits By Type' $lookupHitByType $Top
Write-TopCounts 'Lookup Hits By Resource' $lookupHitByResource ([Math]::Min($Top, 15))
Write-TopCounts 'Demand Misses By Type' $demandMissByType $Top
Write-TopCounts 'Demand Misses By Resource' $demandMissByResource $Top
Write-TopCounts 'Demand Hits By Type' $demandHitByType ([Math]::Min($Top, 15))
Write-TopCounts 'Demand Hits By Resource' $demandHitByResource ([Math]::Min($Top, 15))
