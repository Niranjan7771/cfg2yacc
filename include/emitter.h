#ifndef EMITTER_H
#define EMITTER_H

#include "grammar.h"

void emit_yacc(Grammar *g, const char *out_y, const char *out_l,
               const char *ast_h, const char *ast_c);

#endif


