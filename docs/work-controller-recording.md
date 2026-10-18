# Controller service and scene callback integration

The 294-unit build now links the complete 34-slot controller recording service,
its DirectPlay peer/lobby callbacks, queue/map helpers and recorded-input query
path. DirectPlay calls use the genuine DX8 interfaces, including packet-word
and input-controller pointer contracts established from callers. The service
reserves its observed 0x230-byte extent, retaining retail volatile busy waits.
This does not establish thread safety beyond the original implementation.

The outer and nested textbox managers, both pause-detector tables, media-state
query, foyer name initialization and common widget ownership now link. Their
larger reconstruction bodies remain STUB pending further review. Foyer scene
loading and remaining frontend controls are still staged outside the manifest.
Bootstrap remains active; the menu has not launched.

Review found a callback mapping error: retail 403910 returns zero, whereas
403920 returns one. BodypartManager, ScreenPrinter and the nested textbox must
use zero at +14. The scene checker now verifies both complete emitted constant
bodies and their CSV associations, in addition to checking all 126 table slots.
The recording checker independently verifies all 34 table entries.

Both Python/SP5 and CMake/NMake builds agree on all 1,350 measured scores:
397 exact FUNCTION claims, eighteen reviewed near matches and 935 STUB bodies.
This batch adds 23 exact claims and two reviewed equivalents. Provider-map
SetState (4f3c90, 95%) differs only in scheduling its entries-pointer load after
the empty-count test; lock behavior, key comparison, state write, release and
return paths agree. NestedTextBoxManagerDestroy (437a00, 95.65%) adds an unused
EDX-zero argument for the fastcall bridge; receiver, destructor call, conditional
heap release, returned pointer and RET 4 agree. These are implemented functions.

Both data comparisons verify 242 globals with zero issues. All eleven static
dispatch inventories, eight complete folded aliases and the existing 21
library/tooling tests pass. No game, startup, asset or menu probe was executed,
and no game behavioral tests were added. Link and static checks do not establish
that the recovered frontend renders correctly.
