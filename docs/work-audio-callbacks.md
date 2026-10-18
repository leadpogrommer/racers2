# Audio startup callbacks

`SetAudioParameters` reconstructs service dispatch slot +0x28 at
0x004af7b0. It sets the `hasAudioParameters` flag at +0x34, maps its first
float as `p * 4000 - 2000` to +0x2c, and writes its second float multiplied by
0.1 to +0x30. The constants were checked in the PE literal pool (4000.0,
2000.0, and 0.1). The callback uses the `__fastcall` bridge shape for an
original thiscall target: service in ECX, unused EDX, and two stack floats
(ret 8). It remains `STUB` until measured and wired into the complete service
dispatch table.

Audio output provider callbacks remain an open closure. The PE tables at
0x00536fcc (stereo), 0x00536f50 (3D), and 0x0053700c (mono) each contain 15
entries. The newly implemented constructor and resource setup bodies do not
yet define these tables; the CreatePlaybackBuffer, provider-specific source
updates, and final output-gain callbacks still need reviewed implementations.
No incomplete table or retail VA pointer is emitted by these new units.
