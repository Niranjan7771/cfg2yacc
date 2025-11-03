#!/bin/bash
# Quick script to run the compiler with colorful visualization

# Default style
STYLE="fancy"
EXTRA_ARGS=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -s|--simple)
            STYLE="simple"
            shift
            ;;
        -c|--compact)
            STYLE="compact"
            shift
            ;;
        -f|--fancy)
            STYLE="fancy"
            shift
            ;;
        --stats)
            EXTRA_ARGS="$EXTRA_ARGS --stats"
            shift
            ;;
        --no-color)
            EXTRA_ARGS="$EXTRA_ARGS --no-color"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS] < input_file"
            echo ""
            echo "Options:"
            echo "  -s, --simple     Use simple visualization style"
            echo "  -c, --compact    Use compact visualization style"
            echo "  -f, --fancy      Use fancy visualization style (default)"
            echo "  --stats          Show statistics"
            echo "  --no-color       Disable colors"
            echo "  -h, --help       Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 < test_input.txt"
            echo "  $0 --compact --stats < test_input.txt"
            echo "  ./custom_compiler < input.txt | $0 --simple"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Check if custom_compiler exists
if [ ! -f "./custom_compiler" ]; then
    echo "Error: custom_compiler not found. Run 'make' first."
    exit 1
fi

# Check if visualize_tree.py exists
if [ ! -f "./visualize_tree.py" ]; then
    echo "Error: visualize_tree.py not found."
    exit 1
fi

# Run the compiler and pipe to visualizer
./custom_compiler | python3 visualize_tree.py --style "$STYLE" $EXTRA_ARGS
