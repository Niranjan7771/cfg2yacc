#ifndef CFG2YACC_AST_H
#define CFG2YACC_AST_H

typedef enum {
    AST_UNKNOWN = 0
} ASTKind;

typedef struct AST {
    ASTKind kind;
} AST;

AST *ast_make(ASTKind kind);
void ast_free(AST *node);

#endif /* CFG2YACC_AST_H */


