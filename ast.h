#ifndef AST_H
#define AST_H

#include <stddef.h>

/*
 * ast.h
 * -----
 * Defines the in-memory AST (parse tree) Node structure and the public
 * functions used by the generated parser to build and manipulate the tree.
 *
 * The Node structure is intentionally simple: each node stores a textual
 * `node_type` label, an optional `value` (for terminals), and an array of
 * child pointers.
 */

typedef struct Node {
    char* node_type;       /* e.g. "expression", or token name like "NUMBER" */
    char* value;           /* textual value for terminals (NULL for non-terminals) */
    int num_children;      /* number of children */
    struct Node** children;/* array of child Node* pointers */
} Node;

/* Create a leaf node with a given node_type and textual value. The
 * implementation duplicates strings; caller owns the returned Node and must
 * call free_ast() to release memory.
 */
Node* create_leaf_node(const char* node_type, const char* value);

/* Create an internal node with `num_children` children passed as variadic
 * arguments (Node*...). This mirrors the style used in grammar actions.
 */
Node* create_node(const char* node_type, int num_children, ...);

/* Print the AST in an indented textual form. Useful for debugging and for
 * the terminal visualizer which consumes the same format.
 */
void print_ast(Node* node, int indent);

/* Free the entire AST recursively. Safe to call on NULL. */
void free_ast(Node* node);

#endif
