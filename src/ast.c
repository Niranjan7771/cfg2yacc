#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static char *json_dup_string(const char *value) {
    if (!value) {
        return NULL;
    }

    size_t length = strlen(value) + 1;
    char *copy = malloc(length);
    if (!copy) {
        return NULL;
    }

    memcpy(copy, value, length);
    return copy;
}

static void *json_realloc(void *ptr, size_t count, size_t size) {
    if (size == 0 || count > SIZE_MAX / size) {
        return NULL;
    }
    return realloc(ptr, count * size);
}

JsonNode *json_node_new(JsonNodeKind kind) {
    JsonNode *node = calloc(1, sizeof(JsonNode));
    if (!node) {
        return NULL;
    }

    node->kind = kind;
    return node;
}

JsonNode *json_node_new_string(const char *value) {
    JsonNode *node = json_node_new(JSON_NODE_STRING);
    if (!node) {
        return NULL;
    }

    node->data.s_val = json_dup_string(value);
    if (!node->data.s_val) {
        free(node);
        return NULL;
    }

    return node;
}

JsonNode *json_node_new_number(double value) {
    JsonNode *node = json_node_new(JSON_NODE_NUMBER);
    if (!node) {
        return NULL;
    }

    node->data.d_val = value;
    return node;
}

void json_object_append(JsonNode *object, const char *key, JsonNode *value) {
    if (!object || object->kind != JSON_NODE_OBJECT || !key) {
        return;
    }

    JsonPair **items = json_realloc(object->data.object.items,
                                    object->data.object.count + 1,
                                    sizeof(JsonPair *));
    if (!items) {
        return;
    }

    object->data.object.items = items;

    JsonPair *pair = calloc(1, sizeof(JsonPair));
    if (!pair) {
        return;
    }

    pair->key = json_dup_string(key);
    if (!pair->key) {
        free(pair);
        return;
    }

    pair->value = value;

    object->data.object.items[object->data.object.count++] = pair;
}

void json_array_append(JsonNode *array, JsonNode *value) {
    if (!array || array->kind != JSON_NODE_ARRAY) {
        return;
    }

    JsonNode **items = json_realloc(array->data.array.items,
                                    array->data.array.count + 1,
                                    sizeof(JsonNode *));
    if (!items) {
        return;
    }

    array->data.array.items = items;
    array->data.array.items[array->data.array.count++] = value;
}

static void json_print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
}

void json_node_print(const JsonNode *node, int indent) {
    if (!node) {
        return;
    }

    json_print_indent(indent);

    switch (node->kind) {
        case JSON_NODE_STRING:
            printf("STRING: %s\n", node->data.s_val);
            break;
        case JSON_NODE_NUMBER:
            printf("NUMBER: %g\n", node->data.d_val);
            break;
        case JSON_NODE_TRUE:
            puts("TRUE");
            break;
        case JSON_NODE_FALSE:
            puts("FALSE");
            break;
        case JSON_NODE_NULL:
            puts("NULL");
            break;
        case JSON_NODE_OBJECT: {
            puts("OBJECT {");
            for (int i = 0; i < node->data.object.count; ++i) {
                JsonPair *pair = node->data.object.items[i];
                if (!pair) {
                    continue;
                }
                json_print_indent(indent + 1);
                printf("%s:\n", pair->key ? pair->key : "<null>");
                json_node_print(pair->value, indent + 2);
            }
            json_print_indent(indent);
            puts("}");
            break;
        }
        case JSON_NODE_ARRAY: {
            puts("ARRAY [");
            for (int i = 0; i < node->data.array.count; ++i) {
                json_node_print(node->data.array.items[i], indent + 1);
            }
            json_print_indent(indent);
            puts("]");
            break;
        }
        default:
            puts("<unknown>");
            break;
    }
}

void json_node_free(JsonNode *node) {
    if (!node) {
        return;
    }

    switch (node->kind) {
        case JSON_NODE_STRING:
            free(node->data.s_val);
            break;
        case JSON_NODE_OBJECT:
            for (int i = 0; i < node->data.object.count; ++i) {
                JsonPair *pair = node->data.object.items[i];
                if (pair) {
                    free(pair->key);
                    json_node_free(pair->value);
                    free(pair);
                }
            }
            free(node->data.object.items);
            break;
        case JSON_NODE_ARRAY:
            for (int i = 0; i < node->data.array.count; ++i) {
                json_node_free(node->data.array.items[i]);
            }
            free(node->data.array.items);
            break;
        default:
            break;
    }

    free(node);
}


