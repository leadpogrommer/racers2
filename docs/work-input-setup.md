# Input-device registration helpers

This note covers the input-device setup leaves at `0x004aed00`, `0x004af090`,
`0x004af1e0`, `0x004ec3f0` and `0x004ec450`. Their callers are the DirectInput
service initializer at `0x004aed30` and the mouse-device setup path. Interfaces
come from the DirectInput 8 SDK declarations in `util/directx8.h`; no COM
vtable is synthesized.

`InputDeviceService::RegisterDevice(record)` at `0x004af090` has a 148-byte
extent (`0x004af090–0x004af124`) and a thiscall ABI with one stack argument.
It zeroes the record scratch area at `+0x90` as a `DIDEVCAPS`, writes its
`dwSize` (`0x2c`), calls `IDirectInputDevice8A::GetCapabilities`, saves that
HRESULT at `0x00653114`, runs the DirectInput result helper, asks the record to
load its product name, then stores it at `service->m_devices[count]` and
increments the count. The append is unconditional; there is no capacity or
HRESULT guard in this function.

`InputDeviceRecord::LoadDeviceInfo()` at `0x004af1e0` has a 161-byte extent
(`0x004af1e0–0x004af281`) and no explicit arguments. The local record is an
ANSI `DIDEVICEINSTANCEA`, zero-filled for `0x244` bytes with `dwSize` set to
`0x244`. The method calls `GetDeviceInfo` through DirectInput slot `+0x3c`.
On `DI_OK`, it copies `tszProductName` (offset `0x12c`) including the NUL byte
to the record's `+0x0c` product-name buffer without a length check. On failure,
it writes only a NUL to the first destination byte.

`CheckDirectInputResult(device, result, ignored)` at `0x004aed00` has a
36-byte extent (`0x004aed00–0x004aed24`). It returns true only when the HRESULT
is exactly zero. For nonzero results, if the device pointer is nonnull, it
calls `IDirectInputDevice8A::GetForceFeedbackState` through slot `+0x54`,
passing the address of the stack copy of the HRESULT; this call's HRESULT is
ignored. The caller-supplied third pointer is not read. The registration
caller passes the literal `"IDirectInputDevice8_GetCapabilities"` from
`0x00549f2c`, but the helper does not inspect or report it.

The mouse-axis setup member at `0x004ec3f0` has a 95-byte extent
(`0x004ec3f0–0x004ec44f`), a thiscall ABI and one
`DIDEVICEOBJECTINSTANCEA` argument. It checks bit 0 of `dwType` (the
`DIDFT_RELAXIS` bit). For matching objects it sends `DIPROP_RANGE` to
`SetProperty` (DirectInput slot `+0x18`) using `dwOfs` as `dwObj`,
`DIPH_BYOFFSET`, and limits `-255` through `255`. It stores the HRESULT at
`0x006e4554`. Either branch returns true. `0x004ec450` is the 17-byte
`BOOL CALLBACK` adapter (`ret 8`) that forwards the object record and context
to that member. Its caller enumerates objects with flags zero, so the callback
continues enumeration for every object.

The extents include the complete return instruction and stop before alignment
padding or the next entry. The isolated worker measures the result checker,
relative-axis setter and callback adapter at 100% each; they are `FUNCTION`.
`RegisterDevice` measures 41.38% and `LoadDeviceInfo` measures 57.43% in both
final combined builds. Both remain
`STUB`: their reviewed behavior is supported by the disassembly, but their
current instruction matches are below the repository threshold. The actual
ignored capabilities-message literal at `0x00549f2c` is 36 bytes including
its NUL terminator.
