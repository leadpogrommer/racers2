# Small audio and physics matches

## Exact promotions

The following functions were reviewed against the full retail body and now
compare at 100% raw instruction accuracy in the isolated SP5 build:

| Address | Function | Evidence |
| --- | --- | --- |
| `0x004b0370` | `DeleteBaseAudioSourceRecord` | Selects the base dispatch, conditionally frees the receiver, and returns it. |
| `0x004b0550` | `GetAudioSourceRecordIndex` | Reads the record index at `+0x28`. |
| `0x004b0560` | `SetAudioSourceRecordValues0x10` | Writes three words at `+0x10`, `+0x14`, and `+0x18`; caller uses the matching three-word dispatch slot. |
| `0x004b0580` | `SetAudioSourceRecordValues0x04` | Writes three words at `+0x04`, `+0x08`, and `+0x0c`; caller uses the matching three-word dispatch slot. |
| `0x004b05a0` | `SetAudioSourceRecordValues0x1c` | Writes three words at `+0x1c`, `+0x20`, and `+0x24`; caller uses the matching three-word dispatch slot. |
| `0x004b05c0` | `SetAudioSourceRecordFalloff` | Stores the final floating-point argument at `+0x8c`; the preceding word is unused by the retail body. |
| `0x004b05d0` | `SetAudioSourceRecordFlagsLow` | Preserves the upper 28 bits at `+0xb8` and replaces its low nibble. |
| `0x004b05f0` | `SetAudioSourceRecordFlagsHigh` | Preserves the low nibble at `+0xb8` and ORs in the supplied high flags. |
| `0x004b0610` | `SetAudioSourceRecordValues0x90` | Writes two words at `+0x90` and `+0x94`. |
| `0x004b0630` | `SetAudioSourceRecordValues0x98` | Writes two words at `+0x98` and `+0x9c`. |
| `0x004b0650` | `SetAudioSourceRecordValues0xa0` | Writes two words at `+0xa0` and `+0xa4`. |
| `0x004b0750` | `ResetAudioSourcePlaybackState` | Clears `+0x34`, sets flag `0x10` in `+0xb8`, and sets `+0x30` and `+0x3c` to one. |
| `0x004b0780` | `ClearAudioSourceFlag` | Clears flag `0x10` in `+0xb8`. |
| `0x004b0790` | `SetAudioSourceEnabled` | Sets or clears flag `0x100` in `+0xb8` according to the argument. |
| `0x004e6730` | `ContactCacheAllocationSize` | Returns the controller allocation size, `0x665c`. |
| `0x004fbe50` | `ContactCacheController::ResetContacts` | Clears the contact count at `+0x6658`. |
| `0x004fb680` | `SpatialIndexLink::Destroy` | Tail-calls `Detach`, which removes the link from its current spatial index and clears its cached references. |

The audio record dispatch table supplies the method signatures and groups
the one-word, two-word, three-word, and float setters. The audio record layout
and retail loads/stores establish the field offsets. The spatial link's full
detach routine was reviewed to confirm the tail-call behavior.

All 17 functions above were exact in the generated report.

## Additional exact promotions

The following functions were also promoted after reviewing their complete
retail bodies and call contracts. The getters read the service's quality and
volume fields into their output arguments. The listener setters look up the ID
and update only the corresponding listener field when found. The collision
service destructor restores its abstract dispatch and conditionally frees the
object; its callbacks forward to the proxy methods with the retail argument
layout. The spatial-index methods update or remove a link through the static
grid and registry.

| Address | Function | Raw instruction accuracy |
| --- | --- | ---: |
| `0x004af6c0` | `AudioService::GetQualityMode` | 100% |
| `0x004af790` | `AudioService::GetVolumes` | 100% |
| `0x004b0ca0` | `AudioService::SetListenerField0x04` | 100% |
| `0x004b0cc0` | `AudioService::SetListenerField0x30` | 100% |
| `0x004b96d0` | `DestroyCollisionServiceObject` | 100% |
| `0x004f4c80` | `CollisionServiceSetPrimitivePosition` | 100% |
| `0x004f4ca0` | `CollisionServiceSetTransform` | 100% |
| `0x004f4e00` | `AttachCollisionProxyFromService` | 100% |
| `0x004f52d0` | `CollisionSpatialIndex::RemoveStatic` | 100% |
| `0x004f5330` | `CollisionSpatialIndex::UpdateStatic` | 100% |

## Further exact promotions

These functions were reviewed from entry through their final return. Audio
playback helpers preserve the retail null checks, DirectSound calls, and status
conversion. The music methods retain the unconditional file-position update
after `SetFilePointer` and the constant successful return from `StopAndReset`.
The spatial-index additions refresh bounds before insertion, choose the static
or dynamic index from the filter, and preserve the existing dynamic handle
removal order. The proxy destructor destroys child proxies in reverse order,
then frees the child array and detaches the index link. Primitive position
updates retain the retail inclusive upper bound.

| Address | Function | Raw instruction accuracy |
| --- | --- | ---: |
| `0x004f0690` | `ReleaseDirectSoundOutputInterfaces` | 100% |
| `0x004f0e30` | `MonoDirectSoundProvider::ReleasePlaybackObject` | 100% |
| `0x004f0f20` | `IsAudioPlaybackObjectActive` | 100% |
| `0x004f0fb0` | `ResetAudioPlaybackPosition` | 100% |
| `0x004f19b0` | `DigitalMusicController::StopAndReset` | 100% |
| `0x004f20c0` | `DigitalMusicController::SkipFileBytes` | 100% |
| `0x004f1240` | `CopyAudioSpatialWorldPosition` | 100% |
| `0x004b0bd0` | `ReleaseAudioChannelRecordBuffer` | 100% |
| `0x004d7fa0` | `CollisionProxy::~CollisionProxy` | 100% |
| `0x004d8300` | `CollisionProxy::SetPrimitivePosition` | 100% |
| `0x004fb710` | `SpatialIndexLink::Attach` | 100% |
| `0x004f5260` | `CollisionSpatialIndex::InsertStatic` | 100% |
| `0x004f5290` | `CollisionSpatialIndex::InsertDynamic` | 100% |
| `0x004f52f0` | `CollisionSpatialIndex::RemoveDynamic` | 100% |

## Additional service and playback matches

The collision-service cleanup walks the registry while saving each next link
before detaching the current proxy. Query preparation resets the shared result
count before preparing the static and dynamic structures. Proxy destruction
releases its object and clears the caller's pointer. The spatial link update
refreshes bounds and dispatches to the index selected by its filter. Playback
helpers preserve the DirectSound status checks, loop flag normalization, and
the controller's wait for in-progress buffer work before releasing it.

| Address | Function | Raw instruction accuracy |
| --- | --- | ---: |
| `0x004f4c50` | `CollisionServiceDetachAll` | 100% |
| `0x004f5370` | `PrepareCollisionSpatialQueries` | 100% |
| `0x004f4dc0` | `DestroyCollisionProxyFromService` | 100% |
| `0x004fb7a0` | `SpatialIndexLink::Update` | 100% |
| `0x004efe40` | `StopAudioPlaybackObject` | 100% |
| `0x004f1bc0` | `DigitalMusicController::StopAndReleasePlaybackBuffer` | 100% |
| `0x004f0ed0` | `StartAudioPlaybackObject` | 100% |

## Dispatch and audio-setting follow-up

The audio-mode accessor reads the global context's mode at `+0x88` and falls
back to the initialized `g_reverbDefaultState` at `0x0054225c`, matching both
branches in the retail body. The three spatial registry/axis constructors now
reference their verified two-callback dispatch tables as named globals. Their
only previous diffs were immediate retail table addresses versus those data
symbols; each table's eight-byte extent and callback order were checked against
the PE and linker map.

| Address | Function | Raw instruction accuracy |
| --- | --- | ---: |
| `0x0043dcd0` | `GetConfiguredAudioServiceMode` | 100% |
| `0x004f5140` | `SpatialLinkRegistry::SpatialLinkRegistry` | 100% |
| `0x00507be0` | `SpatialAxisIndex::InitializeFirst` | 100% |
| `0x00507cb0` | `SpatialAxisIndex::InitializeSecond` | 100% |

| `0x004efab0` | `ThreeDimensionalDirectSoundProvider::ReleaseDeviceInterfaces` | 100% |

This provider releases the four optional 3D interfaces and primary buffer only
when a primary buffer exists, then releases the device interface separately.
The conditional release order and pointer resets match the full retail body.

`0x004b0390` (`DeleteAudioSourceRecord`) is a reviewed 95.65% raw near match.
The only instruction difference is a compiler-added `xor edx, edx` before the
call to `DisposeAudioSourceRecord`. The callee ignores its second fastcall
register and operates only on the receiver, so clearing that unused register
does not change the destructor or conditional-free behavior.

Four more near matches were reviewed and promoted. `CollisionProxy` stores its
child count as a signed `LegoS32`; its attach/detach loops use the same
ascending child order and recursive calls as retail. The compiler adds a
separate zero check before its signed `jle` loop guard, which is redundant.
`CanStopStereoAudioBuffer` adds a zero to the unused second fastcall register
before querying playback state. `DynamicSpatialIndex::CollectCandidates` adds
the same unused-register clear before its two axis calls; the bounds, tag-array
offsets, remaining capacity, and intersection range match the retail sequence.

| Address | Function | Raw instruction accuracy |
| --- | --- | ---: |
| `0x004d8110` | `CollisionProxy::DetachFromIndex` | 95.00% |
| `0x004d8150` | `CollisionProxy::AttachToIndex` | 95.00% |
| `0x004f06f0` | `CanStopStereoAudioBuffer` | 97.87% |
| `0x00507e10` | `DynamicSpatialIndex::CollectCandidates` | 97.06% |

`0x0043d750` (`ReverbManager::Reset`) is a reviewed 95.00% raw match and
100% effective match. The compiler uses a two-iteration loop for clearing
the selected volume and camera pointers; retail writes the same four nulls
directly. The remaining instruction difference moves the `m_dirty = 1` store
across the second loop store. No call or aliasing access intervenes, so the
visible state after return is identical.

`AudioService::SetDigitalAudioPath` now takes the ignored second stack word in
its method signature. Its dispatch wrapper forwards that word, and the only
caller found passes `1`. This matches the retail `ret 8` cleanup while leaving
the word unused in the method body. Its final raw score is 100%.

`GetWaveSampleRate` now tests `(LegoS32) sampleByteCount > 0`, preserving the
retail signed-positive check (`jle`) for high-bit byte counts. Before this
correction, the unsigned nonzero check differed by one branch instruction.
Its final raw score is 100%.

| Address | Function | Raw accuracy after final source corrections |
| --- | --- | ---: |
| `0x004ef360` | `AudioService::SetDigitalAudioPath` | 100% |
| `0x004efdd0` | `GetWaveSampleRate` | 100% |
| `0x005088f0` | `StaticSpatialGrid::IntersectSegment` | 97.44% (100% effective) |
| `0x00508970` | `VisitCollisionSegmentCell` | 98.00% |

## Final integration checkpoint — paused

The Python and CMake/NMake builds both pass the final reccmp guard: **833 FUNCTION
claims, 784 exact and 49 reviewed near matches**, with identical 2,721 measured
scores/statuses. All 28 complete folded aliases and both builders' complete dispatch
and input-format inventories pass (including 1,321 scene pointers). Earlier pending
build notes above are superseded by this checkpoint. No game was run.

GetConfiguredAudioServiceMode, both axis initializers and the registry constructor
measure 100%. The newly recovered GetNodeAtIndex helper (15 bytes at 0x00507510)
and its GetNodeValue caller both measure 100%. The audio-path ABI and signed wave
count corrections also measure 100%. RemoveAudioSource now calls the original
resource-release helper; its final 82.05% raw score remains STUB. Matching work is
paused at the user's request.
