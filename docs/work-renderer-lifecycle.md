# Renderer owner and lifecycle evidence

This note records the static renderer object's constructor and destructor from
the retail PE disassembly in `data/retail-disasm.txt`. It does not claim a full
C++ layout: only the visible prefixes, offsets, and accesses below are proven.

## Static owner and extents

Startup wrapper `0x004ab450` places `0x0058b140` in ECX and tail-jumps to the
thiscall constructor at `0x004ab9e0`. The constructor returns that same pointer
in EAX. The wrapper at `0x004ab440` calls it, then transfers to
`0x004ab460`, which registers `0x004ab470` through the runtime atexit routine
at `0x00519f5e`.

The object is static storage at `0x0058b140`; the renderer initializer does
not allocate it. The constructor is 967 bytes, from `0x004ab9e0` through
`0x004abda6`. Its final repeated-field loop starts at `this+0xc7f0c`, writes
four 8-byte records at stride 8, and reaches byte `this+0xc7f2b`. This proves
a minimum extent of `0xc7f2c` bytes. That is a lower bound, not an exact class
size: neither the final member boundary nor the end of this static allocation
is independently established. The exclusive minimum endpoint,
`0x0058b140 + 0xc7f2c`, equals `0x0065306c`, where nearby code stores a
separate scalar float; this corroborates the boundary but does not prove the
renderer type's exact `sizeof`. `DisplayRenderer` now describes the observed
prefix through the hardware-policy flag at `+0xc7ec4`, with intervening unknown
regions. Its `0xc7ec8`-byte size assertion does not describe a constructible
full renderer object.

The constructor first writes a temporary vptr value `0x00535728`, then at
`0x004abd20` installs `0x00535554`. The final table begins with initializer
`0x004ac130` and destructor `0x004ad060`; the disassembly and `.rdata` bytes
agree on this order. The initializer is 3,013 bytes (`0x004ac130` through
`0x004accf4`). The destructor is 198 bytes (`0x004ad060` through
`0x004ad125`). The purpose of the temporary vptr is not established.

## What construction initializes

The constructor is not a small wrapper around the display initializer. It
initializes a large renderer object before display setup:

- It calls `0x004abdb0` for subobjects at offsets `+0x4` and `+0x2c`, then
  installs subobject vptrs at those locations.
- It calls `0x004abe50` 800 times over records beginning at `+0x38d8`, with
  stride `0xa0`.
- It initializes repeated data at `+0x24360`, `+0x245f0`, and the 8-byte
  entries at `+0x24b50`, `+0x24ba0`, and `+0x25100`. The latter groups use
  `0x004abe90` via the local element-initializer loop at `0x00401020`.
- It calls `0x004abeb0` 32 times over `0xc4`-byte records beginning at
  `+0x25148`.
- It calls `0x004abde0` 5,000 times over records beginning at `+0x28768`,
  stride `0x50`; initializes 128 records at `+0x8a1e8`, stride `0x24`; and
  calls `0x004abe30` 5,000 times at `+0x8b3e8`, stride `0x30`.
- It initializes more tables at `+0xc5d74`, `+0xc6290`, `+0xc70a4`,
  `+0xc7a30`, and through `+0xc7f2b`. The `+0xc7a30` table has four entries
  at stride `0x124` and installs nested vptr-like values. A later loop at
  `+0xc7f0c` writes four records at stride 8; its last byte is the maximum
  offset touched by the constructor.

Near the end, construction calls the application singleton at
`0x0049a760`, dispatches its virtual slot `+0x14`, and conditionally changes
three renderer fields at `+0xc7ecc`, `+0xc7ed0`, and `+0xc7ed4`. That behavior
cannot be reproduced by merely zeroing the renderer or by constructing the
known adapter prefix. The full effects and field meanings of the embedded
subobjects are not yet modeled here.

## Destruction and startup requirements

The destructor at `0x004ad060` is a thiscall with no explicit arguments. It
first calls `0x004ec2c0(this, 0)`, then walks both 5,000-entry arrays described
above and conditionally invokes each live element's first virtual method. It
dispatches a renderer virtual method for indexes 0 through 15, releases the
device at `this+0x7b0` and Direct3D object at `this+0x7ac` through COM `Release`
slot `+0x8`, and clears those pointers. It restores the renderer's floating
point control word through `0x004e7b40`. If `this+0x7f4` is 1, it calls the
function pointer at `0x0052f230` with 1. Finally, a non-null result object at
`0x0065307c` is returned to the memory heap through `0x004a41b0` and the global
is cleared.

The real startup order must therefore provide the constructed application
singleton and initialized heap before the renderer's constructor or any path
that consumes its full embedded state. The display initializer is a later
stage: it reads application display configuration, sets up renderer mode
state, and creates Direct3D/device resources. Shutdown must call the real
renderer destructor while the heap and COM objects remain valid, before their
owners are torn down.

## Recovered constructor dependency

`0x004abe90` is a 16-byte thiscall constructor helper that clears two dwords.
The renderer constructor calls it through the local element-initialization
loop `0x00401020` for arrays at parent offsets `+0x24b50` (8 entries),
`+0x24ba0` (0xac entries), and `+0x25100` (8 entries), each with stride 8.
`RendererZeroPair` in `rendererzeropair.h` models only those two observed
words, and its 8-byte size assertion is supported by the array strides. The
isolated VC6/Wine build measured this helper at 100% raw instruction accuracy.

`0x004abeb0` is a 60-byte thiscall constructor helper used for 32 entries
beginning at parent offset `+0x25148`, with stride `0xc4`. Each entry contains
four subrecords of stride `0x30`. For each subrecord, the helper writes zero
to `+0x20`, one to `+0x2c`, zero to `+0x28`, and all ones to the eight dwords
at `+0x00..+0x1c`; it leaves `+0x24` untouched. It then writes zero to the
group's final dword at `+0xc0`. `RendererSubrecordGroup` in
`rendererrecordgroup.h` records these observed offsets and caller strides;
the `+0x24` member is explicitly retained because construction preserves it.
The helper also measured at 100% raw instruction accuracy in the isolated
VC6/Wine build. Both implementations are in `rendererlifecycle.cpp`; the
worker build used normal runtime scaffolding and did not construct or execute
the renderer.

The adjacent helper `0x004abdb0` clears five selected dwords and writes vptr
`0x005358dc` at offset zero; the renderer constructor calls it at parent
offsets `+0x4` and `+0x2c`. The retail table starts with targets `0x004bd870`
and `0x004bd880`, but those targets and a matching rebuilt virtual table are
not recovered. The helper is therefore not implemented: embedding the retail
address in the rebuilt object would create an invalid vptr. The call-site
starts are 0x28 bytes apart, which does not by itself establish either
subobject's complete size.

The full constructor still cannot be represented safely by the current
`DisplayRenderer` class: its modeled layout ends at `+0xc7ec7`, while retail
construction initializes to `+0xc7f2b`, and the repeated subobject types and
their semantics are not recovered. No partial full-renderer constructor,
destructor, or synthetic success path was added; only the complete
`0x004abe90` zero-pair and `0x004abeb0` subrecord-group helpers are
implemented. Many other embedded record types/helpers remain unrecovered.
The application singleton and its development-mode dispatch used by this
constructor are now wired. The remaining renderer dependencies must be
recovered before full construction and teardown
can be implemented without opaque offset arithmetic or skipped cleanup.
