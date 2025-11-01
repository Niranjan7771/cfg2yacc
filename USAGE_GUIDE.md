# cfg2yacc - Complete Usage Guide

## What is cfg2yacc?

`cfg2yacc` is a C-based command-line tool that converts grammars written in Extended Backus-Naur Form (EBNF) into YACC/Bison grammar files (`.y`). 

### Key Features

1. **EBNF to BNF Conversion**: Automatically expands EBNF constructs (repetition `*`/`+`, optional `?`, groups) into pure BNF
2. **Grammar Normalization**: 
   - Eliminates left recursion
   - Handles common prefixes (left factoring)
3. **FIRST/FOLLOW/NULLABLE Analysis**: Computes and reports grammar analysis sets
4. **Conflict Detection**: Identifies LL(1) conflicts in your grammar
5. **Code Generation**: 
   - Generates ready-to-use YACC/Bison grammar files
   - Optionally generates Flex lexer stubs
   - Creates minimal AST scaffolding

### What the Tool Does

```
EBNF Grammar → [Parse] → [Expand to BNF] → [Normalize] → [Analyze] → YACC File
                                    ↓
                              Flex Lexer (optional)
```

## Installation & Building

### Prerequisites
- GCC compiler
- Make
- (Optional) Bison and Flex for testing generated parsers

### Build Instructions

```bash
# Clone or navigate to the project directory
cd CD_PROJECT

# Build the tool
make

# The executable will be created as: cfg2yacc
```

### Verify Installation

```bash
./cfg2yacc -h
# or
./cfg2yacc --help
```

## How to Run

### Basic Command Format

```bash
./cfg2yacc -i <input.ebnf> -o <output.y> [OPTIONS]
```

### Required Arguments

- `-i, --input <file>`: Input EBNF grammar file (required)
- `-o, --output <file>`: Output YACC file path (required)

### Optional Arguments

- `--emit-lex <file>`: Also generate a Flex lexer stub file
- `--no-ff`: Skip FIRST/FOLLOW computation (faster for large grammars)
- `--ff-summary`: Show concise FIRST/FOLLOW summary (set sizes only)
- `--ff-out <file>`: Write FIRST/FOLLOW analysis to a file instead of stdout
- `-h, --help`: Display help message

## Writing EBNF Grammars

### Grammar File Format

cfg2yacc supports a subset of standard EBNF notation. Here's how to write grammars:

### Basic Syntax

#### 1. Productions

```
NonTerminal ::= RightHandSide ;
```

- Each production ends with a semicolon (`;`)
- Multiple productions for the same nonterminal are allowed
- The first nonterminal becomes the start symbol

**Example:**
```
expr ::= term '+' term ;
term ::= NUMBER ;
```

#### 2. Nonterminals

- Use lowercase identifiers: `expr`, `term`, `factor`, `stmt`
- Must start with a letter, followed by letters, digits, or underscores

**Valid:**
```
expr ::= ... ;
term_1 ::= ... ;
stmt ::= ... ;
```

#### 3. Terminals

Two types of terminals are supported:

**a) Literal Strings (Single-quoted)**
```
expr ::= '(' expr ')' ;
stmt ::= 'if' expr 'then' stmt ;
```

**b) Token Names (ALL_CAPS)**
```
expr ::= NUMBER ;
stmt ::= IF expr THEN stmt ;
```

**Examples:**
```
'('    # Single character literal
'if'   # Multi-character literal
NUMBER # Token name (must be ALL_CAPS)
ID     # Token name
```

#### 4. Alternation (Choice)

Use `|` to separate alternatives:

```
expr ::= term '+' term | term '-' term ;
factor ::= NUMBER | ID | '(' expr ')' ;
```

#### 5. Grouping

Use parentheses `()` to group expressions:

```
expr ::= term ( '+' | '-' ) term ;
stmt ::= IF expr THEN stmt ( ELSE stmt )? ;
```

#### 6. Repetition Operators

**Zero or more (`*`):**
```
list ::= item ( ',' item )* ;
# Matches: item, item,item, item,item,item, etc.
```

**One or more (`+`):**
```
numbers ::= NUMBER ( ',' NUMBER )+ ;
# Matches: NUMBER, NUMBER,NUMBER, etc. (at least one)
```

**Optional (`?`):**
```
stmt ::= IF expr THEN stmt ( ELSE stmt )? ;
# ELSE clause is optional
```

#### 7. Comments

Lines starting with `#` are comments:

```
# This is a comment
expr ::= term ;  # Inline comment
# Another comment
```

### Complete Grammar Examples

#### Example 1: Simple Arithmetic Expression

Create `simple_expr.ebnf`:

```
expr ::= term ( ('+' | '-') term )* ;
term ::= factor ( ('*' | '/') factor )* ;
factor ::= NUMBER | '(' expr ')' ;
```

**Convert to YACC:**
```bash
./cfg2yacc -i simple_expr.ebnf -o simple_expr.y
```

#### Example 2: If-Else Statements

Create `ifelse.ebnf`:

```
stmt ::= IF expr THEN stmt ( ELSE stmt )? ;
stmt ::= ID '=' expr ';' ;
expr ::= ID | NUMBER ;
```

**Convert with lexer:**
```bash
./cfg2yacc -i ifelse.ebnf -o ifelse.y --emit-lex ifelse.l
```

#### Example 3: Lists and Repetition

Create `lists.ebnf`:

```
list ::= '[' items? ']' ;
items ::= item ( ',' item )* ;
item ::= NUMBER | STRING ;
```

**Convert:**
```bash
./cfg2yacc -i lists.ebnf -o lists.y --emit-lex lists.l
```

#### Example 4: Complex Grammar with All Features

Create `complex.ebnf`:

```
program ::= stmt+ ;
stmt ::= declaration | assignment | if_stmt ;
declaration ::= TYPE ID ( ',' ID )* ';' ;
assignment ::= ID '=' expr ';' ;
if_stmt ::= IF expr THEN stmt ( ELSE stmt )? ;
expr ::= term ( ('+' | '-' | '*' | '/') term )* ;
term ::= NUMBER | ID | '(' expr ')' ;
```

**Convert with analysis:**
```bash
./cfg2yacc -i complex.ebnf -o complex.y --ff-summary
```

## Common Run Commands

### Basic Conversion

```bash
# Simple conversion
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y
```

### With Lexer Generation

```bash
# Generate both YACC and Flex files
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --emit-lex mygrammar.l
```

### Fast Conversion (Skip Analysis)

```bash
# Skip FIRST/FOLLOW computation for speed
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --no-ff
```

### Get Grammar Analysis

```bash
# Full FIRST/FOLLOW output
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y

# Concise summary
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --ff-summary

# Save analysis to file
./cfg2yacc -i mygrammar.ebnf -o mygrammar.y --ff-out analysis.txt
```

### Complete Workflow: From EBNF to Running Parser

```bash
# Step 1: Write your EBNF grammar (e.g., calculator.ebnf)
cat > calculator.ebnf << 'EOF'
expr ::= term ( ('+' | '-') term )* ;
term ::= factor ( ('*' | '/') factor )* ;
factor ::= NUMBER | '(' expr ')' ;
EOF

# Step 2: Convert to YACC
./cfg2yacc -i calculator.ebnf -o calculator.y --emit-lex calculator.l

# Step 3: Generate parser with Bison
bison -d calculator.y
# Creates: calculator.tab.c, calculator.tab.h

# Step 4: Generate lexer with Flex
flex calculator.l
# Creates: lex.yy.c

# Step 5: Prepare AST files
cp include/ast.h .
./cfg2yacc -i calculator.ebnf -o calculator.y --emit-lex calculator.l --no-ff
# Creates: ast.c

# Step 6: Compile everything
gcc -o calculator calculator.tab.c ast.c lex.yy.c -lfl -Iinclude

# Step 7: Test the parser
echo "1 + 2 * 3" | ./calculator
```

## Testing with Sample Grammars

The project includes several example grammars in the `samples/` directory:

```bash
# View available samples
ls samples/*.ebnf

# Test expression grammar
./cfg2yacc -i samples/expr.ebnf -o expr.y --no-ff
bison -d expr.y  # Verify it's valid

# Test if-else grammar
./cfg2yacc -i samples/ifelse.ebnf -o ifelse.y
bison -dv ifelse.y  # View conflicts
cat ifelse.output | grep conflict

# Test comprehensive EBNF features
./cfg2yacc -i samples/ebnf_features.ebnf -o features.y --ff-summary
```

## Grammar Analysis Output

### Understanding FIRST/FOLLOW Output

When you run without `--no-ff`, cfg2yacc shows:

**NULLABLE sets**: Which nonterminals can derive the empty string
```
NULLABLE:
  expr__gen_1 : yes
  factor : no
```

**FIRST sets**: The set of terminals that can start a derivation
```
FIRST sets:
  expr : {NUMBER, '('}
  term : {NUMBER, '('}
```

**FOLLOW sets**: The set of terminals that can follow a nonterminal
```
FOLLOW sets:
  expr : {$, ')'}
  factor : {'*', '/', $, '+', '-', ')'}
```

**Conflict Warnings**: LL(1) conflicts detected
```
LL(1) conflict checks:
  [warn] expr alternatives 1 and 2 overlap in FIRST
```

### Using --ff-summary

For large grammars, use `--ff-summary` for concise output:

```bash
./cfg2yacc -i large_grammar.ebnf -o large.y --ff-summary
```

Output:
```
FIRST/FOLLOW summary (counts only):
  nonterminals: 25
  nullable: 3
  total FIRST entries: 67
  total FOLLOW entries: 89
  expr -> FIRST:5, FOLLOW:3
  term -> FIRST:3, FOLLOW:4
```

## Troubleshooting

### Common Errors

**1. "unexpected token" error**
```
[grammar] line 3: unexpected token
```
- Check syntax: missing `::=`, semicolon, or quote mismatch
- Ensure terminals are quoted or ALL_CAPS

**2. "FOLLOW set computation exceeded 1000 iterations"**
```
[first_follow] warning: FOLLOW set computation exceeded 1000 iterations
```
- Normal for complex grammars
- Tool stops safely at 1000 iterations
- Use `--no-ff` to skip if not needed

**3. Bison reports "useless nonterminals"**
- These are helper nonterminals from EBNF expansion
- Safe to ignore if parser works correctly

**4. "bad character" warnings from Flex**
- Minor Flex warnings about character patterns
- Generated lexer still compiles and works

### Tips

1. **Start Simple**: Begin with basic productions, add complexity gradually
2. **Test Incrementally**: Convert and test after each major change
3. **Use --no-ff**: Skip analysis during development for faster iteration
4. **Check Conflicts**: Run full analysis before finalizing grammar
5. **Review Output**: Check `bison -dv output.file` for parser states

## Quick Reference

### Command Template
```bash
./cfg2yacc -i <grammar.ebnf> -o <output.y> [--emit-lex <lexer.l>] [OPTIONS]
```

### EBNF Operators
- `|` - Alternation (choice)
- `()` - Grouping
- `*` - Zero or more
- `+` - One or more  
- `?` - Optional (zero or one)

### Terminal Types
- `'literal'` - Single-quoted strings
- `TOKEN` - ALL_CAPS identifiers

### Example Minimal Grammar
```
start ::= item+ ;
item ::= NUMBER | ID ;
```

**Run:**
```bash
./cfg2yacc -i minimal.ebnf -o minimal.y --emit-lex minimal.l --no-ff
```

## Advanced Usage

### Processing Multiple Grammars

```bash
for grammar in *.ebnf; do
    name=$(basename "$grammar" .ebnf)
    ./cfg2yacc -i "$grammar" -o "${name}.y" --no-ff
    echo "Converted: $grammar → ${name}.y"
done
```

### Generating Documentation

```bash
# Save full analysis
./cfg2yacc -i grammar.ebnf -o grammar.y --ff-out grammar_analysis.txt

# Generate YACC with summary
./cfg2yacc -i grammar.ebnf -o grammar.y --ff-summary > summary.txt 2>&1
```

### Integration with Build Systems

**Makefile example:**
```makefile
PARSER = myparser
GRAMMAR = grammar.ebnf

$(PARSER): grammar.tab.c lex.yy.c ast.c
	gcc -o $@ $^ -lfl -Iinclude

grammar.tab.c: grammar.y
	bison -d grammar.y

grammar.y: $(GRAMMAR)
	./cfg2yacc -i $(GRAMMAR) -o grammar.y --emit-lex grammar.l --no-ff

lex.yy.c: grammar.l
	flex grammar.l

ast.c: grammar.y
	./cfg2yacc -i $(GRAMMAR) -o /dev/null --no-ff
```

For more examples, see `EXAMPLES.md` in the project directory.

