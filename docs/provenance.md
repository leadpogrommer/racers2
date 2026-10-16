# Repository and tooling provenance

This checkout was cloned from the supplied `sample_repos/racers`, retaining its
Git history and LGPL-3.0 license. Its starting revision was
`af0ed0f4be0ac8e6114952b9d3b83ffa8dd176a3`. The `template` remote points to
`https://github.com/isledecomp/racers.git`; no LR2 publishing remote is configured.

Retained infrastructure: `cmake/reccmp.cmake`, `util` helpers, clang-format and
clang-tidy configuration, editor settings, and the license. LR1 source code,
resources, binaries, addresses, CRT patches, DX6 SDK, CI publishing workflows,
function counts and compiler-specific instructions were removed from the LR2
working tree. A new source-only workflow lints LR2 annotations and tests the
match guard without retail files. `CLAUDE.md` points to the single LR2 `AGENTS.md`.

The other supplied reference was `sample_repos/isle` at
`8119bd013645972c72480e4de21529f1e91ce5c7`. It informed the separation between
instruction accuracy and whole-binary reproduction; no byte-for-byte whole-game
claim is made here.

Tool revisions:

- reccmp: `https://github.com/isledecomp/reccmp`, revision
  `c3edb121b8dfc92e8679ea3aace743fd6b5ef287`; pinned in `tools/requirements.txt`.
  reccmp retains its own AGPL license and bundled tool notices.
- MSVC SP5: `https://github.com/archaic-msvc/msvc600_sp5`, revision
  `b0b07e29108e2695eb0274c2a377a7b7d7326150`; pinned in `tools/toolchain.py`.
  This is a preserved proprietary toolchain, not covered by this repo's LGPL.
  It is downloaded into ignored local storage, never vendored as project code.
- Rich product/build interpretation:
  `https://github.com/dishather/richprint/blob/master/comp_id.txt`.

The original executable and asset files stay in the separate installation
directory. Only derived identity metadata is included in `docs/original-binary.json`.
