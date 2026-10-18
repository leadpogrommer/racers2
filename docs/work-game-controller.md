# Game-specific frontend startup

The engine GameManager is distinct from the game-specific object whose literal
name is `Game Manager`. The latter is called GameController here. Its global
storage starts at 0x005543f8, allocation size is 0x9be0, template at 0x005542c0,
and complete 21-entry dispatch starts at 0x0052f528. The template's flags are 2;
its packed object type is 0x900 (the helper at 0x00401410 shifts 9 left by 8).
The constructor, CRT template/construction/exit path and dispatch now pass VC6
object compilation. Initialize, Update and Remove still need complete source
and scene dependencies before linking this global into the main build.

Construction preserves the observed unwritten fields. It initializes two
0x14-byte name records at +0x134; two 0xac-byte pair groups at +0x1e4; three
arrays of 25 0x18-byte score records starting +0x374; forty blocks of 36
0x14-byte items with 0x2f4 stride starting +0xaa0; two four-word state records
at +0x809c; and eight player blocks with 0x344 stride starting +0x817c. The
last item block's records end exactly at +0x809c; its nominal padding overlaps
those state records, so the source models 39 padded blocks and a final item
array separately. All boundaries were calculated with a script and confirmed
against the raw LEA/add/store instructions. The item initializer at 0x00409190
returns its receiver without writing storage.

The actual initializer at 0x00407250 performs the media gate, screenshot path,
collision service and controller setup, frontend preferences, localization,
then creates managers including Menu Manager (0x0041c310, size 0x2b4) and
Secondary Manager (0x00426f20, size 0x4f8). The latter participates in the first
state transition through 0x004279b0. These are real startup requirements;
creating only an empty window would not establish the menu milestone.

The input setup calls pass the GameController pointer as the first identity
argument to FindOrCreateController. InputController fields +4/+8 are therefore
named owner key/index, and SetDevice has been renamed SetOwner. Device kinds
remain in the distinct binding records. ConfigureController (0x004b8040)
resets the per-controller device override, optionally installs two axis and
twelve button bindings, selects joystick mappings when one exists, and otherwise
uses the retail keyboard scan codes. RebindController and binding append helpers
preserve their three/four-argument thiscall cleanup contracts.

Frontend support now linked for measurement includes the registry language
preference and screenshot-directory setup, narrow/wide name copying and the
localization loader. The localization object is 0xd4 bytes: language suffix
buffer +0, count +4, path +8, largest file size +0x88, file buffer +0x8c, string
count +0x90, stride records +0x94, stride +0x98, UTF-16 data +0x9c, fallback
text +0xa0, and sorted key/index pairs +0xd0. Loading scans nine suffixes,
allocates for the largest language file and its lookup table; selection loads
or copies the chosen file, relocates string offsets in place, and builds the
sorted lookup. Original explicit CloseManaged followed by WinFile destruction
is retained. The default fallback text is `Not A String`. Proven strings and
function extents are in the reccmp CSVs.

The 0x0040b340 word initializer genuinely folds with another linked zero-store
helper. Its complete seven-byte body and exact decorated linker-map symbol are
verified in both PEs by tools/folded_functions.py. No artificial writes or
compiler/linker flag changes were added. The scene constructor and table stay
unlinked until their real initialize/update/remove paths close. No game or
runtime probe was executed for this batch.
