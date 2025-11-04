#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    SECTION_NONE = 0,
    SECTION_LEX,
    SECTION_YACC
} Section;

typedef enum {
    LEX_ITEM_RULE = 0,
    LEX_ITEM_COMMENT,
    LEX_ITEM_BLANK
} LexItemType;

typedef struct {
    LexItemType type;
    char *name;
    char *pattern;
    char *text;
} LexItem;

typedef enum {
    GRAMMAR_ITEM_RULE = 0,
    GRAMMAR_ITEM_COMMENT,
    GRAMMAR_ITEM_BLANK
} GrammarItemType;

typedef struct {
    char **alternatives;
    size_t alt_count;
    size_t alt_capacity;
    char *lhs;
} GrammarRule;

typedef struct {
    GrammarItemType type;
    GrammarRule rule;
    char *text;
} GrammarItem;

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} TokenList;

static void handle_allocation_failure(void)
{
    fprintf(stderr, "Out of memory\n");
    exit(EXIT_FAILURE);
}

static char *dup_string(const char *input)
{
    size_t length = input ? strlen(input) : 0;
    char *copy = (char *)malloc(length + 1);
    if (!copy) {
        handle_allocation_failure();
    }
    if (length > 0 && input) {
        memcpy(copy, input, length);
    }
    copy[length] = '\0';
    return copy;
}

static char *trim(char *text)
{
    if (!text) {
        return NULL;
    }

    while (*text && isspace((unsigned char)*text)) {
        text++;
    }

    if (*text == '\0') {
        return text;
    }

    char *end = text + strlen(text) - 1;
    while (end > text && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return text;
}

static void ensure_lex_capacity(LexItem **items, size_t *capacity, size_t needed)
{
    if (*capacity >= needed) {
        return;
    }
    size_t new_capacity = *capacity ? *capacity * 2 : 16;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    LexItem *resized = (LexItem *)realloc(*items, new_capacity * sizeof(LexItem));
    if (!resized) {
        handle_allocation_failure();
    }
    *items = resized;
    *capacity = new_capacity;
}

static void ensure_grammar_capacity(GrammarItem **items, size_t *capacity, size_t needed)
{
    if (*capacity >= needed) {
        return;
    }
    size_t new_capacity = *capacity ? *capacity * 2 : 16;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    GrammarItem *resized = (GrammarItem *)realloc(*items, new_capacity * sizeof(GrammarItem));
    if (!resized) {
        handle_allocation_failure();
    }
    *items = resized;
    *capacity = new_capacity;
}

static void ensure_alt_capacity(GrammarRule *rule, size_t needed)
{
    if (rule->alt_capacity >= needed) {
        return;
    }
    size_t new_capacity = rule->alt_capacity ? rule->alt_capacity * 2 : 4;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    char **resized = (char **)realloc(rule->alternatives, new_capacity * sizeof(char *));
    if (!resized) {
        handle_allocation_failure();
    }
    rule->alternatives = resized;
    rule->alt_capacity = new_capacity;
}

static void ensure_token_capacity(TokenList *tokens, size_t needed)
{
    if (tokens->capacity >= needed) {
        return;
    }
    size_t new_capacity = tokens->capacity ? tokens->capacity * 2 : 16;
    while (new_capacity < needed) {
        new_capacity *= 2;
    }
    char **resized = (char **)realloc(tokens->items, new_capacity * sizeof(char *));
    if (!resized) {
        handle_allocation_failure();
    }
    tokens->items = resized;
    tokens->capacity = new_capacity;
}

static int equals_ignore_case(const char *a, const char *b)
{
    if (!a || !b) {
        return 0;
    }
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int contains_ignore_case(const char *haystack, const char *needle)
{
    if (!haystack || !needle) {
        return 0;
    }
    size_t needle_len = strlen(needle);
    if (needle_len == 0) {
        return 1;
    }
    for (const char *cursor = haystack; *cursor; ++cursor) {
        if (tolower((unsigned char)*cursor) == tolower((unsigned char)*needle)) {
            size_t index = 0;
            while (needle[index] && cursor[index] && tolower((unsigned char)cursor[index]) == tolower((unsigned char)needle[index])) {
                index++;
            }
            if (needle[index] == '\0') {
                return 1;
            }
        }
    }
    return 0;
}

static int is_skip_token(const char *name)
{
    return equals_ignore_case(name, "WHITESPACE") ||
           equals_ignore_case(name, "WS") ||
            contains_ignore_case(name, "WHITESPACE") ||
           contains_ignore_case(name, "COMMENT");
}

static int is_number_token(const char *name, const char *pattern)
{
    if (equals_ignore_case(name, "NUMBER") || equals_ignore_case(name, "INT") || equals_ignore_case(name, "INTEGER")) {
        return 1;
    }
    if (pattern && (strstr(pattern, "0-9") || strstr(pattern, "[:digit:]"))) {
        return 1;
    }
    return 0;
}

static int token_exists(const TokenList *tokens, const char *name)
{
    for (size_t i = 0; i < tokens->count; ++i) {
        if (equals_ignore_case(tokens->items[i], name)) {
            return 1;
        }
    }
    return 0;
}

static void add_token(TokenList *tokens, const char *name)
{
    if (!tokens || !name || is_skip_token(name)) {
        return;
    }
    if (token_exists(tokens, name)) {
        return;
    }
    ensure_token_capacity(tokens, tokens->count + 1);
    tokens->items[tokens->count++] = dup_string(name);
}

static void grammar_rule_add_alt(GrammarRule *rule, const char *rhs)
{
    ensure_alt_capacity(rule, rule->alt_count + 1);
    rule->alternatives[rule->alt_count++] = dup_string(rhs);
}

static void free_lex_items(LexItem *items, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        free(items[i].name);
        free(items[i].pattern);
        free(items[i].text);
    }
    free(items);
}

static void free_grammar_items(GrammarItem *items, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        if (items[i].type == GRAMMAR_ITEM_RULE) {
            for (size_t j = 0; j < items[i].rule.alt_count; ++j) {
                free(items[i].rule.alternatives[j]);
            }
            free(items[i].rule.alternatives);
            free(items[i].rule.lhs);
        }
        free(items[i].text);
    }
    free(items);
}

static void free_tokens(TokenList *tokens)
{
    for (size_t i = 0; i < tokens->count; ++i) {
        free(tokens->items[i]);
    }
    free(tokens->items);
}

static void write_lexer(const char *path, const LexItem *items, size_t count)
{
    FILE *out = fopen(path, "w");
    if (!out) {
        perror("Failed to open lexer.l for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "%%{\n");
    fprintf(out, "#include \"y.tab.h\"\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n");
    fprintf(out, "%%}\n\n");

    fprintf(out, "%%option noyywrap\n\n");
    fputs("%%\n\n", out);

    for (size_t i = 0; i < count; ++i) {
        const LexItem *item = &items[i];
        switch (item->type) {
        case LEX_ITEM_BLANK:
            fputc('\n', out);
            break;
        case LEX_ITEM_COMMENT:
            fprintf(out, "%s\n", item->text ? item->text : "");
            break;
        case LEX_ITEM_RULE:
            if (!item->name || !item->pattern) {
                break;
            }
            if (is_skip_token(item->name)) {
                fprintf(out, "%s\t{ /* skip %s */ }\n", item->pattern, item->name);
            } else if (is_number_token(item->name, item->pattern)) {
                fprintf(out, "%s\t{ yylval = (int)strtol(yytext, NULL, 10); return %s; }\n", item->pattern, item->name);
            } else {
                fprintf(out, "%s\t{ return %s; }\n", item->pattern, item->name);
            }
            break;
        }
    }

    fputs(".\t{ return yytext[0]; }\n", out);
    fputs("%%\n", out);

    fclose(out);
}

static void write_parser(const char *path, const GrammarItem *items, size_t count, const TokenList *tokens)
{
    FILE *out = fopen(path, "w");
    if (!out) {
        perror("Failed to open parser.y for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "%%{\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "extern int yylex(void);\n");
    fprintf(out, "extern FILE *yyin;\n");
    fprintf(out, "void yyerror(const char *message);\n");
    fprintf(out, "%%}\n\n");

    if (tokens && tokens->count > 0) {
        fputs("%token", out);
        for (size_t i = 0; i < tokens->count; ++i) {
            fprintf(out, " %s", tokens->items[i]);
        }
        fputs("\n\n", out);
    }

    fputs("%%\n\n", out);

    for (size_t i = 0; i < count; ++i) {
        const GrammarItem *item = &items[i];
        switch (item->type) {
        case GRAMMAR_ITEM_BLANK:
            fputc('\n', out);
            break;
        case GRAMMAR_ITEM_COMMENT:
            fprintf(out, "%s\n", item->text ? item->text : "");
            break;
        case GRAMMAR_ITEM_RULE:
            if (!item->rule.lhs || item->rule.alt_count == 0) {
                break;
            }
            fprintf(out, "%s:\n", item->rule.lhs);
            for (size_t alt = 0; alt < item->rule.alt_count; ++alt) {
                if (alt == 0) {
                    fprintf(out, "    %s\n", item->rule.alternatives[alt]);
                } else {
                    fprintf(out, "  | %s\n", item->rule.alternatives[alt]);
                }
            }
            fputs(";\n\n", out);
            break;
        }
    }

    fputs("%%\n\n", out);

    fputs("int main(int argc, char **argv)\n", out);
    fputs("{\n", out);
    fputs("    if (argc != 2) {\n", out);
    fputs("        fprintf(stderr, \"Usage: %s <source-file>\\n\", argc > 0 ? argv[0] : \"parser\");\n", out);
    fputs("        return EXIT_FAILURE;\n", out);
    fputs("    }\n\n", out);
    fputs("    FILE *input = fopen(argv[1], \"r\");\n", out);
    fputs("    if (!input) {\n", out);
    fputs("        perror(\"Failed to open source file\");\n", out);
    fputs("        return EXIT_FAILURE;\n", out);
    fputs("    }\n\n", out);
    fputs("    yyin = input;\n", out);
    fputs("    int status = yyparse();\n", out);
    fputs("    fclose(input);\n\n", out);
    fputs("    return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;\n", out);
    fputs("}\n\n", out);

    fputs("void yyerror(const char *message)\n", out);
    fputs("{\n", out);
    fputs("    fprintf(stderr, \"Parse error: %s\\n\", message);\n", out);
    fputs("}\n", out);

    fclose(out);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <language.def>\n", argc > 0 ? argv[0] : "generator");
        return EXIT_FAILURE;
    }

    const char *definition_path = argv[1];
    FILE *definition = fopen(definition_path, "r");
    if (!definition) {
        perror("Failed to open definition file");
        return EXIT_FAILURE;
    }

    LexItem *lex_items = NULL;
    size_t lex_count = 0;
    size_t lex_capacity = 0;

    GrammarItem *grammar_items = NULL;
    size_t grammar_count = 0;
    size_t grammar_capacity = 0;

    TokenList tokens = {0};

    Section section = SECTION_NONE;
    size_t last_rule_index = (size_t)(-1);
    char line_buffer[4096];

    while (fgets(line_buffer, sizeof(line_buffer), definition)) {
        size_t length = strlen(line_buffer);
        while (length > 0 && (line_buffer[length - 1] == '\n' || line_buffer[length - 1] == '\r')) {
            line_buffer[--length] = '\0';
        }

        char *line_copy = dup_string(line_buffer);
        char *trimmed = trim(line_copy);

        if (strcmp(trimmed, "%%LEX") == 0) {
            section = SECTION_LEX;
            free(line_copy);
            continue;
        }

        if (strcmp(trimmed, "%%YACC") == 0) {
            section = SECTION_YACC;
            free(line_copy);
            continue;
        }

        if (section == SECTION_NONE) {
            free(line_copy);
            continue;
        }

        if (*trimmed == '\0') {
            if (section == SECTION_LEX) {
                ensure_lex_capacity(&lex_items, &lex_capacity, lex_count + 1);
                lex_items[lex_count].type = LEX_ITEM_BLANK;
                lex_items[lex_count].name = NULL;
                lex_items[lex_count].pattern = NULL;
                lex_items[lex_count].text = NULL;
                lex_count++;
            } else if (section == SECTION_YACC) {
                ensure_grammar_capacity(&grammar_items, &grammar_capacity, grammar_count + 1);
                grammar_items[grammar_count].type = GRAMMAR_ITEM_BLANK;
                grammar_items[grammar_count].text = NULL;
                grammar_count++;
            }
            free(line_copy);
            continue;
        }

        if (trimmed[0] == '/' && trimmed[1] == '/') {
            if (section == SECTION_LEX) {
                ensure_lex_capacity(&lex_items, &lex_capacity, lex_count + 1);
                lex_items[lex_count].type = LEX_ITEM_COMMENT;
                lex_items[lex_count].name = NULL;
                lex_items[lex_count].pattern = NULL;
                lex_items[lex_count].text = dup_string(trimmed);
                lex_count++;
            } else if (section == SECTION_YACC) {
                ensure_grammar_capacity(&grammar_items, &grammar_capacity, grammar_count + 1);
                grammar_items[grammar_count].type = GRAMMAR_ITEM_COMMENT;
                grammar_items[grammar_count].text = dup_string(trimmed);
                grammar_count++;
            }
            free(line_copy);
            continue;
        }

        if (section == SECTION_LEX) {
            char *rule_copy = dup_string(trimmed);
            char *cursor = rule_copy;
            while (*cursor && !isspace((unsigned char)*cursor)) {
                cursor++;
            }
            if (*cursor == '\0') {
                fprintf(stderr, "Invalid lexer rule: %s\n", trimmed);
                free(rule_copy);
                free(line_copy);
                free_lex_items(lex_items, lex_count);
                free_grammar_items(grammar_items, grammar_count);
                free_tokens(&tokens);
                fclose(definition);
                return EXIT_FAILURE;
            }
            *cursor = '\0';
            char *pattern = trim(cursor + 1);

            ensure_lex_capacity(&lex_items, &lex_capacity, lex_count + 1);
            lex_items[lex_count].type = LEX_ITEM_RULE;
            lex_items[lex_count].name = dup_string(rule_copy);
            lex_items[lex_count].pattern = dup_string(pattern);
            lex_items[lex_count].text = NULL;
            add_token(&tokens, rule_copy);
            lex_count++;

            free(rule_copy);
            free(line_copy);
            continue;
        }

        if (section == SECTION_YACC) {
            char *arrow = strstr(trimmed, "->");
            if (arrow) {
                *arrow = '\0';
                char *lhs_text = trim(trimmed);
                char *rhs_text = trim(arrow + 2);

                ensure_grammar_capacity(&grammar_items, &grammar_capacity, grammar_count + 1);
                GrammarItem *item = &grammar_items[grammar_count];
                item->type = GRAMMAR_ITEM_RULE;
                item->text = NULL;
                item->rule.lhs = dup_string(lhs_text);
                item->rule.alternatives = NULL;
                item->rule.alt_count = 0;
                item->rule.alt_capacity = 0;
                grammar_rule_add_alt(&item->rule, rhs_text);
                last_rule_index = grammar_count;
                grammar_count++;
                free(line_copy);
                continue;
            }

            if (trimmed[0] == '|') {
                if (last_rule_index == (size_t)(-1)) {
                    fprintf(stderr, "Dangling alternative without a preceding rule\n");
                    free(line_copy);
                    free_lex_items(lex_items, lex_count);
                    free_grammar_items(grammar_items, grammar_count);
                    free_tokens(&tokens);
                    fclose(definition);
                    return EXIT_FAILURE;
                }
                char *rhs_text = trim(trimmed + 1);
                grammar_rule_add_alt(&grammar_items[last_rule_index].rule, rhs_text);
                free(line_copy);
                continue;
            }

            /* Any other line is treated as a comment in the grammar section. */
            ensure_grammar_capacity(&grammar_items, &grammar_capacity, grammar_count + 1);
            grammar_items[grammar_count].type = GRAMMAR_ITEM_COMMENT;
            grammar_items[grammar_count].text = dup_string(trimmed);
            grammar_count++;
            free(line_copy);
            continue;
        }

        free(line_copy);
    }

    fclose(definition);

    if (lex_count == 0 || grammar_count == 0) {
        fprintf(stderr, "Definition file must contain non-empty %%LEX and %%YACC sections\n");
        free_lex_items(lex_items, lex_count);
        free_grammar_items(grammar_items, grammar_count);
        free_tokens(&tokens);
        return EXIT_FAILURE;
    }

    write_lexer("lexer.l", lex_items, lex_count);
    write_parser("parser.y", grammar_items, grammar_count, &tokens);

    free_lex_items(lex_items, lex_count);
    free_grammar_items(grammar_items, grammar_count);
    free_tokens(&tokens);

    return EXIT_SUCCESS;
}
