# Vector and Matrix3 helper matching notes

This note combines the earlier Vector3 and Matrix3 layout/matching evidence with
the current matrix helper experiment. Disassembly is from /tmp/lr2-disasm.txt;
Ghidra exports are references checked against instructions and callers. Builds
used the isolated build-vector-worker manifest, VC6 SP5, and default translation
unit flags. No behavioral tests or compiler-flag changes were used in the
current batch.

## Layout and exact Vector3 transforms

Vector3 is represented as three consecutive floats at offsets 0, 4, and 8.
Callers use three-float temporaries and adjacent records with a 12-byte stride,
supporting its 0x0c size.

The Matrix3 layout is nine consecutive floats in row-major 3x3 order. Its
methods access the final element at offset 0x20, and adjacent Matrix3 storage in
the surrounding object at offsets 0xd8 and 0xfc is separated by 0x24 bytes.
This supports the 36-byte layout asserted in matrix3.h.

Vector3 methods 0x004a4e00 and 0x004a4e50 are each 79 bytes and match retail
at 100%. The first computes matrix rows dotted with the vector; the second
computes matrix columns dotted with it. Both preserve this in EAX and use ret
4. The C++ return type is written as Vector3*, consistent with the observed
register result and the neighboring Matrix3 product methods, but callers discard
it and a reference return would have the same x86 ABI. The exact source type
remains ambiguous. Their implementation calculates all outputs before writing
the vector components, preserving the retail evaluation and alias order.

Normalize and Blend remain exact. Normalize returns the original magnitude,
leaves a zero vector unchanged, and its retail unordered x87 comparison takes
the early return. Blend stores the first weighted vector before adding the
second, so its order matters when an input aliases the destination.

## Matrix products

The member signatures for 0x004a4ea0 and 0x004a4f00 take two Matrix3 pointers
and return this in EAX; both use ret 8. Their complete bodies are 90 bytes
each (0x004a4ea0..0x004a4ef9 and 0x004a4f00..0x004a4f59).

The first computes a 3x3 product from rows of p_left and columns of p_right.
The second uses columns of p_left and columns of p_right, equivalent to
transpose(p_left) multiplied by p_right. Both read one left row/column into
locals before writing the corresponding three outputs, matching the observed
row-at-a-time alias behavior. The current code retains these semantics and
uses readable matrix indexing.

The best measured natural-forward formulation reached 84.62% for each
function. It keeps the left coefficients in locals, uses nested row/column
loops, and writes each result as the sum of three forward-ordered products.
The matching instructions still differ in x87 scheduling and the address
chosen as the right-matrix traversal base. Earlier source layouts scored
56.41% and 58.97%; the index-based layout raised both to 84.62%. Both methods
remain STUB-annotated because neither reaches 100%. The return type is
supported at the ABI level by EAX=this, while callers do not use the result;
pointer versus reference spelling is not recovered.

## Other matrix evidence and open signatures

The determinant helper at 0x004a5250 is 55 bytes and currently scores 52.38%.
Its natural sequential accumulation is semantically correct, but the compiler
reassociates part of the determinant expression. It remains STUB.

Transpose helper 0x004a5040 reads nine source floats in transposed order and
writes them to this. The body is 50 bytes. Retail preserves ECX=this in EAX
through return, and the callers inspected ignore EAX; the current void
signature is therefore not established as the original source signature.
A Matrix3* return-this declaration is ABI-compatible and plausible, but should
not be treated as recovered source spelling. The operation is not generally
alias-safe: when source and destination are the same matrix, sequential stores
can overwrite values that later loads read.

## Addition and row scaling

0x004a5110 is a 63-byte thiscall helper (ret 4) that adds the corresponding
three values in each of three rows from its argument into this. Its typed
row/column loop scored 21.28%. Retail uses a compact pointer-difference loop
that appears to preserve elementwise aliasing. The implementation remains
STUB; no pointer arithmetic was added to mimic its register layout.

0x004a52c0 is a 66-byte thiscall helper (ret 8) that multiplies each source
row by the matching entry in a three-float scale array and stores it to this.
Its typed loop scored 34.62%. Retail hoists one scale per row and walks the
source and destination in row strides. The implementation remains STUB.

## Compiler options and evaluation order

Trials with /Op and /Og- degraded matches in the existing exact helpers, so the
default /O2 /Ob1 /MT /GX flags were retained. The products and determinant
continue to show that equivalent arithmetic can compile with different x87
scheduling and reassociation. No per-function options or instruction-forcing
constructs were introduced.

Matrix products cache the left row/column before writing its outputs, so
destination aliasing the left operand preserves those coefficients for that
row/column. The destination may alias the right operand, where later reads can
observe earlier writes; do not add temporary whole-matrix copies that alter
that behavior. Similar evaluation order matters in the exact Vector3 blend and
in-place transpose.
