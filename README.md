# cfg2yacc# cfg2yacc - Compiler Generator with Interactive Visualization# cfg2yacc - Compiler Generator with Parse Tree Visualization



**A Modern Compiler Generator with Interactive Parse Tree Visualization**



[![Language](https://img.shields.io/badge/language-Python%203-blue.svg)](https://www.python.org/)A Python-based compiler generator that creates custom text analyzers from simple `.def` files. Generate complete Flex/Bison-based compilers with **beautiful web UI** and **colorful terminal visualization**.A Python-based compiler generator that creates custom text analyzers from simple `.def` files. Write tokenization rules and grammar once, then generate a complete Flex/Bison-based compiler with **beautiful, colorful parse tree visualization**.

[![Tools](https://img.shields.io/badge/tools-Flex%20%7C%20Bison-green.svg)](https://www.gnu.org/software/bison/)

[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)



Transform simple grammar definitions into fully-functional compilers with beautiful, colorful parse tree visualization. Perfect for compiler education, pattern recognition, log analysis, and rapid prototyping of domain-specific languages.Perfect for: pattern detection, log analysis, data extraction, language processing, compiler learning, and rapid prototyping.Perfect for: pattern detection, log analysis, data extraction, language processing, compiler learning, and rapid prototyping.



---



## 🌟 Features------



- **🎯 Simple Syntax** - Define tokens and grammar rules in an intuitive `.def` format

- **🎨 Dual Visualization** - Interactive web UI and colorful terminal output

- **⚡ Auto-Generation** - Automatic token example file generation## 🚀 Quick Start## 🚀 Quick Start (3 Steps)

- **📊 10 Ready Samples** - Production-ready analyzers for common use cases

- **🔧 Precedence Support** - Built-in operator precedence enforcement

- **🚀 Zero Config** - Works out of the box with minimal setup

- **📚 Educational** - Excellent for learning compiler construction### 1. Install Prerequisites### Step 1: Install Prerequisites



---



## 📋 Table of Contents```bash```bash



- [Quick Start](#-quick-start)# Ubuntu/Debian# Ubuntu/Debian

- [Installation](#-installation)

- [Usage](#-usage)sudo apt-get updatesudo apt-get update

- [Sample Analyzers](#-sample-analyzers)

- [Creating Custom Analyzers](#-creating-custom-analyzers)sudo apt-get install build-essential flex bison python3sudo apt-get install build-essential flex bison python3

- [Operator Precedence](#-operator-precedence)

- [Visualization](#-visualization)

- [Project Structure](#-project-structure)

- [Advanced Topics](#-advanced-topics)# For web UI (optional but recommended)# Verify installation

- [Contributing](#-contributing)

./setup_ui.shgcc --version

---

```flex --version

## 🚀 Quick Start

bison --version

```bash

# 1. Clone the repository### 2. Run Your First Analyzerpython3 --version

git clone https://github.com/Niranjan7771/cfg2yacc.git

cd cfg2yacc```



# 2. Install dependencies```bash

sudo apt-get install build-essential flex bison python3

pip3 install -r requirements.txt# Terminal visualization (colorful text)### Step 2: Build Your First Analyzer



# 3. Run a sample analyzermake run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

```bash

# 4. Or launch the web UI

./run_ui.sh# OR Web UI (interactive graphs)# Clone or navigate to the project

```

./run_ui.shcd /path/to/cfg2yacc

**That's it!** You'll see a colorful parse tree visualization of the analyzed text.

```

---

# Build the default analyzer (analyzes structured text)

## 💾 Installation

**That's it!** Choose from 7 ready-to-use analyzers or create your own.make DEF_FILE=analyzer.def

### Prerequisites



- **Operating System**: Linux (Ubuntu/Debian recommended)

- **Build Tools**: GCC, Make---# This creates the 'custom_compiler' executable

- **Parser Generators**: Flex (≥ 2.6), Bison (≥ 3.0)

- **Python**: 3.6 or higher```

- **Optional**: Streamlit (for web UI)

## 📋 Available Analyzers

### System Dependencies

### Step 3: Run with Colorful Visualization

```bash

# Ubuntu/Debian| Sample | Purpose | Key Tokens |

sudo apt-get update

sudo apt-get install -y build-essential flex bison python3 python3-pip|--------|---------|------------|```bash



# Fedora/RHEL| **Sample 1** | Email Analysis | EMAIL_ADDR, PHONE_NUMBER, URGENT, CURRENCY |# See the parse tree in beautiful colors!

sudo dnf install -y gcc make flex bison python3 python3-pip

| **Sample 2** | Code Analysis | FUNCTION_DEF, VARIABLE_ASSIGN, STRING_LITERAL |make run DEF_FILE=analyzer.def

# Arch Linux

sudo pacman -S base-devel flex bison python python-pip| **Sample 3** | Log Analysis | TIMESTAMP, IP_ADDRESS, HTTP_METHOD, STATUS_CODE |```

```

| **Sample 4** | Spam Detection | SPAM_WORDS, MONEY_TERMS, SUSPICIOUS_LINK |

### Python Dependencies

| **Sample 5** | Social Media | HASHTAG, USER_MENTION, EMOJI_FACE, LIKE_COUNT |**That's it!** You now have a working text analyzer with colorful output.

```bash

pip3 install -r requirements.txt| **Sample 6** | Financial Data | CURRENCY_USD, ACCOUNT_NUMBER, STOCK_SYMBOL |

```

| **Sample 7** | Basic Example | EMAIL, PHONE, FLOAT_NUMBER (best for learning) |---

**Contents of requirements.txt:**

```

streamlit>=1.51.0

graphviz>=0.21---## 📖 Complete Step-by-Step Guide

```



### Verification

## 🎨 Visualization Options### Starting from Scratch

```bash

flex --version    # Should show 2.6.x or higher

bison --version   # Should show 3.x or higher

python3 --version # Should show 3.6 or higher### Option 1: Web UI (Recommended)#### 1. **Understand What You Have**

```



---

```bashYour project contains:

## 🎯 Usage

./run_ui.sh- **Python Generator** (`generator.py`) - Converts `.def` files to Flex/Bison code

### Basic Workflow

```- **Parse Tree Library** (`ast.c`, `ast.h`) - Builds and displays parse trees

1. **Create a `.def` file** with your grammar definition

2. **Build the analyzer** using the Makefile- **Visualizer** (`visualize_tree.py`) - Makes output colorful and beautiful

3. **Run with input** and visualize the parse tree

**Features:**- **Sample Analyzers** (in `samples/`) - 7 ready-to-use examples

### Command Line

- 🌐 Interactive web interface at http://localhost:8501- **Build System** (`Makefile`) - Automates everything

```bash

# Build and run with terminal visualization- 📊 Beautiful graph visualization with color-coded nodes

make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

- 📈 Real-time statistics and pattern counts#### 2. **Choose an Analyzer to Run**

# Use custom input file

make run DEF_FILE=path/to/analyzer.def INPUT_FILE=path/to/input.txt- ✏️ Test with custom input instantly



# Different visualization styles- 💾 Export graphs as PNG/SVGPick one of these ready-made analyzers:

make run-simple DEF_FILE=...   # Simple indented tree

make run-compact DEF_FILE=...  # Compact (terminals only)- 🔄 Switch between analyzers with one click

make run-stats DEF_FILE=...    # Statistics summary

| Analyzer | Purpose | Location |

# Clean build artifacts

make clean**Perfect for:** Interactive exploration, presentations, debugging|----------|---------|----------|

```

| Basic Example | Emails, phones, numbers | `samples/sample7_basic_example/S7_analyzer.def` |

### Web Interface

### Option 2: Terminal Visualization| Email Analysis | Email patterns, urgency detection | `samples/sample1_email_analysis/S1_analyzer.def` |

```bash

# Launch interactive web UI| Code Analysis | Functions, variables, comments | `samples/sample2_code_analysis/S2_analyzer.def` |

./run_ui.sh

```bash| Log Analysis | Timestamps, IPs, HTTP codes | `samples/sample3_log_analysis/S3_analyzer.def` |

# Access at http://localhost:8501

# Features:# Fancy style (box drawing)| Spam Detection | Spam words, suspicious links | `samples/sample4_spam_detection/S4_analyzer.def` |

# - Dropdown to select any sample analyzer

# - Live build and runmake run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def| Social Media | Hashtags, mentions, emojis | `samples/sample5_social_media/S5_analyzer.def` |

# - Interactive graph visualization

# - Custom input editor| Financial Data | Currency, account numbers | `samples/sample6_financial_data/S6_analyzer.def` |

# - Parse tree statistics

# - Export functionality# Compact style

```

make run-compact DEF_FILE=samples/sample7_basic_example/S7_analyzer.def#### 3. **Build the Analyzer**

---



## 📦 Sample Analyzers

# With statistics```bash

cfg2yacc includes 10 production-ready sample analyzers demonstrating various use cases:

make run-stats DEF_FILE=samples/sample7_basic_example/S7_analyzer.def# Clean any previous builds

| Sample | Description | Use Case | Tokens |

|--------|-------------|----------|--------|```make clean

| **[Sample 1](samples/sample1_email_analysis/)** | Email Analysis | Extract structured data from emails | EMAIL, PHONE, CURRENCY, DATE, URL, URGENT |

| **[Sample 2](samples/sample2_code_analysis/)** | Code Analysis | Parse source code elements | FUNCTION_DEF, VARIABLE_ASSIGN, STRING_LITERAL |

| **[Sample 3](samples/sample3_log_analysis/)** | Log Analysis | Process server/application logs | TIMESTAMP, IP_ADDRESS, HTTP_METHOD, STATUS_CODE |

| **[Sample 4](samples/sample4_spam_detection/)** | Spam Detection | Identify spam patterns | SPAM_WORDS, MONEY_TERMS, SUSPICIOUS_LINK, CAPS_WORD |**Features:**# Build a specific analyzer (example: email analysis)

| **[Sample 5](samples/sample5_social_media/)** | Social Media | Parse social media content | HASHTAG, USER_MENTION, EMOJI_FACE, LIKE_COUNT |

| **[Sample 6](samples/sample6_financial_data/)** | Financial Data | Extract financial information | CURRENCY_USD, ACCOUNT_NUMBER, STOCK_SYMBOL, TRANSACTION_ID |- 🎨 ANSI color-coded outputmake DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

| **[Sample 7](samples/sample7_basic_example/)** | Basic Example | Simple pattern matching (best for learning) | EMAIL, PHONE, FLOAT_NUMBER, INTEGER |

| **[Sample 8](samples/sample8_medical_analysis/)** | Medical Records | Parse medical data | PATIENT_ID, BLOOD_TYPE, BLOOD_PRESSURE, DIAGNOSIS_CODE |- 🌲 Tree structure visualization```

| **[Sample 9](samples/sample9_calculator/)** | Simple Calculator | Basic arithmetic expressions (educational) | NUMBER, PLUS, MINUS, TIMES, DIVIDE |

| **[Sample 10](samples/sample10_id_arithmetic/)** | ID Arithmetic | Custom grammar demonstration | ID, NUM, operators |- 📊 Optional statistics



### Running Samples- ⚡ Fast, lightweight**What happens:**



```bash1. Python generator reads the `.def` file

# Email analysis

make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def**Perfect for:** Command-line workflows, automation, scripting2. Generates `lexer.l` (tokenization rules)



# Simple calculator (best for beginners)3. Generates `parser.y` (grammar rules)

make run DEF_FILE=samples/sample9_calculator/S9_analyzer.def

---4. Flex creates `lex.yy.c` from `lexer.l`

# Medical records

make run DEF_FILE=samples/sample8_medical_analysis/S8_analyzer.def5. Bison creates `y.tab.c` from `parser.y`

```

## 📖 How It Works6. GCC compiles everything into `custom_compiler`

Each sample includes:

- `SX_analyzer.def` - Grammar definition

- `SX_input.txt` - Test input data

- `SX_rules.txt` - Documentation### The .def File Format#### 4. **Run the Analyzer**

- `*_tokens.txt` - Auto-generated token examples



---

Create a `.def` file with two sections:**Option A: With Colorful Visualization (Recommended)**

## 🔨 Creating Custom Analyzers



### Step 1: Create a `.def` File

#### 1. %%LEX Section - Define Tokens```bash

Create a file with `%%LEX` and `%%YACC` sections:

# Use the built-in visualization

```python

# my_analyzer.def```./custom_compiler < samples/sample1_email_analysis/S1_input.txt | python3 visualize_tree.py



%%LEX%%LEX

# Define tokens with regex patterns

NUMBER [0-9]+EMAIL           [a-zA-Z0-9._]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}# OR use the convenient Makefile target

WORD [a-zA-Z]+

WHITESPACE [ \t\r]+PHONE_NUMBER    [0-9]{3}-[0-9]{3}-[0-9]{4}make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

NEWLINE \n

WHITESPACE      [ \t\n]+```

%%YACC

# Define grammar rules```

document -> item_list

    { $$ = create_node("document", 1, $1); }**Option B: Plain Text Output**



item_list -> item#### 2. %%YACC Section - Define Grammar

    { $$ = create_node("item_list", 1, $1); }

    | item_list item```bash

    { $$ = create_node("item_list", 2, $1, $2); }

```./custom_compiler < samples/sample1_email_analysis/S1_input.txt

item -> WORD NEWLINE

    { $$ = create_node("item", 2, $1, $2); }%%YACC```

    | NUMBER NEWLINE

    { $$ = create_node("item", 2, $1, $2); }document -> element_list

```

#### 5. **Explore Different Visualization Styles**

### Step 2: Create Input File

element_list -> element_list element

```bash

# my_input.txt              | element```bash

hello

42# Fancy style (box drawing, tree structure) - DEFAULT

world

123element -> email_foundmake run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

```

         | phone_found

### Step 3: Build and Run

# Simple style (clean indented view)

```bash

make run DEF_FILE=my_analyzer.def INPUT_FILE=my_input.txtemail_found -> EMAILmake run-simple DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

```

    { $$ = create_leaf_node("email_found", yytext); }

### Grammar Rule Syntax

# Compact style (shows only important nodes)

Each grammar rule follows this format:

phone_found -> PHONE_NUMBERmake run-compact DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

```yacc

non_terminal -> production    { $$ = create_leaf_node("phone_found", yytext); }

    { $$ = create_node("node_name", count, child1, child2, ...); }

``````# With statistics (pattern counts)



**Components:**make run-stats DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

- `non_terminal` - Left-hand side (what you're defining)

- `production` - Right-hand side (what it consists of)### Build Process```

- `create_node()` - Creates a parse tree node

  - First arg: Node label (string)

  - Second arg: Number of children

  - Remaining args: Child nodes ($1, $2, $3, ...)```#### 6. **Try Different Samples**



**Special variables:**your_analyzer.def

- `$$` - Current node being created

- `$1, $2, $3...` - Children from the production (left to right)    ↓ (generator.py)```bash



### Alternative Productionslexer.l + parser.y# Test basic example (emails, phones, floats)



Use `|` for multiple productions:    ↓ (flex + bison)make clean && make DEF_FILE=samples/sample7_basic_example/S7_analyzer.def



```yacclex.yy.c + y.tab.c./custom_compiler < samples/sample7_basic_example/S7_input.txt | python3 visualize_tree.py --stats

expression -> expression PLUS term

    { $$ = create_node("add", 3, $1, $2, $3); }    ↓ (gcc)

    | expression MINUS term

    { $$ = create_node("subtract", 3, $1, $2, $3); }custom_compiler# Test log analysis (timestamps, IPs, status codes)

    | term

    { $$ = $1; }    ↓make clean && make DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def

```

Parse Tree Output./custom_compiler < samples/sample3_log_analysis/S3_input.txt | python3 visualize_tree.py --compact

---

```

## ⚖️ Operator Precedence

# Test spam detection

cfg2yacc supports operator precedence through **grammar stratification** - structuring your grammar in layers.

---make clean && make DEF_FILE=samples/sample4_spam_detection/S4_analyzer.def

### Example: Calculator with Precedence

./custom_compiler < samples/sample4_spam_detection/S4_input.txt | python3 visualize_tree.py

```yacc

%%YACC## 🔧 Creating Your Own Analyzer```

# Layer 1: Addition/Subtraction (lowest precedence - evaluated last)

expr -> expr PLUS term

      | expr MINUS term

      | term### Step 1: Create a .def File---



# Layer 2: Multiplication/Division (higher precedence - evaluated first)

term -> term TIMES factor

      | term DIVIDE factor```bash## 🎨 Visualization Features

      | factor

nano my_analyzer.def

# Layer 3: Atoms (highest precedence)

factor -> NUMBER```### Three Display Styles

        | LPAREN expr RPAREN

```



This ensures that `3 + 5 * 2` correctly parses as `3 + (5 * 2) = 13`, not `(3 + 5) * 2 = 16`.### Step 2: Define Your Tokens and Grammar**1. Fancy (Default)** - Box drawing with tree structure



### How It Works```



**Lower in the grammar = Higher precedence**See `samples/sample7_basic_example/S7_analyzer.def` for a simple example.╔════════════════════════════════════════════════════════════════════╗

- Operations at the `factor` level bind tightest

- Operations at the `term` level bind tighter than `expr`║  Parse Tree Visualization                                           ║

- Operations at the `expr` level bind loosest

### Step 3: Build and Test╚════════════════════════════════════════════════════════════════════╝

### Precedence Examples



See detailed examples with parse trees:

- **[samples/sample9_calculator/S9_precedence.def](samples/sample9_calculator/S9_precedence.def)** - Calculator with `*`, `/` > `+`, `-````bash│email_document

- **[samples/sample10_id_arithmetic/S10_precedence.def](samples/sample10_id_arithmetic/S10_precedence.def)** - ID arithmetic with precedence

- **[PRECEDENCE_GUIDE.md](PRECEDENCE_GUIDE.md)** - Complete precedence documentation# Build│  ├─ │email



---make DEF_FILE=my_analyzer.def│      ├─ │email_address



## 🎨 Visualization│          └─ EMAIL_ADDR ➜ john@example.com



cfg2yacc offers three visualization methods:# Create test input```



### 1. Terminal Visualization (Default)echo "Test data here" > my_input.txt



**Fancy Style** (with box drawing):**2. Simple** - Clean indented view

```

╔════════════════════════════════════╗# Run```

║  Parse Tree Visualization          ║

╚════════════════════════════════════╝./custom_compiler < my_input.txt | python3 visualize_tree.py======================================================================



calculator```  Parse Tree Visualization

  ├─ expr_list

    ├─ expression======================================================================

      ├─ NUMBER ➜ 5

      ├─ add---

        └─ PLUS ➜ +

      └─ NUMBER ➜ 3email_document

```

## 📁 Project Structure  email

**Simple Style** (clean indents):

```bash    email_address

make run-simple DEF_FILE=...

``````      EMAIL_ADDR → john@example.com



**Compact Style** (terminals only):cfg2yacc/```

```bash

make run-compact DEF_FILE=...├── generator.py              # Converts .def → lexer.l + parser.y

```

├── ast.c, ast.h             # Parse tree library**3. Compact** - Shows only terminals (detected patterns)

**Statistics Mode**:

```bash├── Makefile                 # Build automation```

make run-stats DEF_FILE=...

# Shows: node count, depth, terminal count, etc.│┌────────────────────────────────────────────────────────────────────┐

```

├── visualize_tree.py        # Terminal visualization│  Parse Tree (Compact View)                                          │

### 2. Web UI (Interactive)

├── streamlit_visualizer.py  # Web UI└────────────────────────────────────────────────────────────────────┘

```bash

./run_ui.sh├── generate_token_data.py   # Token example generator

```

│▪ email_document

**Features:**

- 📂 Dropdown menu to select any sample├── run_ui.sh               # Launch web UI  ▸ EMAIL_ADDR: john@example.com

- 🔨 One-click build and run

- 📊 Interactive graph visualization├── setup_ui.sh             # Install web UI dependencies  ▸ PHONE_NUMBER: 555-123-4567

- ✏️ Custom input editor

- 📈 Parse tree statistics├── view_tokens.sh          # View token examples  ▸ URGENT: URGENT

- 💾 Export functionality

- 🎨 Responsive design│```



### 3. Programmatic Access├── samples/                # 7 ready-to-use analyzers



```python│   ├── sample1_email_analysis/### Color Coding

import visualize_tree

│   ├── sample2_code_analysis/

# Parse tree from stdin

tree = visualize_tree.parse_tree_from_input()│   ├── sample3_log_analysis/- 🔴 **Red** - URGENT/IMMEDIATE keywords



# Render with custom style│   ├── sample4_spam_detection/- 🟣 **Magenta** - Email addresses

visualize_tree.print_tree(tree, style='fancy')

visualize_tree.print_tree(tree, style='simple')│   ├── sample5_social_media/- 🔵 **Blue** - Phone numbers

visualize_tree.print_tree(tree, style='compact')

│   ├── sample6_financial_data/- 💜 **Bright Magenta** - Website URLs

# Get statistics

stats = visualize_tree.get_statistics(tree)│   └── sample7_basic_example/- 💚 **Bright Green** - Currency amounts

print(f"Nodes: {stats['total_nodes']}")

print(f"Depth: {stats['max_depth']}")│- 🟡 **Yellow** - Terminal tokens

```

└── README.md               # This file- 🔵 **Cyan** - Grammar rules

---

```- 🟢 **Green** - Token values

## 📁 Project Structure



```

cfg2yacc/---### Statistics Mode

├── README.md                      # This file

├── Makefile                       # Build automation

├── generator.py                   # Main generator (def → lex/yacc)

├── ast.c / ast.h                  # Parse tree data structures## 🎯 Common Use Cases```

├── visualize_tree.py              # Terminal visualization

├── streamlit_visualizer.py        # Web UI╔════════════════════════════════════════════════════════════════════╗

├── run_ui.sh                      # Web UI launcher

├── requirements.txt               # Python dependencies### 1. Email Analysis║  Statistics                                                         ║

├── PRECEDENCE_GUIDE.md           # Precedence documentation

│```bash╠════════════════════════════════════════════════════════════════════╣

├── samples/                       # Sample analyzers

│   ├── sample1_email_analysis/make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def║  Total Nodes: 646                                                     ║

│   │   ├── S1_analyzer.def        # Grammar definition

│   │   ├── S1_input.txt           # Test input# Detects: emails, phones, URLs, currency, dates, urgent keywords║  Terminals: 247                                                       ║

│   │   ├── S1_rules.txt           # Documentation

│   │   └── *_tokens.txt           # Auto-generated examples```║  Non-terminals: 399                                                   ║

│   ├── sample2_code_analysis/

│   ├── sample3_log_analysis/║  Detected Patterns:                                                 ║

│   ├── sample4_spam_detection/

│   ├── sample5_social_media/### 2. Log File Analysis║    • email: 6                                                          ║

│   ├── sample6_financial_data/

│   ├── sample7_basic_example/```bash║    • phone_formatted: 4                                                ║

│   ├── sample8_medical_analysis/

│   ├── sample9_calculator/        # Simple calculator (educational)make run DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def║    • float_number: 18                                                  ║

│   ├── sample10_id_arithmetic/    # Custom grammar demo

│   └── PRECEDENCE_HOWTO.md        # Precedence how-to# Detects: timestamps, IPs, HTTP methods, status codes, error levels╚════════════════════════════════════════════════════════════════════╝

│

└── (generated at build time)``````

    ├── lexer.l                    # Generated Flex lexer

    ├── parser.y                   # Generated Bison parser

    ├── lex.yy.c                   # Compiled lexer

    ├── y.tab.c / y.tab.h          # Compiled parser### 3. Spam Detection---

    └── custom_compiler            # Final executable

``````bash



---make run DEF_FILE=samples/sample4_spam_detection/S4_analyzer.def## 📋 Command Reference



## 🎓 Advanced Topics# Detects: spam words, money terms, suspicious links, urgency indicators



### Token Auto-Generation```### Makefile Commands



The generator automatically creates token example files based on 50+ built-in templates:



```python### 4. Code Pattern Analysis```bash

# In generator.py

TOKEN_TEMPLATES = {```bash# Build and run with fancy visualization

    'EMAIL': ['john@example.com', 'alice@test.org'],

    'PHONE_NUMBER': ['555-123-4567', '800-555-0199'],make run DEF_FILE=samples/sample2_code_analysis/S2_analyzer.defmake run DEF_FILE=path/to/analyzer.def

    'CURRENCY': ['$99.99', '$25.00'],

    'PATIENT_ID': ['MR234567', 'AB123456'],# Detects: function definitions, variable assignments, string literals

    # ... 50+ more templates

}```# Different visualization styles

```

make run-simple DEF_FILE=path/to/analyzer.def

When you build an analyzer, token files are automatically created in the same directory as the `.def` file.

### 5. Social Media Contentmake run-compact DEF_FILE=path/to/analyzer.def

### Custom Token Templates

```bashmake run-stats DEF_FILE=path/to/analyzer.def

To add custom token patterns, edit `TOKEN_TEMPLATES` in `generator.py`:

make run DEF_FILE=samples/sample5_social_media/S5_analyzer.def

```python

TOKEN_TEMPLATES['MY_CUSTOM_TOKEN'] = ['example1', 'example2', 'example3']# Detects: hashtags, mentions, emojis, like/share counts# Build only (no visualization)

```

```make DEF_FILE=path/to/analyzer.def

### Grammar Tips



**1. Start with a root rule:**

```yacc---# Clean generated files

document -> content    # Root of your grammar

```make clean



**2. Build recursively for lists:**## 🛠️ Command Reference

```yacc

list -> item# Complete rebuild

      | list item     # Left-recursive (efficient)

```### Buildingmake rebuild DEF_FILE=path/to/analyzer.def



**3. Use meaningful node names:**```

```yacc

# Good```bash

email_record -> EMAIL_ADDR PHONE_NUMBER

    { $$ = create_node("email_record", 2, $1, $2); }# Build with specific analyzer### Manual Commands



# Avoidmake DEF_FILE=path/to/analyzer.def

thing -> STUFF OTHER

    { $$ = create_node("thing", 2, $1, $2); }```bash

```

# Clean generated files# Step-by-step build process

**4. Handle whitespace in lexer:**

```yaccmake cleanpython3 generator.py analyzer.def    # Generate lexer.l and parser.y

%%LEX

WHITESPACE [ \t\r]+    # Define but don't use in grammarflex lexer.l                         # Create lex.yy.c

```

# Rebuild from scratchbison -d -o y.tab.c parser.y         # Create y.tab.c and y.tab.h

### Debugging

make clean allgcc -o custom_compiler y.tab.c lex.yy.c ast.o -lfl  # Compile executable

**Check for shift/reduce conflicts:**

```bash```

bison -d -v parser.y

cat y.output  # Contains conflict details# Run with different options

```

### Running./custom_compiler < input.txt                                    # Plain output

**Test lexer independently:**

```bash./custom_compiler < input.txt | python3 visualize_tree.py        # Fancy visualization

flex lexer.l

gcc -o test_lex lex.yy.c -lfl```bash./custom_compiler < input.txt | python3 visualize_tree.py --style simple

echo "test input" | ./test_lex

```# Terminal visualization (fancy)./custom_compiler < input.txt | python3 visualize_tree.py --style compact --stats



**Validate grammar structure:**make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def./custom_compiler < input.txt | python3 visualize_tree.py --no-color > output.txt

- Use `make run-stats` to see parse tree statistics

- Check for unexpectedly deep trees (may indicate grammar issues)```

- Verify terminal nodes contain expected values

# Different styles

### Performance

make run-simple DEF_FILE=...### Wrapper Script

For large inputs:

- Use left recursion (not right) for lists: `list -> list item` ✓make run-compact DEF_FILE=...

- Avoid deeply nested rules when possible

- Consider preprocessing input to remove unnecessary whitespacemake run-stats DEF_FILE=...```bash



---# Make it executable (already done)



## 🤝 Contributing# Web UIchmod +x run_visual.sh



Contributions are welcome! Here's how you can help:./run_ui.sh



### Adding New Samples# Use it



1. Create a new directory: `samples/sampleN_your_name/`# Manual pipeline./run_visual.sh < input.txt                 # Fancy style

2. Add required files:

   - `SN_analyzer.def` - Grammar definition./custom_compiler < input.txt | python3 visualize_tree.py./run_visual.sh --simple < input.txt        # Simple style

   - `SN_input.txt` - Test input

   - `SN_rules.txt` - Documentation```./run_visual.sh --compact < input.txt       # Compact style

3. Test thoroughly: `make run DEF_FILE=samples/sampleN_your_name/SN_analyzer.def`

4. Submit a pull request./run_visual.sh --stats < input.txt         # Show statistics



### Improving Documentation### Tokens./run_visual.sh --compact --stats < input.txt  # Combine options



- Fix typos or unclear explanations./run_visual.sh --help                      # Show help

- Add more examples

- Translate to other languages```bash```

- Create video tutorials

# Generate token example files for all samples

### Bug Reports

python3 generate_token_data.py---

Open an issue with:

- Your `.def` file

- Input that causes the issue

- Expected vs actual behavior# View token examples## 📁 Project Structure

- System information (OS, Flex/Bison versions)

./view_tokens.sh samples/sample7_basic_example

### Feature Requests

```

We're especially interested in:

- Additional token templates# View specific token filecfg2yacc/

- New sample analyzers

- Visualization improvementscat samples/sample1_email_analysis/email_addr_tokens.txt├── generator.py              # Python generator (converts .def → lexer.l + parser.y)

- Better error messages

```├── ast.c, ast.h             # Parse tree library

---

├── visualize_tree.py        # Colorful visualization tool

## 📄 License

---├── run_visual.sh            # Convenience wrapper script

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

├── Makefile                 # Build automation

---

## 📊 Token Example Files├── analyzer.def             # Example analyzer definition

## 🙏 Acknowledgments

├── test_input.txt           # Example input

- **Flex** - Fast lexical analyzer generator

- **Bison** - GNU parser generatorEach sample includes token files with example data:├── samples/                 # 7 ready-to-use analyzers

- **Streamlit** - Web UI framework

- **Graphviz** - Graph visualization library│   ├── sample1_email_analysis/



---```bash│   │   ├── S1_analyzer.def



## 📧 Contactsamples/sample7_basic_example/│   │   ├── S1_input.txt



- **Author**: Niranjan├── S7_analyzer.def│   │   └── S1_rules.txt

- **GitHub**: [@Niranjan7771](https://github.com/Niranjan7771)

- **Repository**: [cfg2yacc](https://github.com/Niranjan7771/cfg2yacc)├── S7_input.txt│   ├── sample2_code_analysis/



---├── email_tokens.txt          # Example emails│   ├── sample3_log_analysis/



## 🎯 Use Cases├── phone_formatted_tokens.txt # Example phone numbers│   ├── sample4_spam_detection/



cfg2yacc is perfect for:├── float_number_tokens.txt   # Example floating point numbers│   ├── sample5_social_media/



- 📚 **Compiler Education** - Learn lexical analysis, parsing, and AST construction└── ...│   ├── sample6_financial_data/

- 🔍 **Pattern Recognition** - Extract structured data from unstructured text

- 📊 **Log Analysis** - Parse and analyze application/server logs```│   └── sample7_basic_example/

- 💬 **Domain-Specific Languages** - Rapidly prototype custom languages

- 🔧 **Data Extraction** - Pull specific information from documents└── docs/

- 🧪 **Prototyping** - Test grammar ideas quickly

- 🎓 **Teaching** - Demonstrate compiler concepts with visual feedback**Use token files to:**    ├── GENERATOR_README.md        # Complete generator documentation



---- Test individual token patterns    ├── QUICKSTART.md              # Quick start guide



## 📚 Learn More- Create comprehensive test inputs    ├── VISUALIZATION.md           # Visualization guide



- [PRECEDENCE_GUIDE.md](PRECEDENCE_GUIDE.md) - Complete precedence documentation- Validate lexer rules    ├── VISUALIZATION_QUICKREF.md  # Quick reference

- [samples/PRECEDENCE_HOWTO.md](samples/PRECEDENCE_HOWTO.md) - Practical precedence guide

- [Flex Manual](https://westes.github.io/flex/manual/)- Document expected matches    └── SAMPLES_README.md          # Sample analyzers guide

- [Bison Manual](https://www.gnu.org/software/bison/manual/)

```

---

```bash

**Happy Parsing! 🎉**

# View all token files for a sample---

If you find this project useful, please ⭐ star the repository!

./view_tokens.sh samples/sample1_email_analysis

## 🔧 Creating Your Own Analyzer

# Test with token data

cat samples/sample7_basic_example/email_tokens.txt | ./custom_compiler### The `.def` File Format

```

A `.def` file has two sections:

---

#### **%%LEX Section** - Define Tokens

## 🎨 Web UI Guide

```

### Launch%%LEX

EMAIL           [a-zA-Z0-9._]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}

```bashPHONE_NUMBER    [0-9]{3}-[0-9]{3}-[0-9]{4}

./run_ui.shWORD            [a-zA-Z]+

# Opens browser at http://localhost:8501WHITESPACE      [ \t\n]+

``````



### Using the UI#### **%%YACC Section** - Define Grammar



1. **Select Analyzer** - Click any analyzer in sidebar```

2. **Build** - Click "🔨 Build Compiler"%%YACC

3. **Run** - Click "▶️ Run & Visualize"document -> element_list

4. **Explore**:

   - 🔍 Graph View - Interactive tree diagramelement_list -> element_list element

   - 📝 Text View - Indented text format              | element

   - 📊 Statistics - Pattern counts

   - ✏️ Custom Input - Test your own textelement -> email_found

         | phone_found

### Features         | word



- **Color-Coded Nodes**: Red (urgent), Pink (emails), Cyan (phones), Yellow (URLs), Green (currency)email_found -> EMAIL

- **Real-Time**: Build and visualize in seconds    { $$ = create_leaf_node("email_found", yytext); }

- **Export**: Download graphs as images

- **Mobile Friendly**: Works on tablets and phonesphone_found -> PHONE_NUMBER

    { $$ = create_leaf_node("phone_found", yytext); }

---```



## 🐛 Troubleshooting### Creating a New Analyzer



### Build Errors```bash

# 1. Create your .def file

**Problem:** `flex: command not found`nano my_analyzer.def

```bash

sudo apt-get install flex# 2. Create test input

```nano my_input.txt



**Problem:** `bison: command not found`# 3. Build

```bashmake DEF_FILE=my_analyzer.def

sudo apt-get install bison

```# 4. Test

./custom_compiler < my_input.txt | python3 visualize_tree.py

**Problem:** `generator.py not found````

```bash

# Make sure you're in the project directory**See examples in `samples/` for complete templates!**

cd /path/to/cfg2yacc

```---



### Runtime Errors## 🎯 Common Use Cases



**Problem:** `custom_compiler not found`### 1. Extract Email Addresses from Text

```bash

# Build first```bash

make DEF_FILE=analyzer.defmake DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

```./custom_compiler < my_emails.txt | python3 visualize_tree.py --compact

```

**Problem:** Wrong input file used

```bash### 2. Analyze Log Files

# The Makefile auto-detects input files for samples

# For custom analyzers, use test_input.txt or specify manually```bash

```make DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def

./custom_compiler < server.log | python3 visualize_tree.py --stats

### Visualization Issues```



**Problem:** No colors in terminal### 3. Detect Spam Patterns

```bash

# Use --no-color flag```bash

./custom_compiler < input.txt | python3 visualize_tree.py --no-colormake DEF_FILE=samples/sample4_spam_detection/S4_analyzer.def

```./custom_compiler < suspicious_email.txt | python3 visualize_tree.py

```

**Problem:** Web UI won't start

```bash### 4. Parse Financial Data

# Make sure dependencies are installed

./setup_ui.sh```bash

make DEF_FILE=samples/sample6_financial_data/S6_analyzer.def

# Or install manually./custom_compiler < transactions.txt | python3 visualize_tree.py --compact --stats

pip3 install streamlit graphviz```



# Add to PATH### 5. Learn Compiler Construction

export PATH="$HOME/.local/bin:$PATH"

``````bash

# Build different analyzers and see how they work

---make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

make run DEF_FILE=samples/sample2_code_analysis/S2_analyzer.def

## 📚 How-To Examples```



### Example 1: Analyze Email Content---



```bash## 🐛 Troubleshooting

# Use email analyzer

make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def### Build Errors



# See detected: emails, phones, URLs, dates, currency, urgent keywords**Problem:** `generator.py not found`

``````bash

# Solution: Make sure you're in the project directory

### Example 2: Parse Log Filescd /path/to/cfg2yacc

ls generator.py  # Should exist

```bash```

# Build log analyzer

make clean && make DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def**Problem:** `flex: command not found`

```bash

# Run on log file# Solution: Install Flex

./custom_compiler < server.log | python3 visualize_tree.py --compact --statssudo apt-get install flex

``````



### Example 3: Create Custom Test Input**Problem:** `bison: command not found`

```bash

```bash# Solution: Install Bison

# Combine token examplessudo apt-get install bison

cat samples/sample7_basic_example/email_tokens.txt \```

    samples/sample7_basic_example/phone_formatted_tokens.txt > test.txt

**Problem:** `gcc: command not found`

# Test```bash

./custom_compiler < test.txt | python3 visualize_tree.py# Solution: Install GCC

```sudo apt-get install build-essential

```

### Example 4: Test Individual Token

### Runtime Errors

```bash

# Build analyzer**Problem:** `custom_compiler not found`

make DEF_FILE=samples/sample7_basic_example/S7_analyzer.def```bash

# Solution: Build first

# Test email tokenmake DEF_FILE=analyzer.def

cat samples/sample7_basic_example/email_tokens.txt | ./custom_compiler```

```

**Problem:** No colorful output

### Example 5: Interactive Development```bash

# Solution: Install Python 3

```bashsudo apt-get install python3

# Launch web UI

./run_ui.sh# Or use plain output

./custom_compiler < input.txt

# In the UI:```

# 1. Select sample7_basic_example

# 2. Click "Build Compiler"**Problem:** Colors look wrong

# 3. Click "Use Custom Input"```bash

# 4. Type: "Contact john@example.com or call (555) 123-4567"# Solution: Use --no-color option

# 5. Click "Run with Custom Input"./custom_compiler < input.txt | python3 visualize_tree.py --no-color

# 6. See instant visualization!```

```

### Visualization Issues

---

**Problem:** Box characters display as `?`

## ⚙️ Advanced Usage```bash

# Solution: Use simple style

### Custom Visualization Stylesmake run-simple DEF_FILE=analyzer.def



```bash# Or update terminal encoding to UTF-8

# Fancy (box drawing)```

python3 visualize_tree.py --style fancy

**Problem:** Output too wide

# Simple (indented)```bash

python3 visualize_tree.py --style simple# Solution: Use compact style

make run-compact DEF_FILE=analyzer.def

# Compact (terminals only)```

python3 visualize_tree.py --style compact

---

# With statistics

python3 visualize_tree.py --stats## 📚 Documentation



# No colors (for piping to file)- **[GENERATOR_README.md](GENERATOR_README.md)** - Complete generator system documentation

python3 visualize_tree.py --no-color > output.txt- **[QUICKSTART.md](QUICKSTART.md)** - Quick start guide with examples

```- **[VISUALIZATION.md](VISUALIZATION.md)** - Full visualization guide

- **[VISUALIZATION_QUICKREF.md](VISUALIZATION_QUICKREF.md)** - Quick visualization reference

### Batch Processing- **[SAMPLES_README.md](samples/SAMPLES_README.md)** - Sample analyzers guide

- **[PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)** - Project organization

```bash

# Test all samples---

for sample in samples/sample*/S*_analyzer.def; do

    echo "Testing: $sample"## 🎓 Learning Path

    make clean && make DEF_FILE="$sample"

    input="${sample/_analyzer.def/_input.txt}"### Beginner

    ./custom_compiler < "$input" | python3 visualize_tree.py --stats1. Run sample 7 (basic example)

done2. Try different visualization styles

```3. Modify `S7_input.txt` with your own data

4. See how patterns are detected

### Add New Token Data

### Intermediate

Edit `generate_token_data.py` and add to `TOKEN_DATA`:1. Study a `.def` file (start with `samples/sample7_basic_example/S7_analyzer.def`)

2. Add a new token pattern

```python3. Rebuild and test

TOKEN_DATA = {4. Check the generated `lexer.l` and `parser.y`

    'YOUR_TOKEN': [

        'example1',### Advanced

        'example2',1. Create your own `.def` file from scratch

        'example3'2. Design custom grammar rules

    ],3. Use semantic actions

    # ... existing tokens ...4. Build a domain-specific language

}

```---



Then regenerate:## ✨ Features

```bash

python3 generate_token_data.py- ✅ **Simple .def Format** - Define analyzers in minutes

```- ✅ **Automatic Generation** - Python creates Flex/Bison code for you

- ✅ **Colorful Visualization** - Beautiful parse tree display

---- ✅ **Multiple Styles** - Fancy, simple, or compact views

- ✅ **Statistics** - Pattern count summaries

## 🎓 Learning Path- ✅ **7 Sample Analyzers** - Ready-to-use examples

- ✅ **Comprehensive Docs** - 5 documentation files

### Beginner- ✅ **Automated Build** - One command to build and run

1. Run `./run_ui.sh` and explore sample7_basic_example- ✅ **Easy Customization** - Modify or create new analyzers

2. Try different visualization styles

3. Test with custom input in the UI---

4. View token files: `./view_tokens.sh samples/sample7_basic_example`

## 🤝 Contributing

### Intermediate

1. Study a `.def` file (start with `samples/sample7_basic_example/S7_analyzer.def`)This is an educational project. Feel free to:

2. Modify a token pattern- Create new sample analyzers

3. Rebuild and test- Improve visualization

4. Check generated `lexer.l` and `parser.y`- Add documentation

- Report issues

### Advanced

1. Create your own `.def` file---

2. Design custom grammar rules

3. Use semantic actions## 📄 License

4. Build a domain-specific language

See repository for license information.

---

---

## 🚦 Quick Commands Cheat Sheet

## 🚦 Quick Command Cheat Sheet

```bash

# Essential```bash

make clean                                    # Clean build# Essential commands

make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.defmake clean                           # Clean build

make DEF_FILE=analyzer.def          # Build analyzer

# Web UImake run DEF_FILE=analyzer.def      # Build and run with visualization

./run_ui.sh                                   # Launch web interface

# Visualization styles

# Terminal visualizationmake run-simple DEF_FILE=...        # Simple indented view

make run-simple DEF_FILE=...                  # Simple viewmake run-compact DEF_FILE=...       # Compact view (terminals only)

make run-compact DEF_FILE=...                 # Compact viewmake run-stats DEF_FILE=...         # Show statistics

make run-stats DEF_FILE=...                   # With statistics

# Manual pipeline

# Tokens./custom_compiler < input.txt                              # Plain output

python3 generate_token_data.py                # Generate token files./custom_compiler < input.txt | python3 visualize_tree.py  # Fancy visualization

./view_tokens.sh samples/sample1_email_analysis  # View token examples./run_visual.sh --compact --stats < input.txt              # Wrapper script



# Testing# Sample testing

cat samples/sample7_basic_example/email_tokens.txt | ./custom_compilermake run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

```make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

make run DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def

---```



## 📄 Files Overview**Start with:** `make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def`



### Core Files---

- `generator.py` - Converts .def files to Flex/Bison

- `ast.c`, `ast.h` - Parse tree library*Happy parsing! 🎉*

- `Makefile` - Build automation

## Extending the Project

### Visualization

- `streamlit_visualizer.py` - Web UI (interactive)- Enhance `generator.c` to recognise typed semantic values and emit `%union` and `%type` directives automatically.

- `visualize_tree.py` - Terminal UI (colorful text)- Augment the runtime with symbol tables, type checking, or code generation.

- `run_ui.sh` - Launch web UI- Add tests for custom grammars by scripting runs of `./custom_compiler` with expected output comparisons.

- `setup_ui.sh` - Install UI dependencies

Feel free to adapt the generator and runtime to suit your language experiments. The current setup is intentionally simple and aims to get from grammar sketch to executable semantics with minimal boilerplate.

### Utilities
- `generate_token_data.py` - Generate token example files
- `view_tokens.sh` - Browse token files
- `requirements.txt` - Python dependencies

### Samples
- 7 complete analyzer examples in `samples/`
- Each with .def, input, and token files

---

## ✨ Features Summary

✅ **Simple .def Format** - Define analyzers in minutes  
✅ **Automatic Generation** - Python creates Flex/Bison code  
✅ **Web UI** - Interactive graph visualization  
✅ **Terminal UI** - Colorful text visualization  
✅ **Multiple Styles** - Fancy, simple, compact views  
✅ **Statistics** - Pattern count summaries  
✅ **7 Sample Analyzers** - Ready-to-use examples  
✅ **Token Files** - 40+ example data files  
✅ **Comprehensive Docs** - Complete documentation  
✅ **Automated Build** - One command to build and run  
✅ **Easy Customization** - Modify or create new analyzers  

---

## 🤝 Contributing

This is an educational project. Feel free to:
- Create new sample analyzers
- Improve visualization
- Add token templates
- Report issues

---

## 📜 License

See repository for license information.

---

**Happy Parsing! 🎉**

*Get started:* `./run_ui.sh` *or* `make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def`
