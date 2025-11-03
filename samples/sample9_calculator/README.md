# 🧮 Simple Calculator - The Easiest Example

## 📚 What is This?

This is the **SIMPLEST example** to understand how cfg2yacc works.  
It's a basic calculator that recognizes math expressions like:
- `5 + 3`
- `10 - 2`
- `4 * 6`
- `20 / 4`

## 🎯 Why This Example?

All the other samples (email analysis, medical records, etc.) are complex.  
**This one is super simple** - perfect for learning!

## 🔍 How It Works (Step by Step)

### Step 1: Define Tokens (What to Recognize)

In `S9_analyzer.def`, we tell the system what patterns to look for:

```
NUMBER [0-9]+        ← Any digits (5, 10, 100, etc.)
PLUS \+              ← The + symbol
MINUS -              ← The - symbol
TIMES \*             ← The * symbol
DIVIDE \/            ← The / symbol
```

### Step 2: Define Grammar (How to Structure)

We tell it how these tokens fit together:

```
expression -> NUMBER operator NUMBER
```

This means: "An expression is a NUMBER, followed by an OPERATOR, followed by another NUMBER"

So `5 + 3` matches this pattern:
- `5` is a NUMBER
- `+` is an operator (PLUS)
- `3` is a NUMBER

### Step 3: Build the Parser

```bash
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def
```

The system automatically:
1. ✅ Generates a lexer (recognizes tokens)
2. ✅ Generates a parser (builds the structure)
3. ✅ Creates a parse tree (shows the result)

### Step 4: See the Result

For input `5 + 3`, you get this tree:

```
calculator
  └─ expr_list
      └─ expression
          ├─ NUMBER: 5
          ├─ add
          │   └─ PLUS: +
          └─ NUMBER: 3
```

## 📖 The Files

```
S9_analyzer.def  ← The main definition file (tokens + grammar)
S9_input.txt     ← Sample math expressions to parse
S9_rules.txt     ← This explanation
number_tokens.txt ← Auto-generated examples (123, 456, etc.)
```

## 🚀 Quick Start

### Run It!
```bash
# Option 1: Terminal visualization
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Option 2: Web UI
./run_ui.sh
# Then select "Sample 9: Calculator" from the dropdown
```

### Try Your Own Math!
```bash
# Create your own expressions
echo "25 + 17" > my_math.txt
echo "8 * 9" >> my_math.txt

# Run it
./custom_compiler < my_math.txt | python3 visualize_tree.py --style fancy
```

## 🎓 What You Learn

From this simple example, you understand:

1. **Tokens** = Individual pieces (numbers, operators)
2. **Grammar** = How pieces fit together (NUMBER + NUMBER)
3. **Parse Tree** = Visual structure showing the result

Once you understand this, you can look at the complex examples:
- Sample 1: Email analysis (similar, but with email addresses)
- Sample 8: Medical records (same concept, different tokens)

## 🔬 The Complete Flow

```
Input:          5 + 3
                 ↓
Lexer:          [NUMBER:5] [PLUS:+] [NUMBER:3]
                 ↓
Parser:         Matches "expression" pattern
                 ↓
Parse Tree:     
                calculator
                  └─ expression
                      ├─ NUMBER: 5
                      ├─ add: +
                      └─ NUMBER: 3
```

## 💡 Key Concept

**This is a mini compiler!**

1. **Lexer** (tokenizer): Breaks text into tokens
2. **Parser**: Organizes tokens into meaningful structure
3. **Parse Tree**: Shows the structure visually

Real compilers (C, Python, Java) work the exact same way, just with more tokens and more complex grammar!

## 🎨 Visualization Options

```bash
# Fancy style (boxes and colors)
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Simple style (plain indents)
make run-simple DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Compact style (just terminals)
make run-compact DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Statistics
./custom_compiler < samples/sample9_calculator/S9_input.txt | python3 visualize_tree.py --stats
```

## ✅ Next Steps

Once you understand this example:

1. **Modify it**: Add more operators (%, ^)
2. **Extend it**: Support parentheses like `(5 + 3) * 2`
3. **Try other samples**: Look at Sample 7 (basic example)
4. **Create your own**: Make a simple language!

---

**Remember**: All the complex samples work the same way as this simple calculator.  
They just have:
- More tokens (emails, phone numbers, medical codes)
- More complex grammar rules
- But the core concept is identical!
