param(
  [ValidateSet("Debug", "Release")]
  [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path -LiteralPath (Join-Path $PSScriptRoot "..")
$repoRootPath = $repoRoot.Path

$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path -LiteralPath $vswhere)) {
  throw "vswhere.exe was not found. Install Visual Studio 2022 Build Tools with the C++ workload first."
}

$installationPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Workload.VCTools -property installationPath
if (-not $installationPath) {
  throw "No Visual Studio installation with the C++ Build Tools workload was found."
}

$msbuild = Join-Path $installationPath "MSBuild\Current\Bin\MSBuild.exe"
if (-not (Test-Path -LiteralPath $msbuild)) {
  throw "MSBuild.exe was not found under '$installationPath'."
}

$outDir = Join-Path $repoRootPath "build\$Configuration"
if (-not (Test-Path -LiteralPath $outDir)) {
  New-Item -ItemType Directory -Path $outDir | Out-Null
}
if (-not $outDir.EndsWith("\")) {
  $outDir += "\"
}
$outDirForMsbuild = $outDir -replace '\\', '/'

$projects = @(
  @{ Name = "nwncx_hg"; Path = Join-Path $repoRootPath "src\bridge\HgBridge.vcxproj" },
  @{ Name = "hgbridge_launcher"; Path = Join-Path $repoRootPath "src\launcher\HgBridgeLauncher.vcxproj" },
  @{ Name = "hgbridge_proxy"; Path = Join-Path $repoRootPath "src\proxy\HgBridgeProxy.vcxproj" }
)

foreach ($project in $projects) {
  $intDir = Join-Path $repoRootPath "build\.int\$Configuration\$($project.Name)"
  if (-not (Test-Path -LiteralPath $intDir)) {
    New-Item -ItemType Directory -Path $intDir | Out-Null
  }
  if (-not $intDir.EndsWith("\")) {
    $intDir += "\"
  }
  $intDirForMsbuild = $intDir -replace '\\', '/'

  Write-Host "Building $($project.Name) ($Configuration|x64)"
  & $msbuild $project.Path /t:Rebuild "/p:Configuration=$Configuration;Platform=x64;OutDir=$outDirForMsbuild;IntDir=$intDirForMsbuild;WholeProgramOptimization=false;UseMultiToolTask=false" /m:1 /nodeReuse:false /verbosity:minimal
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

Write-Host "Build outputs:"
Get-ChildItem -LiteralPath $outDir -File | Select-Object Name, Length, LastWriteTime | Format-Table -AutoSize
