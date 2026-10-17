# MemoryHeap reconstruction

`MemoryHeap` is the process-wide arena used by thousands of game allocations
and by the mode-3 file manager. Its implementation is now in the runtime
sources; the allocator's original library provenance remains unresolved. The
disassembly establishes arena behavior, not whether this code was written for
the game or imported from another component.

The observed object occupies `0x24` bytes. Retail code initializes fields at
`+0x00` (arena start), `+0x04` (arena end), `+0x08` (a second copy of the
arena start), `+0x0c` (unknown flag), `+0x10` (capacity in bytes), `+0x14`
(free 16-byte units), `+0x18` (free-block count), `+0x1c` (operation-entry
diagnostic state), and `+0x20` (CRT allocation selector). The heap block
header is `0x10` bytes: forward and backward free-list links, a unit count
including the header, and an untouched fourth word. Allocated pointers are
one header past the block start.

At `0x004a3f80`, construction asks the application for the requested arena
size, calls `AllocateAlignedMemory(size, 16)`, and reduces the request by
`0x2000` after each failure. It initializes a single free block, then probes
the allocator by allocating `capacity - 0x810` bytes and freeing the result.
The global at `0x00578c9c` becomes true only if that probe allocation succeeds.
The arena-size leaf is represented by `GetApplicationHeapReserve()` and the
allocation-failure policy by `IsApplicationDevelopmentMode()`. The runtime
now constructs the application singleton and dispatches through its complete
table, so the arena request is reached through the real application instance.
The constructor remains `STUB` because its measured body is below threshold
and semantic equivalence is unresolved. The aligned allocator is the real platform wrapper at
`0x004e7b90`; no success-returning fallback is used.

The list is address-ordered and coalesces neighboring blocks on free. Forward
allocation starts at the rotating cursor; backward allocation searches from
the tail. Both paths charge the allocation header in 16-byte units. The
near-full startup probe exercises the forward split and free/coalesce path.
The recovered free-list core is readable and retains the observed branches,
but allocation, free-list coalescing and forward-search remain `STUB`: their
current raw scores are low, and their full semantic equivalence has not been
established. Cursor adjustment is a reviewed semantic near-match at 95.83%:
it retains the retail cursor interval and tail-selection branches, with the
remaining difference in generated control flow. Retail assumes a non-null free-list head in
`FreeBlock` and `UpdateCursor`, and assumes a non-null starting cursor in
`FindForward`; the source preserves those assumptions rather than adding
empty-list recovery. Allocation wrappers call the recovered diagnostic
interfaces on failure (`ReportFatalError` in development mode and
`PrintDiagnostic` for the byte-count message).

The singleton accessor at `0x004a3ed0` uses pointer `0x00578ca0`, guard byte
`0x00578c60`, and object storage at `0x00578c70`; it registers a no-op exit
callback at `0x004a3f20`. The free-list head, tail, arena start, arena byte
capacity, cursor and initialization-probe result are at `0x00578c58`,
`0x00578c5c`, `0x00578c64`, `0x00578c68`, `0x00578c98` and `0x00578c9c`.

Retail extents, including each final `ret`, are:

| Address | Bytes | Recovered behavior |
| --- | ---: | --- |
| `0x004a3ed0` | 65 | Lazy singleton accessor |
| `0x004a3f20` | 1 | No-op exit callback |
| `0x004a3f30` | 33 | Operation-entry diagnostic state |
| `0x004a3f60` | 32 | Operation-leave diagnostic state |
| `0x004a3f80` | 177 | Arena construction and startup probe |
| `0x004a4040` | 8 | Sets the unknown `+0x0c` flag |
| `0x004a4050` | 126 | Forward allocation wrapper |
| `0x004a40d0` | 126 | Backward allocation wrapper |
| `0x004a4150` | 57 | Free wrapper |
| `0x004a4190` | 23 | One-argument allocation facade; tag is `"Memory for New"` |
| `0x004a41b0` | 18 | Free facade |
| `0x004a41d0` | 375 | Core allocation and splitting |
| `0x004a4350` | 301 | Ordered insertion and coalescing |
| `0x004a4480` | 57 | Cursor adjustment |
| `0x004a44c0` | 42 | Circular forward search |
| `0x004a44f0` | 25 | Backward search |
| `0x004a4510` | 30 | Free-block header initialization |

The isolated build links the application-policy, platform-memory and
diagnostic units. The no-op exit callback, leave-operation check, setter, pool free wrapper,
allocation/free facades, backward search and block initializer are exact
matches after recording their verified retail string mappings. Other
unfinished functions remain `STUB`; the
JSON raw scores below include these stubbed reconstructions and are not
equivalence claims.

| Address | Raw accuracy | Status |
| --- | ---: | --- |
| `0x004a3ed0` | 60.00% | `STUB` |
| `0x004a3f20` | 100.00% | `FUNCTION` |
| `0x004a3f30` | 85.71% | `STUB` |
| `0x004a3f60` | 100.00% | `FUNCTION` |
| `0x004a3f80` | 82.54% | `STUB` |
| `0x004a4040` | 100.00% | `FUNCTION` |
| `0x004a4050` | 79.17% | `STUB` |
| `0x004a40d0` | 79.17% | `STUB` |
| `0x004a4150` | 100.00% | `FUNCTION` |
| `0x004a4190` | 100.00% | `FUNCTION` |
| `0x004a41b0` | 100.00% | `FUNCTION` |
| `0x004a41d0` | 67.84% | `STUB` |
| `0x004a4350` | 79.67% | `STUB` |
| `0x004a4480` | 95.83% | `FUNCTION` |
| `0x004a44c0` | 73.68% | `STUB` |
| `0x004a44f0` | 100.00% | `FUNCTION` |
| `0x004a4510` | 100.00% | `FUNCTION` |
