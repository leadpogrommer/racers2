# Application singleton research

This note records the evidence around the singleton at 0x00578648, its
constructor at 0x0049a610, and the virtual table installed by that
constructor. It is based on /tmp/lr2-disasm.txt, the PE data and strings,
call sites in the executable, and the headless exports at
`data/ghidra-export/0049a610.c`, `0049a760.c`, and `0049b5d0.c`.
Decompiler output was treated as a navigation aid; observations below were
checked against instructions and callers. “Application” is a working label
for this LR2 executable, not a recovered source class name.

## Singleton and constructor

0x0049a760 is a lazy accessor returning 0x00578648. It tests bit zero of
guard byte 0x00578774; on first use it sets that bit, calls 0x0049a610 with
the static object's address as this, and registers 0x0049a7a0 with
0x00519f5e. The accessor is 51 bytes. The constructor is 46 bytes
(0x0049a610 through the ret ending at 0x0049a63d) and installs the primary
vtable pointer 0x00534994 at offset zero.

The constructor writes the following fields before calling its initializer:

| Offset | Initial value | Evidence |
| --- | ---: | --- |
| +0x00 | 0x00534994 | Primary vtable pointer. |
| +0x04 | 640 | Initial width; initializer writes it again. |
| +0x08 | 480 | Initial height; initializer writes it again. |
| +0x0c | 32 | Initial bit depth; initializer sets a fallback of 16. |
| +0x10 | 0 | Read by method 0x0049a6a0; no stronger meaning is established. |

The accessed span only proves an object extent of at least 0x14 bytes. The
exact object size and any fields beyond +0x10 remain unknown. In particular,
the nearby guard at 0x00578774 does not prove the static object size.

The constructor directly calls 0x0049b5d0 (425 bytes), which resets width and
height to 640x480, sets bit depth to 16, and performs display/config setup.
It constructs a path from the current user-data path, "Saved Games", and
"LR2.cfg". The strings at 0x00549798 and 0x005497a8 are respectively
"NextGen Game" and "LEGO Racers 2"; the primary table's title method returns
the latter. If the config file is found and its marker validates, the
initializer copies its width, height, and bit-depth values into offsets
+0x04, +0x08, and +0x0c. Thus the initial 32-bit depth is not sufficient
to infer the final runtime value.

The atexit callback 0x0049a7a0 (11 bytes) only changes the object's vtable
pointer to 0x00534a2c. That second table is therefore a teardown-installed
table, not the table installed during construction. The callback does not
otherwise release resources or clear the singleton guard in its observed
body.

## Primary virtual table

The constructor installs 0x00534994. The following DWORD targets were
decoded from PE data, in four-byte slots. The table contains 38 entries
through offset +0x94; this is the observed run of targets, not a claim about
the complete class ABI or source declaration.

| Slot | Target | Slot | Target | Slot | Target | Slot | Target |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| +00 | 0049b5d0 | +04 | 0049b780 | +08 | 0049a6b0 | +0c | 0049adb0 |
| +10 | 004038d0 | +14 | 00403910 | +18 | 00403910 | +1c | 00403910 |
| +20 | 0049a640 | +24 | 00403920 | +28 | 004038d0 | +2c | 0049a7d0 |
| +30 | 00403910 | +34 | 0049a6a0 | +38 | 00493550 | +3c | 0049a7f0 |
| +40 | 00403910 | +44 | 00479030 | +48 | 00479030 | +4c | 0049a7e0 |
| +50 | 0049a7b0 | +54 | 00403920 | +58 | 004038d0 | +5c | 0049a650 |
| +60 | 0049a660 | +64 | 0049a7c0 | +68 | 00403910 | +6c | 00403910 |
| +70 | 0049a690 | +74 | 00403920 | +78 | 0049a670 | +7c | 00473140 |
| +80 | 0040ec90 | +84 | 0049b5a0 | +88 | 00403930 | +8c | 004038e0 |
| +90 | 0049a800 | +94 | 00403910 |  |  |  |  |

Many entries point to shared small routines (0x004038d0, 0x00403910,
0x00403920, and related addresses). Their likely role in an inherited
interface is not established, so they are left as raw targets. WinMain and
WndProc call sites provide stronger evidence for several slots:

| Slot | Call-site evidence and observed behavior |
| ---: | --- |
| +0x18 | WinMain calls with no explicit arguments; a nonzero result selects the idle/update path. Target is 0x00403910, whose behavior is still unclassified. |
| +0x20 | WinMain passes MSG*; the result gates TranslateMessage and DispatchMessageA. Target 0x0049a640 returns 1 and uses ret 4, so the observed implementation accepts one four-byte argument and always permits normal dispatch. |
| +0x24 | WinMain calls without explicit arguments; the result selects blocking GetMessageA versus nonblocking PeekMessageA. Target is 0x00403920, not yet semantically identified. |
| +0x2c | Target 0x0049a7d0 returns a pointer to "LEGO Racers 2"; WinMain passes it as the title argument to FindWindowA, with class "Saracen II". |
| +0x34 | Target 0x0049a6a0 returns the DWORD at this+0x10. This is a direct field getter; the field's meaning is unknown. |
| +0x38 | WndProc calls target 0x00493550 with (HWND, UINT, WPARAM, LPARAM) and returns early if the result is zero. Its behavior is not recovered here. |
| +0x44, +0x48 | WinMain calls these with no explicit arguments during startup; targets are both 0x00479030. Their meaning is unknown. |
| +0x5c | Target 0x0049a650 returns 0xffff with no explicit arguments. A private-message path calls this slot on a different manager, so do not infer that the app slot has the same contract. |
| +0x60 | Target 0x0049a660 returns 0x8000 with no explicit arguments. |
| +0x70 | Target 0x0049a690 returns integer 30. |
| +0x78 | Target 0x0049a670 obtains the singleton's +0x70 result and returns its reciprocal as floating point; with the current target this is 1.0f / 30. |
| +0x90 | Target 0x0049a800 returns the saved module handle and performs additional window-message registration work. |

The currently safe partial interface is an object with a vptr, four observed
32-bit fields at offsets +0x04 through +0x10, and address-specific method
contracts: slot +0x20 accepts one pointer-sized argument and returns 1;
slot +0x2c returns a pointer to the title string; slot +0x34 returns the
DWORD at +0x10. In a declaration these may be represented as non-virtual
ABI thunks until the complete virtual slot order and signatures are known.
Declaring only a few virtual methods in a C++ class would shift their slots
and would therefore misrepresent the observed table. The signature of
0x0049adb0, other omitted slots, and exact object size remain unresolved.

## Game identity and limits

The singleton is strongly tied to this game executable: its initializer looks
for Saved Games/LR2.cfg, and its title virtual returns "LEGO Racers 2"; the
startup path pairs that title with the "Saracen II" window class string.
These are direct LR2-specific clues. "NextGen Game" is also a nearby
configuration string, but the observed title method returns the LEGO Racers 2
string. Nothing here recovers the original source class name or proves a
particular engine/library inheritance tree. No LR1 type or interface
assumption is needed for the observations above.
