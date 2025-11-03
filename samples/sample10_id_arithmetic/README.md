# Sample 10: ID Arithmetic ✅

## 📋 Overview

This sample demonstrates the exact grammar rules you specified:

**Tokens:**
- `id` → `[a-z]` (single lowercase letter)
- `num` → `[0-9]+` (one or more digits)
- `sym` → `+|*|-` (operators)

**Grammar:**
```
id -> id * id
id -> id + id
id -> id - id
id -> num
```

## ✅ Successfully Built and Tested!

### Input Examples:
```
a           ✓ Single ID
b+c         ✓ ID + ID
x*y         ✓ ID * ID
m-n         ✓ ID - ID
5           ✓ Single NUM (which is an identifier)
3+7         ✓ NUM + NUM
9*2         ✓ NUM * NUM
8-4         ✓ NUM - NUM
a+5         ✓ ID + NUM (mixed)
x*3         ✓ ID * NUM (mixed)
y-2         ✓ ID - NUM (mixed)
```

### Parse Tree Example

For input `b+c`:
```
document
  └─ expr_list
      └─ expression
          ├─ identifier
          │   ├─ identifier (b)
          │   ├─ PLUS (+)
          │   └─ identifier (c)
          └─ NEWLINE
```

For input `3+7`:
```
document
  └─ expr_list
      └─ expression
          ├─ identifier
          │   ├─ identifier
          │   │   └─ NUM (3)
          │   ├─ PLUS (+)
          │   └─ identifier
          │       └─ NUM (7)
          └─ NEWLINE
```

## 🎯 Grammar Explanation

The grammar says:
1. An **identifier** can be a **NUM** (number is treated as an identifier)
2. An **identifier** can be an **ID** (single letter)
3. An **identifier** can be **identifier + identifier**
4. An **identifier** can be **identifier * identifier**
5. An **identifier** can be **identifier - identifier**

This allows mixing letters and numbers in expressions!

## 🚀 How to Run

```bash
# Terminal visualization
make run DEF_FILE=samples/sample10_id_arithmetic/S10_analyzer.def

# Web UI
./run_ui.sh
# Select "Sample 10: ID Arithmetic"

# Custom input
echo "z+9" | ./custom_compiler | python3 visualize_tree.py --style fancy
```

## 📝 Implementation Notes

### Token Mapping
In the `.def` file, the tokens are named differently to avoid conflicts:
- `id` → `ID` (lexer token)
- `num` → `NUM` (lexer token)
- Both become `identifier` in the grammar (non-terminal)

This is because YACC distinguishes between:
- **Terminals** (tokens from lexer): `ID`, `NUM`, `PLUS`, `TIMES`, `MINUS`
- **Non-terminals** (grammar rules): `identifier`, `expression`, `expr_list`

### Why This Works

The grammar is **left-recursive** which YACC handles well:
```
identifier -> identifier PLUS identifier
```

This means you can build up complex expressions:
- `3` is an identifier (base case: NUM)
- `7` is an identifier (base case: NUM)
- `3+7` is an identifier (recursive case: identifier + identifier)

## 🔍 Parse Tree Statistics

All 11 input lines successfully parsed:
- 11 expressions
- Mix of single letters, numbers, and arithmetic operations
- Demonstrates left-to-right parsing
- Shows how simple tokens combine into complex structures

## ✨ Key Takeaway

This demonstrates how **context-free grammars** work:
- Simple rules (`identifier -> NUM`)
- Recursive rules (`identifier -> identifier + identifier`)
- Build complex structures from simple parts
- Same concept used in real programming languages!
