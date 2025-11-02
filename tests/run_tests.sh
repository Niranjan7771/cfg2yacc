#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

mkdir -p logs tests
rm -f tests/results.txt

GRS=(samples/expr.ebnf samples/ifelse.ebnf samples/leftrec.ebnf samples/ebnf_features.ebnf)

for g in "${GRS[@]}"; do
	echo "[run] $g" | tee -a tests/results.txt
	./cfg2yacc -i "$g" -o out.y --emit-lex out.l || { echo "converter failed" | tee -a tests/results.txt; continue; }
    rm -f out.output out.tab.c out.tab.h lex.yy.c parser
    bison -dv out.y >/dev/null 2>logs/bison.err || true
	if [[ -f out.output ]]; then
		cp out.output "logs/$(basename "$g").output"
		grep -E "conflicts|state|error" out.output | tee -a tests/results.txt || true
	else
		echo "no bison .output produced" | tee -a tests/results.txt
	fi
    if command -v flex >/dev/null 2>&1; then
        if [[ -f out.l ]]; then
            flex -o lex.yy.c out.l || true
        fi
    fi
    if [[ -f out.tab.c ]]; then
        if [[ -f lex.yy.c ]]; then
            gcc -o parser out.tab.c src/ast.c lex.yy.c -lfl || true
        else
            gcc -o parser out.tab.c src/ast.c || true
        fi
        if [[ -f parser ]]; then echo "compiled parser OK" | tee -a tests/results.txt; fi
    fi
done

echo "done" | tee -a tests/results.txt

#!/usr/bin/env bash
set -euo pipefail
mkdir -p logs tests
RESULTS=tests/results.txt
: > "$RESULTS"

BIN=./cfg2yacc
SAMPLES=(samples/expr.ebnf samples/ifelse.ebnf samples/leftrec.ebnf samples/ebnf_features.ebnf samples/bad.ebnf)

for s in "${SAMPLES[@]}"; do
  name=$(basename "$s")
  y="logs/${name%.ebnf}.y"
  l="logs/${name%.ebnf}.l"
  echo "[+] Converting $s -> $y" | tee -a "$RESULTS"
  $BIN -i "$s" -o "$y" --emit-lex "$l" || echo "converter failed for $s" | tee -a "$RESULTS"
  if command -v bison >/dev/null 2>&1; then
    echo "    Running bison..." | tee -a "$RESULTS"
    if bison -dv "$y" -o logs/out.tab.c 1>logs/${name%.ebnf}.bison.stdout 2>logs/${name%.ebnf}.bison.stderr; then
      echo "    bison OK" | tee -a "$RESULTS"
    else
      echo "    bison reported issues (see logs)" | tee -a "$RESULTS"
    fi
  else
    echo "    bison not found, skipping" | tee -a "$RESULTS"
  fi
  echo >> "$RESULTS"
done

echo "Results written to $RESULTS"
