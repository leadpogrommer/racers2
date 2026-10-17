# Reserved static renderer storage

`rendererowner.cpp` reserves the renderer's evidenced `0xc7f2c`-byte prefix as
aligned, zero-initialized raw storage at retail `0x0058b140`. The selector at
`0x00549dfc` points to that same allocation. `GetStaticDisplayRenderer()`
provides the address for methods that access the retail singleton directly.
This allocation contains no replacement dispatch table, and no renderer C++
constructor runs yet. The current entry point does not invoke these methods;
real startup must placement-construct the completed renderer first.

This closes an ownership issue in the texture callbacks. Deriving the owner
from `record - offsetof(renderer, records) - resourceId * stride` is valid
only after `FindOrCreateTextureRecord` assigns a physical ID. The texture
record constructor at `0x004abe50` does not initialize that ID, and direct
default creation at `0x004d9f20` also leaves it unchanged. In zero-initialized
storage, default record 1 can therefore still have ID zero. The ID-derived
formula would return the wrong renderer address. Upload and postprocessing
now use the actual reserved static allocation, as the original code does.
Record release callbacks that read the selector use `g_displayRenderer`.

The storage size remains an evidenced prefix, not a claim about unobserved
class hierarchy or every possible field. Future layout extension changes the
single reserved allocation. Do not invoke renderer methods before construction
and full dispatch recovery. No game execution was used to investigate this.
