#!/usr/bin/env python3
"""
visualize_tree.py
------------------
Command-line parse-tree visualizer used by `make run`.

This script reads parse-tree textual output produced by the compiler
executable (it looks for the marker `=== Parse Tree ===` by default) and
renders a colorized, human-readable representation in the terminal. It
supports three styles (fancy, simple, compact) and a statistics mode.

Important functions:
- `parse_tree_output` - converts indented text lines into internal nodes
- `visualize_tree_*` - the presentation functions for different styles
- `show_statistics` - computes and prints tree statistics
"""

import sys
import re
from typing import List, Tuple, Optional


# ANSI color codes
class Colors:
    # Node type colors
    NONTERMINAL = '\033[1;36m'      # Cyan bold for grammar rules
    TERMINAL = '\033[1;33m'         # Yellow bold for tokens
    VALUE = '\033[0;32m'            # Green for token values
    
    # Special detection colors
    EMAIL = '\033[1;35m'            # Magenta for emails
    PHONE = '\033[1;34m'            # Blue for phones
    URL = '\033[1;95m'              # Bright magenta for URLs
    CURRENCY = '\033[1;92m'         # Bright green for currency
    DETECTED = '\033[1;91m'         # Bright red for detected patterns
    
    # Structure colors
    PIPE = '\033[0;90m'             # Gray for tree structure
    ARROW = '\033[0;93m'            # Yellow for arrows
    
    # Text colors
    HEADER = '\033[1;97m'           # Bright white
    RESET = '\033[0m'               # Reset
    BOLD = '\033[1m'
    DIM = '\033[2m'

class TreeNode:
    def __init__(self, node_type: str, value: Optional[str] = None, indent_level: int = 0):
        self.node_type = node_type
        self.value = value
        self.indent_level = indent_level
        self.children = []
        
    def __repr__(self):
        if self.value:
            return f"{self.node_type}: {self.value}"
        return self.node_type

def parse_tree_output(lines: List[str]) -> List[TreeNode]:
    """Parse a list of lines into TreeNode objects.

    Input format expected is the same as `print_ast()` output from `ast.c`:
    each line is either `Nonterminal` or `TOKEN: value` and indentation (spaces)
    denotes tree nesting. This function is intentionally tolerant of small
    formatting differences but assumes a consistent indent step.
    """
    nodes = []
    
    for line in lines:
        if not line.strip():
            continue
            
        # Calculate indent level
        indent = len(line) - len(line.lstrip())
        content = line.strip()
        
        # Check if it has a value (contains :)
        if ': ' in content:
            parts = content.split(': ', 1)
            node_type = parts[0]
            value = parts[1]
            nodes.append(TreeNode(node_type, value, indent))
        else:
            nodes.append(TreeNode(content, None, indent))
    
    return nodes

def get_node_color(node: TreeNode) -> str:
    """Return an ANSI color code string for a given node.

    The decision is based on heuristics:
    - If the node type mentions `email`, `phone`, `url`, or `currency` we use
      specialized colors.
    - If node has a `.value` it is treated as a terminal/token.
    - Otherwise it's considered a non-terminal (grammar rule).
    """

    node_type_lower = node.node_type.lower()

    # Special detected patterns
    if 'detected' in node_type_lower or 'found' in node_type_lower:
        return Colors.DETECTED

    # Pattern-specific colors
    if 'email' in node_type_lower:
        return Colors.EMAIL
    elif 'phone' in node_type_lower:
        return Colors.PHONE
    elif 'url' in node_type_lower or 'website' in node_type_lower:
        return Colors.URL
    elif 'currency' in node_type_lower or 'dollar' in node_type_lower:
        return Colors.CURRENCY

    # Terminal vs Non-terminal
    if node.value:
        # Tokens (terminals)
        if node.node_type.isupper() or '_' in node.node_type.upper():
            return Colors.TERMINAL
        else:
            return Colors.NONTERMINAL
    else:
        # Grammar rules (non-terminals)
        return Colors.NONTERMINAL

def get_tree_chars(is_last: bool, has_children: bool) -> Tuple[str, str]:
    """Get tree structure characters"""
    if is_last:
        return '└──', '   '
    else:
        return '├──', '│  '

def visualize_tree_simple(nodes: List[TreeNode]):
    """Simple colorful visualization"""
    print(f"\n{Colors.HEADER}{'='*70}{Colors.RESET}")
    print(f"{Colors.HEADER}{Colors.BOLD}  Parse Tree Visualization{Colors.RESET}")
    print(f"{Colors.HEADER}{'='*70}{Colors.RESET}\n")
    
    for i, node in enumerate(nodes):
        # Calculate tree structure
        indent_str = '  ' * (node.indent_level // 2)
        
        # Get color
        color = get_node_color(node)
        
        # Build the line
        if node.value:
            # Terminal with value
            line = f"{Colors.PIPE}{indent_str}{color}{node.node_type}{Colors.RESET} {Colors.ARROW}→{Colors.RESET} {Colors.VALUE}{node.value}{Colors.RESET}"
        else:
            # Non-terminal
            line = f"{Colors.PIPE}{indent_str}{color}{node.node_type}{Colors.RESET}"
        
        print(line)
    
    print(f"\n{Colors.HEADER}{'='*70}{Colors.RESET}\n")

def visualize_tree_fancy(nodes: List[TreeNode]):
    """Fancy tree visualization with box drawing characters"""
    print(f"\n{Colors.HEADER}╔{'═'*68}╗{Colors.RESET}")
    print(f"{Colors.HEADER}║{Colors.BOLD}  Parse Tree Visualization{' '*43}║{Colors.RESET}")
    print(f"{Colors.HEADER}╚{'═'*68}╝{Colors.RESET}\n")
    
    # Build tree structure
    for i, node in enumerate(nodes):
        # Calculate tree structure
        base_indent = '  ' * (node.indent_level // 2)
        
        # Determine if this is the last at its level (simple heuristic)
        is_last = (i == len(nodes) - 1) or (i + 1 < len(nodes) and nodes[i + 1].indent_level <= node.indent_level)
        
        # Get tree characters
        if node.indent_level > 0:
            if is_last:
                branch = '└─ '
            else:
                branch = '├─ '
        else:
            branch = ''
        
        # Get color
        color = get_node_color(node)
        
        # Build the line
        if node.value:
            # Terminal with value
            node_text = f"{color}{node.node_type}{Colors.RESET}"
            value_text = f"{Colors.VALUE}{node.value}{Colors.RESET}"
            line = f"{Colors.PIPE}{base_indent}{Colors.PIPE}{branch}{node_text} {Colors.ARROW}➜{Colors.RESET} {value_text}"
        else:
            # Non-terminal
            node_text = f"{color}{node.node_type}{Colors.RESET}"
            line = f"{Colors.PIPE}{base_indent}{Colors.PIPE}{branch}{node_text}"
        
        print(line)
    
    print(f"\n{Colors.DIM}{'─'*70}{Colors.RESET}\n")

def visualize_tree_compact(nodes: List[TreeNode]):
    """Compact visualization showing only important nodes"""
    print(f"\n{Colors.HEADER}┌{'─'*68}┐{Colors.RESET}")
    print(f"{Colors.HEADER}│{Colors.BOLD}  Parse Tree (Compact View){' '*42}│{Colors.RESET}")
    print(f"{Colors.HEADER}└{'─'*68}┘{Colors.RESET}\n")
    
    # Filter out some common non-informative nodes
    filtered_nodes = []
    for node in nodes:
        # Keep detected patterns, terminals with values, and top-level nodes
        if (node.value or 
            'detected' in node.node_type.lower() or 
            'found' in node.node_type.lower() or
            node.indent_level == 0):
            filtered_nodes.append(node)
    
    for node in filtered_nodes:
        indent_str = '  ' * (node.indent_level // 2)
        color = get_node_color(node)
        
        if node.value:
            print(f"{indent_str}{color}▸ {node.node_type}{Colors.RESET}: {Colors.VALUE}{node.value}{Colors.RESET}")
        else:
            print(f"{indent_str}{color}▪ {node.node_type}{Colors.RESET}")
    
    print(f"\n{Colors.DIM}{'─'*70}{Colors.RESET}\n")

def show_statistics(nodes: List[TreeNode]):
    """Show statistics about the parse tree"""
    total_nodes = len(nodes)
    terminals = sum(1 for n in nodes if n.value)
    nonterminals = total_nodes - terminals
    
    # Count detected patterns
    detected = {}
    for node in nodes:
        if 'detected' in node.node_type.lower() or 'found' in node.node_type.lower():
            key = node.node_type
            detected[key] = detected.get(key, 0) + 1
    
    print(f"{Colors.HEADER}╔{'═'*68}╗{Colors.RESET}")
    print(f"{Colors.HEADER}║{Colors.BOLD}  Statistics{' '*57}║{Colors.RESET}")
    print(f"{Colors.HEADER}╠{'═'*68}╣{Colors.RESET}")
    print(f"{Colors.HEADER}║{Colors.RESET}  Total Nodes: {Colors.BOLD}{total_nodes}{Colors.RESET}{' ' * (56 - len(str(total_nodes)))}║")
    print(f"{Colors.HEADER}║{Colors.RESET}  Terminals: {Colors.BOLD}{terminals}{Colors.RESET}{' ' * (58 - len(str(terminals)))}║")
    print(f"{Colors.HEADER}║{Colors.RESET}  Non-terminals: {Colors.BOLD}{nonterminals}{Colors.RESET}{' ' * (54 - len(str(nonterminals)))}║")
    
    if detected:
        print(f"{Colors.HEADER}║{Colors.RESET}  {Colors.BOLD}Detected Patterns:{Colors.RESET}{' ' * 49}║")
        for pattern, count in sorted(detected.items()):
            pattern_clean = pattern.replace('_detected', '').replace('_found', '')
            spacing = 64 - len(pattern_clean) - len(str(count))
            print(f"{Colors.HEADER}║{Colors.RESET}    • {Colors.DETECTED}{pattern_clean}{Colors.RESET}: {Colors.BOLD}{count}{Colors.RESET}{' ' * spacing}║")
    
    print(f"{Colors.HEADER}╚{'═'*68}╝{Colors.RESET}\n")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Visualize parse tree output in color')
    parser.add_argument('input', nargs='?', type=str, 
                       help='Input file (default: read from stdin)')
    parser.add_argument('-s', '--style', choices=['simple', 'fancy', 'compact'], 
                       default='fancy',
                       help='Visualization style (default: fancy)')
    parser.add_argument('--stats', action='store_true',
                       help='Show statistics')
    parser.add_argument('--no-color', action='store_true',
                       help='Disable colors')
    
    args = parser.parse_args()
    
    # Read input
    if args.input:
        with open(args.input, 'r') as f:
            lines = f.readlines()
    else:
        lines = sys.stdin.readlines()
    
    # Disable colors if requested
    if args.no_color:
        for attr in dir(Colors):
            if not attr.startswith('_'):
                setattr(Colors, attr, '')
    
    # Find parse tree section
    tree_lines = []
    in_tree = False
    
    for line in lines:
        if '=== Parse Tree ===' in line:
            in_tree = True
            continue
        if in_tree:
            tree_lines.append(line.rstrip('\n'))
    
    # If no tree section found, assume all input is tree
    if not tree_lines:
        tree_lines = [line.rstrip('\n') for line in lines if line.strip()]
    
    # Parse the tree
    nodes = parse_tree_output(tree_lines)
    
    if not nodes:
        print(f"{Colors.HEADER}No parse tree found in input{Colors.RESET}")
        return 1
    
    # Visualize based on style
    if args.style == 'simple':
        visualize_tree_simple(nodes)
    elif args.style == 'fancy':
        visualize_tree_fancy(nodes)
    elif args.style == 'compact':
        visualize_tree_compact(nodes)
    
    # Show statistics if requested
    if args.stats:
        show_statistics(nodes)
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
