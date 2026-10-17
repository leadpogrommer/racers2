# Joystick force-feedback effect records

`JoystickEffect` is the embedded 0xe4-byte DirectInput effect record stored in
each joystick input object. The owner reserves 32 records beginning at +0xbc,
but startup initializes seven records at offsets +0xbc, +0x1a0, +0x284,
+0x368, +0x44c, +0x530, and +0x614. The constructor dispatches these kinds:

| Kind | DirectInput effect |
| --- | --- |
| 0 | Constant force |
| 1 | Friction |
| 2 | Spring |
| 3 | Sine |
| 4 | Square, with a null descriptor |
| 5 | Constant force with envelope |
| 6 | Sine with envelope |

The constructor clamps the axis count to two, clears only the four prefix
DWORDs and the DWORD at +0xe0, stores the device, and creates the selected
effect when the device is non-null. The default constructor clears only the
four prefix DWORDs. The +0xe0 word is cleared by `Stop()` after a successful
stop; other bytes in the embedded record are not initialized by these
constructors. The explicit destructor is empty, matching the shared retail
no-op cleanup target.

The descriptor builders are in separate units: `joystickparams.cpp` owns the
constant-force, spring, friction, enveloped-constant, and observed null-return
square builders; `joystickperiodic.cpp` owns sine and enveloped-sine builders.
They retain the retail parameter caches and provide `DIEFFECT*` values to the
record. This unit preserves the public COM call order, `SetParameters` flags,
HRESULT global writes, failure check calls, and play-state changes. `Release()`
unloads and releases a present effect, then clears device/effect/play/mode
fields. `Stop()` only calls COM when both the effect and playing flag are set.

Retail extents from the full disassembly are:

| VA | Length | Working signature |
| --- | ---: | --- |
| `0x004ec670` | 16 | `JoystickEffect::JoystickEffect()` |
| `0x004ec680` | 378 | `JoystickEffect::JoystickEffect(LPDIRECTINPUTDEVICE8A, LegoU32, LegoU32)` |
| `0x004ed520` | 84 | `SetConstantForce(LegoFloat, LegoFloat)` |
| `0x004ed580` | 87 | `SetSpringCoefficient(LegoFloat)` |
| `0x004ed5e0` | 87 | `SetFrictionCoefficient(LegoFloat)` |
| `0x004ed640` | 88 | `SetEnvelopeConstantForce(LegoFloat, LegoFloat)` |
| `0x004ed6a0` | 91 | `SetPeriodicParameters(LegoFloat, LegoFloat)` |
| `0x004ed700` | 93 | `SetEnvelopePeriodicParameters(LegoFloat, LegoFloat, LegoFloat)` |
| `0x004ed760` | 93 | `SetSquareParameters(LegoFloat, LegoFloat, LegoFloat)` |
| `0x004ed7c0` | 76 | `Stop()` |
| `0x004ed810` | 59 | `Release()` |

`0x00473140`, used by kind 4 and the square-effect update, is a genuine three-byte
null-return body. The resulting null parameter pointer is passed through to
DirectInput as retail does; no square descriptor is fabricated.
The isolated build succeeded. Raw comparison: default constructor 100% and
`Release()` 100% (both `FUNCTION`); the effect-creation constructor 91.09%,
`SetConstantForce` 61.76%, spring/friction setters 67.65% each, enveloped
constant 58.82%, sine 52.78%, enveloped sine 77.78%, square 75%, and `Stop()`
78.43%. The lower-scoring methods remain `STUB` because the caller/COM behavior
is implemented but code generation is below the project threshold. The full
comparison also exposed the separate descriptor builders as low raw matches;
their owners retain them as documented `STUB`s. No game runtime test was run.
