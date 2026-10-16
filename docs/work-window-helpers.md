# Window geometry helpers

The startup window-creation path at `0x004b4800` calls `0x004b4de0` after
showing and focusing its window. The connected helper group is reconstructed
in `windowutil.cpp`; all eleven functions match the retail instructions in an
isolated SP5 build with the existing flags.

| Original VA | Bytes | Recovered behavior |
| --- | ---: | --- |
| `0x004b4b80` | 33 | Window rectangle width. |
| `0x004b4bb0` | 33 | Window rectangle height. |
| `0x004b4be0` | 27 | Window rectangle left coordinate. |
| `0x004b4c00` | 27 | Window rectangle top coordinate. |
| `0x004b4c20` | 14 | `GetWindowLong(GWL_HWNDPARENT)` cast to HWND. |
| `0x004b4c30` | 14 | `GetWindowLong(GWL_STYLE)`. |
| `0x004b4c40` | 136 | Change window width, preserving position and height. |
| `0x004b4cd0` | 136 | Change window height, preserving position and width. |
| `0x004b4d60` | 124 | Child-enumeration callback expanding the bounds rectangle. |
| `0x004b4de0` | 146 | Size a window around its direct children. |
| `0x004b4e80` | 102 | Convert both rectangle corners from screen to client coordinates. |

The function boundaries and calling conventions were checked in retail x86
instructions, including the callback's `ret 8`. Import addresses identify
`GetWindowRect` at IAT `0x0052f1f0`, `GetWindowLongA` at `0x0052f1ec`,
`MoveWindow` at `0x0052f1f4`, `GetParent` at `0x0052f1f8`,
`EnumChildWindows` at `0x0052f1e0`, `GetMenu` at `0x0052f1e4`,
`AdjustWindowRectEx` at `0x0052f200`, and `ScreenToClient` at `0x0052f1dc`.
Call sites at `0x004b497e` and in the `0x004bb2xx` group corroborate the
window-handle arguments and geometry results. Source names describe behavior;
the original names and engine/library ownership are unknown. The current
archive candidate scan has no matches in this range, which does not rule out
third-party provenance.

The width/height setters return FALSE when the requested size is unchanged
and TRUE after attempting a move, even if `MoveWindow` fails. For a child
window with a nonnull `GWL_HWNDPARENT`, they convert its screen rectangle to
that parent's client coordinates before moving it. `GWL_HWNDPARENT` is kept
as the original API call rather than replaced with `GetParent`.

The enumeration callback accepts `(HWND, LPARAM)` with the Win32 callback
calling convention. It uses zero-initialized global HWND `0x00679b80` to
filter for direct children, then expands the caller's RECT in bottom, right,
left, top order. The fitting helper initializes that RECT to zero, stores
the parent handle in the global, and enumerates descendants. Consequently,
the bounds include the client origin even when every child starts away
from it. It adjusts the rectangle for the window style, extended style and
menu, then changes width and height separately. The routine is represented
with a provisional void return: its startup caller discards EAX, and the
observed paths do not establish a meaningful result contract.

The query and coordinate-conversion helpers retain the retail behavior of
ignoring Win32 error returns. No runtime execution or behavioral tests were
used; validation compares compiled instructions with the supplied executable.
