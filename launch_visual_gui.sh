#!/bin/bash

echo "🎨 Launching Visual Parse Tree GUI"
echo "================================="

# Check requirements
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 required but not found"
    exit 1
fi

if ! python3 -c "import tkinter" 2>/dev/null; then
    echo "❌ tkinter required but not found"
    echo "   Install with: sudo apt-get install python3-tk"
    exit 1
fi

# Build if needed
if [ ! -f "./parser" ]; then
    echo "🔨 Building parser..."
    make clean && make
    ./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff
    bison -d expr.y && flex expr.l && gcc -o parser expr.tab.c lex.yy.c ast.c -lfl
    
    if [ $? -ne 0 ]; then
        echo "❌ Build failed"
        exit 1
    fi
fi

echo "✅ Ready!"
echo "🚀 Launching Visual Parse Tree GUI..."

# Launch the enhanced visual GUI
python3 visual_parse_tree_gui.py

echo "👋 GUI closed"