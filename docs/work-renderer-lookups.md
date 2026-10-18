# Renderer lookup and deferred-resource work

## Lookup-table dispatch

The renderer constructor assigns separate two-slot callback tables to the
lookup states at `this+0x04`, `this+0x2c`, and `this+0x54`. Their original
addresses are `0x005358d4`, `0x005358cc`, and `0x005358c4`, respectively.
The records contain insert and flush callbacks, with ECX as the table receiver
and four stack arguments for insertion. `rendererlookupdispatch.cpp` provides
typed fastcall bridges that preserve that ABI and bind each table to the
observed insert/flush behavior.

`0x004bd870` (65 bytes) walks bucket heads in descending bucket order and
visits each linked chain. `0x004bd8c0` (61 bytes) walks the
non-null-terminated tail-pointer list, visits every chain, and clears the
state after drawing. `0x004bd900` is 278 bytes. The distinction matters: the
first path leaves the table populated, while the second resets it.

## Deferred resources

`0x004bd900` returns immediately when renderer quality is below 3 or the
table has no entries. Otherwise it calls the renderer's extended-texture-stage
predicate. The software path visits each tail-list chain and calls
`0x004c6fe0` for each resource. The hardware path first sends resources with
no initialized nested object to `0x004c6fe0`, applies renderer operation 3,
then sends initialized objects to `0x004c6980`, draws the stencil overlay, and
applies renderer operation 4. Both callbacks receive the resource pointer as
one stack argument; `0x004c6fe0` uses the renderer as ECX, while
`0x004c6980` uses the nested object at resource `+8` as ECX.

The hardware path's initialized-object test dereferences resource `+8` and
then owner `+0x28` directly; it does not guard a null owner. The object at
owner `+0x28` has observed fields at `+0x04`, `+0x08`, `+0x0c`, `+0x10`,
`+0x14`, and `+0x18`. `0x004c6980` treats the object as mesh data: source
points are three floats each, group records are `0x24` bytes, and signed
WORD indices at group offsets `+0x18`, `+0x1a`, `+0x1c`, and `+0x1e` select
point data and validity flags. It fills a renderer vertex buffer and draws
stencil volumes. `0x004c6fe0` also updates renderer state at `+0x4b4` and
uses the six-argument terrain-geometry callback at main-dispatch slot `+0xfc`
while building the deferred pass. The point-depth scratch array read and
written at `0x0067a698` has no confirmed extent yet.

The dispatch tables and three flush methods remain `STUB`. Their observed
extents are 65 bytes for `0x004bd870`, 61 bytes for `0x004bd8c0`, and 278 bytes
for `0x004bd900`. The deferred-resource callbacks are also implemented as
`STUB`: `0x004c6980` is 773 bytes and `0x004c6fe0` is 1505 bytes. Their source
follows the observed mesh fields, branch order, and renderer calls, but the
remaining code-generation differences are too large to claim equivalence.
The point-depth scratch array at `0x0067a698` still has an unconfirmed extent.

`DrawLookupEntry` at `0x004bda20` has source for the key 0 through key 5 paths,
including the special key-0 resource callback sequence, and remains `STUB`
(12.01% in the isolated comparison). `ApplyLookupResourceState` at
`0x004a7510` and its point/origin helpers at `0x004a74b0` and `0x004a9d20` are
also implemented as `STUB` (21.83%, 41.67%, and 91.30%, respectively). The
source preserves the observed owner/resource offsets and call order, but those
scores and the still-partial owner views leave semantic and code-generation
review open. `0x004a9d20` lazily initializes the shared zero-origin storage and
registers the observed no-op cleanup callback.

An isolated build using the current shared source list plus the lookup units
linked successfully. The latest raw measurements for the other owned methods
were 50.00% (`0x004bd870`), 81.48% (`0x004bd8c0`), 27.66% (`0x004bd900`),
14.57% (`0x004c6980`), and 20.43% (`0x004c6fe0`). These are comparison results,
not correctness claims; all functions remain `STUB` for root integration and
combined review.


The three lookup tables are verified by the main renderer pointer checker against all six original targets and rebuilt bridge symbols. Their addresses are recorded as table evidence, without GLOBAL pointer comparisons that would incorrectly equate a fastcall bridge with its member-function implementation.
