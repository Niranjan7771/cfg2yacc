Module: Grammar Frontend & IO
=============================

Functions
- grammar_read_file(path): reads EBNF file, parses productions with grouping and operators into `Grammar` with raw tokens (including parentheses and suffixes which are later expanded).
- grammar_dump(g): prints canonical productions.
- grammar_free(g): frees memory.

Notes
- Top-level alternation is split while respecting parentheses and quotes.
- Tokens recognized: identifiers, single-quoted literals, `(`, `)`, `*`, `+`, `?`.

Run
```bash
./cfg2yacc -i samples/expr.ebnf -o out.y
```


