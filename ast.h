#ifndef AST_H
#define AST_H

// Enum for JSON node types
typedef enum {
    JSON_NODE_OBJECT,
    JSON_NODE_ARRAY,
    JSON_NODE_PAIR,
    JSON_NODE_STRING,
    JSON_NODE_NUMBER,
    JSON_NODE_TRUE,
    JSON_NODE_FALSE,
    JSON_NODE_NULL
} JsonNodeKind;

// Forward declaration for the main struct
typedef struct JsonNode JsonNode;

// A key-value pair in an object
typedef struct {
    char *key;
    JsonNode *value;
} JsonPair;

// The main AST node structure
struct JsonNode {
    JsonNodeKind kind;
    union {
        // For STRING
        char *s_val;
        // For NUMBER
        double d_val;
        // For OBJECT (list of pairs)
        struct {
            JsonPair **items;
            int count;
        } object;
        // For ARRAY (list of nodes)
        struct {
            JsonNode **items;
            int count;
        } array;
    } data;
};

// AST creation functions
JsonNode* json_node_new(JsonNodeKind kind);
JsonNode* json_node_new_string(const char *value);
JsonNode* json_node_new_number(double value);
void json_object_append(JsonNode *object, const char *key, JsonNode *value);
void json_array_append(JsonNode *array, JsonNode *value);

// Utility to print the AST
void json_node_print(const JsonNode *node, int indent);
void json_node_free(JsonNode *node);

#endif // AST_H


