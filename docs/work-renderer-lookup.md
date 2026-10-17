# Renderer lookup-table work

`RendererLookupTableState` is a 0x28-byte projection shared by three tables in
the renderer. The constructor at `0x004ab9e0` installs the vtable at `+0x04`
from `0x005358d4`, at `+0x2c` from `0x005358cc`, and at `+0x54` from
`0x005358c4`; each state object is initialized by the base constructor at
`0x004abdb0`, whose vtable is `0x005358dc` (two purecall entries). The two
tables covered by this batch have these confirmed slots:

| Vtable | Slot 0 | Slot 1 | Observed behavior |
| --- | --- | --- | --- |
| `0x005358d4` | `0x004bd770` | `0x004bd870` | Head-insert into hash buckets; traverse bucket chains in reverse bucket order for cleanup. |
| `0x005358cc` | `0x004bd7c0` | `0x004bd8c0` | Append into ordered bucket chains; traverse the order table and clear when flushed. |

`0x004bd770` is implemented in `rendererbuffer.cpp`. Its retail body returns
with `ret 0x10` and has no defined return-value contract; the recovered source
therefore uses `void`. It stores entry key, previous bucket head, the third
argument at entry `+0x08`, and the second argument at `+0x0c`, then replaces
the bucket head. Its bucket index is `p_hashKey >> m_bucketShift`.

`RendererLookupTableState::InsertOrdered` at `0x004bd7c0` is implemented in
`rendererlookup.cpp` and matches the retail body exactly. It checks `m_itemCount >= m_entryCapacity` and invokes
vtable slot `+4` on the same object before proceeding; retail does not recheck
capacity afterward. It uses `p_hashKey` directly as the bucket number, with
the bucket-head slot addressed at `m_bucketHeads[p_hashKey + 1]`. On a new
bucket it takes a two-word sentinel from the `m_bucketTails` allocation,
stores its address in the bucket-head array, and initializes the sentinel's
next pointer to null. Entries are then appended through the sentinel's first
word (head pointer), rather than prepended. The sentinel array is sized for
`bucketCount + 1` words by the existing initializer; this is why the apparent
`Entry*` sentinels are viewed at four-byte stride rather than `Entry` stride.

The cleanup slots are not complete. `0x004bd870` walks every bucket-head chain
from the last bucket down to the first. `0x004bd8c0` walks the sentinel chains
in order and then calls `Clear()`. Both pass every entry to `0x004bda20` with
the global renderer pointer at `0x00549dfc`. That callee is a 1,215-byte
render-state application switch, not a trivial release callback: it handles
entry kinds 0 through 5, edits material/render records, touches renderer
cache fields, and calls helpers at `0x004c06d0`, `0x004c6fe0`, `0x004c6980`,
`0x004a7510`, and `0x0043b860`, in addition to renderer dispatch slots
`+0x14`, `+0x2c`, `+0x3c`, `+0x4c`, `+0x58`, `+0x80`, `+0x124`, `+0x140`,
`+0x14c`, and `+0x190`. Those helpers and their record layouts are not yet
closed by this work, so no derived vtable is emitted and neither cleanup slot
is represented as a success placeholder.

Measured so far: `0x004bd7c0` is an exact 109-byte match. `0x004bd770` is
60.00% raw instruction accuracy after making its return type reflect the
observed no-result contract. The remaining differences are instruction
scheduling/register choices around loading and updating the bucket head.
Retail extents are 109 bytes for `0x004bd7c0`, 64 for `0x004bd870`, 61 for
`0x004bd8c0`, and 1,215 for `0x004bda20`.
