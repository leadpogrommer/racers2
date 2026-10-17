# Renderer texture-stage plan work

## Evidence and current implementation

`RendererTextureStageManager` is embedded at `DisplayRenderer+0x25148` and its
stage-plan storage is backed by 32 `RendererSubrecordGroup` records with a
`0xc4` stride. The manager cache at `+0x2c80` is a byte array of `0x800`
entries: retail `0x4e24c0` and `0x4e27c0` use byte loads/stores indexed by the
key, and initialization fills those bytes with `0xff`. Each cache byte is a
group index; `0xff` means no group has been allocated for that key.

The methods added to `RendererTextureStageManager` recover three helpers:

- `MaskWithoutHighestPriorityStage` implements `0x4e1c30`. It selects a
  present stage with the greatest configured priority (ties select the later
  stage), then clears that bit from the input mask. With no set bits it still
  clears bit zero, which does not change the zero mask.
- `FindSubrecordGroup` implements `0x4e24c0`. It returns null for a `0xff`
  cache entry and otherwise returns the group at that byte index.
- `CreateSubrecordGroup` implements `0x4e27c0`. It stores the current group
  count into the key's byte, initializes that group, copies the current and
  related counts into the observed snapshots at `+0x3488/+0x348c`, increments
  the group count, and returns the new group.

Isolated raw scores are 96.97% for `MaskWithoutHighestPriorityStage`, 75.00%
for `FindSubrecordGroup`, and 25.64% for `CreateSubrecordGroup`. The priority
helper is marked `FUNCTION`; reccmp reports an effective behavioral match and
the only difference is loop/register scheduling. The two other measured
helpers remain `STUB` because their code-generation gaps are substantial.

The request projection records the observed 0x3c-byte packet prefix. Its
embedded `RendererDefaultRecord` begins at `+0x14`: the first DWORD is a bit
mask, the following WORD is an entry count, and its four 8-byte entries start
at `+0x1c`. The packet's initial words, heap pointer, and two callback-context
pointers remain unnamed beyond their offsets. `CopyTextureStageDefaultSlot`
and `RemoveStageFromOperationPlan` recover the copy and removal operations
used on those entries. Removal shifts later 8-byte entries left, decrements
the WORD count, leaves the vacated tail untouched, and clears the stage bit
even when the entry was not found. Their raw scores are 15.38% and 25.88%;
both remain `STUB` despite the recovered effects.

`CopySubrecordGroupCacheEntry` models the final plan-commit helper `0x4e6360`:
it copies the byte at the source key to the destination key, preserving the
same group index under the original request mask after a reduced plan succeeds.
The source/destination direction follows the resolver's call order. Its extent
is 25 bytes (`0x4e6360..0x4e6378`).

## Remaining resolver closure

The central resolver is `0x4e24f0`, a DisplayRenderer member receiving one
packet pointer and returning with `ret 4`; its complete range is
`0x4e24f0..0x4e2726` (567 bytes). It initializes per-slot callback data,
checks the manager cache, filters plan entries when the device lacks the
separate-texture-memory capability, then repeatedly tries plan builders
`0x4e2850` and `0x4e4fd0`. The final success/update path is `0x4e6360`.
When neither builder succeeds, it removes unsupported stage entries using
`0x4e1c30` and `0x4e2750`, retries, and returns without committing if the
plan stops changing.

The two builders are not leaf routines: `0x4e2850` spans 9,639 bytes and
`0x4e4fd0` spans 3,509 bytes. Their switches construct cached stage groups
for many operation/profile combinations and call additional packet setup
helpers. They remain the closure blockers; a partial switch would silently
drop supported texture operations and is not included here. The new unit
therefore contains only complete helpers and the evidenced packet layout,
not a resolver that pretends those branches succeed.

The caller's resolved initializer tail invokes the resolver through the
renderer dispatch contract with `(this, packet)` and callee cleanup of four
bytes. Its shared dispatch-view slot is not yet wired to this request type.
The builder call graph and packet callback slots remain under review; their
target signatures should be confirmed from the remaining caller and vtable
evidence before adding them.

## Combined checkpoint measurements

Both builders agree on the closed helpers. `MaskWithoutHighestPriorityStage`
is a reviewed `FUNCTION` at 96.97% raw: the only difference is the order of
independent register-zeroing instructions. The unsigned priority comparisons,
later-entry tie rule, and no-matching-bit behavior are unchanged.
`CopyTextureStageDefaultSlot` is an assignment-like operation returning the
destination pointer in EAX; its corrected signature and natural struct copy
match all 19 bytes exactly. `CopySubrecordGroupCacheEntry` copies the source
key's cache byte to the destination key and matches all 25 bytes exactly.
Both are `FUNCTION`. `FindSubrecordGroup` remains STUB at 75%, removal at
25.88%, and group creation at 25.64%. No runtime was run for this checkpoint.
