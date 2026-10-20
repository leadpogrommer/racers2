# Application construction and startup runtime

The rebuilt executable now includes the application singleton path. The
lazy accessor constructs the object, installs the primary dispatch table,
loads the display config through the real user-data/file-manager path, and
registers the observed exit-state callback. The two tables each contain 38
four-byte entries. They are represented as explicit ABI tables so that the
observed slot order does not depend on a guessed C++ inheritance hierarchy.

`tools/check_application_dispatch.py` checks both retail tables and every
emitted callback address. The current Python and CMake outputs pass all 76
pointer checks. This is a binary wiring check only: it does not compare method
bodies, prove semantic equivalence, or claim that the tables are compiler
emitted C++ vtables. Reconstructed functions retain their separate annotation
and raw-score status in [progress](progress.md).

The application constructor loads the 16-byte `LR2.cfg` record from
`C:\users\ilya\Documents\Games\LEGO Racers 2\Saved Games\LR2.cfg`. A runtime
trace of the full rebuilt process confirmed access to that per-user path
before reading the archive metadata.
It then read all 784,312 bytes of `FILELIST.INF`, read all 43,740 bytes of
`COMPRESS.INF`, opened `GAMEDATA.GTC` read-only, and exited with code 0. The
trace is retained in ignored `build/msvc/runtime-application-startup.log`.

The table methods are invoked through explicit ABI bridges: they pass the
receiver in `ECX`, ignore `EDX`, and preserve each target's explicit stack
arguments and cleanup convention. Since bridge symbols do not correspond
one-to-one with the measured original method symbols, the tables carry no
`VTABLE` or `GLOBAL` reccmp claim. The pointer verifier checks table wiring
separately from function-body comparisons.

The process still enters through the temporary `WinMain` in `bootstrap.cpp`.
The observed work comes from runtime/static startup dependencies, including
application, pool and archive initialization; the temporary entry body then
returns. This run does not show that the settings dialog opened, config saving
ran, compressed asset reads were correct, a window was created, or any frame
was rendered. It is an application/config and archive-metadata startup
milestone, not a playable-game milestone. The next runtime target is the real
display initialization and a visible menu; see the
[runtime roadmap](runtime-roadmap.md).

The extracted `.res` contains the retail dialog resource ID `0x66` and its
icon resources and is picked up by both builders from ignored
`build/resources/retail.res`. Extraction verifies the pinned executable hash
and preserves all five entries and 7,022 payload bytes; see
[resource extraction](work-resources.md). Resource linkage makes the original
dialog available to a future run but does not claim the dialog path has been
executed.
