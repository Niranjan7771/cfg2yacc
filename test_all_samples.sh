#!/bin/bash
# Test all sample analyzers
# This script builds and tests each of the 7 sample analyzers

echo "=========================================="
echo "Testing All Sample Analyzers"
echo "=========================================="
echo ""

# Array of samples (name, def_file, input_file)
samples=(
    "1:Email Analysis:samples/sample1_email_analysis/S1_analyzer.def:samples/sample1_email_analysis/S1_input.txt"
    "2:Code Analysis:samples/sample2_code_analysis/S2_analyzer.def:samples/sample2_code_analysis/S2_input.txt"
    "3:Log Analysis:samples/sample3_log_analysis/S3_analyzer.def:samples/sample3_log_analysis/S3_input.txt"
    "4:Spam Detection:samples/sample4_spam_detection/S4_analyzer.def:samples/sample4_spam_detection/S4_input.txt"
    "5:Social Media:samples/sample5_social_media/S5_analyzer.def:samples/sample5_social_media/S5_input.txt"
    "6:Financial Data:samples/sample6_financial_data/S6_analyzer.def:samples/sample6_financial_data/S6_input.txt"
    "7:Basic Example:samples/sample7_basic_example/S7_analyzer.def:samples/sample7_basic_example/S7_input.txt"
)

# Test each sample
for sample in "${samples[@]}"; do
    IFS=':' read -r num name def_file input_file <<< "$sample"
    
    echo "=========================================="
    echo "Sample $num: $name"
    echo "=========================================="
    echo "DEF file: $def_file"
    echo "Input file: $input_file"
    echo ""
    
    # Build
    echo "Building..."
    make clean > /dev/null 2>&1
    if make DEF_FILE="$def_file" > /dev/null 2>&1; then
        echo "✓ Build successful"
    else
        echo "✗ Build failed"
        echo ""
        continue
    fi
    
    # Run
    echo "Running analyzer..."
    if ./custom_compiler "$input_file" > /tmp/sample_output.txt 2>&1; then
        echo "✓ Execution successful"
        
        # Show summary
        echo ""
        echo "Parse tree summary:"
        lines=$(wc -l < /tmp/sample_output.txt)
        echo "  - Total lines: $lines"
        
        # Count detected patterns (sample-specific)
        case $num in
            1)
                emails=$(grep -c "email_address" /tmp/sample_output.txt 2>/dev/null || echo 0)
                phones=$(grep -c "phone_number" /tmp/sample_output.txt 2>/dev/null || echo 0)
                urls=$(grep -c "website_url" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Email addresses detected: $emails"
                echo "  - Phone numbers detected: $phones"
                echo "  - Website URLs detected: $urls"
                ;;
            2)
                funcs=$(grep -c "function_definition" /tmp/sample_output.txt 2>/dev/null || echo 0)
                vars=$(grep -c "variable_assignment" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Function definitions: $funcs"
                echo "  - Variable assignments: $vars"
                ;;
            3)
                ips=$(grep -c "ip_address_entry" /tmp/sample_output.txt 2>/dev/null || echo 0)
                http=$(grep -c "http_method_entry" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - IP addresses: $ips"
                echo "  - HTTP methods: $http"
                ;;
            4)
                spam=$(grep -c "spam_word_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                money=$(grep -c "money_term_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Spam words detected: $spam"
                echo "  - Money terms detected: $money"
                ;;
            5)
                hashtags=$(grep -c "hashtag_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                mentions=$(grep -c "user_mention_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Hashtags detected: $hashtags"
                echo "  - User mentions detected: $mentions"
                ;;
            6)
                currency=$(grep -c "currency_amount_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                stocks=$(grep -c "stock_symbol_detected" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Currency amounts: $currency"
                echo "  - Stock symbols: $stocks"
                ;;
            7)
                emails=$(grep -c "email_found" /tmp/sample_output.txt 2>/dev/null || echo 0)
                floats=$(grep -c "float_number_found" /tmp/sample_output.txt 2>/dev/null || echo 0)
                echo "  - Emails found: $emails"
                echo "  - Float numbers found: $floats"
                ;;
        esac
    else
        echo "✗ Execution failed"
    fi
    
    echo ""
done

echo "=========================================="
echo "Testing Complete"
echo "=========================================="

# Cleanup
rm -f /tmp/sample_output.txt
