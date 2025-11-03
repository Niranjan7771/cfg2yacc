#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"

Node* create_leaf_node(const char* node_type, const char* value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->node_type = strdup(node_type);
    node->value = value ? strdup(value) : NULL;
    node->num_children = 0;
    node->children = NULL;
    return node;
}

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
