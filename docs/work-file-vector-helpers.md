# File and vector helper evidence

The implementations in `winfile.cpp`, `winfileutil.cpp`, and `vector3.cpp`
were recovered from the retail executable and compiled with the verified SP5
toolchain. Ghidra output was used as a reference, with signatures, branches,
and function endings checked against disassembly.

`WinFile` recovers a handle at offset zero. Its total size and the surrounding
file abstraction are still unknown, so the header does not assert a complete
object size. `ReadExact` at `0x004a4ac0` takes four stack arguments: the last
two are unused, but its `ret 0x10` requires preserving their slots. It succeeds
only when `ReadFile` succeeds and reports exactly the requested byte count.
`WriteExact` at `0x004a4b00` has the analogous behavior with two arguments and
`ret 0x08`. Both functions are 50 bytes, including the final three-byte `ret`.
A local byte-count variable produces the exact read-wrapper match; reusing
the count argument for API output produced a different register schedule.
`GetSize` maps `INVALID_FILE_SIZE` to zero. `Close` skips
`INVALID_HANDLE_VALUE` and always resets the stored handle to that value.

The file-manager wrappers at `0x004a2c20` and `0x004a2c40` establish a cached
size at `WinFile` offset `0x04`. A value of `0xffffffff` selects a physical file:
the close wrapper calls `Close`, and the size wrapper calls `GetSize`. Other
cached sizes are returned directly and suppress physical-file closing.

`CreateAlways` and `OpenOrCreate` request read/write access, no sharing, and
normal file attributes. Both retry `ERROR_LOCK_VIOLATION` after a 500 ms sleep;
other failures stop the retry. They differ in their creation disposition.
`SeekEnd` and `SeekCurrent` map a `SetFilePointer` result of `0xffffffff` to
zero. `OpenAppend` opens or creates the file, then seeks to its end when the
returned handle is non-null. It reports successful opening even if seeking
fails, preserving the retail behavior.

`PathExists` at `0x004a4b80` uses `FindFirstFile` and closes a successful search
handle. Its behavior includes directories and wildcard patterns supported by
that API; it is not restricted to ordinary files. Its 55-byte boundary includes
both return paths through the `ret` at `0x004a4bb6`.

`Vector3` contains three consecutive floats. The caller at `0x0044a6d2` uses
an output at object offset `0x46514`, then `0x0044a6e8` selects the adjacent
output at `0x46520` for the next call. A three-float stack temporary is passed
to normalization at `0x0043c1ce`. Together these support the 12-byte layout.

`Normalize` computes the length, divides the components by it, and returns
the original length. The original x87 equality test also takes the early zero
return for an unordered comparison, leaving the vector unchanged. Blending at
`0x004a4ce0` stores the first weighted vector before adding the second, so its
operation order matters when an input aliases the output. The read-only float
at `0x0052f2f0` is positive zero; it is seeded in `reccmp/floats.csv` as a literal
pool entry rather than annotated as a game global.

## Drive classification

`GetPathDriveKind` at `0x004a4bc0` copies the first three path bytes into a
four-byte NUL-terminated local root and calls `GetDriveTypeA`. Its 125-byte
body includes every switch return through `0x004a4c3c`; a five-entry jump
table follows at `0x004a4c40`. The return mapping is removable=0, fixed=1,
remote=2, RAM disk=3, CD-ROM=4, and any other Win32 drive result=5. The caller
at `0x0040a734` explicitly compares the result with 4, corroborating CD-ROM
use. The helper retains the retail three-byte copy without adding path checks.

The initial reconstruction remains STUB at 35.71%. Retail retains an EBP
frame and emits a generic inline copy loop; the default build eliminates the
frame and uses word/byte copies. No frame-forcing source or optimization
pragmas were introduced. The pinned-CRT candidate scan has no match for this
wrapper, so its original engine/library ownership is unestablished.
