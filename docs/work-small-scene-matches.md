# Small scene/script matches

Validated in `build/workers/small-scene` with the pinned VC6 SP5 compiler and
`tools/compare.py --address`. Each entry below is an exact 100% raw instruction
match. The caller contract and source operation agree with the retail body.

| Address | Name | Bytes | Review |
| --- | --- | ---: | --- |
| `0x0045b3c0` | `HasFoyerSceneTargetTrack` | 14 | Base target dispatch predicate; reads the track pointer at `+0x80` and returns whether it is non-null. |
| `0x0045b4a0` | `RemoveFoyerSceneAudioTarget` | 26 | Object removal callback; runs the base remove operation, then frees the optional spline context at `+0x7c`. |
| `0x0045b5c0` | `GetFoyerSceneTargetSpline` | 4 | Dispatch getter returns the spline context at `+0x7c`. |
| `0x0045b850` | `InitializeStaticFoyerSceneAudioTarget` | 10 | Startup wrapper constructs the static target, then registers its exit cleanup. |
| `0x0045b860` | `ConstructStaticFoyerSceneAudioTarget` | 10 | Forwards the static storage receiver to the default target constructor. Its unused EDX placeholder was removed from the internal helper signature so the generated tail jump preserves the retail register contract. |
| `0x0045b870` | `RegisterStaticFoyerSceneAudioTargetExit` | 12 | Registers the target destructor with `atexit`; retail pushes the destructor, calls the CRT registration routine, and returns. |
| `0x0045b880` | `DestroyStaticFoyerSceneAudioTarget` | 10 | Calls the base `GameObject` destructor for the static target. The derived target has no destructor work of its own. |
| `0x0045c630` | `InitializeFoyerSceneActionDefaults` | 10 | Tail-forwards the defaults array to the record initializer. Removing the unused EDX placeholder from that helper and updating both callers preserves the retail one-register call contract. |
| `0x0045cbf0` | `ProgressCurrentValue` | 6 | Progress callback returns the scheduled action's output field. |
| `0x0045cdb0` | `ProgressElapsed` | 6 | Progress callback returns the action value stored at `+0x28`. |
| `0x0045d270` | `GetFinalSceneActionState` | 8 | Scheduled action dispatch callback returns terminal state `2`. |
| `0x0045dc40` | `BindFoyerActionPath` | 18 | Stores the path in the action context and invokes the path initialization method. |
| `0x0047c930` | `ResetFoyerSceneEventResetTarget` | 8 | Loads the reset-object pointer at `+0x7c` and tail-jumps to its reset routine. The callee's unused EDX placeholder was removed from the internal signature so the compiler emits the retail tail jump unchanged. |
| `0x00480300` | `FindFoyerActionAtOrAfter` | 56 | Scans the ordered action list and returns the first index whose action time is at least the requested start, or `-1`. Both callers use that index to shift or scale the trailing action sequence. |
| `0x00480db0` | `ScaleFoyerSceneAction14` | 13 | Action 14 scale callback divides the float at `+0x20` by the supplied scale. |
| `0x0047f320` | `FoyerSceneResourceExitCallback` | 1 | Empty exit callback; exact `ret` body is folded at rebuilt symbol `?FoyerSceneResourceExitCallback@@YAXXZ` (`0x00417730`). |
| `0x0049a640` | `AcceptFoyerSceneTargetPosition` | 8 | Always accepts the position; exact `mov eax,1; ret 4` body is folded at rebuilt symbol `?AcceptFoyerSceneTargetPosition@@YIIPAUGameObject@@PAXPBVVector3@@@Z` (`0x0045e620`). This original address is shared by several retail dispatch aliases. |
| `0x004c8150` | `GetFoyerTemplateObjectCategory` | 6 | Explicit template callback returns category `2`; retail and rebuilt bodies are `mov eax,2; ret`. The helper is referenced by both template and character lifecycle tables. |
| `0x0047e140` | `InitializeControllerSceneAngularFrequency` | 15 | Loads the verified `pi` constant at `0x005331e4`, doubles it, and stores the result in the angular-frequency global at `0x00572bec`. |
| `0x00478f80` | `SecondaryFxManager::~SecondaryFxManager` | 11 | Writes the explicit manager dispatch table at `0x00533024` into `this`, then tail-calls `GameObject::~GameObject`. |
| `0x0047e350` | `DestructControllerSceneObject` | 11 | Writes the explicit controller-scene dispatch table at `0x005331f0` into `this`, then tail-calls `GameObject::~GameObject`. |
| `0x004f5dc0` | `OnFoyerMeshUpdateServiceExit` | 11 | Resets the service singleton's dispatch pointer at `0x006e4e88` to the verified base table at `0x005370a4`. |
| `0x004f5da0` | `DestroyFoyerMeshUpdateService` | 32 | Replaces the service dispatch pointer with the base table at `0x005370a4`, optionally frees the receiver when flag bit zero is set, and returns the receiver. |
| `0x0047d040` | `ParseFoyerSceneVector3` | 40 | Parses three consecutive scene numbers into the caller-provided vector. Removing the unused dummy EDX argument from `ParseFoyerSceneNumber` preserves EDX across its calls and produces the original caller body. |
| `0x0047dce0` | `ParseFoyerSceneTimeBlock` | 12 | Parses one scene number and stores it in the parser-time global at `0x00572f90`; the one-pointer helper ABI preserves the caller's EDX as retail does. |

`tools/compare.py` reported 100% for the fifteen earlier uniquely mapped
addresses and each of the eight additional functions above. Explicit dispatch
tables were matched through verified `GLOBAL` annotations, not `VTABLE`
annotations. The parser number helper remains a `STUB`: its behavior is
understood, but its raw instruction score is only 35.29%; only its unused EDX
parameter was removed, after checking the original helper does not read ECX or
EDX and returns with plain `ret`.
`tools/folded_functions.py` independently verified both folded callbacks against
their complete bytes in the retail executable and rebuilt linker map. The build initially exposed a concurrent rename mismatch
between `secondarymanagerstartup.cpp` and `secondarymanagerlifecycle.cpp`; the
owner updated the declaration/calls, after which the worker rebuilt and linked
successfully. No game runtime was run.

## Final integration checkpoint — paused

The Python and CMake/NMake builds both pass the final reccmp guard: **833 FUNCTION
claims, 784 exact and 49 reviewed near matches**, with identical 2,721 measured
scores/statuses. All 28 complete folded aliases and both builders' complete dispatch
and input-format inventories pass (including 1,321 scene pointers). Earlier pending
build notes above are superseded by this checkpoint. No game was run.

All promoted scene/script functions and the renderer-index-buffer constructor
measure 100%. The five final event-name placement-construction attempts measure
27.27% each and remain STUBs. They initialize the same fully zeroed 24-byte buffers
and preserve the existing once-only caller guard; they are not counted as recovered.
Matching work is paused at the user's request.
