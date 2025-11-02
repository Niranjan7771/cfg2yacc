#!/usr/bin/env python3
"""
Enhanced Visual Parse Tree Demo
Shows a more sophisticated tree visualization
"""

import tkinter as tk
from tkinter import ttk
import math

class TreeNode:
    def __init__(self, label, is_terminal=False):
        self.label = label
        self.is_terminal = is_terminal
        self.children = []
        self.parent = None
        self.x = 0
        self.y = 0
        self.width = 0
        self.height = 40
        
    def add_child(self, child):
        child.parent = self
        self.children.append(child)
        
    def get_leaf_count(self):
        if not self.children:
            return 1
        return sum(child.get_leaf_count() for child in self.children)

class VisualTreeDemo:
    def __init__(self, root):
        self.root = root
        self.root.title("Visual Parse Tree Demo")
        self.root.geometry("1000x700")
        
        self.setup_ui()
        self.create_demo_tree()
        
    def setup_ui(self):
        # Main frame
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Control frame
        control_frame = ttk.Frame(main_frame)
        control_frame.pack(fill=tk.X, pady=(0, 10))
        
        ttk.Label(control_frame, text="Visual Parse Tree Examples:", 
                 font=("Arial", 14, "bold")).pack(side=tk.LEFT)
        
        # Example buttons
        examples = [
            ("Simple: 42", self.demo_simple),
            ("Addition: 1+2", self.demo_addition),
            ("Precedence: 1+2*3", self.demo_precedence),
            ("Parentheses: (1+2)*3", self.demo_parentheses)
        ]
        
        for name, command in examples:
            ttk.Button(control_frame, text=name, command=command).pack(side=tk.LEFT, padx=5)
        
        # Canvas frame
        canvas_frame = ttk.Frame(main_frame)
        canvas_frame.pack(fill=tk.BOTH, expand=True)
        
        # Canvas with scrollbars
        self.canvas = tk.Canvas(canvas_frame, bg="white")
        
        v_scroll = ttk.Scrollbar(canvas_frame, orient=tk.VERTICAL, command=self.canvas.yview)
        h_scroll = ttk.Scrollbar(canvas_frame, orient=tk.HORIZONTAL, command=self.canvas.xview)
        
        self.canvas.configure(yscrollcommand=v_scroll.set, xscrollcommand=h_scroll.set)
        
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
    def create_demo_tree(self):
        """Create initial demo tree"""
        self.demo_precedence()
        
    def demo_simple(self):
        """Demo: 42"""
        root = TreeNode("expr")
        term = TreeNode("term")
        factor = TreeNode("factor")
        number = TreeNode("42", True)
        
        root.add_child(term)
        term.add_child(factor)
        factor.add_child(number)
        
        self.draw_tree(root, "Parse Tree for: 42")
        
    def demo_addition(self):
        """Demo: 1+2"""
        root = TreeNode("expr")
        
        # Left side: term -> factor -> 1
        term1 = TreeNode("term")
        factor1 = TreeNode("factor")
        num1 = TreeNode("1", True)
        
        term1.add_child(factor1)
        factor1.add_child(num1)
        
        # Operator
        plus = TreeNode("+", True)
        
        # Right side: term -> factor -> 2
        term2 = TreeNode("term")
        factor2 = TreeNode("factor")
        num2 = TreeNode("2", True)
        
        term2.add_child(factor2)
        factor2.add_child(num2)
        
        # Build tree
        root.add_child(term1)
        root.add_child(plus)
        root.add_child(term2)
        
        self.draw_tree(root, "Parse Tree for: 1+2")
        
    def demo_precedence(self):
        """Demo: 1+2*3 showing operator precedence"""
        root = TreeNode("expr")
        
        # Left: term -> factor -> 1
        term1 = TreeNode("term")
        factor1 = TreeNode("factor")
        num1 = TreeNode("1", True)
        
        term1.add_child(factor1)
        factor1.add_child(num1)
        
        # Plus operator
        plus = TreeNode("+", True)
        
        # Right: term with multiplication
        term2 = TreeNode("term")
        
        # 2 * 3 part
        factor2 = TreeNode("factor")
        num2 = TreeNode("2", True)
        factor2.add_child(num2)
        
        star = TreeNode("*", True)
        
        factor3 = TreeNode("factor")
        num3 = TreeNode("3", True)
        factor3.add_child(num3)
        
        term2.add_child(factor2)
        term2.add_child(star)
        term2.add_child(factor3)
        
        # Build final tree
        root.add_child(term1)
        root.add_child(plus)
        root.add_child(term2)
        
        self.draw_tree(root, "Parse Tree for: 1+2*3 (shows precedence)")
        
    def demo_parentheses(self):
        """Demo: (1+2)*3"""
        root = TreeNode("expr")
        term = TreeNode("term")
        
        # Left factor: (1+2)
        factor1 = TreeNode("factor")
        lparen = TreeNode("(", True)
        
        # Inner expression: 1+2
        inner_expr = TreeNode("expr")
        inner_term1 = TreeNode("term")
        inner_factor1 = TreeNode("factor")
        num1 = TreeNode("1", True)
        
        inner_term1.add_child(inner_factor1)
        inner_factor1.add_child(num1)
        
        plus = TreeNode("+", True)
        
        inner_term2 = TreeNode("term")
        inner_factor2 = TreeNode("factor")
        num2 = TreeNode("2", True)
        
        inner_term2.add_child(inner_factor2)
        inner_factor2.add_child(num2)
        
        inner_expr.add_child(inner_term1)
        inner_expr.add_child(plus)
        inner_expr.add_child(inner_term2)
        
        rparen = TreeNode(")", True)
        
        factor1.add_child(lparen)
        factor1.add_child(inner_expr)
        factor1.add_child(rparen)
        
        # Multiplication
        star = TreeNode("*", True)
        
        # Right factor: 3
        factor2 = TreeNode("factor")
        num3 = TreeNode("3", True)
        factor2.add_child(num3)
        
        # Build final tree
        term.add_child(factor1)
        term.add_child(star)
        term.add_child(factor2)
        
        root.add_child(term)
        
        self.draw_tree(root, "Parse Tree for: (1+2)*3 (shows grouping)")
        
    def calculate_layout(self, node, x=0, y=50, level=0):
        """Calculate node positions using a proper tree layout algorithm"""
        node.width = max(60, len(node.label) * 8 + 20)
        node.height = 40
        
        if not node.children:
            # Leaf node
            node.x = x
            node.y = y
            return node.width
            
        # Calculate children layout first
        child_x = x
        total_width = 0
        
        for child in node.children:
            child_width = self.calculate_layout(child, child_x, y + 80, level + 1)
            child_x += child_width + 20
            total_width += child_width + 20
            
        if total_width > 0:
            total_width -= 20  # Remove last spacing
            
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
        
    def draw_tree(self, root, title="Parse Tree"):
        """Draw the tree structure"""
        self.canvas.delete("all")
        
        # Calculate layout
        tree_width = self.calculate_layout(root, 50, 80)
        
        # Draw title
        self.canvas.create_text(20, 20, text=title, 
                              font=("Arial", 16, "bold"), anchor="nw", fill="darkblue")
        
        # Draw tree
        self.draw_node(root)
        
        # Update scroll region
        max_y = self.get_max_y(root) + 50
        self.canvas.configure(scrollregion=(0, 0, tree_width + 100, max_y))
        
    def draw_node(self, node):
        """Draw a single node and its connections"""
        # Choose colors
        if node.is_terminal:
            if node.label in ['+', '-', '*', '/', '(', ')']:
                fill = "#ffe6e6"  # Light red for operators
                text_color = "darkred"
                border = "red"
            else:
                fill = "#e6f3ff"  # Light blue for numbers
                text_color = "darkblue"
                border = "blue"
        else:
            fill = "#e6ffe6"  # Light green for non-terminals
            text_color = "darkgreen"
            border = "green"
            
        # Draw node
        x1, y1 = node.x, node.y
        x2, y2 = node.x + node.width, node.y + node.height
        
        # Shadow
        self.canvas.create_oval(x1+3, y1+3, x2+3, y2+3, 
                               fill="lightgray", outline="lightgray")
        
        # Main shape - use oval for terminals, rectangle for non-terminals
        if node.is_terminal:
            self.canvas.create_oval(x1, y1, x2, y2, 
                                   fill=fill, outline=border, width=2)
        else:
            self.canvas.create_rectangle(x1, y1, x2, y2, 
                                       fill=fill, outline=border, width=2)
        
        # Text
        text_x = node.x + node.width / 2
        text_y = node.y + node.height / 2
        font_size = 10 if len(node.label) <= 3 else 8
        self.canvas.create_text(text_x, text_y, text=node.label,
                              font=("Arial", font_size, "bold"), 
                              fill=text_color, anchor="center")
        
        # Draw connections to children
        if node.children:
            parent_x = node.x + node.width / 2
            parent_y = node.y + node.height
            
            for child in node.children:
                child_x = child.x + child.width / 2
                child_y = child.y
                
                # Draw line
                self.canvas.create_line(parent_x, parent_y, child_x, child_y,
                                      fill="darkgray", width=2, smooth=True)
                
                # Draw connection dots
                self.canvas.create_oval(parent_x-2, parent_y-2, parent_x+2, parent_y+2,
                                      fill="darkgray", outline="darkgray")
                
                # Recursively draw children
                self.draw_node(child)
                
    def get_max_y(self, node):
        """Get maximum Y coordinate in tree"""
        max_y = node.y + node.height
        for child in node.children:
            max_y = max(max_y, self.get_max_y(child))
        return max_y

def main():
    root = tk.Tk()
    app = VisualTreeDemo(root)
    root.mainloop()

if __name__ == "__main__":
    main()