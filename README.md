# cfg2yacc - Compiler Generator with Interactive Visualization# cfg2yacc - Compiler Generator with Parse Tree Visualization



A Python-based compiler generator that creates custom text analyzers from simple `.def` files. Generate complete Flex/Bison-based compilers with **beautiful web UI** and **colorful terminal visualization**.A Python-based compiler generator that creates custom text analyzers from simple `.def` files. Write tokenization rules and grammar once, then generate a complete Flex/Bison-based compiler with **beautiful, colorful parse tree visualization**.



Perfect for: pattern detection, log analysis, data extraction, language processing, compiler learning, and rapid prototyping.Perfect for: pattern detection, log analysis, data extraction, language processing, compiler learning, and rapid prototyping.



------



## 🚀 Quick Start## 🚀 Quick Start (3 Steps)



### 1. Install Prerequisites### Step 1: Install Prerequisites



```bash```bash

# Ubuntu/Debian# Ubuntu/Debian

sudo apt-get updatesudo apt-get update

sudo apt-get install build-essential flex bison python3sudo apt-get install build-essential flex bison python3



# For web UI (optional but recommended)# Verify installation

./setup_ui.shgcc --version

```flex --version

bison --version

### 2. Run Your First Analyzerpython3 --version

```

```bash

# Terminal visualization (colorful text)### Step 2: Build Your First Analyzer

make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

```bash

# OR Web UI (interactive graphs)# Clone or navigate to the project

./run_ui.shcd /path/to/cfg2yacc

```

# Build the default analyzer (analyzes structured text)

**That's it!** Choose from 7 ready-to-use analyzers or create your own.make DEF_FILE=analyzer.def



---# This creates the 'custom_compiler' executable

```

## 📋 Available Analyzers

### Step 3: Run with Colorful Visualization

| Sample | Purpose | Key Tokens |

|--------|---------|------------|```bash

| **Sample 1** | Email Analysis | EMAIL_ADDR, PHONE_NUMBER, URGENT, CURRENCY |# See the parse tree in beautiful colors!

| **Sample 2** | Code Analysis | FUNCTION_DEF, VARIABLE_ASSIGN, STRING_LITERAL |make run DEF_FILE=analyzer.def

| **Sample 3** | Log Analysis | TIMESTAMP, IP_ADDRESS, HTTP_METHOD, STATUS_CODE |```

| **Sample 4** | Spam Detection | SPAM_WORDS, MONEY_TERMS, SUSPICIOUS_LINK |

| **Sample 5** | Social Media | HASHTAG, USER_MENTION, EMOJI_FACE, LIKE_COUNT |**That's it!** You now have a working text analyzer with colorful output.

| **Sample 6** | Financial Data | CURRENCY_USD, ACCOUNT_NUMBER, STOCK_SYMBOL |

| **Sample 7** | Basic Example | EMAIL, PHONE, FLOAT_NUMBER (best for learning) |---



---## 📖 Complete Step-by-Step Guide



## 🎨 Visualization Options### Starting from Scratch



### Option 1: Web UI (Recommended)#### 1. **Understand What You Have**



```bashYour project contains:

./run_ui.sh- **Python Generator** (`generator.py`) - Converts `.def` files to Flex/Bison code

```- **Parse Tree Library** (`ast.c`, `ast.h`) - Builds and displays parse trees

- **Visualizer** (`visualize_tree.py`) - Makes output colorful and beautiful

**Features:**- **Sample Analyzers** (in `samples/`) - 7 ready-to-use examples

- 🌐 Interactive web interface at http://localhost:8501- **Build System** (`Makefile`) - Automates everything

- 📊 Beautiful graph visualization with color-coded nodes

- 📈 Real-time statistics and pattern counts#### 2. **Choose an Analyzer to Run**

- ✏️ Test with custom input instantly

- 💾 Export graphs as PNG/SVGPick one of these ready-made analyzers:

- 🔄 Switch between analyzers with one click

| Analyzer | Purpose | Location |

**Perfect for:** Interactive exploration, presentations, debugging|----------|---------|----------|

| Basic Example | Emails, phones, numbers | `samples/sample7_basic_example/S7_analyzer.def` |

### Option 2: Terminal Visualization| Email Analysis | Email patterns, urgency detection | `samples/sample1_email_analysis/S1_analyzer.def` |

| Code Analysis | Functions, variables, comments | `samples/sample2_code_analysis/S2_analyzer.def` |

```bash| Log Analysis | Timestamps, IPs, HTTP codes | `samples/sample3_log_analysis/S3_analyzer.def` |

# Fancy style (box drawing)| Spam Detection | Spam words, suspicious links | `samples/sample4_spam_detection/S4_analyzer.def` |

make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def| Social Media | Hashtags, mentions, emojis | `samples/sample5_social_media/S5_analyzer.def` |

| Financial Data | Currency, account numbers | `samples/sample6_financial_data/S6_analyzer.def` |

# Compact style

make run-compact DEF_FILE=samples/sample7_basic_example/S7_analyzer.def#### 3. **Build the Analyzer**



# With statistics```bash

make run-stats DEF_FILE=samples/sample7_basic_example/S7_analyzer.def# Clean any previous builds

```make clean



**Features:**# Build a specific analyzer (example: email analysis)

- 🎨 ANSI color-coded outputmake DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

- 🌲 Tree structure visualization```

- 📊 Optional statistics

- ⚡ Fast, lightweight**What happens:**

1. Python generator reads the `.def` file

**Perfect for:** Command-line workflows, automation, scripting2. Generates `lexer.l` (tokenization rules)

3. Generates `parser.y` (grammar rules)

---4. Flex creates `lex.yy.c` from `lexer.l`

5. Bison creates `y.tab.c` from `parser.y`

## 📖 How It Works6. GCC compiles everything into `custom_compiler`



### The .def File Format#### 4. **Run the Analyzer**



Create a `.def` file with two sections:**Option A: With Colorful Visualization (Recommended)**



#### 1. %%LEX Section - Define Tokens```bash

# Use the built-in visualization

```./custom_compiler < samples/sample1_email_analysis/S1_input.txt | python3 visualize_tree.py

%%LEX

EMAIL           [a-zA-Z0-9._]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}# OR use the convenient Makefile target

PHONE_NUMBER    [0-9]{3}-[0-9]{3}-[0-9]{4}make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def

WHITESPACE      [ \t\n]+```

```

**Option B: Plain Text Output**

#### 2. %%YACC Section - Define Grammar

```bash

```./custom_compiler < samples/sample1_email_analysis/S1_input.txt

%%YACC```

document -> element_list

#### 5. **Explore Different Visualization Styles**

element_list -> element_list element

              | element```bash

# Fancy style (box drawing, tree structure) - DEFAULT

element -> email_foundmake run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

         | phone_found

# Simple style (clean indented view)

email_found -> EMAILmake run-simple DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

    { $$ = create_leaf_node("email_found", yytext); }

# Compact style (shows only important nodes)

phone_found -> PHONE_NUMBERmake run-compact DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

    { $$ = create_leaf_node("phone_found", yytext); }

```# With statistics (pattern counts)

make run-stats DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

### Build Process```



```#### 6. **Try Different Samples**

your_analyzer.def

    ↓ (generator.py)```bash

lexer.l + parser.y# Test basic example (emails, phones, floats)

    ↓ (flex + bison)make clean && make DEF_FILE=samples/sample7_basic_example/S7_analyzer.def

lex.yy.c + y.tab.c./custom_compiler < samples/sample7_basic_example/S7_input.txt | python3 visualize_tree.py --stats

    ↓ (gcc)

custom_compiler# Test log analysis (timestamps, IPs, status codes)

    ↓make clean && make DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def

Parse Tree Output./custom_compiler < samples/sample3_log_analysis/S3_input.txt | python3 visualize_tree.py --compact

```

# Test spam detection

---make clean && make DEF_FILE=samples/sample4_spam_detection/S4_analyzer.def

./custom_compiler < samples/sample4_spam_detection/S4_input.txt | python3 visualize_tree.py

## 🔧 Creating Your Own Analyzer```



### Step 1: Create a .def File---



```bash## 🎨 Visualization Features

nano my_analyzer.def

```### Three Display Styles



### Step 2: Define Your Tokens and Grammar**1. Fancy (Default)** - Box drawing with tree structure

```

See `samples/sample7_basic_example/S7_analyzer.def` for a simple example.╔════════════════════════════════════════════════════════════════════╗

║  Parse Tree Visualization                                           ║

### Step 3: Build and Test╚════════════════════════════════════════════════════════════════════╝



```bash│email_document

# Build│  ├─ │email

make DEF_FILE=my_analyzer.def│      ├─ │email_address

│          └─ EMAIL_ADDR ➜ john@example.com

# Create test input```

echo "Test data here" > my_input.txt

**2. Simple** - Clean indented view

# Run```

./custom_compiler < my_input.txt | python3 visualize_tree.py======================================================================

```  Parse Tree Visualization

======================================================================

---

email_document

## 📁 Project Structure  email

    email_address

```      EMAIL_ADDR → john@example.com

cfg2yacc/```

├── generator.py              # Converts .def → lexer.l + parser.y

├── ast.c, ast.h             # Parse tree library**3. Compact** - Shows only terminals (detected patterns)

├── Makefile                 # Build automation```

│┌────────────────────────────────────────────────────────────────────┐

├── visualize_tree.py        # Terminal visualization│  Parse Tree (Compact View)                                          │

├── streamlit_visualizer.py  # Web UI└────────────────────────────────────────────────────────────────────┘

├── generate_token_data.py   # Token example generator

│▪ email_document

├── run_ui.sh               # Launch web UI  ▸ EMAIL_ADDR: john@example.com

├── setup_ui.sh             # Install web UI dependencies  ▸ PHONE_NUMBER: 555-123-4567

├── view_tokens.sh          # View token examples  ▸ URGENT: URGENT

│```

├── samples/                # 7 ready-to-use analyzers

│   ├── sample1_email_analysis/### Color Coding

│   ├── sample2_code_analysis/

│   ├── sample3_log_analysis/- 🔴 **Red** - URGENT/IMMEDIATE keywords

│   ├── sample4_spam_detection/- 🟣 **Magenta** - Email addresses

│   ├── sample5_social_media/- 🔵 **Blue** - Phone numbers

│   ├── sample6_financial_data/- 💜 **Bright Magenta** - Website URLs

│   └── sample7_basic_example/- 💚 **Bright Green** - Currency amounts

│- 🟡 **Yellow** - Terminal tokens

└── README.md               # This file- 🔵 **Cyan** - Grammar rules

```- 🟢 **Green** - Token values



---### Statistics Mode



## 🎯 Common Use Cases```

╔════════════════════════════════════════════════════════════════════╗

### 1. Email Analysis║  Statistics                                                         ║

```bash╠════════════════════════════════════════════════════════════════════╣

make run DEF_FILE=samples/sample1_email_analysis/S1_analyzer.def║  Total Nodes: 646                                                     ║

# Detects: emails, phones, URLs, currency, dates, urgent keywords║  Terminals: 247                                                       ║

```║  Non-terminals: 399                                                   ║

║  Detected Patterns:                                                 ║

### 2. Log File Analysis║    • email: 6                                                          ║

```bash║    • phone_formatted: 4                                                ║

make run DEF_FILE=samples/sample3_log_analysis/S3_analyzer.def║    • float_number: 18                                                  ║

# Detects: timestamps, IPs, HTTP methods, status codes, error levels╚════════════════════════════════════════════════════════════════════╝

``````



### 3. Spam Detection---

```bash

make run DEF_FILE=samples/sample4_spam_detection/S4_analyzer.def## 📋 Command Reference

# Detects: spam words, money terms, suspicious links, urgency indicators

```### Makefile Commands



### 4. Code Pattern Analysis```bash

```bash# Build and run with fancy visualization

make run DEF_FILE=samples/sample2_code_analysis/S2_analyzer.defmake run DEF_FILE=path/to/analyzer.def

# Detects: function definitions, variable assignments, string literals

```# Different visualization styles

make run-simple DEF_FILE=path/to/analyzer.def

### 5. Social Media Contentmake run-compact DEF_FILE=path/to/analyzer.def

```bashmake run-stats DEF_FILE=path/to/analyzer.def

make run DEF_FILE=samples/sample5_social_media/S5_analyzer.def

# Detects: hashtags, mentions, emojis, like/share counts# Build only (no visualization)

```make DEF_FILE=path/to/analyzer.def



---# Clean generated files

make clean

## 🛠️ Command Reference

# Complete rebuild

### Buildingmake rebuild DEF_FILE=path/to/analyzer.def

```

```bash

# Build with specific analyzer### Manual Commands

make DEF_FILE=path/to/analyzer.def

```bash

# Clean generated files# Step-by-step build process

make cleanpython3 generator.py analyzer.def    # Generate lexer.l and parser.y

flex lexer.l                         # Create lex.yy.c

# Rebuild from scratchbison -d -o y.tab.c parser.y         # Create y.tab.c and y.tab.h

make clean allgcc -o custom_compiler y.tab.c lex.yy.c ast.o -lfl  # Compile executable

```

# Run with different options

### Running./custom_compiler < input.txt                                    # Plain output

./custom_compiler < input.txt | python3 visualize_tree.py        # Fancy visualization

```bash./custom_compiler < input.txt | python3 visualize_tree.py --style simple

# Terminal visualization (fancy)./custom_compiler < input.txt | python3 visualize_tree.py --style compact --stats

make run DEF_FILE=samples/sample7_basic_example/S7_analyzer.def./custom_compiler < input.txt | python3 visualize_tree.py --no-color > output.txt

```

# Different styles

make run-simple DEF_FILE=...### Wrapper Script

make run-compact DEF_FILE=...

make run-stats DEF_FILE=...```bash

# Make it executable (already done)

# Web UIchmod +x run_visual.sh

./run_ui.sh

# Use it

# Manual pipeline./run_visual.sh < input.txt                 # Fancy style

./custom_compiler < input.txt | python3 visualize_tree.py./run_visual.sh --simple < input.txt        # Simple style

```./run_visual.sh --compact < input.txt       # Compact style

./run_visual.sh --stats < input.txt         # Show statistics

### Tokens./run_visual.sh --compact --stats < input.txt  # Combine options

./run_visual.sh --help                      # Show help

```bash```

# Generate token example files for all samples

python3 generate_token_data.py---



# View token examples## 📁 Project Structure

./view_tokens.sh samples/sample7_basic_example

```

# View specific token filecfg2yacc/

cat samples/sample1_email_analysis/email_addr_tokens.txt├── generator.py              # Python generator (converts .def → lexer.l + parser.y)

```├── ast.c, ast.h             # Parse tree library

├── visualize_tree.py        # Colorful visualization tool

---├── run_visual.sh            # Convenience wrapper script

├── Makefile                 # Build automation

## 📊 Token Example Files├── analyzer.def             # Example analyzer definition

├── test_input.txt           # Example input

Each sample includes token files with example data:├── samples/                 # 7 ready-to-use analyzers

│   ├── sample1_email_analysis/

```bash│   │   ├── S1_analyzer.def

samples/sample7_basic_example/│   │   ├── S1_input.txt

├── S7_analyzer.def│   │   └── S1_rules.txt

├── S7_input.txt│   ├── sample2_code_analysis/

├── email_tokens.txt          # Example emails│   ├── sample3_log_analysis/

├── phone_formatted_tokens.txt # Example phone numbers│   ├── sample4_spam_detection/

├── float_number_tokens.txt   # Example floating point numbers│   ├── sample5_social_media/

└── ...│   ├── sample6_financial_data/

```│   └── sample7_basic_example/

└── docs/

**Use token files to:**    ├── GENERATOR_README.md        # Complete generator documentation

- Test individual token patterns    ├── QUICKSTART.md              # Quick start guide

- Create comprehensive test inputs    ├── VISUALIZATION.md           # Visualization guide

- Validate lexer rules    ├── VISUALIZATION_QUICKREF.md  # Quick reference

- Document expected matches    └── SAMPLES_README.md          # Sample analyzers guide

```

```bash

# View all token files for a sample---

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
