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
- Transforms: EBNF expansion, immediate left recursion elimination, simple left-factoring.
- Diagnostics: NULLABLE/FIRST/FOLLOW printout and undefined-nonterminal warnings.

Demo:

```bash
bash demo.sh
```

Tests:

```bash
bash tests/run_tests.sh
```
