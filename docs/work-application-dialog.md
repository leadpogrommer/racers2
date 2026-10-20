# Application settings dialog

`ApplicationSettingsDialog` wraps the retail modal dialog and the registry-backed
renderer toggles. The recovered code uses `DialogBoxParamA` with the startup
instance handle, dialog resource ID `0x66`, a null owner, and no initialization
parameter. `ApplicationSettingsDialogProc` has the Windows `DLGPROC` calling
convention and handles only `WM_INITDIALOG` and `WM_COMMAND`.

On initialization the procedure creates or opens
`HKLM\Software\Attention To Detail\Rendering Options` with
`RegCreateKeyExA`, `REG_OPTION_NON_VOLATILE`, and `KEY_ALL_ACCESS`. It queries
eight DWORD values in registry order, then separately updates controls
1000–1007. A successful query replaces the value in the global
`DisplayConfiguration`; a failed query leaves the current value intact. The
control update indexes a two-entry `{BST_UNCHECKED, BST_CHECKED}` table directly
with each DWORD value, without clamping it to 0 or 1. This preserves retail's
unchecked behavior for unexpected registry values. On `IDOK`, the procedure
first reads all controls with `BM_GETCHECK` and stores one only for
`BST_CHECKED`, then writes all eight DWORD values with `RegSetValueExA`, closes
the key, and ends the dialog with result 1. `IDCANCEL` ends it with result 2
without reading or writing settings. Unknown `WM_COMMAND` IDs return `TRUE`;
messages other than `WM_INITDIALOG` and `WM_COMMAND` return `FALSE`.

The settings map is supported by the registry value-name pointers and control
IDs in retail `0x0049a820`:

| Control | Registry value | `DisplayConfiguration` field |
| ---: | --- | --- |
| 1000 | `Dithering` | `m_dithering` at `+0x18` |
| 1001 | `AntiAliasing` | `m_antiAliasing` at `+0x1c` |
| 1002 | `WriteOnlyVBs` | `m_writeOnlyVBs` at `+0x20` |
| 1003 | `LargeVBs` | `m_largeVBs` at `+0x24` |
| 1004 | `HardwareTransformAndLighting` | `m_hardwareTransformAndLighting` at `+0x28` |
| 1005 | `NVidia` | `m_nvidia` at `+0x2c` |
| 1006 | `VSync` | `m_vSync` at `+0x30` |
| 1007 | `TripleBuffer` | `m_tripleBuffer` at `+0x34` |

`DisplayConfiguration` spans `0x3c` bytes, matching the zeroing in renderer
initialization at `0x004ac17a`. Its `+0x14` field is the height-to-width ratio,
computed as a float during renderer initialization; `+0x10` and `+0x38`
remain unknown. The dialog's eight setting fields are accessed directly at
the offsets above. Its global pointer is declared in
[`displayconfig.h`](../LEGO%20Racers%202/include/render/device/displayconfig.h); the startup
translation unit owns the pointer storage.

The dialog module requires the `ADVAPI32` import library for the registry APIs.
Its translation unit compiled with the pinned VC6 SP5 compiler. The retail
dialog and icons are extracted and linked through the generated `.res` input;
see [resource extraction](work-resources.md). This translation unit does not
define substitute resource data, and the dialog has not yet been exercised in
the runtime build.
