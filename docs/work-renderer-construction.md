# DisplayRenderer construction and dispatch

The static renderer is raw storage at `0x0058b140`, with its selector at
`0x00549dfc` pointing to the same object. Retail construction is entered by
the helper at `0x004ab450`, which loads `0x0058b140` into ECX and tail-jumps to
`0x004ab9e0`. That constructor returns the receiver in EAX and ends at
`0x004abda6`, so its complete extent is 967 bytes inclusive. The adjacent
`0x004ab440` initializer calls that helper and then registers the cleanup
routine at `0x004ab470` through the CRT `atexit`; the constructor itself does not
register cleanup.

Retail first installs a 103-entry base dispatch at `0x00535728`, then
initializes the object and replaces the pointer with the 103-entry derived
dispatch at `0x00535554` (`0x00535554..0x005356ef`). Both tables have indices
0 through 102, with the final callback at byte offset `+0x198`. The next word
at `0x005356f0` begins the independently installed animation-resource table;
it is not a renderer slot. The base table ends at `0x005358c3`; the next word
at `0x005358c4` begins a separate lookup-table dispatch. The constructor also
initializes three lookup subobjects at `+0x04`, `+0x2c` and `+0x54`, selecting
their tables at `0x005358d4`, `0x005358cc` and `0x005358c4`.
The abstract lookup table at `0x005358dc` consists of two `_purecall`
callbacks.

The constructor body and its nested constructors establish an object prefix
through `+0xc7f2b`; this is the evidence behind the current `0xc7f2c` storage
size. It is not proof that the full source class ends there. The nested
constructor calls initialize 800 texture records, 32 subrecord groups and
256 array markers, the default records, 5000 vertex-buffer records and 5000
index-buffer records. The body initializes the scalar state, 16 viewport
in-use words, 128 vertex-storage records, 64 index-storage records, 32 light
device indices, 10 transform flags, the four renderer-default slots, the
800-entry texture-index map and 16 texture address modes. It reads the
application development-mode virtual at slot `+0x14`; only a true result sets
`m_createDeviceBuffers`, `m_unk0xc7ecc`, and `m_bufferStoragePolicy` to one.

The derived dispatch entries were extracted directly from the supplied retail
PE at `0x00535554`. The sparse `RendererDispatchView` currently used for calls
is only a projection of that table. The complete source dispatch must retain
each entry's receiver/argument ABI and the real callbacks; unresolved target
bodies are listed in the root worker's checkpoint and are not represented by
invented success/no-op callbacks.

No renderer startup or game execution was used to make these findings.

## Animation-resource records

The constructor also initializes four records at `this+0xc7a30`, each with a
`0x124` stride. The primary prefix is a vtable pointer, signed resource ID at
`+0x04`, flags at `+0x06`, and a 128-byte path beginning at `+0x08`; the
remaining bytes through `+0x11f` stay opaque, and a DWORD at `+0x120` is
separate. The record lookup at `0x004ea460` compares those paths and flags,
marks the first free record with flags `|= 3`, copies the path, and assigns an
empty-path texture resource ID. When requested it invokes dispatch slot `+4`.

The constructor briefly installs the five-entry abstract callback table at
`0x0053570c`, preserves flag bits `0xc008` while setting type value 2, then
replaces the vptr with the seven-entry table at `0x005356f0`. Its entries are
the purecall handler, a true-returning callback (`0x00486a70`), a no-op
(`0x00479030`), a resource-reference release callback (`0x004ea440`), the
shared true policy (`0x00403920`), the diagnostic callback (`0x00403930`), and
the development-mode policy (`0x00403910`). The constructor's four-record
loop does not write `+0x120`; that DWORD remains opaque in the source layout.

The constructor source now preserves the exact partial writes at `+0xc7a28`
(WORD), clears the full 344-DWORD timing history, initializes light flags
without prematurely zeroing each device index, and sets light direction to
`(0,-1,0)`. These are field-write facts from the retail constructor, not a
claim that all untouched bytes are initialized.


## Combined integration

The constructor and full dispatch sources are now linked into both comparison builds. `check_renderer_main_dispatch.py` verifies 206 base/derived pointers and six lookup pointers (212 total). Constructor similarity remains 12.01%; compilation and table wiring do not establish runtime correctness. The singleton remains unconstructed until its startup dependencies are ready.
