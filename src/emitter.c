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

static void write_flex_stub(const Grammar *g, const char *out_l, const char *out_y) {
    if (!out_l) return;
    FILE *l = fopen(out_l, "wb");
    if (!l) {
        perror("cfg2yacc: fopen lex");
        return;
    }
    
    // Extract base name from YACC output file for header inclusion
    char header_name[256] = "y.tab.h";  // default
    if (out_y) {
        const char *base_name = strrchr(out_y, '/');
        base_name = base_name ? base_name + 1 : out_y;
        snprintf(header_name, sizeof(header_name), "%.240s", base_name);
        char *dot = strrchr(header_name, '.');
        if (dot) {
            strcpy(dot, ".tab.h");
        } else {
            strcat(header_name, ".tab.h");
        }
    }
    
    // Header section with proper includes and options
    fputs("%{\n", l);
    fprintf(l, "#include \"%s\"\n", header_name);
    fputs("#include <stdlib.h>\n%}\n\n", l);
    fputs("%option noyywrap\n\n", l);  // Removed reentrant option
    
    // Rules section starts here
    fputs("%%\n\n", l);
    
    // Generate patterns for literal tokens (quoted strings)
    for (size_t i = 0; i < g->term_len; ++i) {
        const char *tok = g->terms[i];
        if (is_literal_token(tok)) {
            size_t len = strlen(tok);
            if (len >= 3 && tok[0] == '\'' && tok[len-1] == '\'') {
                char c = tok[1];
                
                // Handle escape sequences
                if (c == '\\' && len >= 4) {
                    char esc = tok[2];
                    switch (esc) {
                        case 'n': fprintf(l, "\\n          return '\\n';\n"); break;
                        case 't': fprintf(l, "\\t          return '\\t';\n"); break;
                        case 'r': fprintf(l, "\\r          return '\\r';\n"); break;
                        case '\\': fprintf(l, "\\\\         return '\\\\';\n"); break;
                        default: fprintf(l, "\\%c          return '%c';\n", esc, esc); break;
                    }
                } else {
                    // For special Flex regex characters, escape them properly
                    if (strchr("+*?|()[]{}$^./\\", c)) {  // Added '/' to the list
                        fprintf(l, "\\%c          return '%c';\n", c, c);
                    } else {
                        fprintf(l, "%c           return '%c';\n", c, c);
                    }
                }
            }
        }
    }
    
    // Generate patterns for common token types
    int has_number = 0;
    int has_ident = 0;
    for (size_t i = 0; i < g->term_len; ++i) {
        if (strcmp(g->terms[i], "NUMBER") == 0) has_number = 1;
        if (strcmp(g->terms[i], "IDENT") == 0 || strcmp(g->terms[i], "ID") == 0) has_ident = 1;
    }
    
    if (has_number) fputs("[0-9]+     return NUMBER;\n", l);
    if (has_ident) {
        // Use ID if available, otherwise IDENT
        int found_id = 0;
        for (size_t j = 0; j < g->term_len; ++j) {
            if (strcmp(g->terms[j], "ID") == 0) {
                found_id = 1;
                break;
            }
        }
        if (found_id) {
            fputs("[A-Za-z_][A-Za-z0-9_]*  return ID;\n", l);
        } else {
            fputs("[A-Za-z_][A-Za-z0-9_]*  return IDENT;\n", l);
        }
    }
    
    // Generate keyword patterns for other ALL_CAPS tokens (excluding NUMBER/IDENT)
    for (size_t i = 0; i < g->term_len; ++i) {
        const char *tok = g->terms[i];
        if (is_all_caps(tok) && 
            strcmp(tok, "NUMBER") != 0 && 
            strcmp(tok, "IDENT") != 0 && 
            strcmp(tok, "ID") != 0) {
            // Convert to lowercase for keyword matching
            fprintf(l, "%s       return %s;\n", tok, tok);
        }
    }
    
    // Whitespace and catch-all rules
    fputs("[ \\t\\r\\n]+  /* skip whitespace */;\n", l);
    fputs(".         return yytext[0];\n\n", l);
    
    // End of rules section and user code section
    fputs("%%\n\n", l);
    fputs("/* User code section - add any additional C code here */\n", l);
    
    fclose(l);
}

static void emit_prologue(FILE *y) {
    fputs("%{\n#include \"ast.h\"\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n", y);
    fputs("extern int yylex(void);\n", y);
    fputs("extern int yyparse(void);\n", y);
    fputs("void yyerror(const char *msg);\n", y);
    fputs("static int parse_step = 0;\n", y);
    fputs("static int json_mode = 0;\n", y);
    fputs("%}\n\n", y);
    fputs("%define parse.error verbose\n\n", y);
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
                fprintf(y, " {\n");
                fprintf(y, "        if (json_mode) {\n");
                fprintf(y, "            printf(\"%%s{\\\"step\\\": %%d, \\\"rule\\\": \\\"%s -> ε\\\"}\",\n", lhs);
                fprintf(y, "                   parse_step > 0 ? \",\\n  \" : \"\\n  \", ++parse_step);\n");
                fprintf(y, "        } else {\n");
                fprintf(y, "            printf(\"  %s -> ε\\n\");\n", lhs);
                fprintf(y, "        }\n");
                fprintf(y, "        $$ = 0;\n");
                fprintf(y, "    }");
            } else {
                for (size_t ri = 0; ri < prod->rhs_len; ++ri) {
                    if (ri) fputc(' ', y);
                    fputs(prod->rhs[ri].text, y);
                }
                // Generate parse tree output with JSON support
                fprintf(y, " {\n");
                fprintf(y, "        if (json_mode) {\n");
                fprintf(y, "            printf(\"%%s{\\\"step\\\": %%d, \\\"rule\\\": \\\"%s ->", lhs);
                for (size_t ri = 0; ri < prod->rhs_len; ++ri) {
                    fprintf(y, " %s", prod->rhs[ri].text);
                }
                fprintf(y, "\\\"}\",\n");
                fprintf(y, "                   parse_step > 0 ? \",\\n  \" : \"\\n  \", ++parse_step);\n");
                fprintf(y, "        } else {\n");
                fprintf(y, "            printf(\"  %s ->", lhs);
                for (size_t ri = 0; ri < prod->rhs_len; ++ri) {
                    fprintf(y, " %s", prod->rhs[ri].text);
                }
                fprintf(y, "\\n\");\n");
                fprintf(y, "        }\n");
                fprintf(y, "        $$ = 0;\n");
                fprintf(y, "    }");
            }
            fputs("\n", y);
        }
        fputs("  ;\n\n", y);
    }
    string_list_free(&groups);
}

static void emit_epilogue(FILE *y) {
    fputs("%%\n\n", y);
    fputs("void yyerror(const char *s) {\n", y);
    fputs("    if (json_mode) {\n", y);
    fputs("        printf(\"{\\\"error\\\": \\\"%s\\\", \\\"step\\\": %d}\\n\", s, parse_step);\n", y);
    fputs("    } else {\n", y);
    fputs("        fprintf(stderr, \"❌ Parse error: %s\\n\", s);\n", y);
    fputs("    }\n", y);
    fputs("}\n\n", y);
    fputs("int main(int argc, char *argv[]) {\n", y);
    fputs("    // Check for JSON mode\n", y);
    fputs("    if (argc > 1 && strcmp(argv[1], \"--json\") == 0) {\n", y);
    fputs("        json_mode = 1;\n", y);
    fputs("        printf(\"{\\\"parse_tree\\\": [\");\n", y);
    fputs("    } else {\n", y);
    fputs("        printf(\"🌳 Parse Tree:\\n\");\n", y);
    fputs("    }\n", y);
    fputs("    \n", y);
    fputs("    int result = yyparse();\n", y);
    fputs("    \n", y);
    fputs("    if (json_mode) {\n", y);
    fputs("        printf(\"], \\\"success\\\": %s, \\\"steps\\\": %d}\\n\",\n", y);
    fputs("               result == 0 ? \"true\" : \"false\", parse_step);\n", y);
    fputs("    } else {\n", y);
    fputs("        if (result == 0) {\n", y);
    fputs("            printf(\"✅ Input parsed successfully!\\n\");\n", y);
    fputs("        } else {\n", y);
    fputs("            printf(\"❌ Parse failed!\\n\");\n", y);
    fputs("        }\n", y);
    fputs("    }\n", y);
    fputs("    return result;\n", y);
    fputs("}\n", y);
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

    write_flex_stub(g, out_l, out_y);
    write_ast_stub(ast_h, ast_c);
}


