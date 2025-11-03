#!/bin/bash
# View all token files for a sample

if [ -z "$1" ]; then
    echo "Usage: $0 <sample_directory>"
    echo ""
    echo "Examples:"
    echo "  $0 samples/sample1_email_analysis"
    echo "  $0 samples/sample7_basic_example"
    echo ""
    echo "Available samples:"
    for dir in samples/sample*; do
        if [ -d "$dir" ]; then
            count=$(ls -1 "$dir"/*_tokens.txt 2>/dev/null | wc -l)
            echo "  • $dir ($count token files)"
        fi
    done
    exit 1
fi

SAMPLE_DIR="$1"

if [ ! -d "$SAMPLE_DIR" ]; then
    echo "❌ Directory not found: $SAMPLE_DIR"
    exit 1
fi

echo "📁 Token Files in $SAMPLE_DIR"
echo "=" | awk '{for(i=1;i<=70;i++)printf "="; print ""}'
echo ""

# Count token files
TOKEN_FILES=$(ls -1 "$SAMPLE_DIR"/*_tokens.txt 2>/dev/null)

if [ -z "$TOKEN_FILES" ]; then
    echo "⚠️  No token files found!"
    echo ""
    echo "Generate them with:"
    echo "  python3 generate_token_data.py"
    exit 0
fi

COUNT=$(echo "$TOKEN_FILES" | wc -l)
echo "Found $COUNT token files:"
echo ""

# List and preview each file
for file in $TOKEN_FILES; do
    filename=$(basename "$file")
    token_name=$(echo "$filename" | sed 's/_tokens.txt//' | tr '[:lower:]' '[:upper:]')
    
    echo "📝 $filename"
    echo "   Token: $token_name"
    
    # Show first 3 examples
    echo "   Examples:"
    grep -v '^#' "$file" | grep -v '^$' | head -3 | while read line; do
        echo "     • $line"
    done
    
    # Count total examples
    total=$(grep -v '^#' "$file" | grep -v '^$' | wc -l)
    echo "   Total: $total examples"
    echo ""
done

echo "=" | awk '{for(i=1;i<=70;i++)printf "="; print ""}'
echo ""
echo "💡 To view a specific file:"
echo "   cat $SAMPLE_DIR/<filename>"
echo ""
echo "💡 To regenerate all token files:"
echo "   python3 generate_token_data.py"
