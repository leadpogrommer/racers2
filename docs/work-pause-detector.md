# Pause detector

`PauseDetector` is a `GameObject`-sized startup object (allocation size `0x58`).
The constructor at `0x00407f00` calls the base constructor with type `0x900`
and installs the base-compatible table at `0x0052f63c`. Its caller at
`0x004073c2` replaces that pointer with the final table at `0x0052f5ec`.
Both tables contain 20 entries; the final table changes only slot `+0x18` to
the pause-detector update callback at `0x0040b4e0`.

The update callback first suppresses work for session state `2`, the global
simulation-scheduler gate, or an active global-pause deadline. It then checks
the state-specific pause conditions for game states `3` through `6`, applies
the independent global suppressors, and optionally searches the session's
object list for an input-target object. The secondary-manager media predicate
can select the manager itself; otherwise the selected session object is used.
The callback requests pause by setting the global controller's enter-request
word and storing the selected object in its `+0x64` field.

The helper at `0x0044a520` returns the input-record address at object `+0xb4`,
or `+0xc8` when its selector is zero and the object's `+0x68` mode is neither
`1` nor `3`. The pause-request helper at `0x004bb760` writes the two controller
fields described above. The `0x0042b300` predicate refreshes media slot `2`
when enabled, then returns bit `0x40` from the byte at media-state `+0x28`.

The callback uses the existing `GameController` and `GameControllerSession`
layouts for their verified members. Small local offset views cover only the
manager and target fields that are read; they do not assert complete class
layouts. The otherwise unowned suppression word at `0x00572f94` is in the
zero-filled virtual tail of the executable's `.data` section and is modeled as
a zero-initialized `LegoU32`. Its semantic name remains unknown.

The combined 294-unit build now links these dependencies. RequestPauseForObject
matches exactly; the larger callback and selection logic remain STUB. Both
20-slot tables pass the complete scene-dispatch check. See
[combined verification](work-controller-recording.md). No game was executed.
