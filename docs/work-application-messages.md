# Application window-message filter and character queue

`FilterApplicationWindowMessage` reconstructs retail `0x00493550`; its
single nontrivial callee, `ApplicationCharacterQueue::QueueCharacter`, is
retail `0x00494f40`. These are game-specific routines: the filter reads a
game-state pointer and mode fields, and the callee implements a small ring
queue with a reentrancy word. They do not match a Win32, CRT, or identified
third-party interface. The window message itself is delivered by the normal
Win32 message loop and WndProc.

## Filter contract

The WndProc at `0x004c32c0` calls the Application slot at vtable offset
`+0x38` with `(HWND, UINT, WPARAM, LPARAM)`. That slot is `0x00493550`, which
returns `TRUE` with `ret 16` for every message except the selected `WM_CHAR`
path. `ECX` and the `HWND` are unused, so the free `WINAPI` declaration has the
same four-stack-argument ABI and cleanup.

The filter consumes a `WM_CHAR` only when global pointer `0x005544b0` is
non-null, the observed word at context `+0x60` equals `2` or the word at
`+0x64` equals `4`, and the pointer at `+0x2a84` is non-null. It then queues
the low 16 bits of **WPARAM**; the value is read from `[esp+0x0c]`, the third
stack argument. The filter returns `FALSE` even if the queue is busy or full,
so those cases still suppress default processing. The fourth argument,
LPARAM, is not read.

The `GameApplicationMessageContextView` models just these reached fields and
the embedded queue at `+0xdc0`. Its gap arrays express offsets; no complete
context size is asserted here. The context constructor at `0x004935b0`
initializes the mode words and clears `+0x2a84`, but calls `0x00479030` on the
embedded queue, which is a one-byte `RET`. The filter/queue path therefore
does not establish queue initialization. A separate startup caller allocates
`0x2b0c` bytes before invoking this constructor at `0x004935b0`; allocation
zeroing has not been used as a source-level constructor claim in this unit.

## Queue behavior

`ApplicationCharacterQueue` has 64 16-bit slots followed by the write index
at `+0x80`, read index at `+0x84`, and a busy flag at `+0x88`. The producer
returns immediately if busy. It computes the next write index with wrap at 64;
when that next index equals the read index, the queue is full and the input is
dropped. Otherwise it sets busy, writes the low word at the old write index,
advances/wraps the write index, and clears busy. One slot remains unused to
distinguish full from empty, so usable capacity is 63 code units.

The writer’s measured retail extent is 100 bytes (`[0x00494f40, 0x00494fa4)`),
and it matches 100% raw instructions. The message filter is 81 bytes
(`[0x00493550, 0x004935a1)`) and also matches 100% raw instructions after
expressing the three retail message gates separately. No constructor or
consumer is claimed by this unit.
