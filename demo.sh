#!/usr/bin/env bash
set -euo pipefail

make
./cfg2yacc -i samples/expr.ebnf -o out.y --emit-lex out.l
echo "Running bison..."
bison -dv out.y || true
echo "Done. See out.y and out.output (if generated)."


