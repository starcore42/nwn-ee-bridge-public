# NWN EE Bridge

Experimental Windows bridge tooling for connecting Neverwinter Nights: Enhanced
Edition clients to legacy NWN 1.69 Higher Ground endpoints.

This is not a finished bridge or a user-ready release. It is a research and
development snapshot.

## Current State

There are two paths in this repo: the injected harness and the standalone proxy.
They are at very different levels of maturity.

### Harnessed EE Client

The harnessed client path mostly works for local testing. It injects
`nwncx_hg.dll` into an EE client process and uses client-side hooks to paper over
many of the protocol and content differences between EE and NWN 1.69.

That does not mean it is stable. There are still crashes, packet-alignment bugs,
resource/content edge cases, and ordinary gameplay issues. Treat this path as a
developer harness that proves pieces of the bridge can work, not as a polished
client setup.

### Standalone Proxy

The standalone proxy is still experimental and does not work properly yet. It is
proof-of-concept code for moving compatibility work out of the client process and
into a middle process that eventually looks like an EE server to EE clients and a
Diamond/1.69 client to HG.

Current known rough areas include, but are not limited to:

- placeable and live-object packet translation
- area loading and area transitions
- quickbar and inventory packet alignment
- NWSync/content advertisement details
- encrypted EE session lifecycle handling
- crashes and desyncs under real play
- multi-client/account handling

The CD-key path also needs real design work. At the moment the proxy uses a
local Diamond-format `nwncdkey.ini` to synthesize the legacy verifier sent to HG.
That means it is effectively useful for one local player/account at a time, not
as a proper multi-user public proxy.

If you are trying this, expect to debug it.

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

The proxy examples below are for experiments. They are not a recipe for a
reliable public server.

Basic local relay:

```powershell
.\build\Release\hgbridge_proxy.exe --listen 127.0.0.1:5121 --server 213
```

Current stock-EE proof-of-concept bundle:

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
synthesize the legacy verifier sent to HG. Do not commit that file. This is not
proper multi-user authentication; it is a one-local-player-at-a-time bridge
mechanism until the CD-key/account path is redesigned.

For local stock EE proxy testing:

```powershell
.\tools\start-stock-proxy-test.ps1 `
  -RestartProxy `
  -LaunchClient `
  -DiamondCdKey C:\path\to\nwncdkey.ini `
  -NwsyncRoot .\hg-bridge-assets\nwsync
```

## Run The Harness

The harness path injects `nwncx_hg.dll` into a local NWN:EE client and uses the
launcher to drive local compatibility tests. This is currently the more useful
path, but it is still a harness with known bugs and crashes.

```powershell
.\tools\test-hg-bridge.ps1 -Server 213 -DriverOnly -Launch
```

Most harness options are local-test conveniences. Use `-SteamRoot`,
`-DiamondConfigRoot`, `-AssetBundleRoot`, and `-Password` to point the helper at
your own local install, credentials, and assets. Keep those files outside Git.
