# Quick Start Guide - cfg2yacc

Get started with cfg2yacc in 5 minutes!

## Step 1: Build the Tool

```bash
cd CD_PROJECT
make
```

You should now have an executable: `./cfg2yacc`

## Step 2: Create Your First Grammar

Create a file `hello.ebnf`:

```
expr ::= NUMBER ;
```

## Step 3: Convert to YACC

```bash
./cfg2yacc -i hello.ebnf -o hello.y
```

This creates `hello.y` - a valid YACC grammar file!

## Step 4: Verify with Bison

```bash
bison -d hello.y
```

If this succeeds without errors, your grammar is valid!

## Common Examples

### Example 1: Simple Expression

**File: `simple.ebnf`**
```
expr ::= NUMBER '+' NUMBER ;
```

**Run:**
```bash
./cfg2yacc -i simple.ebnf -o simple.y --no-ff
bison -d simple.y
```

### Example 2: Expression with Operators

**File: `calc.ebnf`**
```
expr ::= term ( ('+' | '-') term )* ;
term ::= NUMBER | '(' expr ')' ;
```

**Run:**
```bash
./cfg2yacc -i calc.ebnf -o calc.y --emit-lex calc.l --no-ff
```

### Example 3: Lists

**File: `list.ebnf`**
```
list ::= '[' items? ']' ;
items ::= NUMBER ( ',' NUMBER )* ;
```

**Run:**
```bash
./cfg2yacc -i list.ebnf -o list.y
```

## Command Cheat Sheet

```bash
# Basic
./cfg2yacc -i <input.ebnf> -o <output.y>

# With lexer
./cfg2yacc -i <input.ebnf> -o <output.y> --emit-lex <lexer.l>

# Fast (skip analysis)
./cfg2yacc -i <input.ebnf> -o <output.y> --no-ff

# View analysis
./cfg2yacc -i <input.ebnf> -o <output.y> --ff-summary
```

## Grammar Syntax Cheat Sheet

```
NonTerminal ::= ... ;           # Production
'literal'                       # Quoted terminal
TOKEN                          # ALL_CAPS terminal
A | B                          # Alternation (A or B)
( A B )                        # Grouping
A *                            # Zero or more
A +                            # One or more
A ?                            # Optional
# comment                      # Comment
```

## Try the Samples

```bash
# Expression grammar
./cfg2yacc -i samples/expr.ebnf -o expr.y --no-ff

# If-else grammar
./cfg2yacc -i samples/ifelse.ebnf -o ifelse.y

# All features
./cfg2yacc -i samples/ebnf_features.ebnf -o features.y --ff-summary
```

## Next Steps

- Read **[USAGE_GUIDE.md](USAGE_GUIDE.md)** for detailed documentation
- Check **[EXAMPLES.md](EXAMPLES.md)** for more examples
- Explore `samples/` directory for grammar examples

## Troubleshooting

**Error: "unexpected token"**
- Check grammar syntax (semicolons, quotes)
- Ensure terminals are quoted or ALL_CAPS

**Warning: "exceeded 1000 iterations"**
- Normal for complex grammars
- Use `--no-ff` to skip analysis

**Need help?**
```bash
./cfg2yacc --help
```

