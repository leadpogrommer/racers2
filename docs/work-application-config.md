# Application display configuration

The reconstructed methods in `application.cpp` follow retail config loading
at `0x0049b5d0` and saving at `0x0049b780`. The file format is a little-endian
16-byte record: DWORD marker `0xc0f1c0f1`, then width, height and bit depth.
The per-user base comes from the real SHFOLDER helper, with
`\Saved Games\LR2.cfg` appended through the observed path-string globals.

Load resets the display settings to 640 × 480 × 16, sets file-manager flag 1
to request native-file access, checks existence, opens the file and accepts
only a 16-byte file with the marker. Before reading it initializes local
settings to 640 × 480 × 32, preserving the retail defaults and unchecked read
result. It explicitly closes the file and the destructor closes it again,
as in retail. Flag 1 is cleared at the end.

Save sets the same flag, creates the Saved Games directory, and builds the
filename. On first use it sets guard bit 0, initializes the static record's
settings and registers a no-op exit callback. It writes the marker and current
settings, opens with create-always mode, writes 16 bytes, explicitly closes,
clears flag 1, then lets the local WinFile destruct. Errors are not converted
to success or given new fallback behavior.

`FileManager::Exists` at `0x004a2c60` uses the archive list for mode >=2
when flag 1 is clear. It uppercases a zero-padded 128-byte path and compares
all 128 bytes, including bytes after the first NUL, against each 0x88-byte
entry. Other cases use the native PathExists helper. `ManagedPathExists`
at `0x004a2300` forwards through the manager singleton.

An independent instruction/control-flow review found no concrete semantic
mismatch in those load/save/existence paths. Integrated raw scores are:

| Address | Function | Bytes | Raw | Annotation |
| --- | --- | ---: | ---: | --- |
| `0x0040ec90` | Display-settings pointer | 4 | 100.00% | FUNCTION |
| `0x0049a6a0` | Unknown field getter | 4 | 100.00% | FUNCTION |
| `0x0049b5d0` | Load display config | 425 | 66.67% | STUB |
| `0x0049b780` | Save display config | 428 | 94.48% | STUB |
| `0x004a2300` | Managed path existence facade | 18 | 100.00% | FUNCTION |
| `0x004a2c60` | Manager path existence | 188 | 62.30% | STUB |

The save method is retained after review and bounded measurement. Its raw
score is below the initial 95% promotion threshold, so the annotation stays
STUB. Load and existence also remain below threshold. No game behavioral
tests or instruction-scheduling polishing were added.

The Application declaration models only the observed prefix: explicit vptr
at +0, three display DWORDs at +4 and unknown DWORD at +0x10. It asserts those
offsets, not an unproven complete class size. The runtime now constructs the
singleton and uses explicit complete 38-slot ABI tables; the dispatch pointer
checker verifies all 76 emitted entries. This is binary wiring verification,
not a method-body or source-level vtable equivalence claim. See
[application runtime](work-application-runtime.md).

The compiler/linker genuinely folds the no-op callbacks at retail
`0x004a3f20` and `0x0049b930` into one rebuilt RET. This reccmp version pairs
one original address per rebuilt address. The existing heap callback retains
its exact mapping through the emitted config-callback symbol; its FOLDED
name annotation is in `memoryheap.h`. The save callback's one-byte original
extent is inventory-only and is not counted as a second measured match.
Both callback bodies remain unchanged.

Configuration methods have been compiled and compared. A full startup run
confirmed access to the per-user config path before archive initialization;
the run did not demonstrate config saving, dialog behavior, display creation,
or gameplay. The entry point remains temporary.
