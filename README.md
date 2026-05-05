# NWN EE Bridge

Experimental Windows bridge tooling for connecting Neverwinter Nights: Enhanced
Edition clients to legacy NWN 1.69 Higher Ground endpoints.

This public repo contains only source code, helper scripts, and open-source
third-party dependencies. It does not include game assets, HAK/TLK files,
NWSync repositories, CD keys, account files, decompile dumps, logs, or binaries.

## Contents

- `src/proxy`: `hgbridge_proxy.exe`, a standalone UDP proxy with EE crypto,
  packet rewrite, and NWSync advertisement/HTTP serving experiments.
- `src/bridge`: `nwncx_hg.dll`, the injected local harness used for compatibility
  testing against an EE client process.
- `src/launcher`: `hgbridge_launcher.exe`, a small launcher/injector for the
  harness DLL.
- `tools`: build, NWSync, stock-proxy, and harness test helpers.
- `third_party/miniz`: zlib/deflate support used by the proxy.
- `third_party/libhydrogen-legacy-nwn`: bundled legacy libhydrogen source used
  for NWN:EE BNK/encrypted-packet experiments.
- `hg-bridge-assets`: ignored local staging folder for assets and generated
  NWSync repositories.

## Requirements

- Windows x64.
- Visual Studio 2022 Build Tools with the C++ workload.
- PowerShell 5+.
- NWN:EE for local client testing.
- `nwsync_write.exe` or `nwn_nwsync_write.exe` if you want to generate a NWSync
  repository.

## Build

```powershell
.\tools\build.ps1 -Configuration Release
```

Build outputs are written to:

```text
build\Release\
  hgbridge_proxy.exe
  hgbridge_launcher.exe
  nwncx_hg.dll
```

List configured HG endpoints:

```powershell
.\build\Release\hgbridge_proxy.exe --list-servers
```

## Asset And NWSync Setup

Keep local assets under `hg-bridge-assets`. The folder is ignored by Git on
purpose.

Minimum expected staging shape:

```text
hg-bridge-assets\
  cep23\hak\
  cep23\tlk\
  hg-std\hak\
  hg-std\tlk\
  hg-gui\hak\
  hg-gui\tlk\
  hg-override\override\
  hg-overlay\override\
```

Generate a local NWSync repository:

```powershell
.\tools\prepare-nwsync-repository.ps1 `
  -AssetRoot .\hg-bridge-assets `
  -RepositoryRoot .\hg-bridge-assets\nwsync `
  -ToolPath C:\path\to\nwsync_write.exe `
  -Apply -Force
```

After a successful `-Apply`, the helper writes `hg-bridge-nwsync.env` with
`HG_BRIDGE_NWSYNC_ROOT`, `HG_BRIDGE_NWSYNC_HASH`, and `HG_BRIDGE_NWSYNC_URL`.
That env file is also ignored by Git.

## Run The Proxy

Basic local relay:

```powershell
.\build\Release\hgbridge_proxy.exe --listen 127.0.0.1:5121 --server 213
```

Current stock-EE experiment bundle:

```powershell
.\build\Release\hgbridge_proxy.exe `
  --listen 0.0.0.0:5121 `
  --server 213 `
  --allow-remote-clients `
  --ee-crypto `
  --diamond-cdkey C:\path\to\nwncdkey.ini `
  --nwsync-root .\hg-bridge-assets\nwsync `
  --rewrite-quickbar-simple `
  --rewrite-live-object-visual-transform-masks `
  --rewrite-live-object-material-shader-params `
  --rewrite-live-object-updates `
  --rewrite-live-object-item-appearance-visual-transforms `
  --rewrite-live-object-add-visual-transforms `
  --rewrite-live-object-add-records `
  --rewrite-player-list `
  --rewrite-area-client-area `
  --synthetic-area-loaded
```

`--diamond-cdkey` points to a local Diamond-format `nwncdkey.ini`; it is used to
synthesize the legacy verifier sent to HG. Do not commit that file.

For local stock EE testing:

```powershell
.\tools\start-stock-proxy-test.ps1 `
  -RestartProxy `
  -LaunchClient `
  -DiamondCdKey C:\path\to\nwncdkey.ini `
  -NwsyncRoot .\hg-bridge-assets\nwsync
```

## Run The Harness

The harness path injects `nwncx_hg.dll` into a local NWN:EE client and uses the
launcher to drive local compatibility tests.

```powershell
.\tools\test-hg-bridge.ps1 -Server 213 -DriverOnly -Launch
```

Most harness options are local-test conveniences. Use `-SteamRoot`,
`-DiamondConfigRoot`, `-AssetBundleRoot`, and `-Password` to point the helper at
your own local install, credentials, and assets. Keep those files outside Git.

## Publishing To GitHub

After reviewing the files, create a GitHub repo and add it as `origin`:

```powershell
git remote add origin https://github.com/<you>/<repo>.git
git branch -M main
git push -u origin main
```

Choose and add a project license before publishing if you want other people to
have explicit reuse rights. The vendored third-party dependencies keep their own
licenses in their respective folders.
