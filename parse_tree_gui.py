#!/usr/bin/env python3
"""
Parse Tree GUI Visualizer for cfg2yacc
Displays parse trees in a proper visual tree structure
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import subprocess
import re
import json
import math
from typing import List, Dict, Tuple, Optional
import os

class ParseTreeNode:
    def __init__(self, label: str, rule: str = "", is_terminal: bool = False):
        self.label = label  # The symbol name (e.g., "expr", "NUMBER", "+")
        self.rule = rule    # The full rule (e.g., "expr -> term '+' term")
        self.is_terminal = is_terminal
        self.children = []
        self.parent = None
        self.x = 0
        self.y = 0
        self.width = 0
        self.height = 40
        self.level = 0
        
    def add_child(self, child):
        child.parent = self
        child.level = self.level + 1
        self.children.append(child)
        
    def is_leaf(self):
        return len(self.children) == 0
        
    def get_subtree_width(self):
        if not self.children:
            return max(100, len(self.label) * 10 + 20)
        return sum(child.get_subtree_width() for child in self.children) + (len(self.children) - 1) * 20

class ParseTreeGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("cfg2yacc Parse Tree Visualizer")
        self.root.geometry("1200x800")
        
        # Variables
        self.parser_path = "./parser"
        self.parse_tree_data = []
        self.tree_root = None
        
        self.setup_ui()
        
    def setup_ui(self):
        # Main frame
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Top frame for input and controls
        top_frame = ttk.Frame(main_frame)
        top_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Input section
        input_frame = ttk.LabelFrame(top_frame, text="Input Expression", padding=10)
        input_frame.pack(fill=tk.X, pady=(0, 5))
        
        self.input_var = tk.StringVar(value="1+2*3")
        input_entry = ttk.Entry(input_frame, textvariable=self.input_var, font=("Consolas", 12))
        input_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0, 10))
        input_entry.bind('<Return>', lambda e: self.parse_expression())
        
        parse_btn = ttk.Button(input_frame, text="Parse", command=self.parse_expression)
        parse_btn.pack(side=tk.RIGHT)
        
        # Parser selection
        parser_frame = ttk.Frame(top_frame)
        parser_frame.pack(fill=tk.X, pady=(5, 0))
        
        ttk.Label(parser_frame, text="Parser:").pack(side=tk.LEFT)
        self.parser_var = tk.StringVar(value="./parser")
        parser_entry = ttk.Entry(parser_frame, textvariable=self.parser_var, width=30)
        parser_entry.pack(side=tk.LEFT, padx=(5, 5))
        
        browse_btn = ttk.Button(parser_frame, text="Browse", command=self.browse_parser)
        browse_btn.pack(side=tk.LEFT)
        
        # Example buttons
        examples_frame = ttk.Frame(parser_frame)
        examples_frame.pack(side=tk.RIGHT)
        
        examples = [
            ("Number", "42"),
            ("Add", "1+2"),
            ("Multiply", "2*3"), 
            ("Precedence", "1+2*3"),
            ("Parentheses", "(1+2)*3"),
            ("Complex", "((1+2)*3)+4"),
            ("Long", "1+2+3+4")
        ]
        
        for name, expr in examples:
            btn = ttk.Button(examples_frame, text=name, 
                           command=lambda e=expr: self.set_input(e))
            btn.pack(side=tk.LEFT, padx=2)
        
        # Notebook for different views
        notebook = ttk.Notebook(main_frame)
        notebook.pack(fill=tk.BOTH, expand=True)
        
        # Tree visualization tab
        tree_frame = ttk.Frame(notebook)
        notebook.add(tree_frame, text="🌳 Parse Tree")
        
        # Canvas for tree drawing with scrollbars
        canvas_frame = ttk.Frame(tree_frame)
        canvas_frame.pack(fill=tk.BOTH, expand=True)
        
        # Create canvas with scrollbars
        self.canvas = tk.Canvas(canvas_frame, bg="white", scrollregion=(0, 0, 2000, 1000))
        
        v_scrollbar = ttk.Scrollbar(canvas_frame, orient=tk.VERTICAL, command=self.canvas.yview)
        h_scrollbar = ttk.Scrollbar(canvas_frame, orient=tk.HORIZONTAL, command=self.canvas.xview)
        
        self.canvas.configure(yscrollcommand=v_scrollbar.set, xscrollcommand=h_scrollbar.set)
        
        # Pack scrollbars and canvas
        v_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        h_scrollbar.pack(side=tk.BOTTOM, fill=tk.X)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Bind mouse events for interactivity
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.bind("<B1-Motion>", self.on_canvas_drag)
        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)
        
        # Variables for dragging
        self.drag_start_x = 0
        self.drag_start_y = 0
        
        # Raw output tab
        output_frame = ttk.Frame(notebook)
        notebook.add(output_frame, text="📝 Raw Output")
        
        self.output_text = scrolledtext.ScrolledText(output_frame, font=("Consolas", 10))
        self.output_text.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Status bar
        self.status_var = tk.StringVar(value="Ready")
        status_bar = ttk.Label(main_frame, textvariable=self.status_var, relief=tk.SUNKEN)
        status_bar.pack(fill=tk.X, pady=(5, 0))
        
    def set_input(self, expression: str):
        self.input_var.set(expression)
        self.parse_expression()
        
    def browse_parser(self):
        filename = filedialog.askopenfilename(
            title="Select Parser Executable",
            filetypes=[("Executable files", "*"), ("All files", "*.*")]
        )
        if filename:
            self.parser_var.set(filename)
            
    def parse_expression(self):
        expression = self.input_var.get().strip()
        parser_path = self.parser_var.get().strip()
        
        if not expression:
            messagebox.showwarning("Warning", "Please enter an expression to parse")
            return
            
        if not os.path.exists(parser_path):
            messagebox.showerror("Error", f"Parser not found: {parser_path}")
            return
            
        self.status_var.set("Parsing...")
        self.root.update()
        
        try:
            # Try JSON mode first
            result = subprocess.run(
                [parser_path, "--json"],
                input=expression,
                text=True,
                capture_output=True,
                timeout=10
            )
            
            # If JSON mode fails, try regular mode
            if result.returncode != 0 and "unrecognized" in result.stderr:
                result = subprocess.run(
                    [parser_path],
                    input=expression,
                    text=True,
                    capture_output=True,
                    timeout=10
                )
            
            # Display raw output
            output = result.stdout + result.stderr
            self.output_text.delete(1.0, tk.END)
            self.output_text.insert(1.0, output)
            
            if result.returncode == 0:
                # Parse successful
                self.parse_tree_data = self.extract_parse_tree(result.stdout)
                self.draw_parse_tree()
                self.status_var.set(f"✅ Parsed successfully - {len(self.parse_tree_data)} rules applied")
            else:
                # Parse failed
                self.canvas.delete("all")
                self.canvas.create_text(100, 50, text="❌ Parse Failed", 
                                      font=("Arial", 16, "bold"), fill="red", anchor="nw")
                error_lines = result.stderr.split('\n')
                for i, line in enumerate(error_lines[:5]):  # Show first 5 error lines
                    self.canvas.create_text(100, 80 + i*20, text=line, 
                                          font=("Consolas", 10), fill="red", anchor="nw")
                self.status_var.set("❌ Parse failed")
                
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Parser timed out")
            self.status_var.set("❌ Timeout")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to run parser: {e}")
            self.status_var.set("❌ Error")
            
    def extract_parse_tree(self, output: str) -> List[str]:
        """Extract parse tree rules from parser output"""
        lines = output.split('\n')
        tree_lines = []
        
        # Try to parse JSON output first
        try:
            if output.strip().startswith('{'):
                import json
                data = json.loads(output.strip())
                if 'parse_tree' in data:
                    for step in data['parse_tree']:
                        if 'rule' in step:
                            tree_lines.append(step['rule'])
                    return tree_lines
        except:
            pass  # Fall back to text parsing
        
        # Parse regular text output
        for line in lines:
            line = line.strip()
            if ' -> ' in line and not line.startswith('❌') and not line.startswith('✅'):
                # Remove leading spaces and extract the rule
                rule = line.strip()
                if rule:
                    tree_lines.append(rule)
                    
        return tree_lines
        
    def build_tree_structure(self) -> Optional[ParseTreeNode]:
        """Build a proper tree structure from the parse rules"""
        if not self.parse_tree_data:
            return None
            
        # Build tree from bottom-up using a stack-based approach
        # This simulates the parser's reduction process
        
        # Find the start symbol (usually the last rule applied)
        start_rule = None
        for rule in reversed(self.parse_tree_data):
            if ' -> ' in rule:
                lhs = rule.split(' -> ')[0].strip()
                # Look for a rule that's likely the start symbol
                if any(name in lhs.lower() for name in ['expr', 'program', 'start']):
                    start_rule = rule
                    break
        
        if not start_rule:
            start_rule = self.parse_tree_data[-1] if self.parse_tree_data else "start -> ε"
            
        # Create root from start rule
        if ' -> ' in start_rule:
            lhs, rhs = start_rule.split(' -> ', 1)
            root = ParseTreeNode(lhs.strip(), start_rule)
            
            # Build children based on RHS
            if rhs.strip() == 'ε':
                # Empty production
                epsilon = ParseTreeNode('ε', '', True)
                root.add_child(epsilon)
            else:
                # Parse RHS symbols
                symbols = self.parse_rhs_symbols(rhs)
                for symbol in symbols:
                    child = self.build_subtree(symbol, self.parse_tree_data)
                    root.add_child(child)
                    
            return root
        else:
            # Fallback: create simple tree
            root = ParseTreeNode("Parse Tree")
            for rule in self.parse_tree_data[:5]:  # Limit to first 5 for simplicity
                node = ParseTreeNode(rule.split(' -> ')[0] if ' -> ' in rule else rule, rule)
                root.add_child(node)
            return root
            
    def parse_rhs_symbols(self, rhs: str) -> List[str]:
        """Parse RHS of a rule into individual symbols"""
        symbols = []
        current = ""
        in_quotes = False
        
        for char in rhs:
            if char == "'" and not in_quotes:
                in_quotes = True
                current += char
            elif char == "'" and in_quotes:
                in_quotes = False
                current += char
                symbols.append(current.strip())
                current = ""
            elif char == ' ' and not in_quotes:
                if current.strip():
                    symbols.append(current.strip())
                current = ""
            else:
                current += char
                
        if current.strip():
            symbols.append(current.strip())
            
        return symbols
        
    def build_subtree(self, symbol: str, rules: List[str]) -> ParseTreeNode:
        """Build subtree for a given symbol"""
        # Check if it's a terminal (quoted or all caps)
        if (symbol.startswith("'") and symbol.endswith("'")) or symbol.isupper() or symbol in ['ε']:
            return ParseTreeNode(symbol, "", True)
            
        # Find rules for this nonterminal
        matching_rules = [rule for rule in rules if rule.startswith(symbol + ' ->')]
        
        if matching_rules:
            # Use the first matching rule
            rule = matching_rules[0]
            lhs, rhs = rule.split(' -> ', 1)
            node = ParseTreeNode(symbol, rule)
            
            if rhs.strip() == 'ε':
                epsilon = ParseTreeNode('ε', '', True)
                node.add_child(epsilon)
            else:
                symbols = self.parse_rhs_symbols(rhs)
                for child_symbol in symbols:
                    child = self.build_subtree(child_symbol, rules)
                    node.add_child(child)
                    
            return node
        else:
            # No rule found, treat as terminal
            return ParseTreeNode(symbol, "", True)
        
    def calculate_tree_layout(self, node: ParseTreeNode, x: int = 0, y: int = 50):
        """Calculate positions for tree nodes using proper tree layout"""
        if not node:
            return
            
        # Calculate node dimensions
        node.width = max(80, len(node.label) * 8 + 20)
        node.height = 40
        
        if not node.children:
            # Leaf node
            node.x = x
            node.y = y
            return node.width
            
        # Calculate children positions first
        child_x = x
        total_width = 0
        
        for child in node.children:
            child_width = self.calculate_tree_layout(child, child_x, y + 80)
            child_x += child_width + 30  # 30px spacing between siblings
            total_width += child_width + 30
            
        total_width -= 30  # Remove last spacing
        
        # Center parent over children
        if node.children:
            first_child = node.children[0]
            last_child = node.children[-1]
            node.x = (first_child.x + last_child.x + last_child.width) // 2 - node.width // 2
        else:
            node.x = x
            
        node.y = y
        return max(total_width, node.width)
            
    def draw_parse_tree(self):
        """Draw the parse tree as a proper visual tree structure"""
        self.canvas.delete("all")
        
        if not self.parse_tree_data:
            self.canvas.create_text(100, 50, text="No parse tree data", 
                                  font=("Arial", 12), anchor="nw")
            return
            
        # Build tree structure
        tree_root = self.build_tree_structure()
        if not tree_root:
            self.canvas.create_text(100, 50, text="Could not build tree structure", 
                                  font=("Arial", 12), anchor="nw")
            return
            
        # Calculate layout
        tree_width = self.calculate_tree_layout(tree_root, 50, 50)
        
        # Draw title
        self.canvas.create_text(20, 20, text="🌳 Parse Tree Structure", 
                              font=("Arial", 16, "bold"), anchor="nw", fill="darkgreen")
        
        # Draw the tree
        self.draw_tree_node(tree_root)
        
        # Update scroll region
        max_x = tree_width + 100
        max_y = self.get_tree_height(tree_root) + 100
        self.canvas.configure(scrollregion=(0, 0, max_x, max_y))
        
        # Draw legend
        self.draw_legend(max_x - 200, 30)
        
    def draw_tree_node(self, node: ParseTreeNode):
        """Draw a single tree node and its connections"""
        if not node:
            return
            
        # Choose colors based on node type
        if node.is_terminal:
            if node.label == 'ε':
                fill_color = "#f0f0f0"
                text_color = "gray"
                border_color = "lightgray"
            elif node.label.startswith("'") or node.label.isupper():
                fill_color = "#e6f3ff"  # Light blue for terminals
                text_color = "darkblue"
                border_color = "blue"
            else:
                fill_color = "#fff2e6"  # Light orange for tokens
                text_color = "darkorange"
                border_color = "orange"
        else:
            fill_color = "#e6ffe6"  # Light green for non-terminals
            text_color = "darkgreen"
            border_color = "green"
            
        # Draw node rectangle
        x1, y1 = node.x, node.y
        x2, y2 = node.x + node.width, node.y + node.height
        
        # Draw shadow for depth
        self.canvas.create_rectangle(x1+2, y1+2, x2+2, y2+2, 
                                   fill="lightgray", outline="lightgray")
        
        # Draw main rectangle
        self.canvas.create_rectangle(x1, y1, x2, y2, 
                                   fill=fill_color, outline=border_color, width=2)
        
        # Draw node label
        text_x = node.x + node.width // 2
        text_y = node.y + node.height // 2
        self.canvas.create_text(text_x, text_y, text=node.label, 
                              font=("Arial", 10, "bold"), fill=text_color, anchor="center")
        
        # Draw connections to children
        if node.children:
            parent_bottom_x = node.x + node.width // 2
            parent_bottom_y = node.y + node.height
            
            for child in node.children:
                child_top_x = child.x + child.width // 2
                child_top_y = child.y
                
                # Draw line from parent to child
                self.canvas.create_line(parent_bottom_x, parent_bottom_y,
                                      child_top_x, child_top_y,
                                      fill="darkgray", width=2)
                
                # Draw small circle at connection points
                self.canvas.create_oval(parent_bottom_x-3, parent_bottom_y-3,
                                      parent_bottom_x+3, parent_bottom_y+3,
                                      fill="darkgray", outline="darkgray")
                
                # Recursively draw children
                self.draw_tree_node(child)
                
    def get_tree_height(self, node: ParseTreeNode) -> int:
        """Calculate the total height of the tree"""
        if not node:
            return 0
            
        if not node.children:
            return node.y + node.height
            
        max_child_y = max(self.get_tree_height(child) for child in node.children)
        return max_child_y
        
    def draw_legend(self, x: int, y: int):
        """Draw a legend explaining the color coding"""
        legend_items = [
            ("Non-terminals", "#e6ffe6", "darkgreen"),
            ("Terminals", "#e6f3ff", "darkblue"), 
            ("Tokens", "#fff2e6", "darkorange"),
            ("Empty (ε)", "#f0f0f0", "gray")
        ]
        
        self.canvas.create_text(x, y, text="Legend:", 
                              font=("Arial", 12, "bold"), anchor="nw")
        
        for i, (label, fill, text_color) in enumerate(legend_items):
            item_y = y + 25 + i * 25
            
            # Draw color box
            self.canvas.create_rectangle(x, item_y, x+15, item_y+15,
                                       fill=fill, outline=text_color)
            
            # Draw label
            self.canvas.create_text(x+20, item_y+7, text=label,
                                  font=("Arial", 9), anchor="w", fill=text_color)
                                  
    def on_canvas_click(self, event):
        """Handle canvas click events"""
        self.drag_start_x = event.x
        self.drag_start_y = event.y
        
    def on_canvas_drag(self, event):
        """Handle canvas drag events for panning"""
        dx = event.x - self.drag_start_x
        dy = event.y - self.drag_start_y
        self.canvas.scan_dragto(dx, dy, gain=1)
        
    def on_mouse_wheel(self, event):
        """Handle mouse wheel for zooming"""
        if event.delta > 0:
            self.canvas.scale("all", event.x, event.y, 1.1, 1.1)
        else:
            self.canvas.scale("all", event.x, event.y, 0.9, 0.9)

def main():
    # Check if parser exists
    if not os.path.exists("./parser"):
        print("Parser not found. Please build the parser first:")
        print("./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff")
        print("bison -d expr.y && flex expr.l && gcc -o parser expr.tab.c lex.yy.c ast.c -lfl")
        return
        
    root = tk.Tk()
    app = ParseTreeGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()