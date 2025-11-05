/*
 * ast.c
 * -----
 * Implementation of the small AST (parse tree) helper library used by the
 * generated parser. Functions here allocate, print and free Node structures.
 * The generated parser (parser.y) depends on these symbols to construct the
 * in-memory parse tree.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"

/*
 * create_leaf_node
 * -----------------
 * Allocate and return a Node representing a terminal (leaf) with a textual
 * `node_type` and `value`. Both strings are duplicated using `strdup` so the
 * caller may free the originals.
 */
Node* create_leaf_node(const char* node_type, const char* value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->node_type = strdup(node_type);
    node->value = value ? strdup(value) : NULL;
    node->num_children = 0;
    node->children = NULL;
    return node;
}

/*
 * create_node
 * -----------
 * Allocate a non-terminal node with a variable number of child Node* values.
 * Usage in grammar actions looks like: create_node("expr", 2, $1, $2);
 */
Node* create_node(const char* node_type, int num_children, ...) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->node_type = strdup(node_type);
    node->value = NULL;
    node->num_children = num_children;

    if (num_children > 0) {
        node->children = (Node**)malloc(sizeof(Node*) * num_children);
        va_list args;
        va_start(args, num_children);
        for (int i = 0; i < num_children; i++) {
            node->children[i] = va_arg(args, Node*);
        }
        va_end(args);
    } else {
        node->children = NULL;
    }

    return node;
}

/*
 * print_ast
 * ---------
 * Recursively print the AST in a simple indented format. This output is the
 * format consumed by `visualize_tree.py` and `streamlit_visualizer.py`.
 */
void print_ast(Node* node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    printf("%s", node->node_type);
    if (node->value) {
        printf(": %s", node->value);
    }
    printf("\n");

    for (int i = 0; i < node->num_children; i++) {
        print_ast(node->children[i], indent + 1);
    }
}

/*
 * free_ast
 * --------
 * Recursively free all memory owned by the tree. Safe to call with NULL.
 */
void free_ast(Node* node) {
    if (!node) return;

    free(node->node_type);
    if (node->value) free(node->value);

    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }

    if (node->children) free(node->children);
    free(node);
}
