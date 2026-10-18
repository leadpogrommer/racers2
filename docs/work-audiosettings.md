# Audio mode accessor

`GetConfiguredAudioServiceMode` at `0x0043dcd0` reads the DWORD at offset
`0x88` through the process global at `0x005544bc`; if that pointer is null, it
returns the read-only default DWORD at `0x0054225c`. The PE stores the default
as `1`. The global pointer is in BSS and begins null. The code does not
establish the full type, size, or provenance of the object behind that pointer,
so `AudioSettingsContextView` only describes the accessed prefix and does not
assert a total object extent.

The accessor body spans 22 bytes through its second return at `0x0043dce5`.
It has no calls and no arguments. It supplies the value consumed by the audio
singleton accessor before the service constructor, although the constructor
itself does not read its stack parameter. This implementation remains STUB
until the isolated reccmp build is linked with the recovered owner of the
settings context and full audio dispatch closure.
