# Startup, selectors and scene animation

The recovered entry now links through Application, platform services, renderer,
input, audio, GameController, Secondary/Foyer managers and the first menu's
resource loads. `bootstrap.cpp` forwards all WinMain arguments to `RunGameInstance`.
Menu rendering has not yet been verified at this checkpoint.

## Recovered paths

- Character construction, body/head resources, preview modes, material callbacks,
  camera transforms, sound and collision effects.
- Foyer scene parsing, actions, NPC text, manager transitions and asset caching.
- BSA animation caching, four animation players, pose blending, skeleton updates,
  CPU skinning and vertex-buffer upload. Player indexing retains the original
  x87 rounding/control-word behavior and observed unordered comparisons.
- Both selectors' initial loads and cleanup, shared grids, sprites, preview
  models, eight CarDisplay objects, lighting, brick catalogues and model caches.
- GameController construction, initialization, update/removal and resource hooks;
  actual idle NetworkAppLayer and Battle startup behavior.

Renderer storage previously lacked its actual CRT constructor. Recording-service
storage similarly lacked its CRT caller and wrote a field at the wrong offset.
Both now use the evidenced startup hooks. The default scene audio target also
has its actual constructor, complete dispatches and exit registration.

Raw callback review corrected TRUE/FALSE and stack-cleanup errors in Foyer/task
tables, omitted scene-action preparation callbacks, action14 scaling, unconditional
setup initialization and the Sparky menu fade argument. Numeric retail calls and
absolute data references in scene actions were replaced with reconstructed APIs.
Bodypart lookup formats and three startup movie names now use retail spaces.

Working names describe inferred behavior; they are not recovered original symbols.
Large bodies and unresolved behavior retain STUB annotations. Small register or
scheduling differences did not receive repeated matching passes.

## Explicit deferred work

Forty ABI-correct, fail-fast boundaries identify unfinished later race/network
actions by original address. No original-code calls or successful placeholder
returns substitute for their behavior. Static caller/state review found none on
idle cold boot through initial menu construction. Network state3 skips host/client
handlers; Battle state8 skips gameplay states0..6.

Automatic attract mode reaches a deferred race path after 45 simulated idle
seconds. There is no protection against selecting an unfinished later action.
This checkpoint supports attempting the initial menu, not claiming a playable race.

## Static validation

Python and CMake/NMake both link 465 units. Their 2,713 measured rows agree:
588 exact FUNCTION claims, 31 reviewed near matches and 2,094 STUB rows. Twenty
complete folded aliases pass independent byte/map verification. The twelve
dispatch checks pass in both builds, including 1,279 scene callback pointers;
the seven-slot renderer cache replaces its former incomplete five-slot inventory.
The existing 21 tooling/library tests pass. No game-behavior tests were added.

Data comparison checks 476 globals. Its one-to-one string matching flags five
equivalent pointer aliases in two variables: three NPC empty names and two red
peg texture paths. Independent original/rebuilt PE reads confirm identical bytes,
including NUL, and shared pointers within each group. Other compared data agrees.
Typed character/car item records preserve their original 68-byte strides and
represent their string fields as pointers. The alias findings were not suppressed.

Local evidence is under ignored `data/session-tools`: `menu-startup-*` build and
comparison logs, `data-string-alias-review.json`, callback inventories, per-function
reviews and `deferred-first-menu-audit.txt`. Reports remain in each build directory.
Library identification and its tooling remain in place. These are subset counts,
not whole-game completion percentages.

## Launch requirements

Archive initialization executes before WinMain, so the working directory must be
the installation on a non-CD drive. The original CD gate separately requires
`CDpath` to identify a CD-ROM with `language/export.Swe`. `moviespath` prefixes
the original `Game Data\\Movies` and music paths.

The supplied MODE2/2352 image was parsed using its 2048-byte Form1 payloads and
Joliet names. All 94 extracted files were verified against the disc by size and
SHA-256. The ignored tree is `data/media/lego-racers-2-disc`. This workspace maps
it as Wine E: (CD-ROM), with 32-bit game registry values `CDpath=E:\\` and
`moviespath=E:\\install\\`. Existing D: was preserved. Media and generated outputs
are not repository content.

## Menu milestone debugging after the static checkpoint

The full startup graph justified a menu launch attempt. With the real installation
working directory and CD media configuration above, three attempts reached
progressively later startup work but did not produce a verified rendered menu.

The first fault exposed two renderer-packet errors: slot zero of the synthetic
startup defaults was left as a sentinel, and overlay drawing wrote its texture
index into the defaults count rather than the first slot. Text rendering also
used color as the texture index and misplaced rotation/blend fields. A separate
DirectSound initialization failure came from supplying a format pointer while
creating the primary buffer; retail supplies a zeroed primary-buffer descriptor
and applies the PCM format afterward.

The next failure exposed controller-field mixups in menu resource loading:
`+0x88` is the HUD owner, `+0x90` is the menu resource table, and `+0x98` is the
NPC text manager. The corrected callers now use the same receivers as retail.
The third attempt reached car-selector sprite creation, where a null texture
path revealed missing CRT initializers for the later sprite and item records.
Their static PE contents alone do not describe the initialized runtime tables.
Both original initializers are now restored; the record counts and null-path
contracts were preserved.

A bounded static review also corrected overlay glyph positioning/typewriter
indices, Foyer asset names, nested state pointers, transform interfaces,
animation entry counts and pending-action completion. These changes do not
establish that the menu renders correctly; the milestone remains open.

Ignored evidence includes `menu-first-launch.log`, `menu-second-launch.log`,
`menu-third-launch.log`, `menu-firstchance.log`,
`secondary-resource-receiver-fix.txt`, `stage-plan-crash-audit.txt`,
`audio-provider-failure-review.md` and `foyer-startup-retail-review.json`.
WineDbg's first-chance exception mode provides the actual failing registers and
stack despite its separate legacy-PDB symbol-reader errors. No game-behavior
tests were added.

## Working menu checkpoint (2026-10-20)

The user confirmed that the menu works, colors are correct, and sound works.
The captured Sound submenu has the original orange LEGO background, blue panel,
logo and readable text. Keyboard navigation and confirmation reach New Game.
This satisfies the rendered-menu milestone; it does not establish a playable race.

Two further failures were resolved from retail instructions. The font atlas now
copies glyph rows from the bottom occupied row upward while writing destination
rows forward, and its horizontal byte offset is scaled once. The pause detector
now skips its entire input scan outside the eligible game state, allowing menu
controls to receive button edges and preventing a race pause menu during language
selection. The recording query also uses the actual simulation-frame field.

The white backdrop was missing resources, not an intended color scheme: all
sixteen background texture IDs were -1. `EnsureMipSidecar` appended `.MIP` to
`.TGA`, requesting nonexistent `BackOrange1.TGA.MIP`; retail replaces the
extension and loads `BackOrange1.MIP`. Corrected normal-map and cube suffixes,
indexed BGR palette conversion, cache availability/reference fields and the
retail texture-filter reset complete this reviewed batch.

Both 465-unit builders agree on 2,715 measured rows (588 exact, 31 reviewed near,
2,096 STUB). All 619 implemented claims, twenty complete folded aliases and twelve
dispatch inventories per build pass. One additional folded STUB at 0x0041dd50
has no separate comparison row. Data checks cover 477 globals; the same five
byte-equivalent string aliases remain flagged in two variables and were checked
against the current PE/PDB. Twenty-one existing tooling/library tests pass.
No game-behavior tests were added.

New Game reaches the unfinished `StartNewMenuSession` boundary at 0x00411670.
Its compiled but runtime-unverified reset recovery is paused at the user's request
and preserved separately in ignored `data/paused-new-game/session-reset.patch`.
The checkpoint retains the explicit boundary. Menu settings beyond those observed,
attract/race entry and gameplay remain unverified.

Evidence: `data/session-tools/menu-mip-retry-window.png`,
`menu-checkpoint-*-{compare,datacmp}.log`, `font-atlas-orientation-review.txt`,
`menu-input-guard-fix.txt`, `background-resource-watch.log`,
`texture-path-cache-retail-review.json` and `data-string-alias-review.json`.
