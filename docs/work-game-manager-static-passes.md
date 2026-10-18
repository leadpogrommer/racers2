# Game-manager static-object passes

Both functions are reached at the end of `GameManager::InitializeRegisteredObjects` (`0x4a3580`). This inventory records each selector global, its PE-initialized object pointer, the constructor thunk that installs its vptr, that vptr, and the function stored at vtable slot `+8`. The constructor thunk is the `jmp` at body start minus 0x10; it appears as a DWORD entry in the PE `.data` CRT-initializer array at file offsets `0x13d2f8` onward. The vptr assignments and common base-constructor call are visible in the corresponding retail disassembly. Each selector address has a single `.text` reference, the read in its aggregate pass; no code reference stores to any selector.

| Pass | Selector | PE object pointer | Constructor thunk | Installed vptr | vtable +8 target | Selector code refs | CRT ctor entry |
|---|---:|---:|---:|---:|---:|---:|:---:|
| 0x49a490 | `0x00543590` | `0x0056f7e0` | `0x00455c40` | `0x00532170` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00543578` | `0x0056f788` | `0x00454c80` | `0x00531fc4` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x0053fb90` | `0x0056bb08` | `0x0042d1b0` | `0x005306b4` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005435bc` | `0x0056f868` | `0x0045b1f0` | `0x005322f0` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005435c0` | `0x0056f930` | `0x0045b250` | `0x00532300` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00541b64` | `0x0056c2f0` | `0x004390f0` | `0x00530ad0` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00541e68` | `0x0056cbd0` | `0x0043b480` | `0x00530edc` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x0054868c` | `0x00572d20` | `0x0047edd0` | `0x005332b0` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00547248` | `0x00571c20` | `0x0046f6c0` | `0x00532bb0` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00546fc4` | `0x005719f0` | `0x0046d770` | `0x00532abc` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x0054714c` | `0x00571ba0` | `0x0046eb40` | `0x00532b44` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00546fb8` | `0x00571640` | `0x0046b290` | `0x00532a08` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00542ee0` | `0x0056ec50` | `0x0044b1f0` | `0x00531a14` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005490ec` | `0x005773d8` | `0x004933c0` | `0x00534290` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00549334` | `0x005781f8` | `0x00496480` | `0x00534530` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00543508` | `0x0056f698` | `0x00451c30` | `0x00531d44` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00541fec` | `0x0056ccb0` | `0x0043c580` | `0x00531028` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005435a8` | `0x0056f828` | `0x004598b0` | `0x0053225c` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00543560` | `0x0056f748` | `0x00453420` | `0x00531f20` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005477e0` | `0x005726a8` | `0x004774f0` | `0x00532f70` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005475d0` | `0x00572180` | `0x004742d0` | `0x00532ed4` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00543518` | `0x0056f6e0` | `0x00452320` | `0x00531e00` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00542260` | `0x0056cd58` | `0x0043cea0` | `0x005311d8` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00542264` | `0x0056cd38` | `0x0043cf10` | `0x005311e8` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x0054220c` | `0x0056ccf0` | `0x0043c5d0` | `0x00531044` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00542210` | `0x0056ccd0` | `0x0043c720` | `0x005310a4` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005478ec` | `0x005726c8` | `0x00478b90` | `0x00532ff8` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00542268` | `0x0056cd98` | `0x0043dcf0` | `0x00531298` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00541b78` | `0x0056c318` | `0x00439550` | `0x00530ba0` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x00547358` | `0x00571c40` | `0x0046fa50` | `0x00532c20` | `0x00403910` | 1 | yes |
| 0x49a490 | `0x005478f8` | `0x00572710` | `0x00479b40` | `0x005330e4` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054af24` | `0x0067dd20` | `0x004e64e0` | `0x00536be0` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054accc` | `0x0067bb40` | `0x004d7830` | `0x00536b84` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054ab28` | `0x0067b7a8` | `0x004d0b60` | `0x005369d8` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054acb8` | `0x0067bb20` | `0x004d7330` | `0x00536b10` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054ba8c` | `0x006f9bc8` | `0x00501d50` | `0x005374f8` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054aca4` | `0x0067bb00` | `0x004d69d0` | `0x00536aa4` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b940` | `0x006e4eb0` | `0x004f8260` | `0x00537148` | `0x0049a730` | 1 | yes |
| 0x4e7080 | `0x0054aaa0` | `0x0067b6d8` | `0x004cfe00` | `0x00536958` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054ba5c` | `0x006f9ba8` | `0x00501ac0` | `0x0053748c` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054af40` | `0x006843c0` | `0x004e7df0` | `0x00536cf8` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9d8` | `0x006f9b60` | `0x004ff990` | `0x00537334` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9e0` | `0x006f9b80` | `0x004ffbe0` | `0x00537354` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9dc` | `0x006f9b00` | `0x004ffac0` | `0x00537344` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9e4` | `0x006f9b40` | `0x004ffdb0` | `0x005373b8` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9cc` | `0x006f9ae0` | `0x004ff900` | `0x00537320` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054a430` | `0x0067a070` | `0x004bd010` | `0x00536304` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x00549a74` | `0x00578c18` | `0x004a3e00` | `0x0053533c` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x00549a78` | `0x00578c38` | `0x004a3e30` | `0x0053534c` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054aff4` | `0x006844e0` | `0x004e93a0` | `0x00536e2c` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054a620` | `0x0067a3f8` | `0x004c2d60` | `0x005365a4` | `0x00403920` | 1 | yes |
| 0x4e7080 | `0x0054a3e0` | `0x0067a008` | `0x004bccb0` | `0x00536238` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054a3dc` | `0x0067a028` | `0x004bcc10` | `0x00536228` | `0x004ba110` | 1 | yes |
| 0x4e7080 | `0x0054aa94` | `0x0067b690` | `0x004cd720` | `0x005368bc` | `0x00403910` | 1 | yes |
| 0x4e7080 | `0x0054b9ac` | `0x006f9ac0` | `0x004ff850` | `0x00537304` | `0x0049a730` | 1 | yes |
| 0x4e7080 | `0x0054b974` | `0x006f99d0` | `0x004fdcb0` | `0x00537294` | `0x00403910` | 1 | yes |

The first helper call in each pass targets `0x00479030`, which is exactly a one-byte `ret`. All 31 slot-`+8` targets in `0x0049a490` are `0x00403910` (`xor eax,eax; ret`), and its result is not used. The 25 targets in `0x004e7080` divide into 21 calls to `0x00403910`, two to `0x0049a730` (`mov eax,2; ret`), one to `0x00403920` (`mov eax,1; ret`), and one to `0x004ba110` (`mov eax,3; ret`); every result is discarded. These bodies have no writes. All selectors resolve to their listed static objects through the PE data value, and each object vptr is installed by its listed constructor thunk. The current source models both passes as effect-free only for this proven static-table configuration; if the selector/object vptr can be replaced at runtime, direct dispatch must be retained.
