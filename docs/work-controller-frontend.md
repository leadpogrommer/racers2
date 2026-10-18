# Controller and frontend integration

This batch extends the menu dependency graph. The executable still uses the
temporary bootstrap: no menu, window, startup movie or game probe was executed.
The open GameController creation/update graph must link before the runtime
milestone can be attempted.

## Storage and defaults

GameController storage is now owned by `gamecontrollerstorage.cpp`, separately
from its still-staged constructor. The settings and session pointers refer into
that storage at +0x130 and +0x80ec respectively. Their original initialized
pointer values were read from the PE. The nine language suffixes at 53e3c4 are
inline four-byte arrays, rather than an array of pointers.

Two earlier globals duplicated controller fields. The window-message context
at 5544b0 is the controller's network application pointer (+0xb8), and the audio
settings context at 5544bc is its reverb manager (+0xc4). Both now access those
actual fields. Keeping separate zero-filled variables would prevent either
consumer from seeing startup assignments. The existing message-filter exact
match remains guarded by reccmp.

Recovered startup support includes controller defaults, two binding profiles,
world/track/reward flags, session reset, localized player names, eight default
car-file reads and player settings. The language policy calls Application slot
+0x7c, as the raw caller shows. The music helper uses the registry media path,
the selected world or menu/bonus name, and the existing audio service methods.

The player array starts at session +0x4c with 0x344-byte records. Special sound
indices 61 and 62 in the textbox sound selector resolve to the first two
players' +0x334 field. This is shared storage, not an independent sound flag.

## Text and images

The 0xa4-byte auxiliary object created inline in GameController initialization
is the glyph/string replacement manager. Its two-entry dispatch table at
52f57c provides initialization and release. It owns ten icon definitions and
ten localized replacement strings, including the two player names. The
manager pointer aliases GlobalOverlayManager +0x2494c (retail679b74).

The complete icon key occupies 16 UTF-16 words. The earlier projection
incorrectly interpreted key bytes +0x14/+0x18 as scale factors. Drawing now
uses the actual factors at +0x34/+0x38, and its unscaled record factor is 1.0.
Prefix matching also now compares each registered key against the incoming
text, in the order shown by the retail pushes.

Recovered resource loading covers the ten key-icon textures, the HUD/text-box
atlas, pause-menu images and localized art, and the character-builder atlas
and its two image sets. Individual texture reference acquisition/release order
and the resource-mode guards are retained. Large table-driven reconstructions
remain STUB pending further semantic review and rendered output.

The 0x68-byte ScreenPrinter has four overlay text records and a complete
21-entry GameObject table. It creates/releases records, applies the font,
clears text and converts narrow text through the shared conversion path.
The scratch buffer at 654e20 reserves 0x200 bytes before the subsequently
identified append scratch at 655020;
the original buffer capacity remains unproven. The copy has no bound in the
retail function, and the body matches exactly.

## Bodyparts and sprites

BodypartManager allocates 0x49c bytes and copies eight path buffers into
+0x58..+0x458. Its complete 21-entry table at 52f584 ends at 52f5d8. Slot +0x18
is 4047a0 (zero, RET4); a transposed address in early notes was corrected
against the raw pointer table. The initializer loads all eight category lists
and constructs model/texture paths. It stays STUB because allocation-failure
and helper edge behavior have not been fully verified.

The 300-entry overlay sprite pool now has a typed 0x58-byte record, allocation
and list movement, visibility, geometry, color, texture, clipping and list
setters. Textbox and widget recovery reuse these actual helpers. The separate
200-entry overlay text pool continues to use its 0x254-byte records.

Character part presets occupy 41 records at 570e08, with 0x28-byte stride.
The retail initializer writes 20 records inline, fills the next 20 through
467380, and clears the final record. The reconstruction retains those scalar
values as a readable table. The interpretation of the three vectors in each
record remains uncertain; the complete initialization remains STUB.

## Random state and library interfaces

The singleton returned by 4b99b0 contains two 0x14-byte float generators, two
0x0c-byte integer generators and three checkpoint words, for a 0x4c-byte extent
at 679e48. The pointer at 679e94 aliases that storage. Constructors seed from
the existing controller frame query. Its LCG constants differ from CRT rand;
the constants alone do not identify a particular external library.

The recording/network service uses genuine DirectPlay 8 interfaces. GUID
evidence and the SDK bridge are documented in `library-identification.md`.
The SDK compile/link check only emitted a PE; it was not executed.

## Match review

New exact bodies include selection-state construction, icon texture loading
and release, player-name replacement, glyph-manager registration/accessors,
character preset assignment, bodypart size, ScreenPrinter size/removal/clear,
controller font wiring, overlay font assignment/narrow conversion, full texture
regions/HUD release, and sprite initialization/visibility/allocation.

Four reviewed near matches were promoted: session reset 40abc0 (95.83%),
track query 40b0f0 (96%), reward query 40b1b0 (96%), and the random-state
accessor 4b99b0 (95.83%). Session reset writes
exactly the observed offsets and leaves session +4 untouched; its remaining
difference is address computation. The two bit queries retain unsigned
arithmetic, mask the shift count to the x86 low five bits, and return the same
0/1 result. Their remaining differences are instruction choice for the shift
count masking, not an unresolved branch or rounding change. The random-state
accessor retains the guard bit, constructor sequence, bare-RET exit callback,
cached pointer and return paths; its constructor bodies also match exactly.

Additional exact bodies include the sprite size/rectangle/UV/color/type/clip
helpers, text color/alpha/scale/line-height/clip/length helpers, and pause-menu
texture release. Both builders agree on their raw measurements. The vertex
position-offset getter and audio status getter genuinely fold to the same
four-byte `mov eax,[ecx+8]; ret` body. Both original extents, emitted bytes and
exact map symbols are checked alongside the four earlier folded aliases.

`check_scene_dispatch.py` verifies all 44 original targets and emitted pointers
in the bodypart, glyph-manager and ScreenPrinter tables. This checks wiring,
separately from callback behavior. Existing library-identification/tooling
tests remain; no game behavioral tests were introduced.
