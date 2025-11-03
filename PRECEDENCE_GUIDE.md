# Adding Operator Precedence to Your Grammar

## What is Precedence?

Precedence determines which operator is evaluated first in complex expressions.

For example, in `3 + 5 * 2`:
- **Without precedence**: Could be `(3 + 5) * 2 = 16` OR `3 + (5 * 2) = 13`
- **With precedence**: `3 + (5 * 2) = 13` (multiplication happens first)

## How to Add Precedence in cfg2yacc

You need to add a `%%PRECEDENCE` section to your `.def` file.

### Precedence Directives

- `%left` - Left associative (e.g., `a - b - c` = `(a - b) - c`)
- `%right` - Right associative (e.g., `a ^ b ^ c` = `a ^ (b ^ c)`)
- `%nonassoc` - Non-associative (e.g., `a < b < c` is an error)

**Order matters!** List from **lowest** to **highest** precedence.

## Example 1: Calculator with Precedence (Sample 9)

### Current Sample 9 (NO precedence):
```
expression -> NUMBER operator NUMBER
```

Problem: Can only handle simple expressions like `5 + 3`, not `3 + 5 * 2`

### Enhanced Sample 9 (WITH precedence):

```
# Calculator with Precedence
%%LEX
NUMBER [0-9]+
PLUS \+
MINUS -
TIMES \*
DIVIDE \/
LPAREN \(
RPAREN \)
WHITESPACE [ \t\r]+
NEWLINE \n

%%PRECEDENCE
%left PLUS MINUS          # Lowest precedence (evaluated last)
%left TIMES DIVIDE        # Higher precedence (evaluated first)

%%YACC
document -> expr_list
    { $$ = create_node("calculator", 1, $1); }

expr_list -> expression
    { $$ = create_node("expr_list", 1, $1); }
    | expr_list expression
    { $$ = create_node("expr_list", 2, $1, $2); }

expression -> expr NEWLINE
    { $$ = create_node("expression", 2, $1, $2); }

# Arithmetic expression with precedence
expr -> expr PLUS expr
    { $$ = create_node("add", 3, $1, $2, $3); }
    | expr MINUS expr
    { $$ = create_node("subtract", 3, $1, $2, $3); }
    | expr TIMES expr
    { $$ = create_node("multiply", 3, $1, $2, $3); }
    | expr DIVIDE expr
    { $$ = create_node("divide", 3, $1, $2, $3); }
    | LPAREN expr RPAREN
    { $$ = create_node("parens", 3, $1, $2, $3); }
    | NUMBER
    { $$ = create_node("number", 1, $1); }
```

### Input Examples:
```
3 + 5 * 2      → 3 + (5 * 2) = 13
10 - 4 / 2     → 10 - (4 / 2) = 8
(3 + 5) * 2    → (3 + 5) * 2 = 16
2 * 3 + 4 * 5  → (2 * 3) + (4 * 5) = 26
```

## Example 2: ID Arithmetic with Precedence (Sample 10)

### Current Sample 10 (NO precedence):
```
identifier -> identifier PLUS identifier
            | identifier TIMES identifier
            | identifier MINUS identifier
```

Problem: All operators have same precedence!

### Enhanced Sample 10 (WITH precedence):

```
# ID Arithmetic with Precedence
%%LEX
ID [a-z]
NUM [0-9]+
PLUS \+
TIMES \*
MINUS -
WHITESPACE [ \t\r]+
NEWLINE \n

%%PRECEDENCE
%left PLUS MINUS     # Lower precedence
%left TIMES          # Higher precedence (multiply binds tighter)

%%YACC
document -> expr_list
    { $$ = create_node("document", 1, $1); }

expr_list -> expression
    { $$ = create_node("expr_list", 1, $1); }
    | expr_list expression
    { $$ = create_node("expr_list", 2, $1, $2); }

expression -> identifier NEWLINE
    { $$ = create_node("expression", 2, $1, $2); }

identifier -> identifier PLUS identifier
    { $$ = create_node("identifier", 3, $1, $2, $3); }
    | identifier TIMES identifier
    { $$ = create_node("identifier", 3, $1, $2, $3); }
    | identifier MINUS identifier
    { $$ = create_node("identifier", 3, $1, $2, $3); }
    | NUM
    { $$ = create_node("identifier", 1, $1); }
    | ID
    { $$ = create_node("identifier", 1, $1); }
```

### Input Examples:
```
a+b*c      → a+(b*c)
5+3*2      → 5+(3*2)
x*y+z      → (x*y)+z
```

## How Precedence Works

### Without Precedence:
```
3 + 5 * 2

Could parse as:
  add
  ├─ 3
  └─ multiply
      ├─ 5
      └─ 2
```

### With Precedence (%left PLUS, %left TIMES with TIMES higher):
```
3 + 5 * 2

Correctly parses as:
  add
  ├─ 3
  └─ multiply
      ├─ 5
      └─ 2
```

## Common Precedence Levels (Low to High)

```
%left OR                  # Logical OR (lowest)
%left AND                 # Logical AND
%left EQ NE               # Equality (==, !=)
%left LT GT LE GE         # Comparison (<, >, <=, >=)
%left PLUS MINUS          # Addition, Subtraction
%left TIMES DIVIDE MOD    # Multiplication, Division, Modulo
%right POWER              # Exponentiation (right associative)
%right UMINUS             # Unary minus (highest)
```

## Important Notes

⚠️ **Generator.py Support**: The current `generator.py` may need to be updated to handle `%%PRECEDENCE` section.

✅ **Manual Method**: You can manually edit the generated `parser.y` file to add precedence declarations after the `%token` declarations:

```yacc
%token NUMBER PLUS MINUS TIMES DIVIDE
%left PLUS MINUS
%left TIMES DIVIDE
%%
/* grammar rules */
```

## Testing Precedence

Create test inputs that show precedence:

```
# test_precedence.txt
3 + 5 * 2       # Should be 13, not 16
10 - 6 / 2      # Should be 7, not 2
2 * 3 + 4       # Should be 10, not 14
(2 + 3) * 4     # Should be 20 (parentheses override)
```

Look at the parse tree to verify correct grouping!

## Next Steps

1. **Add %%PRECEDENCE support to generator.py** (if needed)
2. **Create enhanced samples** with precedence
3. **Test with complex expressions**
4. **Verify parse trees** show correct operator grouping
