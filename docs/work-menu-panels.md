# Menu panels and resource ownership

SecondaryManager +0x84 owns a 0x28-byte MenuPanelSet containing six pointers,
active/mode words, and two signed selection words. Its first six words are
zeroed using the same retail initializer as CharBuffer24. This does not make
it a text buffer. Initialize (419410) allocates six 0x108-byte panels; the
second panel uses overlay list 10. Begin/End (4194c0/4194f0) acquire/release
the existing controller builder textures and activate/deactivate panels.

Each panel owns two nine-sprite groups and their shadows. The reconstructed
path includes construction, initialization, allocation/release, activation,
four-stage closing/opening transitions, target rectangles, clipping, and
nine-slice geometry. The mutable tuning block at 561160 spans 0x80 bytes;
retail static initialization at 417a30..417cf0 establishes the padding,
frame borders, shadow offsets and transition durations. Rebuilt initialization
preserves the ordered float stores and BSS storage. Menu states, widget
bounds, and paired text records supply the target geometry. Builder texture
indices 179..232 supply the panel faces and shadows.

The allocation parameter records retain retail's uninitialized tile/UV words;
geometry writes those values before making the sprites visible. Resource
loading at 41c500, cleanup at 41c680, and common-widget allocation at 41c750
remain outside this integrated batch until the actual frontend widget
constructors and callback tables close. No substitute callbacks or
runnable-menu claims are added.

Eight newly measured bodies match exactly: panel construction 417d10,
release forwarding 417e30, sprite release 4181c0, closing request 418230,
deactivation 418260, manager rectangle accessors 423090/4230d0, and paired
text clipping 423d00. They use FUNCTION annotations. The other recovered
bodies remain STUB with raw scores retained; no repeated instruction polishing
was performed. The overlay text rectangle helper at 4b3f30 also has source.

This batch compiles and links the dependency-free panel and geometry path.
The frontend state machine, complete widget graph, recording service setup,
scene creation, and startup movie integration still prevent replacing the
bootstrap. No game, asset probe or startup probe was executed for this batch.
