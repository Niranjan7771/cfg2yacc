Module: EBNF Expansion & Grammar Transforms
==========================================

Functions
- transform_expand_ebnf(g): expands groups and `?/*/+` into helper nonterminals `<lhs>__gen_<n>`.
- transform_fix_grammar(g): eliminates immediate left recursion and applies simple left-factoring.

Algorithm Sketch
- Groups: `(X | Y Z)` becomes `<lhs>__gen_k` with productions for each alternative sequence; the factor returns the helper name.
- `?`: `opt -> ε | X`; `*`: `star -> ε | star X`; `+`: `plus -> X plus_tail`, `plus_tail -> ε | plus_tail X`.
- Left recursion: A → Aα | β transformed into A → β A' and A' → α A' | ε.
- Left factoring: if multiple A-alternatives share the first symbol, extract it and introduce A1 for suffixes.

Tests
- See `samples/leftrec.ebnf` and `samples/ebnf_features.ebnf`.


