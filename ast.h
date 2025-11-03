#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef struct Node {
    char* node_type;
    char* value;
    int num_children;
    struct Node** children;
} Node;

Node* create_leaf_node(const char* node_type, const char* value);
Node* create_node(const char* node_type, int num_children, ...);
void print_ast(Node* node, int indent);
void free_ast(Node* node);

#endif
