## cfg2yacc (MVP)

Build:

```bash
make
```

Run:

```bash
./cfg2yacc -i samples/expr.ebnf -o out.y --emit-lex out.l
bison -d out.y
```

Notes:
- Supported EBNF subset per project brief.
- Transforms: EBNF expansion and immediate left recursion elimination.
- Left factoring: Disabled by default to avoid helper explosion on some EBNF patterns. Can be re-enabled in `transform_fix_grammar` if needed.
- Diagnostics: NULLABLE/FIRST/FOLLOW printout and undefined-nonterminal warnings (can be skipped with `--no-ff`).

Flags:
- `--emit-lex <file>`: also emit a minimal Flex lexer alongside the Yacc file.
- `--no-ff`: skip computing/printing FIRST/FOLLOW/NULLABLE (faster for large grammars).

Demo:

```bash
bash demo.sh
```

Tests:

```bash
bash tests/run_tests.sh
```
