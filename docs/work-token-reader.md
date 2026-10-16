# Token reader matching notes

`FUN_0045de20` remains `STUB` at **57.94% raw instruction similarity** with
the current SP5 settings. Its verified 286-byte retail boundary includes the
comment-classification early return and trailing-space trimming. The source is
semantically equivalent to the observed paths, but it is not yet an exact
instruction match.

The best measured shape uses an explicit top-tested `while (TRUE)` loop with a
bounds check and `break` before reading the current character. This raises the
raw score from the earlier 42.79% baseline. Reordering the state locals,
inverting the line-type-3 conditional, and introducing a local alias for the
cursor parameter did not change the score. Earlier bounded trials of combined
versus separate CR/LF branches, positive versus inverted comment branches,
`for`/`while` guards, per-iteration character pointers, flattened whitespace
handling, and nested trim guards also did not beat that prior baseline; the
per-iteration pointer form scored 36.19%, and split CR/LF early-exit branches
scored 41.26%.

The explicit top test now matches the retail bounds-check placement and much of
the cold-exit layout. Remaining differences include register allocation
(retail keeps the cursor parameter in ESI and output length in EDI, while this
build assigns those roles to EDI and ESI), pointer-update sequences, and the
placement of the line-type-3 return block. Natural local-order and branch
inversion changes did not alter those code-generation choices.

Preserve the observed behavior while revisiting the match: CR/LF termination
depends on output length, the comment-skip option is tested against exactly
`TRUE`, and a line classification of 3 clears the output and returns
immediately. The two character predicates and the line classifier remain exact
matches. Validation for this pass was per-function reccmp only; no behavioral
tests were run.
