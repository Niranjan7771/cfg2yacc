#!/usr/bin/env python3
"""
streamlit_visualizer.py
------------------------

Interactive Streamlit application that provides a web-based interface for
visualizing parse trees produced by the `custom_compiler` executable. The app
lets users select a `.def` analyzer, build it (via `make`), run it on sample or
custom input, then view the resulting parse tree as an interactive Graphviz
diagram or as formatted text. This file contains helper functions to discover
analyzers, build and run the compiler, and convert the textual parse tree into
structures suitable for display.

Key functions:
- `get_available_analyzers()` - enumerates .def files in the repo and samples
- `build_compiler(def_file)` - runs `make` to regenerate the compiler for a
    specific analyzer
- `run_compiler(input_file)` - runs `./custom_compiler` and captures output
- `create_graphviz_tree(nodes)` - converts parsed nodes into a Graphviz Digraph
"""

import streamlit as st
import subprocess
import os
import json
from pathlib import Path
import graphviz
import re

# Page config
st.set_page_config(
    page_title="Parse Tree Visualizer",
    page_icon="🌲",
    layout="wide",
    initial_sidebar_state="expanded"
)

# Custom CSS
st.markdown("""
<style>
    .stAlert {
        margin-top: 1rem;
    }
    .node-terminal {
        background-color: #ffe6e6;
        padding: 5px;
        border-radius: 3px;
        font-family: monospace;
    }
    .node-nonterminal {
        background-color: #e6f3ff;
        padding: 5px;
        border-radius: 3px;
        font-family: monospace;
    }
    .stats-box {
        background-color: #f0f2f6;
        padding: 1rem;
        border-radius: 5px;
        margin: 1rem 0;
    }
</style>
""", unsafe_allow_html=True)

class ParseTreeNode:
    def __init__(self, node_type, value=None, indent_level=0):
        self.node_type = node_type
        self.value = value
        self.indent_level = indent_level
        self.children = []
        
    def __repr__(self):
        if self.value:
            return f"{self.node_type}: {self.value}"
        return self.node_type

def get_available_analyzers():
    """Get list of available .def files"""
    analyzers = []
    
    # Root analyzers
    for def_file in Path('.').glob('*.def'):
        analyzers.append({
            'name': def_file.stem,
            'path': str(def_file),
            'category': 'Root'
        })
    
    # Sample analyzers
    samples_dir = Path('samples')
    if samples_dir.exists():
        for sample_dir in sorted(samples_dir.glob('sample*')):
            for def_file in sample_dir.glob('*_analyzer.def'):
                analyzers.append({
                    'name': f"{sample_dir.name} - {def_file.stem}",
                    'path': str(def_file),
                    'category': 'Samples'
                })
    
    return analyzers

def get_input_file(def_file):
    """Get corresponding input file for a .def file"""
    def_path = Path(def_file)
    
    if 'samples' in str(def_path):
        # For sample files
        input_name = def_path.stem.replace('_analyzer', '_input') + '.txt'
        input_path = def_path.parent / input_name
    else:
        # For root files
        input_path = Path('test_input.txt')
    
    return input_path if input_path.exists() else None

def build_compiler(def_file):
    """Build the compiler with the specified .def file"""
    try:
        # Clean
        subprocess.run(['make', 'clean'], capture_output=True, check=True)
        
        # Build
        result = subprocess.run(
            ['make', f'DEF_FILE={def_file}'],
            capture_output=True,
            text=True,
            check=True
        )
        
        return True, "Build successful!"
    except subprocess.CalledProcessError as e:
        return False, f"Build failed:\n{e.stderr}"

def run_compiler(input_file):
    """Run the compiler and get parse tree output"""
    try:
        with open(input_file, 'r') as f:
            input_data = f.read()
        
        result = subprocess.run(
            ['./custom_compiler'],
            input=input_data,
            capture_output=True,
            text=True,
            timeout=5
        )
        
        return result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return None, "Execution timeout"
    except Exception as e:
        return None, str(e)

def parse_tree_output(output):
    """Parse the tree output into structured nodes"""
    lines = output.split('\n')
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
            nodes.append(ParseTreeNode(node_type, value, indent))
        else:
            nodes.append(ParseTreeNode(content, None, indent))
    
    return nodes

def create_graphviz_tree(nodes):
    """Create a graphviz tree from nodes"""
    if not nodes:
        return None
    
    dot = graphviz.Digraph(comment='Parse Tree')
    dot.attr(rankdir='TB')
    dot.attr('node', shape='box', style='rounded,filled')
    
    # Node styling
    node_counter = [0]
    node_map = {}
    
    def add_node(node, parent_id=None):
        node_id = f"node_{node_counter[0]}"
        node_counter[0] += 1
        
        # Determine color based on node type
        if node.value:
            # Terminal
            color = '#ffcccc'
            label = f"{node.node_type}\n{node.value}"
        else:
            # Non-terminal
            color = '#cce5ff'
            label = node.node_type
        
        # Special colors for detected patterns
        if 'email' in node.node_type.lower():
            color = '#ffccff'
        elif 'phone' in node.node_type.lower():
            color = '#ccffff'
        elif 'url' in node.node_type.lower() or 'website' in node.node_type.lower():
            color = '#ffffcc'
        elif 'currency' in node.node_type.lower() or 'dollar' in node.node_type.lower():
            color = '#ccffcc'
        elif 'urgent' in node.node_type.lower():
            color = '#ffaaaa'
        
        dot.node(node_id, label, fillcolor=color)
        node_map[node_id] = (node, parent_id)
        
        return node_id
    
    # Build tree structure
    stack = []
    for i, node in enumerate(nodes):
        node_id = add_node(node)
        
        # Find parent based on indentation
        while stack and stack[-1][1] >= node.indent_level:
            stack.pop()
        
        if stack:
            parent_id = stack[-1][0]
            dot.edge(parent_id, node_id)
        
        stack.append((node_id, node.indent_level))
    
    return dot

def calculate_statistics(nodes):
    """Calculate statistics from parse tree"""
    total = len(nodes)
    terminals = sum(1 for n in nodes if n.value)
    nonterminals = total - terminals
    
    # Detect patterns
    patterns = {}
    for node in nodes:
        if 'found' in node.node_type.lower() or 'detected' in node.node_type.lower():
            key = node.node_type
            patterns[key] = patterns.get(key, 0) + 1
        elif node.value and node.node_type.isupper():
            # Count token types
            if '_' in node.node_type:
                base_type = node.node_type.lower()
                patterns[base_type] = patterns.get(base_type, 0) + 1
    
    return {
        'total': total,
        'terminals': terminals,
        'nonterminals': nonterminals,
        'patterns': patterns
    }

# Main UI
st.title("🌲 Parse Tree Visualizer")
st.markdown("Interactive visualization for custom compiler parse trees")

# Sidebar
with st.sidebar:
    st.header("⚙️ Configuration")
    
    # Get available analyzers
    analyzers = get_available_analyzers()
    
    if not analyzers:
        st.error("No .def files found!")
        st.stop()
    
    # Group analyzers by category
    categories = {}
    for analyzer in analyzers:
        cat = analyzer['category']
        if cat not in categories:
            categories[cat] = []
        categories[cat].append(analyzer)
    
    # Analyzer selection
    selected_analyzer = None
    for category, items in categories.items():
        st.subheader(category)
        for item in items:
            if st.button(item['name'], key=item['path']):
                selected_analyzer = item['path']
    
    st.divider()
    
    # Visualization options
    st.subheader("📊 Display Options")
    show_graph = st.checkbox("Show Graph View", value=True)
    show_text = st.checkbox("Show Text View", value=True)
    show_stats = st.checkbox("Show Statistics", value=True)
    show_input = st.checkbox("Show Input Text", value=False)

# Main content
if 'current_analyzer' not in st.session_state:
    st.session_state.current_analyzer = None
    st.session_state.nodes = None
    st.session_state.input_content = None

# Update if new analyzer selected
if selected_analyzer:
    st.session_state.current_analyzer = selected_analyzer

if st.session_state.current_analyzer:
    def_file = st.session_state.current_analyzer
    input_file = get_input_file(def_file)
    
    st.info(f"**Selected Analyzer:** `{def_file}`")
    
    # Build button
    col1, col2, col3 = st.columns([1, 1, 3])
    with col1:
        if st.button("🔨 Build Compiler", type="primary"):
            with st.spinner("Building..."):
                success, message = build_compiler(def_file)
                if success:
                    st.success(message)
                else:
                    st.error(message)
    
    with col2:
        if st.button("▶️ Run & Visualize", type="primary"):
            if not input_file:
                st.error("Input file not found!")
            else:
                with st.spinner("Running compiler..."):
                    # First build
                    success, build_msg = build_compiler(def_file)
                    if not success:
                        st.error(f"Build failed: {build_msg}")
                    else:
                        # Then run
                        stdout, stderr = run_compiler(input_file)
                        
                        if stderr:
                            with st.expander("⚠️ Compiler Messages", expanded=False):
                                st.text(stderr)
                        
                        if stdout:
                            # Parse output
                            st.session_state.nodes = parse_tree_output(stdout)
                            
                            # Read input
                            with open(input_file, 'r') as f:
                                st.session_state.input_content = f.read()
                            
                            st.success("✅ Parse tree generated!")
                        else:
                            st.error("No output from compiler")
    
    # Custom input
    with st.expander("✏️ Use Custom Input", expanded=False):
        custom_input = st.text_area(
            "Enter custom input text:",
            height=200,
            placeholder="Type or paste your input here..."
        )
        
        if st.button("Run with Custom Input"):
            if custom_input:
                with st.spinner("Running..."):
                    # Build first
                    success, build_msg = build_compiler(def_file)
                    if success:
                        # Run with custom input
                        try:
                            result = subprocess.run(
                                ['./custom_compiler'],
                                input=custom_input,
                                capture_output=True,
                                text=True,
                                timeout=5
                            )
                            
                            if result.stderr:
                                with st.expander("⚠️ Messages"):
                                    st.text(result.stderr)
                            
                            if result.stdout:
                                st.session_state.nodes = parse_tree_output(result.stdout)
                                st.session_state.input_content = custom_input
                                st.success("✅ Parse tree generated!")
                        except Exception as e:
                            st.error(f"Error: {e}")
    
    # Display results
    if st.session_state.nodes:
        st.divider()
        
        # Input text
        if show_input and st.session_state.input_content:
            with st.expander("📄 Input Text", expanded=False):
                st.code(st.session_state.input_content, language=None)
        
        # Statistics
        if show_stats:
            stats = calculate_statistics(st.session_state.nodes)
            
            st.subheader("📊 Statistics")
            col1, col2, col3 = st.columns(3)
            with col1:
                st.metric("Total Nodes", stats['total'])
            with col2:
                st.metric("Terminals", stats['terminals'])
            with col3:
                st.metric("Non-terminals", stats['nonterminals'])
            
            if stats['patterns']:
                st.markdown("**Detected Patterns:**")
                cols = st.columns(4)
                for i, (pattern, count) in enumerate(sorted(stats['patterns'].items())):
                    with cols[i % 4]:
                        st.metric(pattern.replace('_', ' ').title(), count)
        
        st.divider()
        
        # Graph view
        if show_graph:
            st.subheader("🔍 Graph View")
            graph = create_graphviz_tree(st.session_state.nodes)
            if graph:
                st.graphviz_chart(graph)
            else:
                st.warning("Could not generate graph")
        
        # Text view
        if show_text:
            st.subheader("📝 Text View")
            
            # Create formatted text
            text_output = []
            for node in st.session_state.nodes:
                indent = '  ' * (node.indent_level // 2)
                if node.value:
                    text_output.append(f"{indent}🔹 {node.node_type}: {node.value}")
                else:
                    text_output.append(f"{indent}📦 {node.node_type}")
            
            st.text('\n'.join(text_output))

else:
    # Welcome screen
    st.markdown("""
    ## 👋 Welcome to Parse Tree Visualizer!
    
    This interactive tool helps you visualize parse trees from your custom compiler.
    
    ### 🚀 Quick Start:
    
    1. **Select an analyzer** from the sidebar
    2. Click **"Build Compiler"** to compile the analyzer
    3. Click **"Run & Visualize"** to see the parse tree
    
    ### 📋 Available Views:
    
    - **Graph View**: Interactive tree diagram
    - **Text View**: Indented text representation
    - **Statistics**: Pattern counts and node statistics
    - **Custom Input**: Test with your own input
    
    ### 🎨 Features:
    
    - Color-coded nodes (emails, phones, URLs, etc.)
    - Real-time compilation and visualization
    - Multiple analyzer support
    - Export capabilities
    
    **Select an analyzer from the sidebar to get started!**
    """)
    
    # Show available samples
    st.subheader("📁 Available Analyzers")
    for category, items in categories.items():
        with st.expander(f"{category} ({len(items)} analyzers)"):
            for item in items:
                st.markdown(f"- `{item['path']}`")

# Footer
st.divider()
st.markdown("""
<div style='text-align: center; color: #666;'>
    <small>Parse Tree Visualizer | Built with Streamlit | cfg2yacc Project</small>
</div>
""", unsafe_allow_html=True)
