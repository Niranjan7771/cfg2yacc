# cfg2yacc - EBNF to YACC/Bison Converter

A C-based command-line tool that converts grammars written in Extended Backus-Naur Form (EBNF) into YACC/Bison grammar files (`.y`). The tool automatically handles EBNF expansion, grammar normalization, conflict analysis, and code generation.

## Features

- ✅ **EBNF to BNF Conversion**: Expands EBNF constructs (`*`, `+`, `?`, groups) to pure BNF
- ✅ **Grammar Normalization**: Eliminates left recursion and handles common prefixes
- ✅ **Grammar Analysis**: Computes FIRST/FOLLOW/NULLABLE sets
- ✅ **Conflict Detection**: Identifies LL(1) conflicts in grammars
- ✅ **Code Generation**: Produces ready-to-use YACC files and optional Flex lexer stubs
- ✅ **AST Scaffolding**: Generates minimal AST structure for parser actions

## Quick Start

### Build

```bash
make
```

### Basic Usage

```bash
# Convert EBNF to YACC
./cfg2yacc -i samples/expr.ebnf -o expr.y

# Generate YACC + Flex lexer
./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l

# Fast conversion (skip analysis)
./cfg2yacc -i samples/expr.ebnf -o expr.y --no-ff
```

### Complete Example

```bash
# 1. Write your EBNF grammar
cat > calculator.ebnf << 'EOF'
expr ::= term ( ('+' | '-') term )* ;
term ::= factor ( ('*' | '/') factor )* ;
factor ::= NUMBER | '(' expr ')' ;
EOF

# 2. Convert to YACC
./cfg2yacc -i calculator.ebnf -o calculator.y --emit-lex calculator.l --no-ff

# 3. Generate parser with Bison
bison -d calculator.y

# 4. Generate lexer with Flex
flex calculator.l

# 5. Compile
cp include/ast.h .
gcc -o calculator calculator.tab.c ast.c lex.yy.c -lfl -Iinclude
```

## Documentation

- **[USAGE_GUIDE.md](USAGE_GUIDE.md)** - Complete usage guide with examples
- **[EXAMPLES.md](EXAMPLES.md)** - Quick reference examples
- **[REPORT.md](REPORT.md)** - Technical report and algorithms

## Command-Line Options

| Option | Description |
|--------|-------------|
| `-i, --input <file>` | Input EBNF grammar file (required) |
| `-o, --output <file>` | Output YACC file path (required) |
| `--emit-lex <file>` | Also generate Flex lexer stub |
| `--no-ff` | Skip FIRST/FOLLOW computation (faster) |
| `--ff-summary` | Show concise FIRST/FOLLOW summary |
| `--ff-out <file>` | Write FIRST/FOLLOW analysis to file |
| `-h, --help` | Show help message |

## EBNF Grammar Format

### Supported Syntax

- **Productions**: `NonTerminal ::= RHS ;`
- **Terminals**: Single-quoted literals `'+'` or ALL_CAPS tokens `NUMBER`
- **Nonterminals**: Lowercase identifiers `expr`, `term`
- **Operators**: 
  - `|` - Alternation
  - `()` - Grouping
  - `*` - Zero or more
  - `+` - One or more
  - `?` - Optional
- **Comments**: Lines starting with `#`

### Example Grammar

```
expr ::= term ( ('+' | '-') term )* ;
term ::= factor ( ('*' | '/') factor )* ;
factor ::= NUMBER | '(' expr ')' ;
```

## Sample Grammars

The `samples/` directory contains example grammars:

- `expr.ebnf` - Arithmetic expression grammar
- `ifelse.ebnf` - If-else statement grammar (shows conflicts)
- `ebnf_features.ebnf` - Comprehensive EBNF features
- `leftrec.ebnf` - Left recursion examples

## Testing

```bash
# Run all tests
bash tests/run_tests.sh

# Test specific grammar
./cfg2yacc -i samples/expr.ebnf -o test.y
bison -d test.y  # Verify it's valid
```

## Project Structure

```
CD_PROJECT/
├── src/              # Source code
│   ├── main.c        # Entry point
│   ├── grammar.c     # EBNF parsing
│   ├── transform.c   # EBNF→BNF, normalization
│   ├── first_follow.c # Grammar analysis
│   └── emitter.c     # YACC/Flex code generation
├── include/          # Header files
├── samples/          # Example EBNF grammars
├── tests/            # Test suite
├── README.md         # This file
├── USAGE_GUIDE.md    # Detailed usage documentation
└── EXAMPLES.md       # Quick reference examples
```

## Limitations

- Left factoring is heuristic (first token only)
- Indirect left recursion beyond one substitution pass may not be fully optimized
- FOLLOW set computation may exceed 1000 iterations for complex grammars (safety limit)
- AST construction is minimal (skeletal implementation)

## Requirements

- GCC compiler (C11)
- Make
- (Optional) Bison and Flex for testing generated parsers

## License

See project documentation for license information.

## Quick Reference

```bash
# Basic conversion
./cfg2yacc -i grammar.ebnf -o grammar.y

# With lexer
./cfg2yacc -i grammar.ebnf -o grammar.y --emit-lex grammar.l

# Fast (no analysis)
./cfg2yacc -i grammar.ebnf -o grammar.y --no-ff

# Analysis summary
./cfg2yacc -i grammar.ebnf -o grammar.y --ff-summary
```

For detailed documentation, see **[USAGE_GUIDE.md](USAGE_GUIDE.md)**.
