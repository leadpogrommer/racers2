# Playable rebuild: current blockers

Earlier runtime checks constructed the application singleton, loaded its
per-user display configuration, initialized the mode-3 archive reader and exited
cleanly under Wine. The executable still uses a temporary `WinMain`; it has not created
a window, initialized a renderer, shown a menu, or entered gameplay. The next
milestones are complete renderer initialization, connecting DirectInput, audio
services, the message loop, and the game managers needed to reach a playable
race.

## Application and startup

The application singleton at `0x0049a760` constructs through `0x0049a610`,
loads `C:\users\ilya\Documents\Games\LEGO Racers 2\Saved Games\LR2.cfg`,
and registers the observed exit callback.
The two 38-slot dispatch tables are emitted as explicit ABI tables. The
`tools/check_application_dispatch.py` verifier compares all 76 emitted callback
pointers against the retail table targets in both PE builds. This confirms
binary wiring, not exact method bodies, source class hierarchy, or virtual
function equivalence. See [application ABI](work-application-abi.md) and
[runtime startup](work-application-runtime.md).

The application configuration file is a 16-byte record with marker
`0xc0f1c0f1`, width, height and bit depth. Startup runs the native-file branch
while loading it. A full Wine startup trace confirms the per-user config path
was accessed; save behavior and the settings dialog have not been exercised
at runtime. The dialog resource and three icon resources are extracted from
the pinned executable into ignored `build-resources/retail.res` (five
resources, 7,022 payload bytes) and linked by both builders when present. See
[configuration recovery](work-application-config.md),
[dialog behavior](work-application-dialog.md), and
[resource extraction](work-resources.md).

## Shipped asset loading

The installation contains GAMEDATA.GTC (135,813,120 bytes), FILELIST.INF
(784,312 bytes), and COMPRESS.INF (43,740 bytes). The mode-3 startup path reads
the complete metadata files and opens the archive read-only. The shipped
FILELIST has 5,767 records of `0x88` bytes; COMPRESS has 2,187 records of
`0x14` bytes. Their physical archive ranges are contiguous and end at the
archive size. Music and movies are also present in extracted directories, but
most ordinary game assets require the archive. There is no root GameData.Dat;
retail redirects that name to GameData.gtc and enables compressed reads.

An ignored asset probe now reads three real archive entries through `WinFile`
and `MemoryHeap`: a 646-byte text file, a 349,562-byte texture spanning three
logical blocks, and a 6,977-byte late archive text file. The probe exits 0 and
the two text outputs have expected printable/CRLF structure. This exercises
selected compressed reads and late-file seeking. It does not verify rendered
pixels or compare the texture with an independent retail decoder. Several
archive, allocator, and decoder bodies remain below the near-match threshold;
the pool's library provenance is still unresolved. See [file recovery](work-file-facade.md),
[memory pool recovery](work-memoryheap.md), and
[decoder support](work-runtime-support.md).

## Display and game loop

The real display initializer is `0x004ac130`; it creates the Direct3D8
interface, selects an adapter/mode, creates a device, and configures further
rendering subsystems. The static renderer owner and adapter/mode layout are
documented in [renderer adapter research](work-renderer-adapters.md). The full
initializer now has compiled source, still under review at 22.87%
raw similarity. The renderer storage and selector are now reserved at the evidenced layout.
The constructor and complete dispatch tables now compile and their pointers pass static verification; no validated display-startup path is connected. Texture-format
enumeration and
selection, pixel-format descriptions, material colors, clear/present operations,
depth/blend state controls and ordered cache insertion now have recovered
source. The texture-state dispatcher, resource factory, buffer factories, transforms,
viewports and stream setup now have source bodies. Owner integration still
requires record construction and full main-table wiring. Default texture
creation/upload, resource initialization, animation callbacks and draw-plan
resolution now have compiled source. Texture-loading helpers, mip maintenance, sprite submission and immediate
drawing now have source. Scene startup, reset/resource restoration, packet submission, GUI rectangles and all eight quad-packet variants now have compiled source. Constructor/table integration, lookup submission and terrain drawing remain in recovery; these paths have not been run.
The [initialization dependency audit](work-renderer-initialization.md) lists
those concrete prerequisites; no incomplete renderer singleton is instantiated.
DirectInput now has a complete
source path for keyboard, mouse and joystick initialization, including axis
handling and force feedback. The three device tables and service table pass
verification of all 80 emitted pointers; all 307 data-format entries also match
retail. Input initialization still needs the real window and module handles
from startup and has not been runtime-tested. The remaining renderer/startup
dependencies keep the readiness gate from completing. Native-window helpers
and the settings dialog are present, but no
runtime window or frame has been observed. See [native window notes](work-nativewindow.md)
and [input service recovery](work-input-service.md).

The pinned VC6 toolchain lacks DirectX8 interfaces and imports. Renderer
sources require a local configuration generated by `tools/directx8.py` using
either the Wine headers/import archives or a genuine local DX8 SDK. Both the
Python and CMake builds consume that config. The reusable header uses actual
DX8 interfaces; it does not define replacement COM vtables. Wine, Xvfb, and
Mesa are installed, but a real rendering session is not yet validated. See
[DirectX setup](work-directx8.md).

The retail WinMain is at `0x004c3040`. Its readiness gate depends on service
globals `0x00549dfc`/`0x00549e80` and helper `0x004b7c50`; audio initialization
uses `0x004af2f0` and constructor `0x004af380`. The main loop reaches update
and frame work at `0x004a3aa0`. These services and their startup ordering must
be recovered and connected before replacing the temporary `WinMain` with the
retail entry path. No game or probe execution is planned during small
implementation batches. Reserve the next runtime check for a significant
milestone expected to render the menu or enter gameplay, and track a rendered
menu before claiming a playable milestone.

## Controller and clock recovery

The readiness gate also initializes the controller/replay manager selected by `0x0054a330`, backed by `0x00679d48`. Controller records, bindings and recorded-input access now have compiled source, along with the actual WinMM periodic-event and named-mutex adapter. The manager, eight-slot RDTSC clock and readiness helper now have compiled source. Both rebuilt controller/clock tables pass verification of all twelve callback pointers. Polling timers are not started by the temporary entry point; the readable binding mapper still requires precision and aliasing review. Both main renderer tables and the three lookup tables now pass all 212 rebuilt-pointer checks. Their large callback bodies remain under review and have not been run.

## Fixed-step and object-registry integration

The five-field scheduler at `0x006ff520` now has compiled reset and update-decision methods, using controller elapsed time and the simulation frame rate. The game-manager singleton, object classification/update/removal loops and renderer animation sweep also compile. Its initial registry points to the 32-pointer backing array; allocation, insertion, removal and initialization now compile; concrete menu-object creation is still being recovered. Renderer resource cleanup now has source for the platform teardown chain. These additions do not replace the temporary entry point or establish a rendered-menu milestone. See [scheduler](work-simulation-scheduler.md), [registry](work-game-manager.md), [terrain limitations](work-renderer-terrain.md), and [resource shutdown](work-renderer-resource-shutdown.md).


## Frame support and entrypoint staging

The current combined build links the real message-loop implementation, frame
orchestration, GameObject lifecycle and rendering, global pause, contact-cache
ownership, collision queries, RenderFrame, global overlay/Menu Manager, audio
service/providers/music support and the font atlas loader. Both SP5 builders
pass; 201 annotated globals have zero data-comparison issues, and all 144 audio
table pointers pass the independent wiring check. Large recovered routines
remain under review. See [integration notes](work-main-integration.md).

GameController initialization now has staged source for the complete creation
sequence. Its update/removal, concrete scene-manager constructors and renderer
callbacks are still being connected. Secondary Manager's media/input transition
also depends on the actual controller-recording service. Startup movie support
is still incomplete. These dependencies must close before replacing the
bootstrap and attempting the menu milestone. No runtime/game probe was executed
for this integration batch.


## Frontend resources and shared menu support

The combined build now links controller defaults, session reset, default player
cars, bodypart lists, key-icon/string replacement, ScreenPrinter, shared menu
sprite allocation/geometry and the HUD/pause/builder texture paths. The shared
random-state singleton and font metrics are also present. Both builders pass,
and 217 data globals have zero comparison issues. See
[frontend integration](work-controller-frontend.md).

GameController's complete creation/update path remains staged while concrete
widget, foyer, textbox, network and other scene dependencies close. The retail
entry path has not replaced bootstrap. No runtime check was performed; the
next run remains reserved for a build expected to open and render the menu.

## Menu panel ownership

The six-panel owner, panel transitions, nine-slice sprite geometry and shadows,
widget/menu clipping, text bounds, and builder-texture ownership now compile
and link in both builds. Eight new helpers match exactly. The typed 0x108-byte
panel and 0x28-byte owner replace the earlier string-buffer interpretation of
SecondaryManager +0x84. Full frontend widget construction, menu state changes,
recording setup and scene startup remain open. The bootstrap is still active;
no menu, startup or asset probe was executed. See [panel notes](work-menu-panels.md).

## Shared widget callbacks

The base, volume/range controls, choice selector, integer selector, toggle,
label selector and text group now have linked constructors and full six-slot
tables. Their keyboard/mouse, bounds, clipping and release callbacks have
recovered source; all 48 pointers pass static verification. Text append and
overlay scaling dependencies also link. These controls still need the
remaining frontend construction/state connections before a credible rendered
menu check. No game or probe was executed; see [widget notes](work-menu-widgets.md).

## Controller service and scene callbacks

The complete recording-service table, queued DirectPlay handlers, nested
textbox callbacks, pause detector, media state and common widget ownership now
link. Scene callback return values were corrected against the original bytes.
Foyer loading, remaining frontend controls, startup movies and complete
GameController startup still need integration before replacing bootstrap.
No runtime probe was executed. See [batch evidence](work-controller-recording.md).

## Track, network status and character preview callbacks

Track selection, network status, character appearance selection, shared
preview camera/light ownership and character hiding now link. Complete widget
initialization, remaining frontend controls, Foyer events, startup movies and
controller startup still need integration before replacing bootstrap.
No runtime probe was executed. See [batch evidence](work-frontend-preview.md).

## Character preview initialization and foyer event updates

The full character-preview initializer, arrow ownership, idle animation/collision
setup and seven foyer event callbacks now link in the 318-unit build. Eight new
helpers match exactly. Resource parsing, remaining controls, startup movies and
controller startup still need integration before replacing bootstrap. No runtime
probe was executed. See [evidence and corrections](work-foyer-events.md).

## Startup movie graph and renderer integration

The 325-unit build now links the MPEG graph, SDK-based texture renderer,
startup movie sequence and display/heap preparation. Both builders agree on
1,475 measured functions; inherited DirectShow library code is reused. Full
frontend widget construction, Foyer loading and GameController startup still
need integration before replacing bootstrap. No runtime probe was run. See
[movie integration and remaining review](work-startup-movies.md).
