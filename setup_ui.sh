#!/bin/bash
# Setup script for Streamlit UI

echo "🚀 Setting up Parse Tree Visualizer UI..."
echo ""

# Check if pip is installed
if ! command -v pip3 &> /dev/null; then
    echo "❌ pip3 not found. Please install Python3 and pip first."
    echo "   sudo apt-get install python3-pip"
    exit 1
fi

# Install system dependencies for graphviz
echo "📦 Installing system dependencies..."
if command -v apt-get &> /dev/null; then
    sudo apt-get update
    sudo apt-get install -y graphviz
elif command -v yum &> /dev/null; then
    sudo yum install -y graphviz
elif command -v brew &> /dev/null; then
    brew install graphviz
else
    echo "⚠️  Please install graphviz manually for your system"
fi

# Install Python packages
echo ""
echo "📦 Installing Python packages..."
pip3 install -r requirements.txt

echo ""
echo "✅ Setup complete!"
echo ""
echo "🎨 To run the visualizer:"
echo "   streamlit run streamlit_visualizer.py"
echo ""
echo "   Or use the shortcut:"
echo "   ./run_ui.sh"
echo ""
