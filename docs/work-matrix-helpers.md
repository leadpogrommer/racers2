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

Both products now match at 100%. Each uses a natural accumulator in source:
initialize from the first product, then add the second and third products in
sequence. The first method caches a left row; the second caches a left column.
This matches the retail x87 accumulation order and the per-row/per-column
alias behavior. Earlier source variants scored 56.41%, 58.97%, and 84.62%; a
single accumulating result raised both functions to 100%. Both return this in
EAX and use ret 8. The pointer return spelling is retained for ABI/codegen;
callers discard the result, so pointer versus reference remains ambiguous.

## Other matrix evidence and open signatures

The quaternion-to-matrix helper at 0x004a4d30 is 204 bytes and remains STUB
at 25.71%. Its formulas and output order are recovered from the full body.
Retail caches the quaternion-derived inputs before writing the matrix, so the
current implementation preserves behavior if the input overlaps the
destination. A source variant that read quaternion components during output
scored 31.88% but was rejected because it changed that alias behavior.

The Euler helper at 0x004a4f60 is 211 bytes and now matches at 100%. Retail
preserves this in EAX and uses ret 4; callers discard EAX, so the natural
Matrix3* return-this signature is retained for code generation, with the C++
pointer-versus-reference spelling still ambiguous. All input angles are
converted to cached sine and cosine values before writes. Named negative
product intermediates reproduce retail's negation-then-add sequence for two
entries.

The symmetric-fill helper at 0x004a5080 is 137 bytes and remains STUB. Its
current readable implementation caches the squared components and the three
cross-products before storing the symmetric matrix entries; it scores 42.22%,
up from 15.73%. A Matrix3* return-this signature supports the observed EAX=this
value while callers discard it. Retail keeps x-squared live in x87 while
spilling y-squared and z-squared, then duplicates negative cross-products
through integer stores. That schedule is not reproduced by the current natural
source.

The determinant helper at 0x004a5250 is 55 bytes and remains STUB. A staged
source form with named first/second/third minors and products, then sequential
addition, improves the best score from 52.38% to 61.90%. Retail computes the
first cofactor term, then the second, then the third. VC6 still emits the
second term before the first term in the staged build, so the main residual is
compiler scheduling/reassociation rather than a formula or sign mismatch.
Earlier three-term-local and direct-return variants scored 38.10% and 28.57%.
The staged locals are the current best readable form; no compiler options or
instruction-forcing constructs were used.

Transpose helper 0x004a5040 reads nine source floats in transposed order and
writes them to this. The body is 50 bytes and now matches at 100%. Retail
preserves ECX=this in EAX through return, and the callers inspected ignore
EAX. The implementation uses a Matrix3* return-this signature; reference
return has the same x86 ABI, so the original C++ spelling remains ambiguous.
The operation is not generally alias-safe: when source and destination are the
same matrix, sequential stores can overwrite values that later loads read.

## Addition and row scaling

0x004a5110 is a 63-byte thiscall helper (ret 4) that adds the corresponding
three values in each of three rows from its argument into this. The natural
typed row loop now matches at 100%. Retail preserves this in EAX; callers
discard the result, so pointer versus reference source spelling remains
unresolved. Elementwise self-addition is preserved when source aliases this.

Matrix3 inverse helper 0x004a5150 spans 254 bytes and returns this in EAX
(ret 4). The current implementation computes reciprocal determinant, using a
zero scale when the determinant compares equal to zero or is unordered, then
writes the transposed cofactor matrix in column-major traversal order. It uses
the same source/destination alias order as retail. This readable cyclic-index
form scores 77.22% and remains STUB. A first cyclic counter-loop version scored
22.56%; expressing the recovered cycle as the local index table `{1, 2, 0,
1}` brought the generated traversal much closer to retail. Explicitly
unrolling the nine cofactor assignments scored 21.95%, so the typed table loop
remains. The current code does not cache the source matrix: retail reads each
minor as it writes, so in-place inversion observes earlier stores.
Its result depends on 0x004a5250, which is still STUB-annotated despite the
recovered determinant formula.

0x004a52c0 is a 66-byte thiscall helper (ret 8) that multiplies each source
column by the matching entry in a three-float scale array and stores it as a
row of this: output[row][column] = scale[row] * source[column][row]. This is
the row-scaled transpose of the source. The natural typed loop now matches at
100%. Retail preserves this in EAX; callers discard the result, so pointer
versus reference source spelling remains unresolved. Its sequential write
order is observable when source and destination alias; the implementation
keeps that order.

## Adjacent closed leaf candidate

The nearby standalone quaternion normalizer at 0x004a5310 is 102 bytes,
ending at 0x004a5375. It reads four consecutive floats from ECX, computes
their Euclidean magnitude, returns that magnitude in x87 ST(0), and scales
all four components by its reciprocal when the magnitude is nonzero. The zero
case returns zero and leaves the components alone. The packed-quaternion
decoder at 0x004a5440 writes all four components and invokes this normalizer;
array indexing at 0x0044a63c/0x0044a680 advances quaternion records in 16-byte
steps. This supports a four-float Quaternion layout and 0x10 size assertion.
The new Quaternion::Normalize source is STUB at 85.37%. Direct, reordered,
and staged square-sum forms all generated the same residual x87 scheduling;
the direct expression remains the clearest form.

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
that behavior. Similar evaluation order matters in the exact Vector3 blend,
matrix transpose, and row-scaled transpose.
