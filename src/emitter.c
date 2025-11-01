#include "emitter.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **items;
    size_t len;
    size_t cap;
} StringList;

static void *xrealloc_checked(void *ptr, size_t n) {
    void *p = realloc(ptr, n);
    if (!p && n != 0) {
        fprintf(stderr, "cfg2yacc: out of memory (emitter)\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static char *xstrdup_checked(const char *src) {
    if (!src) return NULL;
    size_t n = strlen(src);
    char *copy = (char *)malloc(n + 1);
    if (!copy) {
        fprintf(stderr, "cfg2yacc: out of memory (strdup)\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copy, src, n + 1);
    return copy;
}

static void string_list_free(StringList *list) {
    if (!list) return;
    for (size_t i = 0; i < list->len; ++i) free(list->items[i]);
    free(list->items);
    list->items = NULL;
    list->len = 0;
    list->cap = 0;
}

static int string_list_contains(const StringList *list, const char *value) {
    for (size_t i = 0; i < list->len; ++i) {
        if (strcmp(list->items[i], value) == 0) return 1;
    }
    return 0;
}

static void string_list_append_unique(StringList *list, const char *value) {
    if (!value || string_list_contains(list, value)) return;
    if (list->len + 1 > list->cap) {
        list->cap = list->cap ? list->cap * 2 : 8;
        list->items = (char **)xrealloc_checked(list->items, list->cap * sizeof(char *));
    }
    list->items[list->len++] = xstrdup_checked(value);
}

static int is_literal_token(const char *sym) {
    size_t n = sym ? strlen(sym) : 0;
    return n >= 2 && sym[0] == '\'' && sym[n - 1] == '\'';
}

static int is_all_caps(const char *sym) {
    int seen_alpha = 0;
    for (const char *p = sym; p && *p; ++p) {
        if (isalpha((unsigned char)*p)) {
            seen_alpha = 1;
            if (!isupper((unsigned char)*p)) return 0;
        }
    }
    return seen_alpha;
}

static void write_ast_stub(const char *ast_h, const char *ast_c) {
    // Only write to ast.c, not ast.h (which should already exist)
    (void)ast_h;  // Unused parameter
    if (ast_c) {
        FILE *fc = fopen(ast_c, "wb");
        if (fc) {
            fputs("#include \"ast.h\"\n#include <stddef.h>\n\nAST *ast_make(ASTKind kind) { (void)kind; return NULL; }\nvoid ast_free(AST *node) { (void)node; }\n", fc);
            fclose(fc);
        }
    }
}

static void write_flex_stub(const Grammar *g, const char *out_l) {
    if (!out_l) return;
    FILE *l = fopen(out_l, "wb");
    if (!l) {
        perror("cfg2yacc: fopen lex");
        return;
    }
    fputs("%{\n#include \"y.tab.h\"\n#include <stdlib.h>\n%}\n\n%option noyywrap\n%option reentrant\n\n", l);

    for (size_t i = 0; i < g->term_len; ++i) {
        const char *tok = g->terms[i];
        if (is_literal_token(tok)) {
            // For literal tokens like '+', extract the character
            size_t len = strlen(tok);
            if (len >= 3 && tok[0] == '\'' && tok[len-1] == '\'') {
                char c = tok[1];
                char flex_pattern[4] = {0};
                char return_char = c;
                
                // Handle escape sequences
                if (c == '\\' && len >= 4) {
                    char esc = tok[2];
                    switch (esc) {
                        case 'n': return_char = '\n'; flex_pattern[0] = '\\'; flex_pattern[1] = 'n'; break;
                        case 't': return_char = '\t'; flex_pattern[0] = '\\'; flex_pattern[1] = 't'; break;
                        case 'r': return_char = '\r'; flex_pattern[0] = '\\'; flex_pattern[1] = 'r'; break;
                        case '\\': return_char = '\\'; flex_pattern[0] = '\\'; flex_pattern[1] = '\\'; break;
                        default: return_char = esc; flex_pattern[0] = '\\'; flex_pattern[1] = esc; break;
                    }
                } else {
                    // For special Flex characters, use character classes
                    if (strchr("+*?|()[]{}$^.\\", c)) {
                        // Use character class for special characters
                        fprintf(l, "[%c]    return '%c';\n", c, c);
                        continue;
                    } else {
                        flex_pattern[0] = c;
                    }
                }
                fprintf(l, "%s    return '%c';\n", flex_pattern, return_char);
            }
        } else if (is_all_caps(tok)) {
            // For ALL_CAPS tokens, use as-is (but need to define them as tokens first)
            fprintf(l, "%s    return %s;\n", tok, tok);
        }
    }

    int has_number = 0;
    int has_ident = 0;
    for (size_t i = 0; i < g->term_len; ++i) {
        if (strcmp(g->terms[i], "NUMBER") == 0) has_number = 1;
        if (strcmp(g->terms[i], "IDENT") == 0 || strcmp(g->terms[i], "ID") == 0) has_ident = 1;
    }
    if (has_number) fputs("[0-9]+    return NUMBER;\n", l);
    if (has_ident) fputs("[A-Za-z_][A-Za-z0-9_]*    return IDENT;\n", l);
    fputs("[\\t\\r\\n ]+    ;\n", l);
    fputs(".          return yytext[0];\n\n%%\n", l);
    fclose(l);
}

static void emit_prologue(FILE *y) {
    fputs("%{\n#include \"ast.h\"\n#include <stdio.h>\ntypedef void* YYSTYPE;\nextern int yylex(YYSTYPE *);\nextern int yyparse(void);\nvoid yyerror(const char *msg);\n%}\n\n", y);
    fputs("%define api.pure full\n%define parse.error verbose\n%define api.value.type {void*}\n\n", y);
}

static void emit_tokens(FILE *y, const Grammar *g) {
    int printed = 0;
    for (size_t i = 0; i < g->term_len; ++i) {
        const char *tok = g->terms[i];
        if (is_literal_token(tok)) continue;
        if (!printed) {
            fputs("%token ", y);
            printed = 1;
        } else {
            fputc(' ', y);
        }
        fputs(tok, y);
    }
    if (printed) fputs("\n\n", y);
}

static void emit_start(FILE *y, const Grammar *g) {
    if (g->start_symbol) {
        fprintf(y, "%%start %s\n\n", g->start_symbol);
    }
}

static void emit_rules(FILE *y, const Grammar *g) {
    StringList groups = {0};
    for (size_t i = 0; i < g->prod_len; ++i) {
        string_list_append_unique(&groups, g->prods[i].lhs);
    }

    fputs("%%\n", y);
    for (size_t gi = 0; gi < groups.len; ++gi) {
        const char *lhs = groups.items[gi];
        fprintf(y, "%s:\n", lhs);
        int first_alt = 1;
        for (size_t pi = 0; pi < g->prod_len; ++pi) {
            const Production *prod = &g->prods[pi];
            if (strcmp(prod->lhs, lhs) != 0) continue;
            fputs(first_alt ? "    " : "  | ", y);
            first_alt = 0;
            if (prod->rhs_len == 0) {
                fputs("/* empty */", y);
            } else {
                for (size_t ri = 0; ri < prod->rhs_len; ++ri) {
                    if (ri) fputc(' ', y);
                    fputs(prod->rhs[ri].text, y);
                }
            }
            fputs(" { $$ = NULL; }\n", y);
        }
        fputs("  ;\n\n", y);
    }
    string_list_free(&groups);
}

static void emit_epilogue(FILE *y) {
    fputs("%%\n\n", y);
    fputs("void yyerror(const char *s) { fprintf(stderr, \"parse error: %s\\n\", s); }\n", y);
    fputs("int main(void) { return yyparse(); }\n", y);
}

void emit_yacc(Grammar *g, const char *out_y, const char *out_l,
               const char *ast_h, const char *ast_c) {
    if (!g || !out_y) return;

    FILE *y = fopen(out_y, "wb");
    if (!y) {
        perror("cfg2yacc: fopen yacc");
        return;
    }

    emit_prologue(y);
    emit_tokens(y, g);
    emit_start(y, g);
    emit_rules(y, g);
    emit_epilogue(y);

    fclose(y);

    write_flex_stub(g, out_l);
    write_ast_stub(ast_h, ast_c);
}


