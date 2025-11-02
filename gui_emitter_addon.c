/*
 * Enhanced emitter functions for GUI support
 * This generates JSON-formatted parse tree output
 */

#include <stdio.h>
#include <string.h>

// Add this to the YACC prologue for JSON output
static const char* json_prologue = 
"%{\n"
"#include \"ast.h\"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"extern int yylex(void);\n"
"extern int yyparse(void);\n"
"void yyerror(const char *msg);\n"
"static int parse_step = 0;\n"
"static int json_mode = 0;\n"
"%}\n\n"
"%define parse.error verbose\n\n";

// Enhanced epilogue with JSON support
static const char* json_epilogue = 
"%%\n\n"
"void yyerror(const char *s) {\n"
"    if (json_mode) {\n"
"        printf(\"{\\\"error\\\": \\\"%s\\\", \\\"step\\\": %d}\\n\", s, parse_step);\n"
"    } else {\n"
"        fprintf(stderr, \"❌ Parse error: %s\\n\", s);\n"
"    }\n"
"}\n\n"
"int main(int argc, char *argv[]) {\n"
"    // Check for JSON mode\n"
"    if (argc > 1 && strcmp(argv[1], \"--json\") == 0) {\n"
"        json_mode = 1;\n"
"        printf(\"{\\\"parse_tree\\\": [\\n\");\n"
"    } else {\n"
"        printf(\"🌳 Parse Tree:\\n\");\n"
"    }\n"
"    \n"
"    int result = yyparse();\n"
"    \n"
"    if (json_mode) {\n"
"        printf(\"], \\\"success\\\": %s, \\\"steps\\\": %d}\\n\", \n"
"               result == 0 ? \"true\" : \"false\", parse_step);\n"
"    } else {\n"
"        if (result == 0) {\n"
"            printf(\"✅ Input parsed successfully!\\n\");\n"
"        } else {\n"
"            printf(\"❌ Parse failed!\\n\");\n"
"        }\n"
"    }\n"
"    return result;\n"
"}\n";

// Enhanced rule generation with JSON support
void emit_rule_with_json(FILE *y, const char *lhs, const char **rhs_symbols, int rhs_count, int is_empty) {
    if (is_empty) {
        fprintf(y, "    /* empty */ {\n");
        fprintf(y, "        if (json_mode) {\n");
        fprintf(y, "            printf(\"%%s{\\\"step\\\": %%d, \\\"rule\\\": \\\"%s -> ε\\\"}\", \n");
        fprintf(y, "                   parse_step > 0 ? \",\\n  \" : \"  \", ++parse_step);\n");
        fprintf(y, "        } else {\n");
        fprintf(y, "            printf(\"  %s -> ε\\n\");\n", lhs);
        fprintf(y, "        }\n");
        fprintf(y, "        $$ = 0;\n");
        fprintf(y, "    }");
    } else {
        fprintf(y, " {\n");
        fprintf(y, "        if (json_mode) {\n");
        fprintf(y, "            printf(\"%%s{\\\"step\\\": %%d, \\\"rule\\\": \\\"%s ->", lhs);
        for (int i = 0; i < rhs_count; i++) {
            fprintf(y, " %s", rhs_symbols[i]);
        }
        fprintf(y, "\\\"}\", \n");
        fprintf(y, "                   parse_step > 0 ? \",\\n  \" : \"  \", ++parse_step);\n");
        fprintf(y, "        } else {\n");
        fprintf(y, "            printf(\"  %s ->", lhs);
        for (int i = 0; i < rhs_count; i++) {
            fprintf(y, " %s", rhs_symbols[i]);
        }
        fprintf(y, "\\n\");\n");
        fprintf(y, "        }\n");
        fprintf(y, "        $$ = 0;\n");
        fprintf(y, "    }");
    }
}