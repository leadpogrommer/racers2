# Character preview initialization and foyer events

The character selector now has its complete six-argument initializer, including
active-state setup, shared character/camera leases, lighting, viewport and
position setup, and child arrow allocation. Idle preview preparation recovers
the fixed effects of 44f6e0(6) and calls the real 450c30 animation/collision path;
the unclaimed fixed-call helper does not stand in for the generic dispatcher.

Animation helpers use the observed controller prefix: enabled at +84, four
layer pointers at +90, track records at +a0 and the selected track ID list at
+a8. Starting a track binds its record+4 through the complete 4f68c0 effect.
The four-argument setter at 4c56a0 retains its unused final argument (RET16).
The tick scale at 572b30 has an actual CRT initializer, matching 47c040's
1/0.4 computation. Both annotated animation globals are BSS in the original.

Seven foyer event functions now link independently of the unfinished resource
loader. Their resource projection exposes only the state at +58 and event
array/count at +6c/+70. The event stride is 94 bytes. Raw instruction review
corrected three errors in the previous staged reconstruction:

- 47c449 restarts the baseline animation when elapsed is at least duration;
  the opposite branch resets the layer and consumes a pending animation.
- A pending flag is cleared before calling the animation setter.
- 47c806 tests x87 equality with zero, including unordered values. It is not
  a negative-duration sentinel. Duration comes from the bound track's +144
  field without an extra track lookup.

Face-state updates retain the shared second floating random stream and its
bit construction, toggle states, thresholds and timing. Attachment updates
use the target's actual orientation/position callbacks and the owner's
embedded transform setters. Larger functions remain STUB at their measured
scores; there is no claim that the entire foyer or animation subsystem is
ready for execution.

The eight exact new helpers are 47c260, 47c270 and 4c5680..4c5720. Their complete
function extents were checked against control flow and the original function
inventory. Other functions were measured without further instruction polishing.
This batch does not replace bootstrap or demonstrate a rendered menu. No game,
startup, movie, asset probe or game behavioral test was run.
