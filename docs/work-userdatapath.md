# User-data path helper

`BuildUserDataPath` reconstructs the behavior at retail VA `0x0040a7d0`.
The routine calls `SHGetFolderPathA` with a null owner/token, folder value
`0x8005` (`CSIDL_PERSONAL | CSIDL_FLAG_CREATE`), zero flags, and the caller's
buffer. It appends `\Games`, then `\LEGO Racers 2`. If the second argument is
nonzero, it calls `CreateDirectoryA(path, NULL)` after each append. It ignores
all three API results and does not bound-check the destination buffer; the
reconstruction preserves that behavior.

The import table identifies `SHGetFolderPathA` as an import from
`SHFOLDER.dll`; `CreateDirectoryA` is imported from `KERNEL32.dll`. These are
Windows APIs, not a CRT or third-party path library. The rebuilt function calls
the same Shell API directly. Its build therefore needs the corresponding
SHFOLDER import library in addition to the existing kernel32/user32 libraries.

The retail routine ends at `0x0040a860` inclusive: 145 bytes total, including
the three-byte `ret 8` at `0x0040a85e`. It has two stack arguments and callee
cleanup. Several callers set `ECX` to `0x005543f8`, while another passes a
local object pointer; the routine never reads `ECX`. The address is also passed
to unrelated path/file methods, so this helper does not establish a layout or
class identity for that receiver. The public helper is `WINAPI` to retain the
observed two-argument callee-cleanup ABI without asserting an object layout.

The first isolated comparison scored **96.83% raw**. The integrated build
reaches **100% raw** after recording the verified retail string mappings.
The initial differences involved static string operand mappings; the implementation
retains the observed API arguments, append order, conditional directory calls,
ignored results, and caller-provided unbounded destination behavior. The known
callers use a writable local path buffer and nonoverlapping literals, so the
`strcat` operations are behaviorally equivalent for the observed contract.

Observed callers in Application config setup use create=false and then append
`Saved Games` and `LR2.cfg`; a separate path caller requests directory
creation. This helper only builds the user-data base path. The surrounding
configuration lifecycle and caller-side path joins are separate work.

## Import library setup

The preserved VC6 tree lacks SHFOLDER.LIB. The Python builder accepts
`--shfolder-lib <path>` or `SHFOLDER_LIB`; on native Windows it also searches
SDK directories in LIB. In this Linux workspace it uses the installed Wine
PE-i386 `libshfolder.a`. CMake accepts `LR2_SHFOLDER_LIBRARY`, honors
SHFOLDER_LIB, or selects that archive through Wine's Z: drive. The Wine archive
is an LGPL Wine artifact, not a vendored Microsoft SDK library. No runtime
LoadLibrary replacement or older Shell API is substituted.
