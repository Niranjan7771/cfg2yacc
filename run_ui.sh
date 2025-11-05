#!/bin/bash
# run_ui.sh
# ---------
# Small convenience script to launch the Streamlit parse tree visualizer.
# It performs a couple of checks (ensure streamlit is installed) and then
# runs `streamlit run streamlit_visualizer.py`.

# Add ~/.local/bin to PATH if it exists (common location for `pip --user` installs)
if [ -d "$HOME/.local/bin" ]; then
    export PATH="$HOME/.local/bin:$PATH"
fi

# Check for streamlit
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

# Launch Streamlit app from the repository root
streamlit run streamlit_visualizer.py
