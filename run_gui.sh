#!/bin/bash

# Parse Tree GUI Launcher for cfg2yacc
# This script builds the parser and launches the GUI

echo "🚀 cfg2yacc Parse Tree GUI Launcher"
echo "=================================="

# Check if Python3 is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Error: Python3 is required but not installed"
    exit 1
fi

# Check if tkinter is available
if ! python3 -c "import tkinter" 2>/dev/null; then
    echo "❌ Error: tkinter is required but not available"
    echo "   Install with: sudo apt-get install python3-tk"
    exit 1
fi

# Build cfg2yacc if needed
if [ ! -f "./cfg2yacc" ]; then
    echo "🔨 Building cfg2yacc..."
    make clean && make
    if [ $? -ne 0 ]; then
        echo "❌ Failed to build cfg2yacc"
        exit 1
    fi
fi

# Generate parser if needed
if [ ! -f "./parser" ]; then
    echo "🔨 Generating parser from expression grammar..."
    ./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff
    bison -d expr.y
    flex expr.l
    gcc -o parser expr.tab.c lex.yy.c ast.c -lfl
    
    if [ $? -ne 0 ]; then
        echo "❌ Failed to generate parser"
        exit 1
    fi
fi

# Test parser
echo "🧪 Testing parser..."
echo "1+2*3" | ./parser > /dev/null
if [ $? -ne 0 ]; then
    echo "❌ Parser test failed"
    exit 1
fi

echo "✅ All checks passed!"
echo "🎨 Launching Parse Tree GUI..."
echo ""

# Launch GUI
python3 parse_tree_gui.py

echo "👋 GUI closed. Thanks for using cfg2yacc!"