# Physical lifecycle parent detachment

The shared physical-state view is an evidenced prefix through 0x128, rather than a full class-size claim. It retains the existing lifecycle prefix, quaternion/position/matrix, parent-relative transform and value snapshots, momentum/angular fields, world matrix, force/torque, scale, mass and inverse mass.

Parent detachment at 0x004a6c30 keeps the old parent during position/orientation capture, force clearing, inherited velocity calculation, world-matrix rebuild, inherited angular conversion and velocity clamping. It clears the parent pointer last. Its nonzero-flag branch only clears that pointer. Both transform getters take no explicit arguments. Parent-velocity calculation 0x004a6f00 takes two stack pointers and RET 8; the decompiler's apparent receiver offset and single argument were incorrect.

The world-matrix rebuild 0x004a6480 constructs a scaled temporary matrix then combines it with rotation. It matches exactly. Velocity update 0x004a64c0 converts momentum by inverse mass, rotates angular velocity and clamps both magnitudes. Both raw FCOM branches skip clamping for unordered comparisons; the source uses ordered greater-than. The larger bodies remain STUB with raw scores retained.

Three BSS limit values are initialized by the actual CRT callback bodies at 0x004a5f50, 0x004a5f80 and 0x004a5fb0: angular speed uses float pi times 20, then its stored square; linear speed uses float 331 squared. Source startup glue preserves that dependency order. The constants are read from 0x00535370, 0x00531894 and 0x0053537c. These file-backed constants are distinct from the BSS destinations at 0x005790d8, 0x00579124 and 0x005790dc.

No game or incremental runtime probe was run.
