#!/bin/bash
# Quick launcher for Streamlit UI

# Add ~/.local/bin to PATH if it exists
if [ -d "$HOME/.local/bin" ]; then
    export PATH="$HOME/.local/bin:$PATH"
fi

if ! command -v streamlit &> /dev/null; then
    echo "❌ Streamlit not installed!"
    echo ""
    echo "Run setup first:"
    echo "  ./setup_ui.sh"
    echo ""
    echo "Or install manually:"
    echo "  pip3 install streamlit graphviz"
    exit 1
fi

echo "🌲 Starting Parse Tree Visualizer UI..."
echo ""
echo "The web interface will open automatically in your browser."
echo "If it doesn't, go to: http://localhost:8501"
echo ""
echo "Press Ctrl+C to stop the server."
echo ""

streamlit run streamlit_visualizer.py
