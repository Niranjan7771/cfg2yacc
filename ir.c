#include "ir.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    Instruction *head;
    Instruction *tail;
    int temp_index;
} IrContext;

typedef struct ValueEntry {
    char name[IR_NAME_MAX];
    int value;
    struct ValueEntry *next;
} ValueEntry;

static Operand operand_from_constant(int value);
static Operand operand_from_name(const char *name);
static Operand operand_empty(void);
static Instruction *append_instruction(IrContext *ctx, IrOpcode op, const char *result, Operand lhs, Operand rhs);
static Operand emit_binary_operation(IrContext *ctx, IrOpcode op, Operand lhs, Operand rhs);
static Operand generate_expr(Node *node, IrContext *ctx);
static void generate_stmt(Node *node, IrContext *ctx);
static int operand_value(const Operand *operand, ValueEntry *symbols);
static ValueEntry *find_symbol(ValueEntry *symbols, const char *name);
static void set_symbol_value(ValueEntry **symbols, const char *name, int value);
static void free_symbols(ValueEntry *symbols);

Instruction *generate_ir(Node *root)
{
    IrContext ctx = {0};
    generate_stmt(root, &ctx);
    return ctx.head;
}

void print_ir(const Instruction *head)
{
    const Instruction *current = head;

    while (current) {
        const Operand *lhs = &current->lhs;
        const Operand *rhs = &current->rhs;

        switch (current->op) {
        case IR_OP_ASSIGN:
            if (lhs->is_constant) {
                printf("%s = %d\n", current->result, lhs->value);
            } else {
                printf("%s = %s\n", current->result, lhs->name);
            }
            break;
        case IR_OP_ADD:
        case IR_OP_SUB:
        case IR_OP_MUL:
        case IR_OP_DIV:
        {
            const char *symbol = "";
            switch (current->op) {
            case IR_OP_ADD:
                symbol = "+";
                break;
            case IR_OP_SUB:
                symbol = "-";
                break;
            case IR_OP_MUL:
                symbol = "*";
                break;
            case IR_OP_DIV:
                symbol = "/";
                break;
            default:
                break;
            }

            const char *lhs_repr = lhs->is_constant ? NULL : lhs->name;
            const char *rhs_repr = rhs->is_constant ? NULL : rhs->name;

            if (lhs->is_constant && rhs->is_constant) {
                printf("%s = %d %s %d\n", current->result, lhs->value, symbol, rhs->value);
            } else if (lhs->is_constant) {
                printf("%s = %d %s %s\n", current->result, lhs->value, symbol, rhs_repr);
            } else if (rhs->is_constant) {
                printf("%s = %s %s %d\n", current->result, lhs_repr, symbol, rhs->value);
            } else {
                printf("%s = %s %s %s\n", current->result, lhs_repr, symbol, rhs_repr);
            }
            break;
        }
        case IR_OP_PRINT:
            if (lhs->is_constant) {
                printf("print %d\n", lhs->value);
            } else {
                printf("print %s\n", lhs->name);
            }
            break;
        case IR_OP_INPUT:
            printf("input %s\n", current->result);
            break;
        default:
            break;
        }

        current = current->next;
    }
}

void free_ir(Instruction *head)
{
    Instruction *current = head;

    while (current) {
        Instruction *next = current->next;
        free(current);
        current = next;
    }
}

const char *ir_opcode_name(IrOpcode op)
{
    switch (op) {
    case IR_OP_ASSIGN:
        return "ASSIGN";
    case IR_OP_ADD:
        return "ADD";
    case IR_OP_SUB:
        return "SUB";
    case IR_OP_MUL:
        return "MUL";
    case IR_OP_DIV:
        return "DIV";
    case IR_OP_PRINT:
        return "PRINT";
    case IR_OP_INPUT:
        return "INPUT";
    default:
        return "UNKNOWN";
    }
}

static Operand generate_expr(Node *node, IrContext *ctx)
{
    if (!node) {
        return operand_empty();
    }

    switch (node->type) {
    case NODE_NUMBER:
        return operand_from_constant(node->value);
    case NODE_IDENTIFIER:
        return operand_from_name(node->identifier ? node->identifier : "");
    case NODE_ADD:
    {
        Operand lhs = generate_expr(node->left, ctx);
        Operand rhs = generate_expr(node->right, ctx);
        return emit_binary_operation(ctx, IR_OP_ADD, lhs, rhs);
    }
    case NODE_SUB:
    {
        Operand lhs = generate_expr(node->left, ctx);
        Operand rhs = generate_expr(node->right, ctx);
        return emit_binary_operation(ctx, IR_OP_SUB, lhs, rhs);
    }
    case NODE_MUL:
    {
        Operand lhs = generate_expr(node->left, ctx);
        Operand rhs = generate_expr(node->right, ctx);
        return emit_binary_operation(ctx, IR_OP_MUL, lhs, rhs);
    }
    case NODE_DIV:
    {
        Operand lhs = generate_expr(node->left, ctx);
        Operand rhs = generate_expr(node->right, ctx);
        return emit_binary_operation(ctx, IR_OP_DIV, lhs, rhs);
    }
    case NODE_PRINT:
    case NODE_INPUT:
        generate_stmt(node, ctx);
        return operand_empty();
    case NODE_ASSIGN:
        generate_stmt(node, ctx);
        return operand_empty();
    case NODE_STMT_LIST:
        generate_stmt(node, ctx);
        return operand_empty();
    default:
        return operand_empty();
    }
}

static void generate_stmt(Node *node, IrContext *ctx)
{
    if (!node) {
        return;
    }

    switch (node->type) {
    case NODE_STMT_LIST:
        generate_stmt(node->left, ctx);
        generate_stmt(node->right, ctx);
        break;
    case NODE_ASSIGN:
    {
        if (!node->left) {
            break;
        }

        Operand destination = operand_from_name(node->left->identifier ? node->left->identifier : "");
        Operand value = generate_expr(node->right, ctx);
        append_instruction(ctx, IR_OP_ASSIGN, destination.name, value, operand_empty());
        break;
    }
    case NODE_PRINT:
    {
        Operand value = generate_expr(node->left, ctx);
        append_instruction(ctx, IR_OP_PRINT, NULL, value, operand_empty());
        break;
    }
    case NODE_INPUT:
    {
        if (!node->left) {
            break;
        }
        Operand destination = operand_from_name(node->left->identifier ? node->left->identifier : "");
        append_instruction(ctx, IR_OP_INPUT, destination.name, operand_empty(), operand_empty());
        break;
    }
    default:
        (void)generate_expr(node, ctx);
        break;
    }
}

static Operand operand_from_constant(int value)
{
    Operand operand = {0};
    operand.is_constant = 1;
    operand.value = value;
    operand.name[0] = '\0';
    return operand;
}

static Operand operand_from_name(const char *name)
{
    Operand operand = {0};
    operand.is_constant = 0;
    operand.value = 0;
    if (name) {
        strncpy(operand.name, name, IR_NAME_MAX - 1);
        operand.name[IR_NAME_MAX - 1] = '\0';
    } else {
        operand.name[0] = '\0';
    }
    return operand;
}

static Operand operand_empty(void)
{
    Operand operand = {0};
    operand.is_constant = 0;
    operand.value = 0;
    operand.name[0] = '\0';
    return operand;
}

static Instruction *append_instruction(IrContext *ctx, IrOpcode op, const char *result, Operand lhs, Operand rhs)
{
    Instruction *instruction = (Instruction *)calloc(1, sizeof(Instruction));
    if (!instruction) {
        fprintf(stderr, "Out of memory while allocating IR instruction\n");
        exit(EXIT_FAILURE);
    }

    instruction->op = op;
    if (result) {
        strncpy(instruction->result, result, IR_NAME_MAX - 1);
        instruction->result[IR_NAME_MAX - 1] = '\0';
    }
    instruction->lhs = lhs;
    instruction->rhs = rhs;
    instruction->next = NULL;

    if (!ctx->head) {
        ctx->head = instruction;
    } else {
        ctx->tail->next = instruction;
    }

    ctx->tail = instruction;
    return instruction;
}

static Operand emit_binary_operation(IrContext *ctx, IrOpcode op, Operand lhs, Operand rhs)
{
    char temp_name[IR_NAME_MAX];
    snprintf(temp_name, sizeof(temp_name), "t%d", ctx->temp_index++);
    append_instruction(ctx, op, temp_name, lhs, rhs);
    return operand_from_name(temp_name);
}

void execute_ir(const Instruction *head)
{
    const Instruction *current = head;
    ValueEntry *symbols = NULL;

    while (current) {
        switch (current->op) {
        case IR_OP_ASSIGN:
        {
            int value = operand_value(&current->lhs, symbols);
            set_symbol_value(&symbols, current->result, value);
            break;
        }
        case IR_OP_ADD:
        case IR_OP_SUB:
        case IR_OP_MUL:
        case IR_OP_DIV:
        {
            int lhs = operand_value(&current->lhs, symbols);
            int rhs = operand_value(&current->rhs, symbols);
            int result = 0;

            switch (current->op) {
            case IR_OP_ADD:
                result = lhs + rhs;
                break;
            case IR_OP_SUB:
                result = lhs - rhs;
                break;
            case IR_OP_MUL:
                result = lhs * rhs;
                break;
            case IR_OP_DIV:
                if (rhs == 0) {
                    fprintf(stderr, "Runtime error: division by zero\n");
                    free_symbols(symbols);
                    exit(EXIT_FAILURE);
                }
                result = lhs / rhs;
                break;
            default:
                break;
            }

            set_symbol_value(&symbols, current->result, result);
            break;
        }
        case IR_OP_PRINT:
        {
            int value = operand_value(&current->lhs, symbols);
            printf("%d\n", value);
            break;
        }
        case IR_OP_INPUT:
        {
            int value = 0;
            if (current->result[0] != '\0') {
                printf("%s? ", current->result);
            } else {
                printf("input? ");
            }
            fflush(stdout);
            if (scanf("%d", &value) != 1) {
                fprintf(stderr, "Runtime error: failed to read integer input\n");
                free_symbols(symbols);
                exit(EXIT_FAILURE);
            }
            set_symbol_value(&symbols, current->result, value);
            break;
        }
        default:
            break;
        }

        current = current->next;
    }

    free_symbols(symbols);
}

static int operand_value(const Operand *operand, ValueEntry *symbols)
{
    if (!operand) {
        return 0;
    }

    if (operand->is_constant) {
        return operand->value;
    }

    if (!operand->name[0]) {
        return 0;
    }

    ValueEntry *entry = find_symbol(symbols, operand->name);
    if (!entry) {
        return 0;
    }

    return entry->value;
}

static ValueEntry *find_symbol(ValueEntry *symbols, const char *name)
{
    ValueEntry *current = symbols;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void set_symbol_value(ValueEntry **symbols, const char *name, int value)
{
    if (!name || !symbols) {
        return;
    }

    ValueEntry *entry = find_symbol(*symbols, name);
    if (entry) {
        entry->value = value;
        return;
    }

    entry = (ValueEntry *)calloc(1, sizeof(ValueEntry));
    if (!entry) {
        fprintf(stderr, "Out of memory while storing symbol value\n");
        free_symbols(*symbols);
        exit(EXIT_FAILURE);
    }

    strncpy(entry->name, name, IR_NAME_MAX - 1);
    entry->name[IR_NAME_MAX - 1] = '\0';
    entry->value = value;
    entry->next = *symbols;
    *symbols = entry;
}

static void free_symbols(ValueEntry *symbols)
{
    while (symbols) {
        ValueEntry *next = symbols->next;
        free(symbols);
        symbols = next;
    }
}
