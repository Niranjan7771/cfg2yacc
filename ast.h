#ifndef CFG2YACC_AST_IMPL
#define CFG2YACC_AST_IMPL

#include "ast.h"

AST *ast_make(ASTKind kind) { (void)kind; return NULL; }
void ast_free(AST *node) { (void)node; }

#endif /* CFG2YACC_AST_IMPL */
