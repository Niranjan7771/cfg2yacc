Module: FIRST/FOLLOW/NULLABLE & Conflicts
=========================================

Purpose
- Derive NULLABLE, FIRST, and FOLLOW sets from the normalised grammar stored in `Grammar->prods`.
- Surface LL(1) conflicts early so transforms or grammars can be adjusted before emission.

Primary API
- `compute_first_follow(Grammar *g)`: orchestrates nullable/FIRST/FOLLOW fixed-point passes, then prints a concise report and conflict warnings.

Execution Flow
- Preconditions: call `transform_expand_ebnf` then `transform_fix_grammar` so productions are epsilon-aware, left-recursion free, and left-factored.
- Nullable: iterate until convergence, marking a production nullable when all RHS symbols are nullable.
- FIRST: seed terminals, then propagate through productions using newly discovered nullable information.
- FOLLOW: seed `$` for the start symbol, walk RHS trailers right-to-left pushing FIRST sets (and FOLLOW when nullable).

Diagnostics
- `FIRST/FIRST`: flagged when two alternatives of the same nonterminal share an initial terminal.
- `FIRST/FOLLOW`: flagged when a nullable production shares lookahead with the nonterminal's FOLLOW set.
- Missing productions: warned when a nonterminal is declared but has no entries in `g->prods`.

Working Notes
- Arrays are sized from `g->nonterm_len`; indexes are resolved through a stable symbol table built during transform.
- Reports print to stdout today; redirect to a file or harness for automated validation.

Future Work
- Cache the computed sets on the `Grammar` object for reuse by emitters/tests instead of recomputing.
- Emit machine-readable diagnostics (e.g., JSON) to drive IDE integrations and regression tests.


