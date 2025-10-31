#ifndef CFG2YACC_GRAMMAR_H
#define CFG2YACC_GRAMMAR_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    SYMBOL_NONTERM = 0,
    SYMBOL_TERM = 1
} SymbolKind;

typedef struct {
    char *text;
    SymbolKind kind;
} Symbol;

typedef enum {
    RHS_NODE_SYMBOL,
    RHS_NODE_SEQUENCE,
    RHS_NODE_ALTERNATION,
    RHS_NODE_OPTIONAL,
    RHS_NODE_KLEENE_STAR,
    RHS_NODE_KLEENE_PLUS
} RhsNodeKind;

typedef struct RhsNode {
    RhsNodeKind kind;
    int line;
    union {
        struct {
            char *name;
            SymbolKind kind;
        } symbol;
        struct {
            struct RhsNode **items;
            size_t len;
        } list;
        struct RhsNode *unary;
    } data;
} RhsNode;

typedef struct {
    char *lhs;
    RhsNode *expr;
    int line;
} RawProduction;

typedef struct {
    char *lhs;
    Symbol *rhs;
    size_t rhs_len;
} Production;

typedef struct Grammar {
    char *start_symbol;

    RawProduction *raw;
    size_t raw_len;
    size_t raw_cap;

    Production *prods;
    size_t prod_len;
    size_t prod_cap;

    char **nonterms;
    size_t nonterm_len;
    size_t nonterm_cap;

    char **terms;
    size_t term_len;
    size_t term_cap;

    unsigned long gen_counter;
} Grammar;

Grammar *grammar_create(void);
Grammar *grammar_read_file(const char *path);
void grammar_free(Grammar *g);

void grammar_dump_raw(const Grammar *g);
void grammar_dump(const Grammar *g);

void grammar_clear_productions(Grammar *g);
void grammar_add_production(Grammar *g, const char *lhs, const Symbol *rhs, size_t rhs_len);

void grammar_register_symbol(Grammar *g, const char *name, SymbolKind kind);
bool grammar_is_terminal_name(const Grammar *g, const char *name);
bool grammar_is_nonterminal_name(const Grammar *g, const char *name);

Symbol symbol_make(const char *text, SymbolKind kind);
void symbol_free(Symbol *sym);
void symbol_array_free(Symbol *syms, size_t len);

#endif /* CFG2YACC_GRAMMAR_H */


