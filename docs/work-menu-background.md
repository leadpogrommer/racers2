# Menu backdrop and startup supervisor

The canonical build now links the menu backdrop's resource ownership, color
selection and crossfades, plus the KillSphere supervisor lifecycle required by
GameController construction. Full Secondary Manager and GameController startup
remain staged. Bootstrap is still the entry point; this is not a rendered-menu
milestone.

## Backdrop resources and transitions

The 0x3c-byte MenuBackground owns eight sprites: four current-color quadrants
and four previous-color quadrants. LoadControllerBackgroundTextures uses flag
0x40 and fills event records 233 through 248 with the Blue, Red, Yellow and
Orange `Game Data\Frontend\Background\PC\Back<Color>1..4.tga` textures. Release
clears the flag and releases all sixteen texture references.

Mode two selects a color from the actual menu and session states. A category
change retains the previous category and restarts the half-second transition.
Update copies the texture/UV records into the sprites and applies opposite alpha
ramps. Alpha conversion preserves FISTP under the current rounding mode. Hidden
categories skip alpha arithmetic, matching the original branch structure.
Unordered completion comparisons and the original uninitialized initial tile/UV
words are retained. Release and the destructor have distinct retail behavior:
Release frees sprites and textures; the destructor only releases loaded textures.

Functions are 0x4267c0, 0x4267e0, 0x426800, 0x426820, 0x426a30, 0x426b60,
0x426bb0, 0x426bd0, 0x426c00, 0x436e10 and 0x436f20. The constructor, SetMode,
Hide and texture release match exactly. Remaining routines stay STUB; the large
category switch and crossfade code require further review despite complete
source branches.

## KillSphere supervisor lifecycle

The 0x258-byte object contains two 64-pointer arrays at +0x58 and +0x158. Its
constructor retains GameObject type 0x800 and the supplied template. Initialize
clears both arrays then calls the base initializer. Remove walks both arrays,
invokes each present object's remove callback, then calls base removal. It does
not destroy or clear those entries. The allocation-size helper returns 0x258
and matches exactly.

The full 21-slot retail table at 0x535d18 is represented by an explicit ABI
struct, including the size callback at +0x50. The scene inventory verifies all
targets and rebuilt pointers. Four functions have reviewed extents: 0x48e510
(6 bytes), 0x4b5460 (39), 0x4b5490 (37), and 0x4b54c0 (69). Constructor and
lifecycle methods remain STUB below the near-match threshold.

## Validation

Both pinned SP5 builders compile/link 356 units. Their 1,616 measured rows agree
on scores/statuses: 487 exact functions, nineteen reviewed near matches and
1,110 STUB rows. Five new exact helpers were promoted after measurement. All
twelve dispatch checkers pass in both PEs; the scene check now covers 167
pointers. Twelve complete folded aliases pass. Data comparison has zero issues
across 267 globals; the fade constant's bytes are separately checked through
both linker maps. All 21 retained tooling tests pass. No game behavior test,
asset probe or runtime run was performed.

Logs: `build/msvc/backdrop-supervisor-build.log`,
`build/msvc/backdrop-supervisor-final-compare.log`,
`build/msvc/reports/backdrop-supervisor-datacmp.txt` and corresponding CMake paths;
dispatch, tooling-test and fade-storage checks are in
`build/msvc/backdrop-supervisor-dispatch.log`, `build/msvc/backdrop-supervisor-tooltests.log`
and `build/msvc/backdrop-fade-storage.log`. Unlinked worker annotations may still
produce missing-symbol notices and are outside the canonical measured count.
