# How to Enforce Operator Precedence

## ✅ Working Examples Included!

I've created working precedence examples for both Sample 9 and Sample 10.

## 📁 Files Created

### Sample 9 (Calculator) with Precedence:
- `samples/sample9_calculator/S9_precedence.def` - Grammar with precedence
- `samples/sample9_calculator/S9_precedence_input.txt` - Test inputs

### Sample 10 (ID Arithmetic) with Precedence:  
- `samples/sample10_id_arithmetic/S10_precedence.def` - Grammar with precedence
- `samples/sample10_id_arithmetic/S10_precedence_input.txt` - Test inputs

## 🎯 Two Methods to Enforce Precedence

### Method 1: Grammar Stratification (✅ USED IN EXAMPLES)

This is the **recommended method** - structure your grammar in layers:

```
expression   (lowest precedence - addition/subtraction)
    ↓
term         (higher precedence - multiplication/division)
    ↓  
factor       (highest precedence - numbers/parentheses)
```

**Example from S9_precedence.def:**
```yacc
# Layer 1: Addition and Subtraction (lowest precedence)
expr -> expr PLUS term
      | expr MINUS term
      | term

# Layer 2: Multiplication and Division (higher precedence)
term -> term TIMES factor
      | term DIVIDE factor
      | factor

# Layer 3: Atoms (highest precedence)
factor -> NUMBER
        | LPAREN expr RPAREN
```

**Why it works:**
- `expr` can contain `term`, but `term` must be fully evaluated first
- `term` can contain `factor`, but `factor` must be fully evaluated first
- This forces `*` and `/` to bind tighter than `+` and `-`

### Method 2: YACC Precedence Declarations

You can also use `%left`, `%right`, `%nonassoc` directives:

```yacc
%left PLUS MINUS       # Lower precedence
%left TIMES DIVIDE     # Higher precedence

%%
expr -> expr PLUS expr
      | expr MINUS expr
      | expr TIMES expr
      | expr DIVIDE expr
      | NUMBER
```

**Note:** This requires modifying `generator.py` to support `%%PRECEDENCE` section.

## 🚀 How to Test

### Test Sample 9 (Calculator with Precedence):
```bash
python3 generator.py samples/sample9_calculator/S9_precedence.def
bison -d -o y.tab.c parser.y
flex lexer.l
gcc -Wall -g -c ast.c -o ast.o
gcc -Wall -g -o custom_compiler y.tab.c lex.yy.c ast.o -lfl
./custom_compiler < samples/sample9_calculator/S9_precedence_input.txt | python3 visualize_tree.py --style simple
```

### Test Sample 10 (ID Arithmetic with Precedence):
```bash
python3 generator.py samples/sample10_id_arithmetic/S10_precedence.def
bison -d -o y.tab.c parser.y
flex lexer.l
gcc -Wall -g -o custom_compiler y.tab.c lex.yy.c ast.o -lfl
./custom_compiler < samples/sample10_id_arithmetic/S10_precedence_input.txt | python3 visualize_tree.py --style simple
```

## 📊 Verification Results

### Sample 9 Results:

| Input | Expected | Parse Tree Shows |
|-------|----------|------------------|
| `3 + 5 * 2` | 3 + (5 * 2) | add { 3, multiply { 5, 2 } } ✅ |
| `10 - 4 / 2` | 10 - (4 / 2) | subtract { 10, divide { 4, 2 } } ✅ |
| `2 * 3 + 4 * 5` | (2 * 3) + (4 * 5) | add { multiply {2, 3}, multiply {4, 5} } ✅ |
| `(3 + 5) * 2` | (3 + 5) * 2 | multiply { parens { add {3, 5} }, 2 } ✅ |

### Sample 10 Results:

| Input | Expected | Parse Tree Shows |
|-------|----------|------------------|
| `a+b*c` | a + (b * c) | add { a, multiply { b, c } } ✅ |
| `5+3*2` | 5 + (3 * 2) | add { 5, multiply { 3, 2 } } ✅ |
| `x*y+z` | (x * y) + z | add { multiply { x, y }, z } ✅ |
| `2*3+4*5` | (2 * 3) + (4 * 5) | add { multiply {2, 3}, multiply {4, 5} } ✅ |

## 🔍 How to Read Parse Trees

Look at the **structure** to verify precedence:

**Without Precedence (WRONG):**
```
Input: 3 + 5 * 2

multiply           ← Multiplication at top (evaluated last)
  ├─ add           ← Addition evaluated first
  │   ├─ 3
  │   └─ 5
  └─ 2

Result: (3 + 5) * 2 = 16  ❌ WRONG!
```

**With Precedence (CORRECT):**
```
Input: 3 + 5 * 2

add                ← Addition at top (evaluated last)
  ├─ 3
  └─ multiply      ← Multiplication evaluated first
      ├─ 5
      └─ 2

Result: 3 + (5 * 2) = 13  ✅ CORRECT!
```

**Rule:** Operations **deeper** in the tree are evaluated **first**!

## 💡 Key Concepts

1. **Precedence**: Which operator is evaluated first
   - High precedence = evaluated first = deeper in tree
   - Low precedence = evaluated last = higher in tree

2. **Associativity**: How operators of same precedence are grouped
   - Left: `a - b - c` = `(a - b) - c`
   - Right: `a ^ b ^ c` = `a ^ (b ^ c)`

3. **Grammar Layers**: Natural way to express precedence
   - Each layer handles operators of same precedence
   - Lower layers = higher precedence

## 📝 Summary

✅ **Method 1 (Grammar Stratification)** - No special syntax needed, works now!
- Sample 9: `expr → term → factor` (3 layers)
- Sample 10: `identifier → term → factor` (3 layers)

⚠️ **Method 2 (Precedence Declarations)** - Requires generator.py update
- Would need `%%PRECEDENCE` section support
- More concise but requires code changes

**Recommendation:** Use Method 1 (grammar stratification) - it's already working and doesn't require any tool modifications!
