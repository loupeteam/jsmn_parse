# Off-PLC regression test

Compiles the library sources against stub versions of `bur/plctypes.h` and the
AS-generated `jsmn_parse.h` so the parser can be exercised on a desktop
compiler. Covers the bugs from issue #5:

- `jsmn_init()` must clear `callback.pFunction` / `callback.pUserData` / `pcache`
  (previously stack garbage in `JsonParse()` → wild call / wild write → page fault)
- `JsonParse()` + `JsonGetValue()` round-trip on a multi-key document
  (previously `jsmn_cache()` remapped string token offsets even with no callback)
- `JsonGetValue()` on a missing key returns -1 (previously read `t2[-1]`)

Run with any gcc (32- or 64-bit):

```sh
cd test/off_plc
gcc -std=gnu99 -w -I stub -I ../../src/Ar/jsmn_parse \
    test_main.c \
    ../../src/Ar/jsmn_parse/jsmn.c \
    ../../src/Ar/jsmn_parse/JsonParse.c \
    ../../src/Ar/jsmn_parse/JsonGetValue.c \
    ../../src/Ar/jsmn_parse/JsonTokenEqual.c \
    -o test_jsmn_parse && ./test_jsmn_parse
```

Exit code is the number of failed checks. On the pre-fix sources this crashes
with an access violation (the page fault from issue #5) instead of completing.

Note: `stub/jsmn_parse.h` duplicates the types from `Types.typ` /
`Constants.var` by hand — keep it in sync if those change.
