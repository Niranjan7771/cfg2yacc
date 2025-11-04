#include "optimizer.h"

void optimize_ir(Instruction **head)
{
    if (!head || !*head) {
        return;
    }

    Instruction *current = *head;

    while (current) {
        if ((current->op == IR_OP_ADD || current->op == IR_OP_SUB ||
             current->op == IR_OP_MUL || current->op == IR_OP_DIV) &&
            current->lhs.is_constant && current->rhs.is_constant) {
            int lhs = current->lhs.value;
            int rhs = current->rhs.value;
            int result = 0;
            int foldable = 1;

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
                    foldable = 0;
                } else {
                    result = lhs / rhs;
                }
                break;
            default:
                foldable = 0;
                break;
            }

            if (foldable) {
                current->op = IR_OP_ASSIGN;
                current->lhs.is_constant = 1;
                current->lhs.value = result;
                current->lhs.name[0] = '\0';
                current->rhs.is_constant = 0;
                current->rhs.value = 0;
                current->rhs.name[0] = '\0';
            }
        }

        current = current->next;
    }
}
