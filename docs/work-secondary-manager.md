# SecondaryManager input gate

The earlier staged notes below describe the original investigation. The current
combined 294-unit build now links the complete 34-slot recording-service table,
message queue/processor and provider maps, with the full observed 0x230-byte
service storage. The live ABI uses word packets at +48 and InputController*
at +74. All 34 callbacks pass static verification; larger bodies remain STUB.
See [current combined evidence](work-controller-recording.md). SecondaryManager
startup and splash orchestration remain staged outside the manifest.


This batch recovers the full `SecondaryManager::PrepareSplash` path (0x0042ac90), its four-slot input-state refresh (0x0042b220), the input-owner read path (0x004b8250), and the recorded-frame service query helpers (0x004f46a0, 0x004f4710, 0x004f47c0). The sources preserve the retail offline fallback, polling and recorded-input branches. The splash descriptor leaves its first two position words untouched, as the original stack setup does.

The splash path is active only when `GetFileStorageMode()` returns 3. An already-set splash flag first refreshes the secondary manager's four enabled input slots when its state is 9, then returns early if none has an active state or resource. The language-export check gates splash creation/removal. Creation allocates two global overlay records, initializes string IDs `0x39f` and `0x2b`, scales each to a maximum measured width of 0.8 using a starting scale of 0.5, positions them at `(0.5, 0.45)` and `(0.5, 0.52)`, and repeats text initialization after positioning. Removal releases each non-null record and clears the splash flag.

`SecondaryManagerMediaState::Refresh` takes a bit mask, checks each corresponding enabled bit, and asks the input manager for one four-byte controller state. Slot 3 is also copied to the state object's current-slot field. The global owner key is the live pointer stored at `GameController + 0x78`, not the `SecondaryManagerMediaState` pointer.

The recording service object is at `0x00654768`, selected through the initialized pointer at `0x00549fbc`. The constructor writes only the fields visible in the retail body; its storage is allocated to the minimum observed extent through `+0xe8`. The typed dispatch projection covers observed slots: `+0x48` is a five-stack-argument value processor; `+0x54` is wait; `+0x70` records a value; `+0x74` updates a record; `+0x78` tests for a frame; `+0x7c` requests a frame; `+0x80` reads the service mode at `+0xdc`; and `+0x84` clears/sets that mode. Each record has stride `0x250`, owner key at `+0x44`, one 64-word array beginning at `+0x48`, a second 64-word ring at `+0x148`, and a trailing count at `+0x248`. The exact roles of the two arrays are not settled: `4f46a0` compares the requested frame against the array ending at `+0x144`, while `4f4190` writes its third argument to the ring at `+0x148`.

The full service table at `0x00535be8` has 34 entries. This is the complete target inventory in slot order; methods marked open still need source bodies before any full table can be emitted:

| Slot | Target | State |
| ---: | ---: | --- |
| `+00` | `4b1550` | open; parameterized setup |
| `+04` | `4b14e0` | scalar setter implemented |
| `+08` | `4b1700` | open; cleanup |
| `+0c` | `4b1db0` | open |
| `+10` | `4b2470` | device-name list copy implemented; awaits integrated comparison |
| `+14` | `4b24f0` | session-name list copy implemented; awaits integrated comparison |
| `+18` | `4b1fa0` | open |
| `+1c` | `4b1ee0` | open |
| `+20` | `4b1f80` | pending-operation cancellation implemented; awaits integrated comparison |
| `+24` | `4b2580` | session-record snapshot copy implemented; awaits integrated comparison |
| `+28` | `4b2160` | open |
| `+2c` | `4b22c0` | open |
| `+30` | `4b2340` | provider-state reset implemented; still awaits integrated comparison |
| `+34` | `4b23e0` | close-and-initialize peer implemented; awaits integrated comparison |
| `+38` | `4b2370` | session terminate/reinitialize implemented; awaits integrated comparison |
| `+3c` | `4b14f0` | scalar setter implemented |
| `+40` | `4b1500` | scalar getter implemented |
| `+44` | `4b1510` | scalar getter implemented |
| `+48` | `4f3670` | open; value processor |
| `+4c` | `4f37d0` | provider-key value lookup implemented; awaits integrated comparison |
| `+50` | `4f37f0` | provider-key removal implemented; awaits integrated comparison |
| `+54` | `4b2410` | open; wait path |
| `+58` | `4b2730` | current-record name setter implemented; still awaits integrated comparison |
| `+5c` | `4b28e0` | open |
| `+60` | `4b27d0` | current-record value getter implemented |
| `+64` | `4b27f0` | occupied-record value lookup implemented |
| `+68` | `4b2830` | record-value copy implemented; still awaits integrated comparison |
| `+6c` | `4b2870` | owner assignment implemented; still awaits integrated comparison |
| `+70` | `4f4190` | recording body implemented; still awaits integrated comparison |
| `+74` | `4f4450` | owner-record update dispatch implemented |
| `+78` | `4f46a0` | frame query implemented |
| `+7c` | `4f4710` | request/wait implemented |
| `+80` | `4b1520` | mode getter implemented |
| `+84` | `4b1530` | mode setter implemented |

The helper `4b26f0` searches for the record matching the current key, and `4b27e0` returns that key. `4b2340` clears the local state word, queries the primary provider at vtable offset `0x60`, and clears `+0xd8` only on success. `4b2730` copies a current-record name into its 32-byte field and preserves the retail side effect of truncating the caller's input at byte 31. `4b2830` copies each record's value to an output array and writes the record count. `4b2870` binds the input controller to records matching a value and updates the controller's disabled flag against the current value.

`4f4190` compares the service's `+0x64` and `+0x60` values, selects processor mode `0x12` or `0x4a`, skips frames already in the `+0x148` ring, appends successful frames with a wrapping 64-entry cursor, and in the matching-value branch updates lagging unoccupied records. The per-record array at `+0x148` is the frame ring because this method writes the supplied frame number there; the separate `+0x48` array supplies the maximum input-frame bound used by its catch-up loop. The typed dispatch view now exposes the two scalar getters at `+0x60/+0x64`, and the `+0x48` processor returns the EAX status used by the method. The `4b1f80` callback checks the pending-operation handle at service offset `+0x48`, cancels it through the verified `IDirectPlay8Peer::CancelAsyncOperation(handle, 2)` interface method, and clears the handle whenever it was non-null. The direct helper `4f3080` maps the observed DirectPlay message IDs to their payload sizes, returning zero for flag value 1 and unrecognized IDs. Slots `+4c/+50` operate on the key map at service `+0xc0`: the former returns an entry's word at `+8`, while the latter removes a matching key, decrements the used count, and clears the first three fields. Removal also clears the active flag of a matching object in the 12-byte records referenced by service `+0x78`; the pointed-to object's 16-bit identifier is read at offset `+6`. Both map operations preserve the observed lock field and spin while its value is 1.

The peer callbacks at `4f2fd0` and `4f2ff0` feed their three DirectPlay arguments plus the retail mode value into `4f3010`. That helper now builds the observed five-word message record, queues messages with a positive payload size, and sends the other messages to `4f3100`. The enqueue body at `4f3f20` and immediate message processor at `4f3100` are still pending, so this callback chain remains incomplete. The staged storage constructor does not install a partial table or add a static initializer; the remaining targets need real bodies before startup can safely use the service.

Verified retail extents are: `0x0042ac90` 0x295 bytes; `0x0042b220` 0x83; `0x004b1410` 0xca; `0x004b14e0` 0x0a; `0x004b14f0` 0x0b; `0x004b1500` 7; `0x004b1510` 4; `0x004b1520` 7; `0x004b1530` 0x17; `0x004b1f80` 0x1f; `0x004b2340` 0x2e; `0x004b2370` 0x62; `0x004b23e0` 0x2f; `0x004b2470` 0x7d; `0x004b24f0` 0x8d; `0x004b2580` 0xe8; `0x004b26f0` 0x35; `0x004b2730` 0x52; `0x004b27d0` 0x10; `0x004b27e0` 4; `0x004b27f0` 0x33; `0x004b2830` 0x39; `0x004b2870` 0x66; `0x004b8250` 0x1f5; `0x004b9040` 0x6c; `0x004f2fd0` 0x1e; `0x004f2ff0` 0x20; `0x004f3010` 0x67; `0x004f3080` 0x49; `0x004f37d0` 0x13; `0x004f37f0` 0x24; `0x004f3a90` 0x64; `0x004f3d00` 0x52; `0x004f3d60` 0x64; `0x004f4190` 0x2be; `0x004f4450` 0x6b; `0x004f46a0` 0x6c; `0x004f4710` 0xaa; and `0x004f47c0` 0xa7. Extents include the complete return instruction and exclude following padding; function matching has not been measured for this batch.

The isolated MSVC 6 worker compiled all added translation units. Its link stopped on expected missing modules in the minimal manifest and the intentionally absent full recording-service table. No game execution or behavioral tests were run.
