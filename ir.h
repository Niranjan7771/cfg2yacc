#ifndef IR_H
#define IR_H

#include "ast.h"

#define IR_NAME_MAX 32

typedef enum {
    IR_OP_ASSIGN = 0,
    IR_OP_ADD,
    IR_OP_SUB,
    IR_OP_MUL,
    IR_OP_DIV,
    IR_OP_PRINT,
    IR_OP_INPUT
} IrOpcode;

typedef struct Operand {
    int is_constant;
    int value;
    char name[IR_NAME_MAX];
} Operand;

typedef struct Instruction {
    IrOpcode op;
    char result[IR_NAME_MAX];
    Operand lhs;
    Operand rhs;
    struct Instruction *next;
} Instruction;

Instruction *generate_ir(Node *root);
void print_ir(const Instruction *head);
void free_ir(Instruction *head);
const char *ir_opcode_name(IrOpcode op);
void execute_ir(const Instruction *head);

#endif /* IR_H */
