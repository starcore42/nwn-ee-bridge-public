# Local Asset Staging

Put local NWN and HG/CEP content here when you want to build a NWSync repository.

This directory is intentionally ignored by Git. Do not commit HAK, TLK, KEY, ERF,
NWSync repository data, CD keys, account files, logs, or copied game installs.

Expected layout for `tools/prepare-nwsync-repository.ps1`:

```text
hg-bridge-assets/
  cep23/
    hak/
    tlk/
  hg-std/
    hak/
    tlk/
  hg-gui/
    hak/
    tlk/
  hg-override/
    override/
  hg-overlay/
    override/
```

The NWSync helper looks for the HG module HAKs in `hg-std/hak`,
`hg-gui/hak`, then `cep23/hak`; it adds TLKs from the listed TLK folders and
optional loose overrides from the override folders.

Generated output can also live under this folder, for example:

```text
hg-bridge-assets/
  nwsync/
  ee-fixes/
```
