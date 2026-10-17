# Renderer default texture-stage plans

This unit recovers the default-plan builder at `0x004e2850` and its request
resolver at `0x004e24f0`. Both remain `STUB`: the isolated VC6 build links,
but current raw instruction accuracy is 8.48% for the builder and 39.53% for
the resolver. The scores are visible evidence, not a claim of equivalence.

`BuildDefaultStagePlan` covers every dispatch case in the retail jump table:
`0`, `1`, `2`, `3`, `0x11`, `0x200`, `0x201`, `0x205`, and `0x241`. It builds
the corresponding cached subrecord group, applies the observed masks and
minimum-count adjustments, appends the extra stage record where present, and
forwards the original apply mode. Case `0x241` first tries the shared-memory
profile when the renderer advertises that capability, then builds the fallback
three-group profile if the application attempt fails.

Retail initializes large descriptor tables on first use through hundreds of
stores and registers cleanup callbacks. The replacement represents the same
four-DWORD descriptors as `static const` arrays. The arrays were checked
against the values written by the retail initializer blocks and the helper
calls to `0x004e4e60`/`0x004e4e80`. The builder copies each descriptor into the
manager-owned records before use; no table pointer is retained. Both cleanup
callbacks are single-`ret` functions, so omitting their registrations has no
observable cleanup effect. This accounts for much of the builder's low raw
score and avoids modeling mutable storage or retail-only addresses.

The resolver first dispatches the per-slot update through renderer table slot
`+0x120`, then checks the manager cache. For low renderer modes it strips the
observed unsupported mask bits and removes corresponding request slots. It
tries default and profile plans in order, aliases a successful reduced-mask
cache entry back to the original request key, and otherwise repeatedly removes
the highest-priority unsupported stage until a plan is found or no further
mask change is possible. The method is expressed through the recovered manager
and renderer dispatch views; no retail vtable address is embedded.

The builder branches were audited against the raw register/stack arguments to
`0x004e5e70` and `0x004e5f30`, not only the decompiler's inferred parameter
order. In particular, profile 1 goes through the apply helper; profile `0x201`
appends a `(0, 0x18, 2|3)` record then applies mode 1; profile `0x205` writes
value 9 at map index 1, appends `(1, 0x18, 2|3)` to its second subrecord, and
returns success without applying; the `0x241` fallback maps value 9 at index 0
of its second subrecord and appends there. These distinctions are easy to lose
when reading the unreviewed decompiler output.

The resolver's control flow and builder's supported cases have been reviewed
against `data/ghidra-export/004e24f0.c`, `data/ghidra-export/004e2850.c`, and
`data/retail-disasm.txt`. The large builder's use of literal descriptor data
and different helper boundaries make instruction matching especially poor;
the current isolated builder score is 9.05% after the branch corrections, and
the resolver score is 39.53%. These low scores do not establish behavioral
equivalence. Recheck data and caller behavior when these methods are integrated
with the complete renderer owner and the device-backed stage-apply path.
