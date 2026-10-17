# Native window creation

The display initializer calls `CreateNativeWindow` (`0x004b46c0`) with the
saved window procedure. It first registers the `Saracen II` class,
reads optional `windowx`/`windowy` values with the existing command-line
helpers, and then calls `CreateNativeWindowEx` (`0x004b4800`) using fields
from the display record at `0x0065307c`.

The retail extents are 132 bytes (`0x84`) for `0x004b46c0`, 165 bytes
(`0xa5`) for `0x004b4750`, and 466 bytes (`0x1d2`) for `0x004b4800`.
`RegisterNativeWindowClass` returns the `ATOM` from `RegisterClassExA`; the
caller checks its low word before continuing. All three functions use the
cdecl stack convention.

The class uses redraw styles, the supplied procedure, the current instance,
the application icon resource ID, and either the system arrow cursor or no
cursor in development mode. The stock-object handle is incremented when its
numeric value is at most `0x18`, matching the retail conversion before it is
stored as `hbrBackground`.

The window helper treats zero coordinates as `CW_USEDEFAULT`. Zero dimensions
become three quarters of the corresponding screen metric using the retail
signed-rounding expression. Its normal-window path adjusts the client bounds
with style `0x00cf0000` and extended style `0x300`, then creates the actual
window with style `0x00cb0000`. The fullscreen path uses a topmost popup at
`(0, 0)` with full-screen dimensions. A development-mode window gets the
observed static child and accelerator lookup. Successful creation performs
the observed hide/foreground/focus/show sequence and fits the window around
its children before storing the main and view handles.

Application-dependent calls now go through the constructed singleton and its
complete explicit dispatch table, including icon-resource and
development-mode queries. The development-mode child-window failure follows
the existing fatal-error helper; that helper itself remains separately
annotated unfinished. The startup runtime check has not entered display
initialization, so this window path and its registry/dialog dependencies
remain unexercised at runtime.

`g_mainWindow` (`0x0067a4a0`), `g_viewWindow` (`0x0067a4a4`), the 64-byte
mutable title buffer (`0x0067a4ac`), and `g_windowAccelerators`
(`0x0067a4f4`) are zero-initialized storage used by the display initializer,
window code, and message loop. No class-unregistration path was observed in
the startup teardown; the message loop destroys the main HWND.

The integrated VC6 build measured raw instruction similarity of 90.72% for
`0x004b46c0`, 32.00% for `0x004b4750`, and 43.07% for `0x004b4800` at this
checkpoint. Differences include relocated string operands, local structure
initialization, call setup and table-dispatch code shape, and different
control-flow/register allocation. These scores do not by themselves establish
equivalence. The three annotations remain `STUB` because they are below the
near-match threshold and their behavior has not been fully reviewed. The
native API code and application wiring are not evidence that the temporary
bootstrap reaches this path or that a created window is a playable milestone. The build needs the
GDI32 import library for `GetStockObject` in addition to USER32.
