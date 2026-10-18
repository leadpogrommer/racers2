# Global overlay manager

The global overlay owner is a `0x24950`-byte object at retail VA `0x655228`.
Its record array begins at `+0x711c` with 200 records of `0x254` bytes. The
compact object-list nodes begin at `+0x6778`, and their 300 corresponding
`0x58`-byte records occupy `+0x58..+0x6777`. The record list nodes begin at
`+0x242bc`. The band heads are at `+0x70dc` and `+0x24900` respectively.

The first head offset is supported by both the constructor and render pass:
the render pass forms `this + 0x24900 + band * 4`, then loads the other head
from that address minus `0x1d824`; the resulting field is `+0x70dc`. The
constructor initializes that array and the compact list sentinel separately.
`+0x24944` and `+0x2494c` are members of this same owner; their retail VAs are
`0x679b6c` and `0x679b74`. They control overlay scaling and dynamic glyph
lookup. The separate render-object pointer at `0x679ef0` is defined by the
renderer lookup unit.

The record dispatch table at `0x535ca0` contains three overlay callbacks
followed by the manager's 20-entry `GameObject` dispatch view at `0x535cac`.
The constructor installs these pointers into the global owner and each pooled
record. Table targets and callback stack cleanup were checked against the
retail PE. The implementation remains annotated `STUB` while dynamic glyph
callbacks and exact draw-packet effects remain unresolved.

| Retail address | Length | Working behavior |
| --- | ---: | --- |
| `0x4b2d30` | 10 | initialize the owner and register process-exit cleanup |
| `0x4b2d40` | 92 | build the overlay `GameObjectTemplate` and construct the owner |
| `0x4b2da0` | 12 | register owner cleanup with the CRT |
| `0x4b2db0` | 34 | reset overlay record dispatch pointers and destroy the base object |
| `0x4b30c0` | 603 | initialize the manager, record pool and both linked-list arrays |
| `0x4b3330` | 57 | reset record dispatch pointers, destroy, optionally free |
| `0x4b3370` | 52 | forward the frame parameter to active record callbacks |
| `0x4b3540` | 827 | iterate overlay and text records by band and draw visible records |
| `0x4b3fd0` | 1009 | draw a text record, including spacing, wrapping and animation branches |
| `0x4b43d0` | 259 | advance an animated text record |
| `0x4b44e0` | 391 | measure the text record's horizontal advance |

The function lengths above include the return instruction and exclude padding.
The render and text callbacks still need review against all animation and
dynamic-font paths before any `FUNCTION` promotion.
