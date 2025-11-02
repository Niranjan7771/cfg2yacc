#!/usr/bin/env python3
"""
Visual Parse Tree GUI for cfg2yacc
Creates proper tree visualizations from parser output
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import subprocess
import json
import re
from typing import List, Dict, Tuple, Optional
import os

class ParseTreeNode:
    def __init__(self, label: str, is_terminal: bool = False, rule: str = ""):
        self.label = label
        self.is_terminal = is_terminal
        self.rule = rule  # Full rule for tooltip
        self.children = []
        self.parent = None
        self.x = 0
        self.y = 0
        self.width = 0
        self.height = 40
        self.level = 0
        
    def add_child(self, child):
        if child is None:
            return
        child.parent = self
        child.update_level(self.level + 1)
        self.children.append(child)

    def update_level(self, level: int):
        self.level = level
        for grandchild in self.children:
            grandchild.update_level(level + 1)
        
    def get_subtree_width(self):
        if not self.children:
            return max(80, len(self.label) * 8 + 20)
        return sum(child.get_subtree_width() for child in self.children) + (len(self.children) - 1) * 30

class VisualParseTreeGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("🌳 cfg2yacc Visual Parse Tree")
        self.root.geometry("1400x900")
        
        # Variables
        self.parser_path = "./parser"
        self.parse_rules = []
        self.tree_root = None
        
        self.setup_ui()
        
    def setup_ui(self):
        # Main container
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Top control panel
        self.setup_control_panel(main_frame)
        
        # Main content area with notebook
        notebook = ttk.Notebook(main_frame)
        notebook.pack(fill=tk.BOTH, expand=True, pady=(10, 0))
        
        # Visual tree tab
        self.setup_visual_tab(notebook)
        
        # Text output tab
        self.setup_text_tab(notebook)
        
        # Status bar
        self.status_var = tk.StringVar(value="Ready - Enter an expression to parse")
        status_bar = ttk.Label(main_frame, textvariable=self.status_var, 
                              relief=tk.SUNKEN, padding=5)
        status_bar.pack(fill=tk.X, pady=(5, 0))
        
    def setup_control_panel(self, parent):
        """Setup the top control panel"""
        control_frame = ttk.LabelFrame(parent, text="Parse Expression", padding=10)
        control_frame.pack(fill=tk.X, pady=(0, 5))
        
        # Input row
        input_row = ttk.Frame(control_frame)
        input_row.pack(fill=tk.X, pady=(0, 5))
        
        ttk.Label(input_row, text="Expression:").pack(side=tk.LEFT)
        
        self.input_var = tk.StringVar(value="1+2*3")
        self.input_entry = ttk.Entry(input_row, textvariable=self.input_var, 
                                    font=("Consolas", 12), width=30)
        self.input_entry.pack(side=tk.LEFT, padx=(5, 10), fill=tk.X, expand=True)
        self.input_entry.bind('<Return>', lambda e: self.parse_expression())
        
        ttk.Button(input_row, text="🔍 Parse", command=self.parse_expression).pack(side=tk.LEFT, padx=2)
        ttk.Button(input_row, text="🗑️ Clear", command=self.clear_tree).pack(side=tk.LEFT, padx=2)
        
        # Examples row
        examples_row = ttk.Frame(control_frame)
        examples_row.pack(fill=tk.X)
        
        ttk.Label(examples_row, text="Examples:").pack(side=tk.LEFT)
        
        examples = [
            ("42", "42"),
            ("1+2", "1+2"),
            ("2*3", "2*3"),
            ("1+2*3", "1+2*3"),
            ("(1+2)*3", "(1+2)*3"),
            ("((1+2)*3)+4", "((1+2)*3)+4"),
            ("Error: 1++2", "1++2")
        ]
        
        for name, expr in examples:
            btn = ttk.Button(examples_row, text=name, 
                           command=lambda e=expr: self.set_and_parse(e))
            btn.pack(side=tk.LEFT, padx=2)
            
        # Parser settings
        settings_row = ttk.Frame(control_frame)
        settings_row.pack(fill=tk.X, pady=(5, 0))
        
        ttk.Label(settings_row, text="Parser:").pack(side=tk.LEFT)
        self.parser_var = tk.StringVar(value="./parser")
        parser_entry = ttk.Entry(settings_row, textvariable=self.parser_var, width=20)
        parser_entry.pack(side=tk.LEFT, padx=(5, 5))
        ttk.Button(settings_row, text="Browse", command=self.browse_parser).pack(side=tk.LEFT)
        
    def setup_visual_tab(self, notebook):
        """Setup the visual tree tab"""
        visual_frame = ttk.Frame(notebook)
        notebook.add(visual_frame, text="🌳 Visual Tree")
        
        # Canvas with scrollbars
        canvas_frame = ttk.Frame(visual_frame)
        canvas_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.canvas = tk.Canvas(canvas_frame, bg="white", highlightthickness=0)
        
        # Scrollbars
        v_scroll = ttk.Scrollbar(canvas_frame, orient=tk.VERTICAL, command=self.canvas.yview)
        h_scroll = ttk.Scrollbar(canvas_frame, orient=tk.HORIZONTAL, command=self.canvas.xview)
        
        self.canvas.configure(yscrollcommand=v_scroll.set, xscrollcommand=h_scroll.set)
        
        # Pack elements
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # Bind events
        self.canvas.bind("<Button-1>", self.on_canvas_click)
        self.canvas.bind("<B1-Motion>", self.on_canvas_drag)
        self.canvas.bind("<MouseWheel>", self.on_mouse_wheel)
        self.canvas.bind("<Motion>", self.on_canvas_motion)
        
        # Drag variables
        self.drag_start_x = 0
        self.drag_start_y = 0
        self.tooltip_id = None
        
    def setup_text_tab(self, notebook):
        """Setup the text output tab"""
        text_frame = ttk.Frame(notebook)
        notebook.add(text_frame, text="📝 Text Output")
        
        self.text_output = scrolledtext.ScrolledText(text_frame, font=("Consolas", 10),
                                                    wrap=tk.WORD)
        self.text_output.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
    def set_and_parse(self, expression):
        """Set input and parse immediately"""
        self.input_var.set(expression)
        self.parse_expression()
        
    def browse_parser(self):
        """Browse for parser executable"""
        filename = filedialog.askopenfilename(
            title="Select Parser Executable",
            filetypes=[("Executable files", "*"), ("All files", "*.*")]
        )
        if filename:
            self.parser_var.set(filename)
            
    def clear_tree(self):
        """Clear the tree display"""
        self.canvas.delete("all")
        self.text_output.delete(1.0, tk.END)
        self.parse_rules = []
        self.tree_root = None
        self.status_var.set("Tree cleared")
        
    def parse_expression(self):
        """Parse the input expression"""
        expression = self.input_var.get().strip()
        parser_path = self.parser_var.get().strip()
        
        if not expression:
            messagebox.showwarning("Warning", "Please enter an expression")
            return
            
        if not os.path.exists(parser_path):
            messagebox.showerror("Error", f"Parser not found: {parser_path}")
            return
            
        self.status_var.set("Parsing...")
        self.root.update()
        
        try:
            # Run parser
            result = subprocess.run(
                [parser_path],
                input=expression,
                text=True,
                capture_output=True,
                timeout=10
            )
            
            # Show text output
            output = result.stdout + result.stderr
            self.text_output.delete(1.0, tk.END)
            self.text_output.insert(1.0, output)
            
            if result.returncode == 0:
                # Parse successful - extract rules and build tree
                self.parse_rules = self.extract_parse_rules(result.stdout)
                if self.parse_rules:
                    self.tree_root = self.build_visual_tree()
                    self.draw_visual_tree()
                    self.status_var.set(f"✅ Parsed successfully - {len(self.parse_rules)} steps")
                else:
                    self.canvas.delete("all")
                    self.canvas.create_text(50, 50, text="No parse rules found", 
                                          font=("Arial", 12), anchor="nw")
                    self.status_var.set("⚠️ No parse data")
            else:
                # Parse failed
                self.canvas.delete("all")
                self.canvas.create_text(50, 50, text="❌ Parse Failed", 
                                      font=("Arial", 16, "bold"), fill="red", anchor="nw")
                error_lines = result.stderr.split('\\n')[:3]
                for i, line in enumerate(error_lines):
                    if line.strip():
                        self.canvas.create_text(50, 80 + i*20, text=line.strip(), 
                                              font=("Arial", 10), fill="red", anchor="nw")
                self.status_var.set("❌ Parse failed")
                
        except subprocess.TimeoutExpired:
            messagebox.showerror("Error", "Parser timed out")
            self.status_var.set("❌ Timeout")
        except Exception as e:
            messagebox.showerror("Error", f"Error running parser: {e}")
            self.status_var.set("❌ Error")
            
    def extract_parse_rules(self, output: str) -> List[str]:
        """Extract parse rules from output"""
        rules = []
        lines = output.split('\\n')
        
        for line in lines:
            line = line.strip()
            if ' -> ' in line and not line.startswith('❌') and not line.startswith('✅'):
                rules.append(line)
                
        return rules
        
    def build_visual_tree(self) -> Optional[ParseTreeNode]:
        """Build a visual tree from parse rules"""
        if not self.parse_rules:
            return None

        parsed_rules = []
        nonterminals = set()

        for rule_line in self.parse_rules:
            parsed = self.parse_rule_line(rule_line)
            if not parsed:
                continue
            lhs, rhs = parsed
            parsed_rules.append((lhs, rhs, rule_line))
            nonterminals.add(lhs)

        if not parsed_rules:
            return None

        tree = self.build_tree_from_rules(parsed_rules, nonterminals)
        if tree:
            tree.update_level(0)
        return tree

    def parse_rule_line(self, rule_line: str) -> Optional[Tuple[str, List[str]]]:
        """Split a rule line into lhs and rhs symbols"""
        if "->" not in rule_line:
            return None

        lhs, rhs_str = rule_line.split("->", 1)
        lhs = lhs.strip()
        rhs_str = rhs_str.strip()

        if not lhs:
            return None

        if rhs_str in {"ε", "/* empty */", ""}:
            return lhs, []

        rhs_symbols = []
        for token in rhs_str.split():
            token = token.strip()
            if (not token or token == "ε" or "/*" in token or "*/" in token):
                continue
            rhs_symbols.append(token)

        return lhs, rhs_symbols

    def build_tree_from_rules(self, parsed_rules, nonterminals):
        """Use shift-reduce style reconstruction to build the parse tree"""
        stack: List[ParseTreeNode] = []

        for lhs, rhs, rule_text in parsed_rules:
            children: List[ParseTreeNode] = []

            for symbol in reversed(rhs):
                if symbol in nonterminals:
                    child = stack.pop() if stack else ParseTreeNode(symbol)
                    children.append(child)
                else:
                    terminal_node = self.create_terminal_node(symbol)
                    if terminal_node:
                        children.append(terminal_node)

            children.reverse()

            node = ParseTreeNode(lhs, is_terminal=False, rule=rule_text)
            for child in children:
                node.add_child(child)

            stack.append(node)

        return stack[-1] if stack else None

    def create_terminal_node(self, symbol: str) -> Optional[ParseTreeNode]:
        """Create a terminal node, normalising quotes if needed"""
        if symbol in {"ε", "/*", "*/"}:
            return None

        label = symbol
        if (label.startswith("'") and label.endswith("'")) or (label.startswith('"') and label.endswith('"')):
            label = label[1:-1]

        return ParseTreeNode(label, True, rule="")
        
    def calculate_layout(self, node, x=0, y=80):
        """Calculate node positions"""
        if not node:
            return 0
            
        # Node dimensions
        node.width = max(80, len(node.label) * 10 + 20)
        node.height = 50
        
        if not node.children:
            # Leaf node
            node.x = x
            node.y = y
            return node.width
            
        # Calculate children first
        child_x = x
        total_width = 0
        
        for child in node.children:
            child_width = self.calculate_layout(child, child_x, y + 90)
            child_x += child_width + 25
            total_width += child_width + 25
            
        if total_width > 0:
            total_width -= 25
            
        # Center parent over children
        if node.children:
            first_child = node.children[0]
            last_child = node.children[-1]
            center_x = (first_child.x + last_child.x + last_child.width) / 2
            node.x = center_x - node.width / 2
        else:
            node.x = x
            
        node.y = y
        return max(total_width, node.width)
        
    def draw_visual_tree(self):
        """Draw the visual tree"""
        self.canvas.delete("all")
        
        if not self.tree_root:
            self.canvas.create_text(50, 50, text="No tree to display", 
                                  font=("Arial", 12), anchor="nw")
            return
            
        # Calculate layout
        tree_width = self.calculate_layout(self.tree_root, 50, 80)
        
        # Draw title
        expression = self.input_var.get()
        self.canvas.create_text(20, 20, text=f"🌳 Parse Tree for: {expression}", 
                              font=("Arial", 16, "bold"), anchor="nw", fill="darkgreen")
        
        # Draw tree
        self.draw_node(self.tree_root)
        
        # Update scroll region
        max_y = self.get_max_y(self.tree_root) + 50
        self.canvas.configure(scrollregion=(0, 0, tree_width + 100, max_y))
        
        # Draw legend
        self.draw_legend(tree_width - 150, 30)
        
    def draw_node(self, node):
        """Draw a single node"""
        if not node:
            return
            
        # Colors based on node type
        if node.is_terminal:
            if node.label in ['+', '-', '*', '/', '(', ')']:
                fill = "#ffe6cc"  # Light orange for operators
                text_color = "darkorange"
                border = "orange"
            else:
                fill = "#cce6ff"  # Light blue for literals
                text_color = "darkblue"
                border = "blue"
        else:
            fill = "#ccffcc"  # Light green for non-terminals
            text_color = "darkgreen"
            border = "green"
            
        # Node coordinates
        x1, y1 = node.x, node.y
        x2, y2 = node.x + node.width, node.y + node.height
        
        # Draw shadow
        self.canvas.create_oval(x1+3, y1+3, x2+3, y2+3, 
                               fill="lightgray", outline="")
        
        # Draw node shape
        if node.is_terminal:
            # Oval for terminals
            node_id = self.canvas.create_oval(x1, y1, x2, y2, 
                                            fill=fill, outline=border, width=2,
                                            tags=f"node_{id(node)}")
        else:
            # Rectangle for non-terminals
            node_id = self.canvas.create_rectangle(x1, y1, x2, y2, 
                                                 fill=fill, outline=border, width=2,
                                                 tags=f"node_{id(node)}")
        
        # Draw text
        text_x = node.x + node.width / 2
        text_y = node.y + node.height / 2
        font_size = 11 if len(node.label) <= 5 else 9
        self.canvas.create_text(text_x, text_y, text=node.label,
                              font=("Arial", font_size, "bold"), 
                              fill=text_color, anchor="center",
                              tags=f"node_{id(node)}")
        
        # Store node reference for tooltips
        self.canvas.tag_bind(f"node_{id(node)}", "<Enter>", 
                           lambda e, n=node: self.show_tooltip(e, n))
        self.canvas.tag_bind(f"node_{id(node)}", "<Leave>", self.hide_tooltip)
        
        # Draw connections to children
        if node.children:
            parent_x = node.x + node.width / 2
            parent_y = node.y + node.height
            
            for child in node.children:
                child_x = child.x + child.width / 2
                child_y = child.y
                
                # Draw connection line
                self.canvas.create_line(parent_x, parent_y, child_x, child_y,
                                      fill="gray", width=2, smooth=True)
                
                # Draw connection dots
                self.canvas.create_oval(parent_x-3, parent_y-3, parent_x+3, parent_y+3,
                                      fill="gray", outline="")
                
                # Recursively draw children
                self.draw_node(child)
                
    def get_max_y(self, node):
        """Get maximum Y coordinate"""
        max_y = node.y + node.height
        for child in node.children:
            max_y = max(max_y, self.get_max_y(child))
        return max_y
        
    def draw_legend(self, x, y):
        """Draw color legend"""
        legend_items = [
            ("Non-terminals", "#ccffcc", "darkgreen"),
            ("Operators", "#ffe6cc", "darkorange"),
            ("Numbers/IDs", "#cce6ff", "darkblue")
        ]
        
        self.canvas.create_text(x, y, text="Legend:", 
                              font=("Arial", 10, "bold"), anchor="nw")
        
        for i, (label, fill, color) in enumerate(legend_items):
            item_y = y + 20 + i * 20
            self.canvas.create_rectangle(x, item_y, x+15, item_y+15,
                                       fill=fill, outline=color)
            self.canvas.create_text(x+20, item_y+7, text=label,
                                  font=("Arial", 9), anchor="w", fill=color)
                                  
    def show_tooltip(self, event, node):
        """Show tooltip for node"""
        if self.tooltip_id:
            self.canvas.delete(self.tooltip_id)
            
        tooltip_text = f"Symbol: {node.label}"
        if node.rule:
            tooltip_text += f"\\nRule: {node.rule}"
        if node.is_terminal:
            tooltip_text += "\\nType: Terminal"
        else:
            tooltip_text += "\\nType: Non-terminal"
            
        self.tooltip_id = self.canvas.create_text(
            event.x + 10, event.y - 10, text=tooltip_text,
            font=("Arial", 9), anchor="nw", fill="black",
            tags="tooltip"
        )
        
        # Tooltip background
        bbox = self.canvas.bbox(self.tooltip_id)
        if bbox:
            self.canvas.create_rectangle(
                bbox[0]-2, bbox[1]-2, bbox[2]+2, bbox[3]+2,
                fill="lightyellow", outline="black", tags="tooltip"
            )
            self.canvas.tag_lower("tooltip")
            
    def hide_tooltip(self, event):
        """Hide tooltip"""
        if self.tooltip_id:
            self.canvas.delete("tooltip")
            self.tooltip_id = None
            
    # Event handlers
    def on_canvas_click(self, event):
        self.drag_start_x = event.x
        self.drag_start_y = event.y
        
    def on_canvas_drag(self, event):
        dx = event.x - self.drag_start_x
        dy = event.y - self.drag_start_y
        self.canvas.scan_dragto(dx, dy, gain=1)
        
    def on_canvas_motion(self, event):
        # Update cursor based on what's under mouse
        pass
        
    def on_mouse_wheel(self, event):
        # Zoom functionality
        if event.delta > 0:
            self.canvas.scale("all", event.x, event.y, 1.1, 1.1)
        else:
            self.canvas.scale("all", event.x, event.y, 0.9, 0.9)

def main():
    if not os.path.exists("./parser"):
        print("Parser not found. Please build first:")
        print("make && ./cfg2yacc -i samples/expr.ebnf -o expr.y --emit-lex expr.l --no-ff")
        print("bison -d expr.y && flex expr.l && gcc -o parser expr.tab.c lex.yy.c ast.c -lfl")
        return
        
    root = tk.Tk()
    app = VisualParseTreeGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()