# Small core, controller and platform matching batches

Matching work is paused at the user's request on 2026-10-20. No game runs were
performed. Python and CMake/NMake SP5 builds agree on all 2,721 measured function
scores and statuses. The full FUNCTION guard passes: **833 implemented functions,
784 exact and 49 reviewed near matches** against 6,612 inventoried original entries.

Compared with the 619-function starting checkpoint, this work promotes **213
STUBs: 195 exact and 18 reviewed near matches**. One additional original helper,
`SpatialSlotAllocator::GetNodeAtIndex` at `0x00507510`, is newly reconstructed and
exact. All 619 prior FUNCTION claims retain their raw accuracy or improve.

The root-owned promotions below comprise 72 exact functions and seven reviewed
near matches. Other workers' evidence is in [secondary/menu matches](work-small-secondary-matches.md),
[scene matches](work-small-scene-matches.md) and [audio/physics matches](work-small-audio-physics-matches.md).

## Body and contract review

Small allocation-size getters preserve their exact constants; record getters and
setters preserve observed offsets, widths and cleanup. CRT wrappers call the same
identified initializer or exit-registration target. Conditional destructors retain
the flag test, destruction call, optional free and returned receiver. Wrapper
recovery establishes its own call contract; a callee may remain separately stubbed.

The seven near matches differ solely by an added `xor edx, edx` for an unused
fastcall bridge argument. The actual target callees ignore incoming EDX or overwrite
it before use. Stack arguments, receiver, branches, state writes and returns match.
For the view-depth helper, the floating-point operation order also matches.

| Original VA | Function | Final raw accuracy |
| --- | --- | ---: |
| `0x00406070` | `GetBattleManagerSize` | 100.00% |
| `0x00406080` | `DestroyBattleManager` | 100.00% |
| `0x004060a0` | `BattleManager::~BattleManager` | 100.00% |
| `0x00406b00` | `DestroyControllerStartupScene` | 100.00% |
| `0x00406cd0` | `FUN_004c32b0(unsigned int)` | 100.00% |
| `0x00406f60` | `RegisterGameControllerExit` | 100.00% |
| `0x00406f70` | `DestroyGameControllerAtExit` | 100.00% |
| `0x004071c0` | `GameControllerAllocationSize` | 100.00% |
| `0x004071d0` | `DeleteGameController` | 100.00% |
| `0x0040b670` | `InitializeGamePauseObserver` | 100.00% |
| `0x004196f0` | `MenuWidgetBase::MenuWidgetBase` | 100.00% |
| `0x00419730` | `MenuWidgetBase::RestoreBaseDispatch` | 100.00% |
| `0x0041c110` | `InitializeMenuActionDefaultPosition` | 100.00% |
| `0x0041c140` | `InitializeMenuActionPausePosition` | 100.00% |
| `0x0041c1b0` | `InitializeMenuActionFooterPosition` | 100.00% |
| `0x0041c2f0` | `InitializeMenuActionNoticePosition` | 100.00% |
| `0x0041c3f0` | `MenuManagerAllocationSizeBridge` | 100.00% |
| `0x0041c400` | `MenuManagerDestroyBridge` | 100.00% |
| `0x0041c4d0` | `MenuManager::Remove` | 100.00% |
| `0x00420bd0` | `ReleaseMenuResolutionNames` | 100.00% |
| `0x00422fd0` | `SetMenuActionInputMask` | 100.00% |
| `0x00423230` | `GetSecondaryPendingMenuAction` | 100.00% |
| `0x00423a00` | `ReleaseSecondaryObject8c` | 100.00% |
| `0x00427060` | `SecondaryManagerAllocationSizeBridge` | 100.00% |
| `0x00427070` | `SecondaryManagerDestroyBridge` | 100.00% |
| `0x0042af30` | `IsRunningFromOpticalMedia` | 100.00% |
| `0x00439050` | `SetNpcTextCharacterInterval` | 100.00% |
| `0x00439060` | `GetNpcTextCharacterInterval` | 100.00% |
| `0x00450e40` | `CharacterVisualApplyAnimation` | 95.24% |
| `0x004534e0` | `GameObjectBaseDestroy` | 100.00% |
| `0x004608a0` | `ShowSecondarySpriteSet` | 100.00% |
| `0x004608d0` | `HideSecondarySpriteSet` | 100.00% |
| `0x00460970` | `FindSecondarySprite` | 100.00% |
| `0x00462190` | `InitializeSelectionGridArrowSize` | 100.00% |
| `0x004621b0` | `AllocateSelectionGridEntry` | 100.00% |
| `0x004637a0` | `ReleaseSelectionGridNavigationArrows` | 100.00% |
| `0x00463870` | `ConstructSecondarySelectionLabel` | 100.00% |
| `0x00463900` | `ReleaseSecondarySelectionLabel` | 100.00% |
| `0x004639a0` | `SetSecondarySelectionLabelFont` | 100.00% |
| `0x00467520` | `InitializeSecondaryBuilderGridDimensions` | 100.00% |
| `0x00467550` | `InitializeSecondaryBuilderCellDimensions` | 100.00% |
| `0x00467da0` | `GetSecondaryBrickBuilderSize` | 100.00% |
| `0x0046a200` | `FindSecondaryChassisTag` | 100.00% |
| `0x00470bb0` | `RemoveCharacterCollisionParticle` | 97.96% |
| `0x0047b5d0` | `FoyerAssetCacheExitCallback` | 100.00% |
| `0x00493680` | `GetNetworkAppLayerAllocationSize` | 100.00% |
| `0x00493990` | `RefreshMenuDiscoveredNetworkSessions` | 96.00% |
| `0x00495010` | `ClearNetworkTextQueue` | 100.00% |
| `0x00495060` | `ClearNetworkChatInput` | 100.00% |
| `0x00495080` | `ClearNetworkChatHistory` | 100.00% |
| `0x00495360` | `AllocateNetworkPayloadKey` | 100.00% |
| `0x004a2ba0` | `FileManager::Read` | 100.00% |
| `0x004a37f0` | `FindGameObjectByName` | 95.24% |
| `0x004a9870` | `ResetGameObjectModelBindingDispatch` | 100.00% |
| `0x004aa3c0` | `SetRenderProjectionRatio` | 100.00% |
| `0x004ab440` | `InitializeStaticDisplayRenderer` | 100.00% |
| `0x004ab460` | `RegisterStaticDisplayRendererExit` | 100.00% |
| `0x004b1400` | `InitializeStaticControllerRecordingService` | 100.00% |
| `0x004b2d30` | `InitializeGlobalOverlayManagerAndRegisterExit` | 100.00% |
| `0x004b2da0` | `RegisterGlobalOverlayManagerExit` | 100.00% |
| `0x004b3950` | `SetSelectionGridTextVisible` | 100.00% |
| `0x004b9280` | `AreMenuControllersReady` | 100.00% |
| `0x004b9370` | `MergeSecondaryMediaSlot` | 100.00% |
| `0x004b94c0` | `InputControllerManager::AcquirePollMutex` | 100.00% |
| `0x004ba370` | `GetGameObjectViewDepth` | 97.44% |
| `0x004bb6d0` | `RegisterGlobalPauseControllerExit` | 100.00% |
| `0x004bb6e0` | `DestroyGlobalPauseController` | 100.00% |
| `0x004bb740` | `GlobalPauseController::SetObserver` | 100.00% |
| `0x004bb750` | `GlobalPauseController::ClearObserver` | 100.00% |
| `0x004bb8f0` | `GlobalPauseController::SetDeadline` | 100.00% |
| `0x004bb980` | `RegisterGamePauseObserver` | 100.00% |
| `0x004bb990` | `ClearGamePauseObserver` | 100.00% |
| `0x004bb9a0` | `SetGamePauseObserverMode` | 100.00% |
| `0x004c3020` | `StopNativeGame` | 100.00% |
| `0x004d6e30` | `UpdateGameTemplateInstance` | 96.30% |
| `0x004e6aa0` | `GameManager::RefreshCollisionStates` | 100.00% |
| `0x004e9390` | `ForwardPositionToChildGeometry` | 100.00% |
| `0x004ef550` | `UpdateCharacterPassiveAnimation` | 100.00% |
| `0x004f6360` | `ShutdownPlatformServices` | 96.00% |

## ABI and mapping corrections

`GlobalPauseController::ClearObserver` at `0x004bb750` consumes one ignored stack
argument (`ret 4`). Its signature and caller now preserve that contract. Its
verified extent is 10 bytes; the previous eight-byte record truncated the return.
Pause-observer wrappers use the actual static pause storage rather than loading a
C++ reference variable. Controller and pause exit callbacks likewise use their
original static receivers. A private ECX-only registration helper no longer needs
an artificial second register argument.

Verified GLOBAL annotations identify explicit callback-table data, enabling
relocated table operands to compare correctly. Full binary slot inventories
validate the original and rebuilt targets. The scene inventory now also covers
both complete 21-slot game-controller and menu-manager tables (42 added slots).
These are data mappings, not C++ VTABLE claims.

Twenty-eight relocation-free folded bodies are verified against complete original
and rebuilt bytes and exact linker-map symbols. No prefix matching or relocation
masking was introduced. The shared `GameObjectBaseRemove` tail-jump is annotated
at both real source aliases and is measured by native reccmp, including its branch
target; it is not assigned a synthetic byte-match score.

## Final verification and limits

Both builders pass the FUNCTION guard, all twelve dispatch inventories (including
1,321 scene pointers), all 307 input-format records and all 29 existing tooling /
library tests. The source manifest remains 465 units; no behavioral game tests were
added. Console, JSON, HTML and SVG report 833 / 6,612 implemented (12.60%), 99.80%
accuracy and 12.57% progress.

Datacmp checks 497 variables and flags 18 in both builds. Sixteen flags are the
newly mapped callback tables' ABI-bridge / folded-pointer identities; complete
original/rebuilt slot inventories independently pass for these tables. Two existing
variables contain five duplicate-string aliases. This is not a claim of byte-perfect
data equality or a new runtime validation.

The final constructor experiments remain STUBs: localization-path initialization,
five character event-name initializers and default-bodypart-name initialization
measure 27.27% with direct placement construction. Their original constructor
calls and fixed strings were verified; both old temporary assignment and direct
construction clear the complete buffer before copying. The outer lazy-init guards
remain in place, including the template family's once-only guard. These functions
are not counted as implemented. `RemoveAudioSource` now calls the retail resource
release helper instead of the destructor-style disposal helper; its measured
82.05% also remains STUB. See worker notes for the other retained low-score cases.

No further decompilation should start until the user resumes. New Game recovery
remains separately paused. Generated logs/reports are under ignored build/ and data/;
all code and review notes are saved in the working tree.
