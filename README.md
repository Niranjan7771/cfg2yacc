# cfg2yacc

**A Modern Compiler Generator with Interactive Parse Tree Visualization**

[![Language](https://img.shields.io/badge/language-Python%203-blue.svg)](https://www.python.org/)
[![Tools](https://img.shields.io/badge/tools-Flex%20%7C%20Bison-green.svg)](https://www.gnu.org/software/bison/)
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)

Transform simple grammar definitions into fully-functional compilers with beautiful, colorful parse tree visualization. Perfect for compiler education, pattern recognition, log analysis, and rapid prototyping of domain-specific languages.

---

## 🌟 Features

- **🎯 Simple Syntax** - Define tokens and grammar rules in an intuitive `.def` format.
- **🎨 Dual Visualization** - Interactive web UI and colorful terminal output.
- **⚡ Auto-Generation** - Automatic token example file generation.
- **📊 10 Ready Samples** - Production-ready analyzers for common use cases.
- **🔧 Precedence Support** - Built-in operator precedence enforcement.
- **🚀 Zero Config** - Works out of the box with minimal setup.
- **📚 Educational** - Excellent for learning compiler construction.

---

## 📋 Table of Contents

- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [Sample Analyzers](#-sample-analyzers)
- [Creating Custom Analyzers](#-creating-custom-analyzers)
- [Operator Precedence](#-operator-precedence)
- [Visualization](#-visualization)
- [Project Structure](#-project-structure)
- [Advanced Topics](#-advanced-topics)
- [Contributing](#-contributing)

---

## 🚀 Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/Niranjan7771/cfg2yacc.git
cd cfg2yacc

# 2. Install dependencies
sudo apt-get install build-essential flex bison python3
pip3 install -r requirements.txt

# 3. Run a sample analyzer
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# 4. Or launch the web UI
./run_ui.sh
```

**That's it!** You'll see a colorful parse tree visualization of the analyzed text.

---

## 💾 Installation

### Prerequisites

- **Operating System**: Linux (Ubuntu/Debian recommended)
- **Build Tools**: GCC, Make
- **Parser Generators**: Flex (≥ 2.6), Bison (≥ 3.0)
- **Python**: 3.6 or higher
- **Optional**: Streamlit (for web UI)

### System Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential flex bison python3 python3-pip

# Fedora/RHEL
sudo dnf install -y gcc make flex bison python3 python3-pip

# Arch Linux
sudo pacman -S base-devel flex bison python python-pip
```

### Python Dependencies

```bash
pip3 install -r requirements.txt
```

**Contents of `requirements.txt`:**
```
streamlit>=1.51.0
graphviz>=0.21
```

### Verification

```bash
flex --version    # Should show 2.6.x or higher
bison --version   # Should show 3.x or higher
python3 --version # Should show 3.6 or higher
```

---

## 🎯 Usage

### Basic Workflow

1. **Create a `.def` file** with your grammar definition.
2. **Build the analyzer** using the Makefile.
3. **Run with input** and visualize the parse tree.

### Command Line

```bash
# Build and run with terminal visualization
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Use custom input file
make run DEF_FILE=path/to/analyzer.def INPUT_FILE=path/to/input.txt

# Different visualization styles
make run-simple DEF_FILE=...   # Simple indented tree
make run-compact DEF_FILE=...  # Compact (terminals only)
make run-stats DEF_FILE=...    # Statistics summary

# Clean build artifacts
make clean
```

### Web Interface

```bash
# Launch interactive web UI
./run_ui.sh
```

Access at `http://localhost:8501`.

**Features:**
- Dropdown to select any sample analyzer.
- Live build and run.
- Interactive graph visualization.
- Custom input editor.
- Parse tree statistics.
- Export functionality.

---

## 📦 Sample Analyzers

cfg2yacc includes 10 production-ready sample analyzers demonstrating various use cases:

| Sample | Description | Use Case | Tokens |
|--------|-------------|----------|--------|
| **[Sample 1](samples/sample1_email_analysis/)** | Email Analysis | Extract structured data from emails | EMAIL, PHONE, CURRENCY, DATE, URL, URGENT |
| **[Sample 2](samples/sample2_code_analysis/)** | Code Analysis | Parse source code elements | FUNCTION_DEF, VARIABLE_ASSIGN, STRING_LITERAL |
| **[Sample 3](samples/sample3_log_analysis/)** | Log Analysis | Process server/application logs | TIMESTAMP, IP_ADDRESS, HTTP_METHOD, STATUS_CODE |
| **[Sample 4](samples/sample4_spam_detection/)** | Spam Detection | Identify spam patterns | SPAM_WORDS, MONEY_TERMS, SUSPICIOUS_LINK, CAPS_WORD |
| **[Sample 5](samples/sample5_social_media/)** | Social Media | Parse social media content | HASHTAG, USER_MENTION, EMOJI_FACE, LIKE_COUNT |
| **[Sample 6](samples/sample6_financial_data/)** | Financial Data | Extract financial information | CURRENCY_USD, ACCOUNT_NUMBER, STOCK_SYMBOL, TRANSACTION_ID |
| **[Sample 7](samples/sample7_basic_example/)** | Basic Example | Simple pattern matching (best for learning) | EMAIL, PHONE, FLOAT_NUMBER, INTEGER |
| **[Sample 8](samples/sample8_medical_analysis/)** | Medical Records | Parse medical data | PATIENT_ID, BLOOD_TYPE, BLOOD_PRESSURE, DIAGNOSIS_CODE |
| **[Sample 9](samples/sample9_calculator/)** | Simple Calculator | Basic arithmetic expressions (educational) | NUMBER, PLUS, MINUS, TIMES, DIVIDE |
| **[Sample 10](samples/sample10_id_arithmetic/)** | ID Arithmetic | Custom grammar demonstration | ID, NUM, operators |

### Running Samples

```bash
# Email analysis
make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

# Simple calculator (best for beginners)
make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

# Medical records
make run DEF_FILE=samples/sample8_medical_analysis/S8_analyzer.def
```

Each sample includes:
- `SX_analyzer.def` - Grammar definition.
- `SX_input.txt` - Test input data.
- `SX_rules.txt` - Documentation.
- `*_tokens.txt` - Auto-generated token examples.

---

## 🔨 Creating Custom Analyzers

### Step 1: Create a `.def` File

Create a file with `%%LEX` and `%%YACC` sections:

```python
# my_analyzer.def

%%LEX
# Define tokens with regex patterns
NUMBER [0-9]+
EMAIL           [a-zA-Z0-9._]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}
WORD [a-zA-Z]+
WHITESPACE [ \t\r]+
PHONE_NUMBER    [0-9]{3}-[0-9]{3}-[0-9]{4}
NEWLINE \n

%%YACC
# Define grammar rules
document -> item_list
    { $$ = create_node("document", 1, $1); }

item_list -> item
    { $$ = create_node("item_list", 1, $1); }
    | item_list item
    { $$ = create_node("item_list", 2, $1, $2); }

item -> WORD NEWLINE
    { $$ = create_node("item", 2, $1, $2); }
    | NUMBER NEWLINE
    { $$ = create_node("item", 2, $1, $2); }
```

### Step 2: Create Input File

```bash
# my_input.txt
hello
42
world
123
```

### Step 3: Build and Run

```bash
make run DEF_FILE=my_analyzer.def INPUT_FILE=my_input.txt
```

### Grammar Rule Syntax

Each grammar rule follows this format:

```yacc
non_terminal -> production
    { $$ = create_node("node_name", count, child1, child2, ...); }
```

**Components:**
- `non_terminal` - Left-hand side (what you're defining).
- `production` - Right-hand side (what it consists of).
- `create_node()` - Creates a parse tree node.
  - First arg: Node label (string).
  - Second arg: Number of children.
  - Remaining args: Child nodes ($1, $2, $3, ...).

**Special variables:**
- `$$` - Current node being created.
- `$1, $2, $3...` - Children from the production (left to right).

### Alternative Productions

Use `|` for multiple productions:

```yacc
expression -> expression PLUS term
    { $$ = create_node("add", 3, $1, $2, $3); }
    | expression MINUS term
    { $$ = create_node("subtract", 3, $1, $2, $3); }
    | term
    { $$ = $1; }
```

---

## ⚖️ Operator Precedence

cfg2yacc supports operator precedence through **grammar stratification** - structuring your grammar in layers.

### Example: Calculator with Precedence

```yacc
%%YACC
# Layer 1: Addition/Subtraction (lowest precedence - evaluated last)
expr -> expr PLUS term
      | expr MINUS term
      | term

# Layer 2: Multiplication/Division (higher precedence - evaluated first)
term -> term TIMES factor
      | term DIVIDE factor
      | factor

# Layer 3: Atoms (highest precedence)
factor -> NUMBER
        | LPAREN expr RPAREN
```

This ensures that `3 + 5 * 2` correctly parses as `3 + (5 * 2) = 13`, not `(3 + 5) * 2 = 16`.

### How It Works

**Lower in the grammar = Higher precedence**
- Operations at the `factor` level bind tightest.
- Operations at the `term` level bind tighter than `expr`.
- Operations at the `expr` level bind loosest.

### Precedence Examples

See detailed examples with parse trees:
- **[samples/sample9_calculator/S9_precedence.def](samples/sample9_calculator/S9_precedence.def)** - Calculator with `*`, `/` > `+`, `-`.
- **[samples/sample10_id_arithmetic/S10_precedence.def](samples/sample10_id_arithmetic/S10_precedence.def)** - ID arithmetic with precedence.
- **[PRECEDENCE_GUIDE.md](PRECEDENCE_GUIDE.md)** - Complete precedence documentation.

---

## 🎨 Visualization

cfg2yacc offers three visualization methods:

### 1. Terminal Visualization (Default)

**Fancy Style** (with box drawing):
```
╔════════════════════════════════════╗
║  Parse Tree Visualization          ║
╚════════════════════════════════════╝

calculator
  ├─ expr_list
    ├─ expression
      ├─ NUMBER ➜ 5
      ├─ add
        └─ PLUS ➜ +
      └─ NUMBER ➜ 3
```

**Simple Style** (clean indents):
```bash
make run-simple DEF_FILE=...
```

**Compact Style** (terminals only):
```bash
make run-compact DEF_FILE=...
```

**Statistics Mode**:
```bash
make run-stats DEF_FILE=...
# Shows: node count, depth, terminal count, etc.
```

### 2. Web UI (Interactive)

```bash
./run_ui.sh
```

**Features:**
- 📂 Dropdown menu to select any sample.
- 🔨 One-click build and run.
- 📊 Interactive graph visualization.
- ✏️ Custom input editor.
- 📈 Parse tree statistics.
- 💾 Export functionality.
- 🎨 Responsive design.

### 3. Programmatic Access

```python
import visualize_tree

# Parse tree from stdin
tree = visualize_tree.parse_tree_from_input()

# Render with custom style
visualize_tree.print_tree(tree, style='fancy')
visualize_tree.print_tree(tree, style='simple')
visualize_tree.print_tree(tree, style='compact')

# Get statistics
stats = visualize_tree.get_statistics(tree)
print(f"Nodes: {stats['total_nodes']}")
print(f"Depth: {stats['max_depth']}")
```

---

## 📁 Project Structure

```
cfg2yacc/
├── README.md                      # This file
├── Makefile                       # Build automation
├── generator.py                   # Main generator (def → lex/yacc)
├── ast.c / ast.h                  # Parse tree data structures
├── visualize_tree.py              # Terminal visualization
├── streamlit_visualizer.py        # Web UI
├── run_ui.sh                      # Web UI launcher
├── requirements.txt               # Python dependencies
├── PRECEDENCE_GUIDE.md           # Precedence documentation
│
├── samples/                       # Sample analyzers
│   ├── sample1_email_analysis/
│   │   ├── S1_analyzer.def        # Grammar definition
│   │   ├── S1_input.txt           # Test input
│   │   ├── S1_rules.txt           # Documentation
│   │   └── *_tokens.txt           # Auto-generated examples
│   ├── sample2_code_analysis/
│   ├── sample3_log_analysis/
│   ├── sample4_spam_detection/
│   ├── sample5_social_media/
│   ├── sample6_financial_data/
│   ├── sample7_basic_example/
│   ├── sample8_medical_analysis/
│   ├── sample9_calculator/        # Simple calculator (educational)
│   ├── sample10_id_arithmetic/    # Custom grammar demo
│   └── PRECEDENCE_HOWTO.md        # Precedence how-to
│
└── (generated at build time)
    ├── lexer.l                    # Generated Flex lexer
    ├── parser.y                   # Generated Bison parser
    ├── lex.yy.c                   # Compiled lexer
    ├── y.tab.c / y.tab.h          # Compiled parser
    └── custom_compiler            # Final executable
```

---

## 🎓 Advanced Topics

### Token Auto-Generation

The generator automatically creates token example files based on 50+ built-in templates:

```python
# In generator.py
TOKEN_TEMPLATES = {
    'EMAIL': ['john@example.com', 'alice@test.org'],
    'PHONE_NUMBER': ['555-123-4567', '800-555-0199'],
    'CURRENCY': ['$99.99', '$25.00'],
    'PATIENT_ID': ['MR234567', 'AB123456'],
    # ... 50+ more templates
}
```

When you build an analyzer, token files are automatically created in the same directory as the `.def` file.

### Custom Token Templates

To add custom token patterns, edit `TOKEN_TEMPLATES` in `generator.py`:

```python
TOKEN_TEMPLATES['MY_CUSTOM_TOKEN'] = ['example1', 'example2', 'example3']
```

### Grammar Tips

**1. Start with a root rule:**
```yacc
document -> content    # Root of your grammar
```

**2. Build recursively for lists:**
```yacc
list -> item
      | list item     # Left-recursive (efficient)
```

**3. Use meaningful node names:**
```yacc
# Good
email_record -> EMAIL_ADDR PHONE_NUMBER
    { $$ = create_node("email_record", 2, $1, $2); }

# Avoid
thing -> STUFF OTHER
    { $$ = create_node("thing", 2, $1, $2); }
```

**4. Handle whitespace in lexer:**
```yacc
%%LEX
WHITESPACE [ \t\r]+    # Define but don't use in grammar
```

### Debugging

**Check for shift/reduce conflicts:**
```bash
bison -d -v parser.y
cat y.output  # Contains conflict details
```

**Test lexer independently:**
```bash
flex lexer.l
gcc -o test_lex lex.yy.c -lfl
echo "test input" | ./test_lex
```

**Validate grammar structure:**
- Use `make run-stats` to see parse tree statistics.
- Check for unexpectedly deep trees (may indicate grammar issues).
- Verify terminal nodes contain expected values.

### Performance

For large inputs:
- Use left recursion (not right) for lists: `list -> list item` ✓.
- Avoid deeply nested rules when possible.
- Consider preprocessing input to remove unnecessary whitespace.

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Adding New Samples

1. Create a new directory: `samples/sampleN_your_name/`.
2. Add required files:
   - `SN_analyzer.def` - Grammar definition.
   - `SN_input.txt` - Test input.
   - `SN_rules.txt` - Documentation.
3. Test thoroughly: `make run DEF_FILE=samples/sampleN_your_name/SN_analyzer.def`.
4. Submit a pull request.

### Improving Documentation

- Fix typos or unclear explanations.
- Add more examples.
- Translate to other languages.
- Create video tutorials.

### Bug Reports

Open an issue with:
- Your `.def` file.
- Input that causes the issue.
- Expected vs actual behavior.
- System information (OS, Flex/Bison versions).

### Feature Requests

We're especially interested in:
- Additional token templates.
- New sample analyzers.
- Visualization improvements.
- Better error messages.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- **Flex** - Fast lexical analyzer generator
- **Bison** - GNU parser generator
- **Streamlit** - Web UI framework
- **Graphviz** - Graph visualization library

---

## 📧 Contact

- **Author**: Niranjan
- **GitHub**: [@Niranjan7771](https://github.com/Niranjan7771)
- **Repository**: [cfg2yacc](https://github.com/Niranjan7771/cfg2yacc)

---

## 🎯 Use Cases

cfg2yacc is perfect for:

- 📚 **Compiler Education** - Learn lexical analysis, parsing, and AST construction.
- 🔍 **Pattern Recognition** - Extract structured data from unstructured text.
- 📊 **Log Analysis** - Parse and analyze application/server logs.
- 💬 **Domain-Specific Languages** - Rapidly prototype custom languages.
- 🔧 **Data Extraction** - Pull specific information from documents.
- 🧪 **Prototyping** - Test grammar ideas quickly.
- 🎓 **Teaching** - Demonstrate compiler concepts with visual feedback.

---

## 📚 Learn More

- [PRECEDENCE_GUIDE.md](PRECEDENCE_GUIDE.md) - Complete precedence documentation.
- [samples/PRECEDENCE_HOWTO.md](samples/PRECEDENCE_HOWTO.md) - Practical precedence guide.
- [Flex Manual](https://westes.github.io/flex/manual/)
- [Bison Manual](https://www.gnu.org/software/bison/manual/)

---

**Happy Parsing! 🎉**

If you find this project useful, please ⭐ star the repository!
