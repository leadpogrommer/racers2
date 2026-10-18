# System clock recovery notes

The clock singleton is initialized lazily at `0x004b2960`. Retail sets its
one-byte guard at `0x00654c18` before initializing the object at
`0x00654ba0`; after initialization, the accessor registers the no-op callback
at `0x004b29a0` with the CRT exit-handler API. Library provenance for this
clock code remains unresolved.

The vtable at `0x00535c74` has eight 32-bit entries, confirmed from the PE
`.rdata` bytes. The typed source dispatch preserves the callback ABI and points
the millisecond slot at a zero-argument WINAPI bridge to `timeGetTime`:

| Offset | Retail target | Recovered behavior |
| --- | --- | --- |
| `+0x00` | `0x004b2b90` | Push an RDTSC sample on the nested sample stack. |
| `+0x04` | `0x004b2bc0` | Pop the latest sample and return the cycle difference in EDX:EAX. |
| `+0x08` | `0x004b2c30` | Imported `timeGetTime` thunk; the receiver register is ignored. |
| `+0x0c` | `0x004b2c40` | Return elapsed seconds and microseconds through two output pointers (`ret 8`). |
| `+0x10` | `0x004b2cc0` | Return elapsed microseconds and update the elapsed-cycle baseline. |
| `+0x14` | `0x004b2b70` | Reset both current baselines from one RDTSC sample. |
| `+0x18` | `0x004b2b50` | Return a zero 64-bit value. |
| `+0x1c` | `0x004b2b60` | Return the calibrated DWORD at object offset `+0x70`. |

The object projection uses the observed offsets: dispatch pointer `+0`, sample
depth `+4`, ten 64-bit sample slots `+8..+0x57`, performance-counter-derived
cycles-per-second `+0x58`, query baseline `+0x60`, elapsed baseline `+0x68`,
and a benchmark-derived DWORD `+0x70`. The static guard begins at
`0x00654c18`, 0x78 bytes after the object address. That spacing is compatible
with the expected 0x78-byte C++ layout including trailing alignment, but the
adjacency alone does not prove the object's exact total size; no independent
allocated-object stride was found.

The constructor raises process priority to `HIGH_PRIORITY_CLASS` while
calibrating, then restores `NORMAL_PRIORITY_CLASS` (not the previous priority).
It uses `QueryPerformanceFrequency` and waits for a counter change, samples
RDTSC over one quarter of the QPC frequency, and multiplies that difference by
four for the stored rate. If QPF fails, it waits for a `timeGetTime` tick change
and then 250 ms. It separately benchmarks 100,000 `1*2+3-4` float operations;
when the elapsed cycle count is at most 1000 it stores 10000, otherwise it
stores integer `10000000 / elapsed`. The exact purpose of this final field is
not established, so it is named only as a calibration rate.

`QueryTime` treats QPC-calibrated cycles-per-second as the divisor. It computes
signed 64-bit elapsed cycles, quotient seconds, and the remainder scaled by
1,000,000 for microseconds. This output contract is used by the controller at
`0x004b8460`, which dispatches through slot `+0x0c` with two stack pointers and
callee cleanup of eight bytes. `UpdateElapsedTime` computes signed elapsed
cycles times 1,000,000 divided by the cycles-per-second value, returns the
quotient in EDX:EAX, and stores the new sample at `+0x68`. The original
`timeGetTime` thunk at `0x004b2c30` is not reconstructed as game code. Push and
pop sample RDTSC before changing the nesting depth, matching retail order.

Both builders verify the zero-value callback, calibration getter and elapsed-time update at 100%. Other clock bodies retain STUB status and measured scores. The clock table is fully initialized in the PE: VC6 otherwise generates CRT-time assignments for the imported function pointer and following entries. The local WINAPI bridge avoids those assignments while preserving the import ABI. QueryTime computes both outputs before writing either pointer, preserving the original alias ordering. No game execution or behavioral tests were run.
