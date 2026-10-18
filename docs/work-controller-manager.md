# Controller polling and replay manager

The original selector at `0x0054a330` points to fixed BSS storage at
`0x00679d48`. The initializer at `0x004b93e0` writes absolute addresses; it is
a CRT global initializer, not an ordinary constructor taking an ECX receiver.
The rebuilt initializer preserves the observed writes and leaves the other
BSS fields alone. The visible object prefix ends after the mutex at `+0xd8`;
the complete original allocation extent has not been established.

The four-slot table at `0x00535f84` initializes polling, stops polling,
acquires the named mutex, and releases a mutex handle. Typed ABI bridges
preserve ECX and stack cleanup. `tools/check_controller_dispatch.py` checks
these four pointers and the eight system-clock entries directly in the PE
files. This verifies wiring independently of instruction similarity.

`InitializeTiming` stores the simulation frequency and application polling
multiplier, then invokes initialization. Polling creates the
`ATDControlInterfacePollMutex` mutex and a WinMM periodic event with the
retail unsigned interval expression `(multiplier * 1000) / frequency - 5`.
The five-argument timer callback polls the fixed object, even if the selector
has changed. Stop acquires the mutex, kills the event, releases and closes the
mutex, and shuts down the timer adapter.

The manager owns twelve controller pointers, with keyboard and mouse service
records at `+0x5c` and `+0x60`. Polling reads the RDTSC clock, computes the
relative frame, refreshes device state, evaluates bindings and records changed
frames. Replay timing preserves the original 32-bit multiplication before
division and the separate paused-time fields. The controller packed state has
two button bytes followed by two signed axis bytes. Binding kinds distinguish
full/negative/positive digital and analog axes and digital/analog buttons.
Digital axes either snap to their direction or accumulate by the elapsed
frame count; inactive axes use the observed linear or half-value decay mode.

The mapping body remains STUB pending measurement and further semantic review.
In particular, the original keeps selected callback results in extended x87
precision while other branches explicitly round to float; the readable first
pass stores a float before dispatch. NaN branch handling, input-record aliasing
across callbacks and the unvalidated axis indices require continued review.
The unusual full-axis button expression `value * 0.5 + 1.0` is present in the
retail instructions and retained. No safety bounds checks or altered timer
arithmetic have been inserted.

The platform readiness helper at `0x004f6310` initializes renderer resources,
then DirectInput, then controller timing using the application's `+0x74`
value. Its source does not by itself make startup ready: renderer construction,
the complete renderer tables and the game startup callbacks still need to be
connected before replacing the temporary entry point. No game execution or
incremental startup probe has been performed for this work.

The combined Python and CMake comparison verifies current-frame calculation, mutex release, polling initialization and timer callback at 100%. Shutdown is a reviewed near match at 98.18%: the sole extra instruction clears EDX before the typed stop bridge, whose EDX parameter is unused. Both builds verify all twelve controller/clock table pointers and all 99 annotated globals with zero data issues. The remaining manager methods retain STUB status.
