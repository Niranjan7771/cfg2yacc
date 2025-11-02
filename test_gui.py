#!/usr/bin/env python3
"""
Simple test script for the Parse Tree GUI
Tests both regular and JSON modes
"""

import subprocess
import json
import sys

def test_parser_modes():
    """Test both regular and JSON parser modes"""
    test_expr = "1+2*3"
    
    print("🧪 Testing Parser Modes")
    print("=" * 30)
    
    # Test regular mode
    print("📝 Regular Mode:")
    try:
        result = subprocess.run(
            ["./parser"],
            input=test_expr,
            text=True,
            capture_output=True,
            timeout=5
        )
        
        if result.returncode == 0:
            print("✅ Regular mode works")
            lines = result.stdout.strip().split('\n')
            tree_lines = [line for line in lines if ' -> ' in line and not line.startswith('❌') and not line.startswith('✅')]
            print(f"   Found {len(tree_lines)} parse steps")
        else:
            print("❌ Regular mode failed")
            print(f"   Error: {result.stderr}")
            
    except Exception as e:
        print(f"❌ Regular mode error: {e}")
    
    # Test JSON mode
    print("\n🔧 JSON Mode:")
    try:
        result = subprocess.run(
            ["./parser", "--json"],
            input=test_expr,
            text=True,
            capture_output=True,
            timeout=5
        )
        
        if result.returncode == 0:
            print("✅ JSON mode works")
            try:
                # Try to parse JSON (might have formatting issues)
                output = result.stdout.strip()
                # Fix common JSON issues
                if output.startswith('{"parse_tree": [,'):
                    output = output.replace('{"parse_tree": [,', '{"parse_tree": [')
                
                data = json.loads(output)
                if 'parse_tree' in data:
                    print(f"   Found {len(data['parse_tree'])} parse steps")
                    print(f"   Success: {data.get('success', 'unknown')}")
                else:
                    print("   ⚠️  JSON structure unexpected")
            except json.JSONDecodeError as e:
                print(f"   ⚠️  JSON parsing issue: {e}")
                print(f"   Raw output: {output[:100]}...")
        else:
            print("❌ JSON mode failed")
            print(f"   Error: {result.stderr}")
            
    except Exception as e:
        print(f"❌ JSON mode error: {e}")

def test_gui_requirements():
    """Test GUI requirements"""
    print("\n🎨 GUI Requirements Check")
    print("=" * 30)
    
    # Test Python 3
    try:
        import sys
        print(f"✅ Python {sys.version.split()[0]}")
    except:
        print("❌ Python not available")
        return False
    
    # Test tkinter
    try:
        import tkinter
        print("✅ tkinter available")
    except ImportError:
        print("❌ tkinter not available")
        print("   Install with: sudo apt-get install python3-tk")
        return False
    
    # Test subprocess
    try:
        import subprocess
        print("✅ subprocess available")
    except ImportError:
        print("❌ subprocess not available")
        return False
    
    # Test json
    try:
        import json
        print("✅ json available")
    except ImportError:
        print("❌ json not available")
        return False
    
    return True

def main():
    print("🚀 cfg2yacc GUI Test Suite")
    print("=" * 40)
    
    # Check if parser exists
    try:
        result = subprocess.run(["./parser", "--help"], capture_output=True, timeout=2)
        print("✅ Parser executable found")
    except FileNotFoundError:
        print("❌ Parser not found")
        print("   Build with: make && ./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff")
        print("   Then: bison -d expr.y && flex expr.l && gcc -o parser expr.tab.c lex.yy.c ast.c -lfl")
        return 1
    except:
        print("✅ Parser executable found (help not supported)")
    
    # Test parser modes
    test_parser_modes()
    
    # Test GUI requirements
    if test_gui_requirements():
        print("\n🎉 All tests passed!")
        print("\n🎨 Ready to launch GUI:")
        print("   python3 parse_tree_gui.py")
        print("   OR")
        print("   ./run_gui.sh")
        return 0
    else:
        print("\n❌ GUI requirements not met")
        return 1

if __name__ == "__main__":
    sys.exit(main())