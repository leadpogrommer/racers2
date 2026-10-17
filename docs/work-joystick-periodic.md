# Periodic force-feedback descriptors

The periodic helpers return DirectInput effect descriptions for the sine and
enveloped-sine effects. Their callers in the joystick effect class pass the
returned `DIEFFECT*` directly to `CreateEffect` or `SetParameters`; both
helpers use persistent per-kind storage because the descriptor points into
adjacent payload arrays.

The cache layouts follow the retail pointer setup and writes:

| Cache | Layout | Retail storage address |
| --- | --- | --- |
| Sine | `DIEFFECT` at `+0x00`, `DIPERIODIC` at `+0x38`, three `LONG` direction values at `+0x48` | `0x006e4774` |
| Enveloped sine | `DIEFFECT` at `+0x00`, `DIPERIODIC` at `+0x38`, `DIENVELOPE` at `+0x48`, three `LONG` direction values at `+0x5c` | `0x006e4570` |

The shared axis-offset array is `g_forceFeedbackAxisOffsets` at `0x0054b0fc`,
with values `{0, 4}`. Each descriptor uses size `0x38`, flags `0x12`, gain
`DI_FFNOMINALMAX`, and `DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS`. The sine
descriptor has no envelope. The enveloped-sine descriptor points at its
envelope, initialized with size `0x14`, attack level `100`, attack time
`100000`, fade level `100`, and fade time `300000`. Its three direction
entries start at `1`, matching the writes at cache offsets `+0x5c`, `+0x60`,
and `+0x64`. The sine direction entries are also initialized to `1`.

Both helpers clamp frequency to `[1, 300]`. The comparison flow leaves the
default `300` for unordered input, so a NaN frequency takes the maximum branch.
The period is calculated as `(1 / frequency) * 1000 * 10000`, then converted
with `FISTP`, preserving the current x87 rounding mode. Magnitude is scaled by
`10000` and converted the same way. The enveloped variant interprets its last
argument as duration: it stores `duration * 1000000` in `dwDuration`, then
stores that converted value scaled by `0.2` and the retail float constant
`0.33333334` into envelope attack
and fade times. Those are the observed retail formulas, including their use of
the duration input for all three fields.

The descriptor helpers initialize their cache once, then update only their
changing parameter fields. This preserves the static cache lifetime seen in
the binary. Retail extents are `0x004ed030..0x004ed242` (531 bytes including
the return) and `0x004ed250..0x004ed51c` (717 bytes including the return).
The isolated source build succeeds. The current raw matches are 9.94% for
`BuildSineDescription` and 10.40% for `BuildEnvelopedSineDescription`, so both
remain `STUB`; these scores reflect the current source/code-generation gap and
are not a semantic equivalence claim. The implementations preserve the
observed descriptor, cache, clamp, and conversion behavior for integration.
