# cfg2yacc Usage Examples

## Basic Usage

### Example 1: Simple EBNF to YACC conversion
```bash
./cfg2yacc -i samples/expr.ebnf -o expr.y
```
Converts the expression grammar to a YACC file. Output: `expr.y`

### Example 2: Generate YACC + Flex lexer
```bash
./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l
```
Generates both YACC grammar and Flex lexer. Output: `expr.y`, `expr.l`

## Performance Options

### Example 3: Skip FIRST/FOLLOW analysis (faster)
```bash
./cfg2yacc -i samples/expr.ebnf -o expr.y --no-ff
```
Useful for large grammars when you don't need conflict analysis.

### Example 4: Concise FIRST/FOLLOW summary
```bash
./cfg2yacc -i samples/expr.ebnf -o expr.y --ff-summary
```
Shows only set sizes, not full contents.

### Example 5: Save FIRST/FOLLOW analysis to file
```bash
./cfg2yacc -i samples/expr.ebnf -o expr.y --ff-out analysis.txt
```
Redirects FIRST/FOLLOW output to a file instead of stdout.

## Real-World Examples

### Example 6: Expression Parser
```bash
# Convert EBNF to YACC
./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff

# Generate parser (requires Bison)
bison -d expr.y

# Generate lexer (requires Flex)
flex expr.l

# Compile (copy ast.h first)
cp include/ast.h .
./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff
gcc -o parser expr.tab.c ast.c lex.yy.c -lfl -Iinclude
```

### Example 7: If-Else Statement Grammar
```bash
./cfg2yacc -i samples/ifelse.ebnf -o ifelse.y --emit-lex ifelse.l
bison -d ifelse.y
```
Check `ifelse.output` for shift/reduce conflicts (this grammar has intentional conflicts).

### Example 8: Comprehensive EBNF Features
```bash
./cfg2yacc -i samples/ebnf_features.ebnf -o features.y --ff-summary
```
Tests all EBNF operators: `*`, `+`, `?`, groups, alternation.

## Input Grammar Format

### Supported EBNF Syntax:
- **Productions**: `NonTerminal ::= RHS ;`
- **Terminals**: Single-quoted literals `'+'`, `'('` or ALL_CAPS tokens `NUMBER`, `ID`
- **Nonterminals**: Lowercase identifiers `expr`, `term`, `factor`
- **Operators**:
  - Alternation: `|`
  - Grouping: `( ... )`
  - Repetition: `*` (zero or more), `+` (one or more)
  - Optional: `?`
- **Comments**: Lines starting with `#`

### Sample Input (`samples/expr.ebnf`):
```
expr ::= term ( ('+' | '-') term )* ;
term ::= factor ( ('*' | '/') factor )* ;
factor ::= NUMBER | '(' expr ')' ;
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `-i, --input <file>` | Input EBNF file (required) |
| `-o, --output <file>` | Output YACC file (required) |
| `--emit-lex <file>` | Also generate Flex lexer stub |
| `--no-ff` | Skip FIRST/FOLLOW computation |
| `--ff-summary` | Show concise FIRST/FOLLOW summary |
| `--ff-out <file>` | Write FIRST/FOLLOW to file |
| `-h, --help` | Show usage information |

## Complete Workflow Example

```bash
# 1. Convert EBNF to YACC
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --emit-lex mygrammar.l

# 2. Generate parser with Bison
bison -d mygrammar.y

# 3. Generate lexer with Flex
flex mygrammar.l

# 4. Prepare AST files
cp include/ast.h .
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --emit-lex mygrammar.l --no-ff

# 5. Compile everything
gcc -o myparser mygrammar.tab.c ast.c lex.yy.c -lfl -Iinclude

# 6. Run the parser
./myparser < input.txt
```

## Testing Grammars

Test all sample grammars:
```bash
for gram in samples/*.ebnf; do
    echo "Testing $gram"
    ./cfg2yacc -i "$gram" -o /tmp/test.y --no-ff
    bison -d /tmp/test.y 2>&1 | grep -q "error" && echo "  ❌ Failed" || echo "  ✅ OK"
done
```

