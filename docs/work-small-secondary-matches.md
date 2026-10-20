# Small secondary stub review

Reviewed the complete original bodies for the promoted entries below. The 31
initial exact raw scores and three near-match scores are from the session's
pre-change `small-stubs.json` report. Four additional wrappers and the menu
manager destructor have since measured exact after dispatch-table mapping.

| Address | Working name | Raw score | Equivalence evidence |
| --- | --- | ---: | --- |
| `0x00404870` | `FindSecondaryBrickPath` | 100% | Searches the pool path strings byte-for-byte and returns the matching index or -1. |
| `0x0040b020` | `UnlockAllSecondaryWorlds` | 100% | Writes both leading selection fields to -1, then fills the five world slots with 0 through 4. |
| `0x0040b250` | `MergeSecondarySelection` | 100% | ORs the two selection masks and appends source world IDs absent from the destination list. The valid five-world list bounds the append set. |
| `0x0040b350` | `UnlockAllSecondaryBonusFlags` | 100% | Stores `0xffffffff` to the flags word. |
| `0x00410b60` | `SetSecondarySessionName` | 100% | Copies the 20-byte name to the player-indexed session slot. |
| `0x00410b80` | `SetSecondarySessionCharacter` | 100% | Copies four part IDs to the player slot, then stores its character value. |
| `0x00410bc0` | `SetSecondarySessionCar` | 100% | Copies the 0x2f4-byte item block to the player/car-set indexed slot. |
| `0x00411700` | `LoadSecondarySessionData` | 100% | Accepts an empty record; otherwise checks sentinel `0xb00bface` at byte 0x1e2c, copies 0x1e34 bytes on success, and returns the validation result. |
| `0x00411740` | `CaptureSecondarySessionData` | 100% | Copies 0x1e34 bytes of current session state to the adjacent saved-state region. |
| `0x00411e90` | `SetMenuExitAction` | 100% | Stores the action only for values 9 through 12. |
| `0x004121a0` | `InitializeSecondaryWheelCenterOffset` | 100% | Writes the two observed center-offset float constants. |
| `0x004122b0` | `InitializeSecondaryWheelShadowOffset` | 100% | Writes the two observed shadow-offset float constants. |
| `0x004122d0` | `ConstructSecondaryAuxiliary` | 100% | Zeroes the constructor's 26 observed 32-bit words and returns the input object. |
| `0x00422ff0` | `SetMenuManagerSelectedInput` | 100% | Stores the supplied input value at manager offset 0x21c. |
| `0x00423200` | `GetSecondaryNestedMenuAction` | 100% | Returns -1 below depth 3 or when the current stack entry's action markers differ; otherwise returns its action. |
| `0x00423e40` | `InitializeSecondaryPointerSize` | 100% | Writes the two observed pointer-size constants. |
| `0x00423e70` | `InitializeSecondaryPointerOrigin` | 100% | Writes the two observed pointer-origin constants. |
| `0x00423fd0` | `ReleaseSecondaryPointer` | 100% | Calls `GlobalOverlaySprite::Release` at `0x004b2e00`, clears the sprite pointer, and clears the active flag. The release callee remains a separate stub. |
| `0x00426f00` | `InitializeSecondarySaveMessagePosition` | 100% | Writes both message-position coordinates as 0.5. |
| `0x004293d0` | `SecondaryManager::EnterSpecialMode` | 100% | Sets state 5, marks the transition active, and clears the special-mode selector. |
| `0x004293f0` | `SetSecondarySaveSlot` | 100% | Stores the supplied slot at manager offset 0x240; the save-menu caller passes the selected slot plus one. |
| `0x0042a860` | `RequestDeferredWindowDestruction` | 100% | Stores 1 to the deferred-window-destruction global. All owned declarations and calls use this descriptive name. |
| `0x0042dd40` | `GetSecondaryProgressLayoutPosition` | 100% | Returns the indexed pair in an array of 8-byte float positions. |
| `0x00437350` | `ReleaseSecondaryAuxiliaryModel` | 100% | If flag 0x80 is set, clears it and releases 19 texture references at the observed descending record stride. |
| `0x00437400` | `ReleaseSecondaryModelResources` | 100% | If flag 0x100 is set, clears it and releases the model's texture reference. |
| `0x004b3530` | `SetGlobalOverlayRecordListCount` | 100% | Stores the requested count at manager offset 0x24940; the overlay pass uses it to split its 16 bands. |
| `0x004bb780` | `RequestSecondaryPauseLeave` | 100% | Marks leave requested and stores the requested resume input. Callers use it to carry menu decisions into the pause controller. |
| `0x004c0ec0` | `AddSecondaryRendererTextureReference` | 100% | Skips id -1; otherwise looks up the texture reference and increments its reference count when found. |
| `0x004c1d00` | `CreateSecondaryLanguageRecord` | 100% | Returns null after consuming the full thiscall argument frame. |
| `0x004c1d10` | `QuerySecondaryLanguageRecord` | 100% | Returns zero after consuming the full thiscall argument frame. |
| `0x004c1d20` | `SecondaryContentArchiveThreeArguments` | 100% | Is a three-argument no-op (`ret 0x0c`). |
| `0x00427090` | `SecondaryManager::~SecondaryManager` | 100% | Restores the manager's 21-slot dispatch table; the complete slot layout and pointer targets were checked against the explicit ABI dispatch contract. |
| `0x004c1cd0` | `ConstructSecondaryContentArchiveManager` | 100% | Installs the 13-slot archive dispatch table and returns the input object. |
| `0x004c1ce0` | `DestroySecondaryContentArchiveManager` | 100% | Restores the 13-slot base dispatch table, conditionally frees the object, and returns it. |
| `0x004c1d70` | `OnSecondaryContentArchiveExit` | 100% | Restores the archive object's base dispatch table during process exit. |
| `0x004a9580` | `CopySecondaryModelPrimitive` | 95.65% | Constructs a zero translation and forwards destination, source, and translation to `CopySecondaryTranslatedPrimitive`; that callee remains a separate stub. |
| `0x00429750` | `SecondaryManager::BeginCarSelection` | 97.56% | Derives the character-enable flag from current session state and car set, calls `BeginSecondaryCarSelection`, then sets manager state 7. The chooser callee remains a separate deferred stub. |
| `0x00429580` | `SecondaryManager::ApplySavedSettings` | 95.05% | Applies both player names, audio levels and quality, the three graphics settings, an available registry language override, and the selected menu language. These callees remain separately marked stubs. |

The exact entries have completely observed bodies. The three near matches retain
the observed argument and state behavior. Their unmatched instructions are compiler
setup: `0x004a9580` and `0x00429750` each add `xor edx,edx` for an unused
fastcall placeholder; the original callees do not read that register.
`0x00429580` adds four such clears and omits a move into ECX before
`AccessGameLanguagePreference`; that callee reads its parameters from the stack
and ignores incoming ECX.

The 21-slot manager and both 13-slot archive dispatch tables have verified
explicit ABI layouts, including each slot's target and calling convention. Their
`GLOBAL` annotations record those data addresses so the four table-address
references match retail without hard-coding a retail address in executable code.
The archive callbacks also have limited but resolved retail behavior: null,
zero, and no-op.
The four table-mapped function promotions measured 100% in the latest combined
reccmp report.
`ReleaseSecondaryPointer` calls the known sprite release method, whose
implementation at `0x004b2e00` remains stubbed.

No game runs were performed. The isolated worker compile stalled in the VC6
invocation during Wine startup and was stopped; final build and report
validation are being handled by the root integration build.

## Additional dispatch and widget batch

The original executable contains three 21-pointer dispatch tables used by
secondary-batch destructors. I verified the pointer values directly from the
mapped executable image: `NpcTextManager` at `0x00532628`, `MenuManager` at
`0x005302fc`, and `GameController` at `0x0052f528`. Each occupies `0x54` bytes.
The menu and controller slot sequences were sent to the integration owner for
the complete dispatch CSV and table check. The NPC table has a complete slot
inventory in its source. The three destructors only restore their object's
table pointer and tail-call `GameObject::~GameObject`; their pre-mapping raw
score was 50%, with the sole diff being retail's absolute table address versus
the corresponding source table symbol. The menu destructor now measures 100%.
The controller and NPC annotations initially failed because a descriptive
comment sat between the `GLOBAL` annotation and the declaration. I moved those
comments above the annotations; the corrected mapping scores are pending the
next reccmp gate.

The same review confirmed `GetNpcTextManagerSize` at `0x0045fa20` is already an
exact 6-byte function, and `DestroyGameControllerAtExit` at `0x00406f70` is
already exact. I left both unchanged.

Three small widget source fixes are also pending a fresh comparison:

| Address | Function | Change and evidence |
| --- | --- | --- |
| `0x00424300` | `InitializeMenuTextEntrySpriteSpacingX` | 100% after correcting the multiplier from `2.0f` to the retail `3.0f`; the original loads `g_menuTextEntryTuning[2]`, multiplies by the `3.0f` constant at `0x0052f440`, and stores element 4. |
| `0x0041c210` | `InitializeMenuChoiceColorA` | 100% after replacing the single 32-bit assignment with byte writes `62 6a ff ff`, matching the original four byte stores and preserving the same little-endian color value. |
| `0x0041c240` | `InitializeMenuChoiceColorB` | 100% after replacing the single 32-bit assignment with byte writes `c8 6a 62 ff`, matching the original four byte stores and preserving the same little-endian color value. |

The three widget functions are now marked `FUNCTION` after the combined report
measured each at 100%. No game runs were performed.

## Final integration checkpoint — paused

The Python and CMake/NMake builds both pass the final reccmp guard: **833 FUNCTION
claims, 784 exact and 49 reviewed near matches**, with identical 2,721 measured
scores/statuses. All 28 complete folded aliases and both builders' complete dispatch
and input-format inventories pass (including 1,321 scene pointers). Earlier pending
build notes above are superseded by this checkpoint. No game was run.

The controller, menu and NPC destructors, four secondary dispatch wrappers,
and all three widget tuning/color changes measure 100%. The localization-path
placement-construction attempt measures 27.27% and remains STUB; it is not counted
among the implemented functions. Matching work is paused at the user's request.
