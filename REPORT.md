% CFG → YACC Converter (EBNF → BNF → YACC)

Overview
--------
This report summarizes the algorithms and implementation of `cfg2yacc`, a C tool that reads a subset of EBNF, expands it to BNF, applies grammar transforms, computes FIRST/FOLLOW/NULLABLE, and emits a Bison grammar plus optional Flex stub.

EBNF Subset
-----------
- Productions: `NonTerminal ::= RHS ;`
- Terminals: single-quoted literals and ALL_CAPS tokens
- Nonterminals: other identifiers
- Operators: grouping `( )`, repetition `* +`, optional `?`, alternation `|`
- Comments start with `#`

Key Algorithms
--------------
- EBNF expansion: introduce helper nonterminals `<lhs>__gen_<n>` for groups and suffix operators.
- Left recursion: immediate elimination `A→Aα|β` → `A→β A'` and `A'→α A'|ε`.
- Left factoring: simple first-symbol factoring when two or more alternatives share the same first symbol.
- FIRST/FOLLOW/NULLABLE: iterative fixpoint with ε tracking; diagnostics printed.

Emission
--------
- YACC: groups alternatives per LHS with placeholder actions `{ $$ = NULL; }` and `%start` set to the first LHS.
- Flex: stub rules for literals and ALL_CAPS tokens; whitespace skipping and number token example.

Limitations
-----------
- Left factoring is heuristic (first token only).
- Indirect left recursion beyond one substitution pass is not fully optimized.
- AST construction is skeletal.

Usage
-----
See `README.md`. To generate this PDF (if `pandoc` installed): `make report`.


