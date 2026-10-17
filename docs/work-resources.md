# Retail PE resource extraction

`tools/extract_resources.py` converts the supplied retail executable's PE
resource tree into a Windows `.res` file for linking. It checks the executable
SHA-256 against the pinned input before reading it and never writes to the
executable. By default it writes the generated file under the ignored
`build-resources/` directory:

```sh
python tools/extract_resources.py
python tools/extract_resources.py --original "/path/to/LEGO Racers 2.exe" \
  --output build-resources/retail.res
```

The checked input has SHA-256
`6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d`.
Extraction produced five entries with 7,022 total payload bytes: three
numeric `RT_ICON` entries (IDs 1–3), dialog resource ID 102, and icon-group
resource ID 101. All entries use language ID `0x0809`. The generated `.res`
was 7,216 bytes with SHA-256
`eb82826ecf76df600c8a3689e9e2f51d608ad40bf5e1f1656647656a920aa531`.
An independent `.res` parse-back check compared all emitted identifiers,
language IDs, and payload bytes against the PE tree and matched all five
entries and 7,022 bytes.

The converter preserves resource type, name, language, and payload bytes,
including UTF-16 names when present. PE resource leaves do not carry every
field in a `.res` record; data-version, version, characteristics, and reserved
record fields are therefore zeroed, and the conventional memory flags are
used. The current retail resources use numeric identifiers and do not depend
on those normalized fields.

The generated file was accepted by the pinned VC6 `LINK.EXE` in an isolated
`/dll /noentry /machine:I386` probe. Inspection of the resulting PE confirmed
that its `.rsrc` tree retained all five type/name/language entries and payload
sizes. The generated `.res`, probe DLL, and original executable remain build
artifacts outside version control; no retail resource payload is committed.
