#!/usr/bin/env python3
"""
Token Data Generator
Automatically generates example token data files for all samples
"""

import os
import re
from pathlib import Path

# Token data templates for each pattern type
TOKEN_DATA = {
    # Email patterns
    'EMAIL': [
        'john.doe@example.com',
        'alice_smith@university.edu',
        'support@company.org',
        'info@website.net',
        'user123@domain.co.uk',
        'test.email@subdomain.example.com'
    ],
    'EMAIL_ADDR': [
        'john.doe@company.com',
        'support@securebank.com',
        'marketing@deals.net',
        'newsletter@techblog.org',
        'alerts@bank.com',
        'security@bank.com',
        'sales@deals.net',
        'editor@techblog.org',
        'promo@store.com',
        'help@store.com'
    ],
    
    # Phone patterns
    'PHONE_NUMBER': [
        '555-123-4567',
        '800-555-0199',
        '555-987-6543',
        '555-444-3333',
        '555-111-2222'
    ],
    'PHONE_FORMATTED': [
        '(555) 123-4567',
        '(800) 555-0199',
        '(212) 555-1234',
        '(999) 888-7777'
    ],
    'PHONE_SIMPLE': [
        '555-987-6543',
        '123 456 7890',
        '555.123.4567'
    ],
    
    # Website/URL patterns
    'WEBSITE_URL': [
        'www.example.com',
        'www.securebank.com',
        'www.deals.net',
        'www.techblog.org',
        'www.bank.com',
        'www.store.com'
    ],
    'URL': [
        'http://www.example.com',
        'https://github.com/user/repo',
        'www.website.org',
        'ftp://files.domain.com'
    ],
    
    # Currency patterns
    'CURRENCY': [
        '$99.99',
        '$25.00',
        '$15.50',
        '$0.00',
        '$500.00'
    ],
    'CURRENCY_USD': [
        '$1,234.56',
        '$999.99',
        '$50.00',
        '$10,000.00'
    ],
    
    # Date patterns
    'DATE_FORMAT': [
        '12/15/2024',
        '01/01/2025',
        '12/25/2024',
        '03/14/2024',
        '07/04/2024'
    ],
    'DATE': [
        '2024-12-15',
        '2025-01-01',
        '2024-07-04'
    ],
    
    # Urgent/Spam keywords
    'URGENT': [
        'URGENT',
        'IMMEDIATE',
        'ACT',
        'NOW',
        'LIMITED',
        'TIME'
    ],
    'SPAM_WORDS': [
        'winner',
        'free',
        'prize',
        'congratulations',
        'click here',
        'limited time'
    ],
    'MONEY_TERMS': [
        'cash',
        'money',
        'profit',
        'income',
        'earnings',
        'revenue'
    ],
    'URGENCY_WORDS': [
        'urgent',
        'immediate',
        'now',
        'hurry',
        'quick',
        'fast'
    ],
    'SUSPICIOUS_LINK': [
        'bit.ly/xyz123',
        'tinyurl.com/abc',
        'shorturl.at/def'
    ],
    'CAPS_WORD': [
        'WINNER',
        'FREE',
        'GUARANTEED',
        'AMAZING'
    ],
    
    # Numeric patterns
    'FLOAT_NUMBER': [
        '3.14159',
        '2.71828',
        '1.23',
        '4.56',
        '7.89',
        '23.5',
        '19.99',
        '40.7128',
        '74.0060',
        '85.7',
        '0.5',
        '100.0',
        '999.99',
        '0.001',
        '42.42'
    ],
    'NUMBER': [
        '123',
        '456',
        '789',
        '0',
        '999'
    ],
    'INTEGER': [
        '42',
        '100',
        '0',
        '9999'
    ],
    
    # Log analysis patterns
    'TIMESTAMP': [
        '2024-11-03 14:23:45',
        '2024-11-03 09:15:30',
        '2024-11-03 18:45:12'
    ],
    'IP_ADDRESS': [
        '192.168.1.100',
        '10.0.0.1',
        '172.16.0.50',
        '8.8.8.8',
        '127.0.0.1'
    ],
    'HTTP_METHOD': [
        'GET',
        'POST',
        'PUT',
        'DELETE',
        'PATCH'
    ],
    'STATUS_CODE': [
        '200',
        '404',
        '500',
        '301',
        '403'
    ],
    'ERROR_LEVEL': [
        'ERROR',
        'WARNING',
        'INFO',
        'DEBUG',
        'CRITICAL'
    ],
    
    # Social media patterns
    'HASHTAG': [
        '#technology',
        '#ai',
        '#programming',
        '#python',
        '#coding',
        '#webdev'
    ],
    'USER_MENTION': [
        '@user123',
        '@john_doe',
        '@tech_news',
        '@developer'
    ],
    'EMOJI_FACE': [
        '😀',
        '😊',
        '🎉',
        '❤️',
        '👍'
    ],
    'LIKE_COUNT': [
        '1.5K likes',
        '234 likes',
        '10K likes'
    ],
    'SHARE_COUNT': [
        '500 shares',
        '1.2K shares',
        '50 shares'
    ],
    
    # Financial patterns
    'ACCOUNT_NUMBER': [
        'ACC-123456789',
        'ACC-987654321',
        'ACC-111222333'
    ],
    'TRANSACTION_ID': [
        'TXN-ABC123XYZ',
        'TXN-DEF456UVW',
        'TXN-GHI789RST'
    ],
    'STOCK_SYMBOL': [
        'AAPL',
        'GOOGL',
        'MSFT',
        'TSLA',
        'AMZN'
    ],
    
    # Code analysis patterns
    'FUNCTION_DEF': [
        'def calculate_total():',
        'function getData() {',
        'void processData() {'
    ],
    'VARIABLE_ASSIGN': [
        'x = 10',
        'result = calculate()',
        'data = []'
    ],
    'STRING_LITERAL': [
        '"Hello, World!"',
        "'test string'",
        '"""multi-line"""'
    ],
    'COMMENT_LINE': [
        '// This is a comment',
        '# Python comment',
        '/* C-style comment */'
    ],
    'KEYWORD': [
        'if',
        'else',
        'for',
        'while',
        'return',
        'class',
        'def',
        'import'
    ]
}

def extract_tokens_from_def(def_file):
    """Extract token names from a .def file"""
    tokens = []
    
    with open(def_file, 'r') as f:
        content = f.read()
    
    # Find %%LEX section
    lex_match = re.search(r'%%LEX\s*\n(.*?)%%YACC', content, re.DOTALL)
    if not lex_match:
        return tokens
    
    lex_section = lex_match.group(1)
    
    # Extract token names (ignore WHITESPACE, NEWLINE, etc.)
    for line in lex_section.split('\n'):
        line = line.strip()
        if not line or line.startswith('//'):
            continue
        
        # Token format: TOKEN_NAME    regex_pattern
        parts = line.split(None, 1)
        if len(parts) >= 1:
            token_name = parts[0]
            
            # Skip common skip tokens
            skip_tokens = ['WHITESPACE', 'NEWLINE', 'SPACE', 'TAB', 
                          'COMMA', 'DOT', 'COLON', 'SEMICOLON', 
                          'LPAREN', 'RPAREN', 'LBRACE', 'RBRACE',
                          'PLUS', 'MINUS', 'TIMES', 'DIVIDE',
                          'EQUALS', 'DASH', 'WORD', 'LETTER',
                          'DIGIT', 'PERCENT', 'DOLLAR', 'AT']
            
            if token_name.upper() not in skip_tokens and not any(x in token_name.upper() for x in ['WHITESPACE', 'COMMENT']):
                tokens.append(token_name)
    
    return tokens

def generate_token_files(sample_dir):
    """Generate token data files for a sample directory"""
    sample_path = Path(sample_dir)
    
    # Find .def file
    def_files = list(sample_path.glob('*_analyzer.def'))
    if not def_files:
        print(f"  ⚠️  No analyzer.def found in {sample_dir}")
        return
    
    def_file = def_files[0]
    print(f"\n📁 Processing: {sample_path.name}")
    print(f"  Analyzer: {def_file.name}")
    
    # Extract tokens
    tokens = extract_tokens_from_def(def_file)
    print(f"  Found {len(tokens)} tokens")
    
    # Generate files for each token
    files_created = 0
    for token in tokens:
        # Get sample data
        if token in TOKEN_DATA:
            data = TOKEN_DATA[token]
        elif token.upper() in TOKEN_DATA:
            data = TOKEN_DATA[token.upper()]
        else:
            # Generate generic data
            print(f"  ⚠️  No template for {token}, skipping")
            continue
        
        # Create token file
        filename = f"{token.lower()}_tokens.txt"
        filepath = sample_path / filename
        
        with open(filepath, 'w') as f:
            f.write(f"# Example {token} tokens\n")
            f.write(f"# Generated automatically for testing\n\n")
            for item in data:
                f.write(f"{item}\n")
        
        files_created += 1
        print(f"  ✅ Created: {filename}")
    
    print(f"  📊 Total: {files_created} token files created")

def main():
    print("🔧 Token Data Generator")
    print("=" * 70)
    
    # Find all sample directories
    samples_dir = Path('samples')
    if not samples_dir.exists():
        print("❌ samples/ directory not found!")
        return
    
    sample_dirs = sorted(samples_dir.glob('sample*'))
    
    if not sample_dirs:
        print("❌ No sample directories found!")
        return
    
    print(f"Found {len(sample_dirs)} sample directories\n")
    
    # Process each sample
    for sample_dir in sample_dirs:
        if sample_dir.is_dir():
            generate_token_files(sample_dir)
    
    print("\n" + "=" * 70)
    print("✅ Token data generation complete!")
    print("\nGenerated files can be used for:")
    print("  • Testing individual tokens")
    print("  • Creating test inputs")
    print("  • Validating lexer rules")
    print("  • Documentation examples")

if __name__ == '__main__':
    main()
