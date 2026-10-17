# Controller startup dependencies

The readiness helper at `0x004f6310` initializes renderer resources, initializes
DirectInput, and calls the controller manager at `0x0054a330` through
`0x004b7c50`. That selector points to the static object at `0x00679d48`.
The manager includes polling, input bindings, recorded input and replay state;
it is not just a frame clock. Its timer callback runs through `0x004b85a0`.
Manager initialization and polling remain open; no reconstructed polling timer
is started by the temporary executable entry point.

`InputController` recovers the `0x17c` child record and four direct helpers:
constructor `0x004b7b80`, cleanup `0x004b7c20`, device assignment
`0x004b7d80` (three explicit arguments, the third unused), and binding append
`0x004b7da0`. Fourteen `0x18` binding records start at `+0x18`; the appended
record is copied before incrementing the count, without a retail bounds check.
The constructor allocates `0x10000` bytes tagged `Controller Data` only when
recording is enabled. Fields at `+0x170/+0x174` distinguish recorded and live
input; both are retained separately. These names are working descriptions.

The WinMM and named-mutex adapter at `0x004f4a40`–`0x004f4c24` now has source.
Initialization publishes its selector before attempting timer capability/period
setup, including retail's behavior of retaining that selector after failure.
A later call therefore returns success if the selector is already non-null.
The chosen timer period clamps 5 ms to the supported capability range.
Periodic events retain the actual three-stack-argument member ABI. The named
mutex acquisition path uses zero/infinite timeout, accepts only WAIT_OBJECT_0,
and retains the original release/close behavior on other wait results.
These wrappers call the actual Win32/WinMM APIs. Their library provenance is
unresolved: the existing local library-candidate inventory has no match for
this address range, which does not establish game authorship. No replacement
third-party source has been identified or imported.

The cleanup, device assignment, binding append, binding lookup and seven timer/mutex wrappers measured at 100% in the combined Python build. They use FUNCTION annotations; broader initialization and recorded-input paths remain STUB. Both builders agree on these measurements; 95 annotated globals compare without issues. No game
or timer execution was used to validate this batch.

Recorded input helpers `0x004b7e40`, `0x004b7eb0` and `0x004b7f20` now preserve the 16,384-frame ring, unsigned frame comparison, two distinct lazy zero-state fallbacks and inclusive range writes. The current-input path returns the live four-byte field at `+0x10` when no record buffer exists; otherwise it selects the recorded frame or the separate initial-state fallback. The range writer changes the record state after frame `0x3fff`, preserving the retail threshold. These helpers remain STUB.
