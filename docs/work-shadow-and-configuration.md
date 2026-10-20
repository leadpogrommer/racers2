# Generated shadows and frontend configuration

Three new canonical units connect generated model shadows and recover display,
keyboard and input configuration used by Secondary Manager startup. Its complete
lifecycle, car-preview objects and foyer startup remain in progress; the
temporary bootstrap has not been replaced.

## Shadow generation

`ConfigureModelShadow` (`0x004a9630`) releases disabled shadows or requests their
creation, respecting the renderer's extended-texture-stage capability.
`GenerateModelShadowData` (`0x004c5bb0`) retains shared geometry, optional texture
references and separate reference masks. It chooses group two, falling back to
group one, and preserves the original 1,000-face limit and early-return ownership.

The generator reads actual vertex/index buffers through their existing dispatch
interfaces, computes normalized face normals, welds coincident vertices, removes
duplicate faces and constructs edge adjacency. If welded edges have more than
two incident faces, it rereads the geometry and retries once without welding.
Coplanar edges are removed, normals are compacted and remapped, and the final
0x24-byte edge records are emitted. Non-manifold edges produce separate records
per incident face. Temporary allocations are released in retail order.

Raw disassembly was necessary: the current Ghidra export incorrectly removes
the welding blocks as unreachable and confuses vertex-buffer state. The copied
vertex count and triangle index base are separate counters. A failed vertex
lock advances the index base but does not add copied vertices. The index-buffer
format guard skips both. Retail float subtraction rounding and unordered x87
comparisons were reviewed; exact floating-point code generation remains under
review. The large reconstruction deliberately remains `STUB`.

## Menu configuration

`SecondaryManager::EnumerateDisplayModes` (`0x0042a870`) builds the fifty-entry
width/height/depth list at owner +0x264. It filters depth and resolution using
reported device memory, sorts and deduplicates entries, and preserves the
original full-list insertion/drop behavior. The original 0x4f8 owner size and
all following offsets remain asserted.

`DetectKeyboardLanguage` (`0x0042ab00`) parses the eight hexadecimal characters
returned by GetKeyboardLayoutName, defaults to language two and selects the
retail keyboard-label language, including the English sublanguage distinction.

`SecondaryManagerMediaState::InitializeBindings` (`0x0042af70`) detects keyboard
and joystick slots and configures the existing controller manager. It appends
the original keyboard, joystick and mouse binding records; mouse activation
still depends on the enabled-slot mask. `EnableSlot` is `0x0042b1e0`. The two
initialized control arrays at `0x005305e0` and `0x0053060c` match retail data.
The controller owner key is the Secondary Manager pointer.

## Validation and limits

Both SP5 builders link 353 units and agree on all 1,601 measured rows: 482 exact
functions, nineteen reviewed near matches and 1,100 STUB rows. All twelve dispatch
inventories, twelve folded aliases and 267 annotated globals with zero issues
pass in both builds. No tooling logic changed; existing tooling tests passed in
the preceding loading-screen batch. The six reviewed code extents are recorded
in function-sizes.csv.

| Function | Raw accuracy in both builds |
| --- | ---: |
| ConfigureModelShadow | 76.67% |
| GenerateModelShadowData | 9.38% |
| EnumerateDisplayModes | 37.43% |
| DetectKeyboardLanguage | 48.54% |
| InitializeBindings | 44.80% |
| EnableSlot | 92.31% |

All six remain `STUB`; algorithm recovery and successful compilation do not
establish behavioral equivalence. Shadow generation is split into readable
helpers, so its instruction comparison also reflects that outlining.

Logs are `build/msvc/shadow-config-final-build.log`,
`build/msvc/shadow-config-final-compare.log`,
`build/msvc/reports/shadow-config-final-datacmp.txt` and the corresponding CMake paths.
All dispatch checks are in `build/msvc/shadow-config-final-dispatch.log`.
Unlinked worker annotations still produce missing-symbol notices; they are not
counted as canonical functions. No game execution or asset probe was run.
