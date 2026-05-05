param(
    [string]$AssetRoot,
    [string]$RepositoryRoot = 'C:\Program Files (x86)\Steam\steamapps\common\Neverwinter Nights\hg-bridge-assets\nwsync',
    [string]$ToolPath,
    [string]$Name = 'Higher Ground EE Bridge',
    [string]$Description = 'Higher Ground 1.69 content staged for NWN:EE bridge clients.',
    [int]$GroupId = 213,
    [int]$LimitFileSizeMB = 1024,
    [switch]$Force,
    [switch]$WriteOrigins,
    [switch]$SkipEeModelFixes,
    [switch]$Apply
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Resolve-RepositoryRoot {
    return (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
}

function Resolve-RequiredDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [string]$Label
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        throw "$Label not found: $Path"
    }
    return (Resolve-Path -LiteralPath $Path).Path
}

function Resolve-NwsyncWriter {
    param([string]$ExplicitPath)

    if (-not [string]::IsNullOrWhiteSpace($ExplicitPath)) {
        if (-not (Test-Path -LiteralPath $ExplicitPath -PathType Leaf)) {
            throw "nwn_nwsync_write tool not found: $ExplicitPath"
        }
        return (Resolve-Path -LiteralPath $ExplicitPath).Path
    }

    $repoRoot = Resolve-RepositoryRoot
    $repoCandidate = Get-ChildItem -LiteralPath (Join-Path $repoRoot 'third_party') -Recurse -File -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -eq 'nwsync_write.exe' -or $_.Name -eq 'nwn_nwsync_write.exe' } |
        Sort-Object FullName |
        Select-Object -First 1
    if ($null -ne $repoCandidate) {
        return $repoCandidate.FullName
    }

    $command = Get-Command 'nwsync_write.exe' -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }
    $command = Get-Command 'nwn_nwsync_write.exe' -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    throw @"
nwsync_write.exe was not found.

Install or unpack the NWN NWSync tools, then rerun with -ToolPath:
  .\tools\prepare-nwsync-repository.ps1 -ToolPath C:\path\to\nwsync_write.exe -Apply
"@
}

function Add-ExistingPath {
    param(
        [System.Collections.Generic.List[string]]$List,
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [switch]$Leaf
    )

    $pathType = if ($Leaf) { 'Leaf' } else { 'Container' }
    if (Test-Path -LiteralPath $Path -PathType $pathType) {
        $resolved = (Resolve-Path -LiteralPath $Path).Path
        if (-not $List.Contains($resolved)) {
            $List.Add($resolved)
        }
    }
}

function Resolve-HakFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Resref,
        [Parameter(Mandatory = $true)]
        [string]$AssetRoot
    )

    $fileName = "$Resref.hak"
    $candidates = @(
        (Join-Path $AssetRoot "hg-std\hak\$fileName"),
        (Join-Path $AssetRoot "hg-gui\hak\$fileName"),
        (Join-Path $AssetRoot "cep23\hak\$fileName")
    )
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    throw "Required hak not found for resref '$Resref' under $AssetRoot"
}

function Get-FirstSha1FromText {
    param([string]$Text)

    $match = [regex]::Match($Text, '(?i)(?<![0-9a-f])[0-9a-f]{40}(?![0-9a-f])')
    if ($match.Success) {
        return $match.Value.ToLowerInvariant()
    }
    return ''
}

function ConvertTo-SanitizedAsciiMdl {
    param([byte[]]$Bytes)

    if ($null -eq $Bytes -or $Bytes.Length -eq 0) {
        return $null
    }

    $text = [System.Text.Encoding]::ASCII.GetString($Bytes)
    if ($text.IndexOf('#MAXMODEL ASCII', [System.StringComparison]::OrdinalIgnoreCase) -lt 0 -and
        $text.IndexOf('newmodel ', [System.StringComparison]::OrdinalIgnoreCase) -lt 0) {
        return $null
    }

    $cleaned = [regex]::Replace(
        $text,
        '(?im)^[ \t]*filedepend(?:ency|ancy)\b[^\r\n]*(?:\r?\n)?',
        '')
    if ($cleaned -eq $text) {
        return $null
    }

    return [System.Text.Encoding]::ASCII.GetBytes($cleaned)
}

function Get-ErfResRef {
    param([byte[]]$Bytes)

    $nul = [Array]::IndexOf($Bytes, [byte]0)
    if ($nul -lt 0) {
        $nul = $Bytes.Length
    }
    return [System.Text.Encoding]::ASCII.GetString($Bytes, 0, $nul).Trim()
}

function Update-EeMdlFixFile {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ResRef,
        [byte[]]$SanitizedBytes,
        [Parameter(Mandatory = $true)]
        [string]$OutputRoot,
        [hashtable]$Stats
    )

    $fileName = "$($ResRef.ToLowerInvariant()).mdl"
    $outPath = Join-Path $OutputRoot $fileName
    if ($null -eq $SanitizedBytes) {
        if (Test-Path -LiteralPath $outPath -PathType Leaf) {
            Remove-Item -LiteralPath $outPath -Force
            $Stats.Removed += 1
        }
        return
    }

    [System.IO.File]::WriteAllBytes($outPath, $SanitizedBytes)
    $Stats.Written += 1
}

function Update-EeMdlFixesFromErf {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [string]$OutputRoot,
        [hashtable]$Stats
    )

    $stream = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::Read)
    try {
        $reader = [System.IO.BinaryReader]::new($stream)
        try {
            $fileType = [System.Text.Encoding]::ASCII.GetString($reader.ReadBytes(4))
            $version = [System.Text.Encoding]::ASCII.GetString($reader.ReadBytes(4))
            if (($fileType -ne 'HAK ' -and $fileType -ne 'ERF ' -and $fileType -ne 'MOD ' -and $fileType -ne 'NWM ') -or
                $version -ne 'V1.0') {
                return
            }

            [void]$reader.ReadUInt32() # language count
            [void]$reader.ReadUInt32() # localized string size
            $entryCount = $reader.ReadUInt32()
            [void]$reader.ReadUInt32() # localized string offset
            $keyOffset = $reader.ReadUInt32()
            $resourceOffset = $reader.ReadUInt32()

            for ($index = 0; $index -lt $entryCount; $index++) {
                $stream.Position = [int64]$keyOffset + ($index * 24)
                $resRef = Get-ErfResRef -Bytes ($reader.ReadBytes(16))
                if ([string]::IsNullOrWhiteSpace($resRef)) {
                    continue
                }

                $resourceId = $reader.ReadUInt32()
                $resourceType = $reader.ReadUInt16()
                [void]$reader.ReadUInt16()
                if ($resourceType -ne 2002) {
                    continue
                }

                $stream.Position = [int64]$resourceOffset + ($resourceId * 8)
                $dataOffset = $reader.ReadUInt32()
                $dataSize = $reader.ReadUInt32()
                if ($dataSize -eq 0 -or $dataSize -gt 64MB) {
                    continue
                }

                $stream.Position = $dataOffset
                $bytes = $reader.ReadBytes([int]$dataSize)
                $sanitized = ConvertTo-SanitizedAsciiMdl -Bytes $bytes
                Update-EeMdlFixFile -ResRef $resRef -SanitizedBytes $sanitized -OutputRoot $OutputRoot -Stats $Stats
                $Stats.Scanned += 1
            }
        } finally {
            $reader.Dispose()
        }
    } finally {
        $stream.Dispose()
    }
}

function Update-EeMdlFixesFromDirectory {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,
        [Parameter(Mandatory = $true)]
        [string]$OutputRoot,
        [hashtable]$Stats
    )

    Get-ChildItem -LiteralPath $Path -Recurse -File -Filter '*.mdl' -ErrorAction SilentlyContinue |
        Sort-Object FullName |
        ForEach-Object {
            $bytes = [System.IO.File]::ReadAllBytes($_.FullName)
            $sanitized = ConvertTo-SanitizedAsciiMdl -Bytes $bytes
            Update-EeMdlFixFile -ResRef $_.BaseName -SanitizedBytes $sanitized -OutputRoot $OutputRoot -Stats $Stats
            $Stats.Scanned += 1
        }
}

function Write-EeModelFixHak {
    param(
        [Parameter(Mandatory = $true)]
        [string]$SourceRoot,
        [Parameter(Mandatory = $true)]
        [string]$OutputPath
    )

    $files = @(Get-ChildItem -LiteralPath $SourceRoot -File -Filter '*.mdl' -ErrorAction SilentlyContinue |
        Sort-Object Name)
    if ($files.Count -eq 0) {
        if (Test-Path -LiteralPath $OutputPath -PathType Leaf) {
            Remove-Item -LiteralPath $OutputPath -Force
        }
        return 0
    }

    $headerSize = 160
    $keyEntrySize = 24
    $resourceEntrySize = 8
    $keyOffset = $headerSize
    $resourceOffset = $keyOffset + ($files.Count * $keyEntrySize)
    $dataOffset = $resourceOffset + ($files.Count * $resourceEntrySize)

    $parent = Split-Path -Parent $OutputPath
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    $stream = [System.IO.File]::Open($OutputPath, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write, [System.IO.FileShare]::None)
    try {
        $writer = [System.IO.BinaryWriter]::new($stream)
        try {
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes('HAK '))
            $writer.Write([System.Text.Encoding]::ASCII.GetBytes('V1.0'))
            $writer.Write([uint32]0) # language count
            $writer.Write([uint32]0) # localized string size
            $writer.Write([uint32]$files.Count)
            $writer.Write([uint32]$headerSize) # localized string offset
            $writer.Write([uint32]$keyOffset)
            $writer.Write([uint32]$resourceOffset)
            $writer.Write([uint32]0) # build year
            $writer.Write([uint32]0) # build day
            $writer.Write([uint32]::MaxValue) # description strref
            $writer.Write((New-Object byte[] 116))

            for ($index = 0; $index -lt $files.Count; $index++) {
                $resRefBytes = New-Object byte[] 16
                $nameBytes = [System.Text.Encoding]::ASCII.GetBytes($files[$index].BaseName.ToLowerInvariant())
                [Array]::Copy($nameBytes, $resRefBytes, [Math]::Min($nameBytes.Length, $resRefBytes.Length))
                $writer.Write($resRefBytes)
                $writer.Write([uint32]$index)
                $writer.Write([uint16]2002)
                $writer.Write([uint16]0)
            }

            $currentDataOffset = $dataOffset
            foreach ($file in $files) {
                $writer.Write([uint32]$currentDataOffset)
                $writer.Write([uint32]$file.Length)
                $currentDataOffset += [int64]$file.Length
            }

            foreach ($file in $files) {
                $input = [System.IO.File]::Open($file.FullName, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::Read)
                try {
                    $input.CopyTo($stream)
                } finally {
                    $input.Dispose()
                }
            }
        } finally {
            $writer.Dispose()
        }
    } finally {
        $stream.Dispose()
    }

    return $files.Count
}

$repoRoot = Resolve-RepositoryRoot
if ([string]::IsNullOrWhiteSpace($AssetRoot)) {
    $AssetRoot = Join-Path $repoRoot 'hg-bridge-assets'
}

$assetRootResolved = Resolve-RequiredDirectory -Path $AssetRoot -Label 'HG bridge asset root'
$repositoryRootResolved = [System.IO.Path]::GetFullPath($RepositoryRoot)
$writer = Resolve-NwsyncWriter -ExplicitPath $ToolPath

$moduleHakOrderTopFirst = @(
    'cep2_custom',
    'cep2_top_v23',
    'cep2_add_phenos5',
    'cep2_add_phenos4',
    'cep2_add_phenos3',
    'cep2_add_phenos2',
    'cep2_add_phenos1',
    'cep2_add_loads',
    'cep2_add_rules',
    'cep2_add_sb_v1',
    'cep2_core6',
    'cep2_core5',
    'cep2_core4',
    'cep2_core3',
    'cep2_core2',
    'cep2_core1',
    'cep2_core0',
    'cep2_add_doors',
    'cep2_add_tiles2',
    'cep2_add_tiles1',
    'cep2_ext_tiles',
    'cep2_add_skies',
    'cep2_crp'
)

$inputs = [System.Collections.Generic.List[string]]::new()
for ($index = $moduleHakOrderTopFirst.Count - 1; $index -ge 0; $index--) {
    $inputs.Add((Resolve-HakFile -Resref $moduleHakOrderTopFirst[$index] -AssetRoot $assetRootResolved))
}

foreach ($tlkRoot in @(
    (Join-Path $assetRootResolved 'cep23\tlk'),
    (Join-Path $assetRootResolved 'hg-std\tlk'),
    (Join-Path $assetRootResolved 'hg-gui\tlk')
)) {
    if (Test-Path -LiteralPath $tlkRoot -PathType Container) {
        Get-ChildItem -LiteralPath $tlkRoot -File -Filter '*.tlk' |
            Sort-Object Name |
            ForEach-Object {
                if (-not $inputs.Contains($_.FullName)) {
                    $inputs.Add($_.FullName)
                }
            }
    }
}

foreach ($looseRoot in @(
    (Join-Path $assetRootResolved 'cep23\override'),
    (Join-Path $assetRootResolved 'hg-override\override'),
    (Join-Path $assetRootResolved 'hg-overlay\override')
)) {
    Add-ExistingPath -List $inputs -Path $looseRoot
}

$eeModelFixRoot = Join-Path $assetRootResolved 'ee-fixes\override'
$eeModelFixHak = Join-Path $assetRootResolved 'ee-fixes\hg_ee_model_fixes.hak'
if (-not $SkipEeModelFixes) {
    if ($Apply) {
        New-Item -ItemType Directory -Path $eeModelFixRoot -Force | Out-Null
        $fixStats = @{
            Scanned = 0
            Written = 0
            Removed = 0
        }
        foreach ($input in @($inputs)) {
            if (Test-Path -LiteralPath $input -PathType Leaf) {
                $extension = [System.IO.Path]::GetExtension($input).ToLowerInvariant()
                if ($extension -eq '.hak' -or $extension -eq '.erf' -or $extension -eq '.mod' -or $extension -eq '.nwm') {
                    Update-EeMdlFixesFromErf -Path $input -OutputRoot $eeModelFixRoot -Stats $fixStats
                }
            } elseif (Test-Path -LiteralPath $input -PathType Container) {
                Update-EeMdlFixesFromDirectory -Path $input -OutputRoot $eeModelFixRoot -Stats $fixStats
            }
        }
        Write-Host "EE model fixes:  scanned=$($fixStats.Scanned) written=$($fixStats.Written) removed=$($fixStats.Removed) root=$eeModelFixRoot"
        $packedFixes = Write-EeModelFixHak -SourceRoot $eeModelFixRoot -OutputPath $eeModelFixHak
        Write-Host "EE model fixes:  packed=$packedFixes hak=$eeModelFixHak"
    } elseif (Test-Path -LiteralPath $eeModelFixRoot -PathType Container) {
        Write-Host "EE model fixes:  using existing overlay at $eeModelFixRoot (add -Apply to regenerate)"
    }
} elseif (Test-Path -LiteralPath $eeModelFixHak -PathType Leaf) {
    Write-Host "EE model fixes:  using existing hak overlay at $eeModelFixHak"
} elseif ($Apply -and (Test-Path -LiteralPath $eeModelFixRoot -PathType Container)) {
    $packedFixes = Write-EeModelFixHak -SourceRoot $eeModelFixRoot -OutputPath $eeModelFixHak
    Write-Host "EE model fixes:  packed existing overlay=$packedFixes hak=$eeModelFixHak"
}

Add-ExistingPath -List $inputs -Path $eeModelFixHak -Leaf

$lookupPaths = [System.Collections.Generic.List[string]]::new()
foreach ($lookupRoot in @(
    (Join-Path $assetRootResolved 'cep23\hak'),
    (Join-Path $assetRootResolved 'cep23\tlk'),
    (Join-Path $assetRootResolved 'hg-std\hak'),
    (Join-Path $assetRootResolved 'hg-std\tlk'),
    (Join-Path $assetRootResolved 'hg-gui\hak'),
    (Join-Path $assetRootResolved 'hg-gui\tlk')
)) {
    Add-ExistingPath -List $lookupPaths -Path $lookupRoot
}

if ($inputs.Count -eq 0) {
    throw "No NWSync inputs were found under $assetRootResolved"
}

if ($Apply) {
    New-Item -ItemType Directory -Path $repositoryRootResolved -Force | Out-Null
} else {
    Write-Host 'Dry run only. Add -Apply to write the NWSync repository.'
}

Write-Host "Asset root:      $assetRootResolved"
Write-Host "Repository root: $repositoryRootResolved"
Write-Host "Writer:          $writer"
Write-Host "Inputs:          $($inputs.Count)"

$writerArgs = @()
if (-not $Apply) {
    $writerArgs += '--dry-run'
}
if ($Force) {
    $writerArgs += '-f'
}
if ($WriteOrigins) {
    $writerArgs += '--write-origins'
}
$writerArgs += "--name=$Name"
$writerArgs += "--description=$Description"
$writerArgs += "--group-id=$GroupId"
$writerArgs += "--limit-file-size=$LimitFileSizeMB"
foreach ($lookupPath in $lookupPaths) {
    $writerArgs += '--path'
    $writerArgs += $lookupPath
}
$writerArgs += $repositoryRootResolved
$writerArgs += $inputs

& $writer @writerArgs
if ($LASTEXITCODE -ne 0) {
    throw "nwsync_write failed with exit code $LASTEXITCODE"
}

$latestPath = Join-Path $repositoryRootResolved 'latest'
if ($Apply -and (Test-Path -LiteralPath $latestPath -PathType Leaf)) {
    $hash = Get-FirstSha1FromText -Text (Get-Content -LiteralPath $latestPath -Raw)
    if ([string]::IsNullOrWhiteSpace($hash)) {
        throw "Repository was written, but no SHA1 hash was found in $latestPath"
    }

    $envPath = Join-Path $repoRoot 'hg-bridge-nwsync.env'
    $envLines = @(
        "HG_BRIDGE_NWSYNC_ROOT=$repositoryRootResolved",
        "HG_BRIDGE_NWSYNC_HASH=$hash",
        'HG_BRIDGE_NWSYNC_URL=http://127.0.0.1:5122/'
    )
    Set-Content -LiteralPath $envPath -Value $envLines -Encoding ASCII
    Write-Host "Manifest hash:   $hash"
    Write-Host "Env file:        $envPath"
}
