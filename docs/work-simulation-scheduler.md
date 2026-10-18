# Fixed-step simulation scheduler

The frame routine at `0x004a3aa0` lazily initializes the five-field object at
`0x006ff520`. Its frame counter at `+0x10` is incremented by the frame routine,
and `0x004a3840` compares the corresponding simulation time with the controller
manager's elapsed time. This provides a concrete connection from the recovered
clock/controller services to the actual update loop.

`Reset` (`0x004a3810`, 47 bytes) clears the five fields, resets the clock through
slot `+0x14`, and resets controller recording with its one stack argument.
`NeedsSimulationStep` (`0x004a3840`, 468 bytes) honors the observed global gate,
alternates its result in development mode, and forces the initial update once.
It computes seconds/microseconds with unsigned 32-bit arithmetic. Updates are
requested while simulation time is behind the controller clock, except after
an observed clock interval greater than one second. Otherwise it emits a frame
interpolation value, rejecting values outside zero through 1.001.

The original zero-extends the 32-bit scaled interpolation offset for a QWORD
integer-to-x87 conversion. The reconstruction preserves that unsigned wrap;
changing it to signed or 64-bit intermediate arithmetic changes behavior.
Both builders measure Reset at 72.73% and NeedsSimulationStep at 66.46%; both methods remain STUB pending further semantic review. The gate at `0x00679fe0` is deliberately unnamed: other game-state
callers use it, and its broader purpose is not yet established. No runtime
probe has been performed.
