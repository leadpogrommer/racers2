# Startup and message-loop roadmap

This note traces the native startup path from the executable entry point to
the application message pump. Addresses and literal values below were checked
against `/tmp/lr2-disasm.txt`, the supplied PE's imports/data, and the
headless Ghidra exports under `data/ghidra-export/004c30*.c`. Ghidra output is
used as a navigation aid; function boundaries and behavior were checked in the
disassembly. No LR1 class or address assumptions are used here.

## Entry path

The PE entry point is `0x0051a33b`, a CRT startup routine rather than the game
entry. After CRT setup and command-line processing, it obtains the module
handle with `GetModuleHandleA(NULL)` and calls `0x004c3040` at `0x0051a416`.
The four stack arguments are consistent with `WinMain(HINSTANCE, HINSTANCE,
LPSTR, int)`: module handle, null previous-instance value, the command-line
tail after argv[0], and the startup `nCmdShow` value. `0x004c3040` returns with
`ret 0x10` and its result is passed into CRT exit processing. Its proven body
length is 617 bytes (`0x269`, ends at `0x004c32a8`).

The first path in `WinMain` creates a named mutex using the literal
`"Saracen II"` at `0x0054a108`. If creation fails, or the successful call's
`GetLastError()` is `ERROR_ALREADY_EXISTS` (`183`), startup finds the existing
window with class name `"Saracen II"` and a title returned by application
singleton virtual slot `0x2c`. If that window is minimized it is restored with
`ShowWindow(..., SW_RESTORE)`; it is then foregrounded and this process returns
zero.

On the new-instance path, WinMain installs the small callback at
`0x004c32b0` as an unhandled-exception filter; it returns `1` and cleans its
single stack argument. WinMain then calls `CoInitializeEx(NULL, 0)`, ensures a
lazy singleton through `0x004a3ed0`, and checks global `0x00578c9c`. If the
check passes, application singleton method `+0x44` is called before
`0x004b4690(hInstance, 0x004c32c0, commandLine)`.

`0x004b4690` records startup values in globals `0x0067a4a8`, `0x0067a4ec`, and
`0x0067a4f0`, then calls `0x0052e00a` on the command-line tail. The call target
is the CRT `_strlwr` routine: it lowercases that tail in place. It is not the
switch parser. The command-line helpers at `0x004b49e0` and `0x004b4a60`
perform separate substring searches; `WinMain` uses `0x004b4a60` for
`/backlevel` (`0x0054a690`), `/highqualitysound` (`0x0054a678`), and
`/lowqualitysound` (`0x0054a664`). `/backlevel` changes the current directory
to the literal `"..\\"` at `0x0054a68c`.

`0x004b49e0` copies both strings into 256-byte local buffers, lowercases each,
and searches for the second string in the first. If found, it translates the
match offset back to the original first string. `0x004b4a60` turns that pointer
result into a Boolean. `0x004b4a80` uses the same search to locate a switch
value, then copies the next space-delimited or quoted value into a caller
buffer. `0x004b4b30` scans that value to its first ASCII digit and converts the
remaining text with the CRT `_atol` routine. These observations are based on their callers,
the disassembly, and the headless decompiler exports; edge behavior for
overlong or malformed command lines remains dependent on the original CRT
string routines. The 161-byte value extractor now reaches 100% instruction
accuracy. Initializing the output index before the quote state and
using an explicit top-tested loop with a continuation branch reproduced the
retail register assignment and branch direction under the existing flags.

Next, `0x004f6310` acts as a readiness gate. It probes global service objects
at `0x00549dfc` and `0x00549e80`, then calls application singleton virtual
slot `0x74` and `0x004b7c50`; it returns a Boolean-like result. If ready,
WinMain calls application virtual slot `+0x48`, then
`0x004af2f0(1)`. That accessor lazily allocates a `0x2a624`-byte object at
`0x0065312c`; the nearby `/highqualitysound` and `/lowqualitysound` checks
select virtual slot `+0x14` with value `2` or `0`. This is strong evidence
that this manager participates in audio setup, but its class identity remains
unknown.

## Message pump and teardown

WinMain keeps a local `MSG`, makes an initial non-removing `PeekMessageA`, and
sets global `0x0067a4fc` to `1`. Its loop checks `WM_QUIT` (`0x12`) before
reading another message. Application singleton virtual slot `+0x24` chooses
between blocking `GetMessageA` and nonblocking `PeekMessageA(..., PM_REMOVE)`.
For a retrieved message, virtual slot `+0x20` gets first refusal; if it returns
nonzero, WinMain calls `TranslateMessage` and `DispatchMessageA`. It then calls
virtual slot `+0x18`; a nonzero result takes the idle/update path. That path
checks `GetAsyncKeyState(VK_F10)`, calls `0x004bb210` when the low key-state
bit is set, and calls `0x004a3aa0` for the frame/update work.

When global `0x0067a500` is `1`, the loop calls `0x004c3020`, destroys window
handle global `0x0067a4a0`, and clears the flag. On `WM_QUIT`, WinMain calls
`CoUninitialize()` and returns `MSG.wParam`. The shutdown wrapper
`0x004c3020` is 18 bytes: it clears `0x0067a4fc`, calls `0x004f6360`, and
returns zero. `0x004f6360` performs multi-subsystem teardown, including
`0x004a3ed0`, `0x004a4040`, `0x004a3ca0`, `0x004af360`, and methods on the
objects at `0x0054a330` and `0x00549e80`.

The message callback at `0x004c32c0` is 225 bytes (`0xe1`). It first calls
application singleton virtual slot `+0x38` with the four window-message
arguments; a zero result returns zero immediately. It then suppresses system
commands `0xf140` and `0xf170`, handles `WM_DESTROY` by posting quit, and
handles `WM_CLOSE` by calling `0x004c3020` and destroying global window
`0x0067a4a0`. `WM_GETMINMAXINFO` (`0x24`) delegates to `0x004c33b0`;
`WM_POWERBROADCAST` (`0x218`) returns `0x424d5144`; private message `0x3b9`
delegates to `0x004c33f0`. Other messages reach `DefWindowProcA`.

`0x004c33b0` (52 bytes) receives `(HWND, MINMAXINFO*)` from the WndProc, with
the caller cleaning both stack arguments. It writes the first two integers
from the record pointed to by `0x0065307c` into both
`MINMAXINFO.ptMinTrackSize` and `ptMaxTrackSize`, fixing the window's minimum
and maximum tracking dimensions to that pair. `0x004c33f0` (37 bytes)
only handles `wParam == 1`: it obtains the object from `0x004af2f0(1)` and
calls virtual slot `+0x5c` with `-1`. The meaning of this private message is
not established. The callback `0x004c32b0` is a separate eight-byte function
by its address use, even though it was not among the automatic Ghidra range
exports.

The existing-single-instance and failed-startup path in WinMain calls
application virtual slot `+0x2c`, passes the returned title and
`"Saracen II"` class to `FindWindowA`, then restores/foregrounds a found window.
The main function's second failure path performs this same lookup and returns
zero. `CoUninitialize` is directly visible on the normal `WM_QUIT` exit path;
other early exits do not share that sequence in the observed body.

## Global and object anchors

These are address-based observations; names below describe current evidence,
not recovered source identifiers.

| Address | Evidence and current interpretation |
| --- | --- |
| `0x00578648` | Object returned by lazy accessor `0x0049a760`; its vtable is read through the returned pointer. Accessor uses a one-time guard and constructor `0x0049a610`. |
| `0x00578ca0`, `0x00578c70` | Lazy singleton pointer and object used by `0x004a3ed0`; constructor is `0x004a3f80`. |
| `0x0067a4a0` | Window handle passed to `DestroyWindow`. |
| `0x0067a4a8` | HINSTANCE saved by `0x004b4690`. |
| `0x0067a4ec`, `0x0067a4f0` | Startup callback and command-line pointer saved by `0x004b4690`. |
| `0x0067a4fc` | Startup/message-loop activity flag; set before the pump and cleared by `0x004c3020`. |
| `0x0067a500` | Deferred window-destruction flag checked by the message pump. |
| `0x0065307c` | Zero-initialized pointer to a heap-allocated `0x3c`-byte display configuration record. Its first two integer fields are copied to both min/max track-size points in `MINMAXINFO`; the full record type is unknown. |
| `0x0065312c` | Lazy manager returned by `0x004af2f0`; used with the audio-quality command-line switches. |
| `0x00549dfc`, `0x00549e80` | Global service objects checked by `0x004f6310` and touched during teardown. |

The names in Ghidra's startup export are `FUN_...`; the application class,
method names, and meanings of most vtable slots remain unknown. The stable
observations are the call sites and slot offsets above.

## Practical implementation order

The first independent helpers are implemented in `startup.cpp`:

- `FUN_004c32b0` is an eight-byte `LONG WINAPI` exception filter. It returns
  `EXCEPTION_EXECUTE_HANDLER` and matches 100%.
- `FUN_004c33b0` is a 52-byte cdecl helper taking `(HWND, MINMAXINFO*)`. The
  WndProc passes its window handle and `lParam`, then cleans the arguments. It
  matches 100%.
- The pointer global at `0x0065307c` is represented as a zero-initialized
  `LegoS32*`; its initial state passes the isolated `reccmp-datacmp` check.
  Allocation code at `0x004ac1e5` installs a heap record there, display setup
  writes its first two integers at `0x004ac31a` and `0x004ac4df`, and teardown
  frees it at `0x004ad113` then clears the pointer at `0x004ad11b`. The target
  is larger than a `POINT`, so the record itself remains an unknown type.

The remaining message callback is the next coherent slice:

1. Implement the small `0x004c33f0` private-message helper, then implement
   `0x004c32c0` with an explicitly documented application-object
   interface for vtable slots `+0x38`, `+0x20`, `+0x18`, `+0x5c`, and `+0x2c`.
   This callback depends on global `0x0065307c` and the object accessor
   `0x0049a760`, but does not require recreating the full frame renderer.
2. Implement `0x004c3020` and then recover the teardown chain beginning at
   `0x004f6360`. The shutdown chain is shorter than the startup path, though
   it touches several still-unknown managers.
3. The switch-search/value helpers are now represented in `commandline.cpp`;
   all four command-line helpers and the startup initializer are exact raw
   matches, and the helper CRT identities are mapped. Recover the readiness/startup gate before attempting all of
   `0x004c3040`. The main function is 617 bytes and depends on the application
   singleton, command-line helpers, service globals, window state, audio
   manager, and frame/update routine.

The window-creation dependency at `0x004b4800` also calls a connected group of
geometry and child-window fitting helpers. Those eleven routines are now
reconstructed in `windowutil.cpp`; see [window-helper evidence](work-window-helpers.md).

Avoid naming `0x00578648` or the other globals as LR1 types without separate
LR2 evidence. The separate `0x004c3460` candidate scans rows in 8-bit or
32-bit data for a non-`0xff`/nonzero byte and appears to be an image-buffer
helper; it is not on the direct startup or message-loop path.
