#include "grammar.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) (void)(x)

/* -------------------------------------------------------------------------- */
/* Memory helpers                                                             */
/* -------------------------------------------------------------------------- */

static void *xmalloc(size_t n) {
	void *p = malloc(n);
	if (!p && n) {
		fprintf(stderr, "cfg2yacc: out of memory (malloc %zu)\n", n);
		exit(EXIT_FAILURE);
	}
	return p;
}

static void *xcalloc(size_t count, size_t size) {
	void *p = calloc(count, size);
	if (!p && count && size) {
		fprintf(stderr, "cfg2yacc: out of memory (calloc %zu x %zu)\n", count, size);
		exit(EXIT_FAILURE);
	}
	return p;
}

static void *xrealloc(void *ptr, size_t size) {
	void *p = realloc(ptr, size);
	if (!p && size) {
		fprintf(stderr, "cfg2yacc: out of memory (realloc %zu)\n", size);
		exit(EXIT_FAILURE);
	}
	return p;
}

static char *xstrdup(const char *s) {
	if (!s) return NULL;
	size_t n = strlen(s);
	char *copy = (char *)xmalloc(n + 1);
	memcpy(copy, s, n + 1);
	return copy;
}

/* -------------------------------------------------------------------------- */
/* String set helpers                                                         */
/* -------------------------------------------------------------------------- */

static void string_array_free(char **arr, size_t len) {
	if (!arr) return;
	for (size_t i = 0; i < len; ++i) free(arr[i]);
	free(arr);
}

static void string_set_add(char ***arr, size_t *len, size_t *cap, const char *value) {
	if (!value || !*value) return;
	for (size_t i = 0; i < *len; ++i) {
		if (strcmp((*arr)[i], value) == 0) return;
	}
	if (*len + 1 > *cap) {
		*cap = *cap ? *cap * 2 : 8;
		*arr = (char **)xrealloc(*arr, *cap * sizeof(char *));
	}
	(*arr)[(*len)++] = xstrdup(value);
}

/* -------------------------------------------------------------------------- */
/* RHS node helpers                                                           */
/* -------------------------------------------------------------------------- */

static RhsNode *rhs_node_new(RhsNodeKind kind, int line) {
	RhsNode *node = (RhsNode *)xcalloc(1, sizeof(RhsNode));
	node->kind = kind;
	node->line = line;
	return node;
}

static RhsNode *rhs_node_symbol(const char *name, SymbolKind kind, int line) {
	RhsNode *node = rhs_node_new(RHS_NODE_SYMBOL, line);
	node->data.symbol.name = xstrdup(name);
	node->data.symbol.kind = kind;
	return node;
}

static RhsNode *rhs_node_sequence(RhsNode **items, size_t len, int line) {
	RhsNode *node = rhs_node_new(RHS_NODE_SEQUENCE, line);
	node->data.list.items = items;
	node->data.list.len = len;
	return node;
}

static RhsNode *rhs_node_alternation(RhsNode **items, size_t len, int line) {
	RhsNode *node = rhs_node_new(RHS_NODE_ALTERNATION, line);
	node->data.list.items = items;
	node->data.list.len = len;
	return node;
}

static RhsNode *rhs_node_unary(RhsNodeKind kind, RhsNode *child, int line) {
	RhsNode *node = rhs_node_new(kind, line);
	node->data.unary = child;
	return node;
}

static void rhs_node_free(RhsNode *node) {
	if (!node) return;
	switch (node->kind) {
		case RHS_NODE_SYMBOL:
			free(node->data.symbol.name);
			break;
		case RHS_NODE_SEQUENCE:
		case RHS_NODE_ALTERNATION:
			if (node->data.list.items) {
				for (size_t i = 0; i < node->data.list.len; ++i) {
					rhs_node_free(node->data.list.items[i]);
				}
				free(node->data.list.items);
			}
			break;
		case RHS_NODE_OPTIONAL:
		case RHS_NODE_KLEENE_STAR:
		case RHS_NODE_KLEENE_PLUS:
			rhs_node_free(node->data.unary);
			break;
	}
	free(node);
}

/* -------------------------------------------------------------------------- */
/* Symbol helpers                                                             */
/* -------------------------------------------------------------------------- */

Symbol symbol_make(const char *text, SymbolKind kind) {
	Symbol sym;
	sym.text = xstrdup(text);
	sym.kind = kind;
	return sym;
}

void symbol_free(Symbol *sym) {
	if (!sym) return;
	free(sym->text);
	sym->text = NULL;
}

void symbol_array_free(Symbol *syms, size_t len) {
	if (!syms) return;
	for (size_t i = 0; i < len; ++i) {
		free(syms[i].text);
		syms[i].text = NULL;
	}
	free(syms);
}

/* -------------------------------------------------------------------------- */
/* Grammar lifecycle                                                          */
/* -------------------------------------------------------------------------- */

Grammar *grammar_create(void) {
	Grammar *g = (Grammar *)xcalloc(1, sizeof(Grammar));
	return g;
}

static void grammar_append_raw(Grammar *g, const RawProduction *src) {
	if (g->raw_len + 1 > g->raw_cap) {
		g->raw_cap = g->raw_cap ? g->raw_cap * 2 : 8;
		g->raw = (RawProduction *)xrealloc(g->raw, g->raw_cap * sizeof(RawProduction));
	}
	g->raw[g->raw_len++] = *src;
}

void grammar_clear_productions(Grammar *g) {
	if (!g) return;
	for (size_t i = 0; i < g->prod_len; ++i) {
		Production *p = &g->prods[i];
		free(p->lhs);
		symbol_array_free(p->rhs, p->rhs_len);
	}
	free(g->prods);
	g->prods = NULL;
	g->prod_len = 0;
	g->prod_cap = 0;

	string_array_free(g->nonterms, g->nonterm_len);
	g->nonterms = NULL;
	g->nonterm_len = 0;
	g->nonterm_cap = 0;

	string_array_free(g->terms, g->term_len);
	g->terms = NULL;
	g->term_len = 0;
	g->term_cap = 0;
}

void grammar_register_symbol(Grammar *g, const char *name, SymbolKind kind) {
	if (!g || !name) return;
	if (kind == SYMBOL_NONTERM) {
		string_set_add(&g->nonterms, &g->nonterm_len, &g->nonterm_cap, name);
	} else {
		string_set_add(&g->terms, &g->term_len, &g->term_cap, name);
	}
}

bool grammar_is_terminal_name(const Grammar *g, const char *name) {
	if (!g || !name) return false;
	for (size_t i = 0; i < g->term_len; ++i) {
		if (strcmp(g->terms[i], name) == 0) return true;
	}
	return false;
}

bool grammar_is_nonterminal_name(const Grammar *g, const char *name) {
	if (!g || !name) return false;
	for (size_t i = 0; i < g->nonterm_len; ++i) {
		if (strcmp(g->nonterms[i], name) == 0) return true;
	}
	return false;
}

void grammar_add_production(Grammar *g, const char *lhs, const Symbol *rhs, size_t rhs_len) {
	if (!g || !lhs) return;
	if (g->prod_len + 1 > g->prod_cap) {
		g->prod_cap = g->prod_cap ? g->prod_cap * 2 : 16;
		g->prods = (Production *)xrealloc(g->prods, g->prod_cap * sizeof(Production));
	}
	Production *p = &g->prods[g->prod_len++];
	p->lhs = xstrdup(lhs);
	p->rhs_len = rhs_len;
	if (rhs_len) {
		p->rhs = (Symbol *)xcalloc(rhs_len, sizeof(Symbol));
		for (size_t i = 0; i < rhs_len; ++i) {
			p->rhs[i] = symbol_make(rhs[i].text, rhs[i].kind);
			grammar_register_symbol(g, p->rhs[i].text, p->rhs[i].kind);
		}
	} else {
		p->rhs = NULL;
	}
	grammar_register_symbol(g, lhs, SYMBOL_NONTERM);
}

void grammar_dump_raw(const Grammar *g) {
	if (!g) return;
	fprintf(stdout, "# Raw productions (%zu)\n", g->raw_len);
	for (size_t i = 0; i < g->raw_len; ++i) {
		const RawProduction *rp = &g->raw[i];
		fprintf(stdout, "%s ::=%s\n", rp->lhs, rp->expr ? "" : " /*null*/");
	}
}

static void dump_node(const RhsNode *node, int depth, FILE *out) {
	if (!node) return;
	for (int i = 0; i < depth; ++i) fputs("  ", out);
	switch (node->kind) {
		case RHS_NODE_SYMBOL:
			fprintf(out, "%s (%s)\n", node->data.symbol.name,
					node->data.symbol.kind == SYMBOL_NONTERM ? "nonterm" : "term");
			break;
		case RHS_NODE_SEQUENCE:
			fputs("SEQ\n", out);
			for (size_t i = 0; i < node->data.list.len; ++i) {
				dump_node(node->data.list.items[i], depth + 1, out);
			}
			break;
		case RHS_NODE_ALTERNATION:
			fputs("ALT\n", out);
			for (size_t i = 0; i < node->data.list.len; ++i) {
				dump_node(node->data.list.items[i], depth + 1, out);
			}
			break;
		case RHS_NODE_OPTIONAL:
			fputs("OPTIONAL\n", out);
			dump_node(node->data.unary, depth + 1, out);
			break;
		case RHS_NODE_KLEENE_STAR:
			fputs("STAR\n", out);
			dump_node(node->data.unary, depth + 1, out);
			break;
		case RHS_NODE_KLEENE_PLUS:
			fputs("PLUS\n", out);
			dump_node(node->data.unary, depth + 1, out);
			break;
	}
}

void grammar_dump(const Grammar *g) {
	if (!g) return;
	fprintf(stdout, "# Productions (%zu)\n", g->prod_len);
	for (size_t i = 0; i < g->prod_len; ++i) {
		const Production *p = &g->prods[i];
		fprintf(stdout, "%s ::=", p->lhs);
		if (p->rhs_len == 0) {
			fprintf(stdout, " /*empty*/");
		} else {
			for (size_t j = 0; j < p->rhs_len; ++j) {
				fprintf(stdout, " %s", p->rhs[j].text);
			}
		}
		fprintf(stdout, " ;\n");
	}
}

void grammar_free(Grammar *g) {
	if (!g) return;
	free(g->start_symbol);
	for (size_t i = 0; i < g->raw_len; ++i) {
		RawProduction *rp = &g->raw[i];
		free(rp->lhs);
		rhs_node_free(rp->expr);
	}
	free(g->raw);
	grammar_clear_productions(g);
	free(g);
}

/* -------------------------------------------------------------------------- */
/* Scanner                                                                    */
/* -------------------------------------------------------------------------- */

typedef struct {
	const char *src;
	size_t len;
	size_t pos;
	int line;
} Scanner;

typedef enum {
	TOK_IDENT,
	TOK_TERMNAME,
	TOK_LITERAL,
	TOK_COLON_EQ,
	TOK_SEMI,
	TOK_BAR,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_STAR,
	TOK_PLUS,
	TOK_QMARK,
	TOK_EOF,
	TOK_INVALID
} TokenKind;

typedef struct {
	TokenKind kind;
	char *lexeme;
	int line;
} Token;

static void scanner_init(Scanner *s, const char *src, size_t len) {
	s->src = src;
	s->len = len;
	s->pos = 0;
	s->line = 1;
}

static int scanner_peek(const Scanner *s) {
	if (s->pos >= s->len) return EOF;
	return (unsigned char)s->src[s->pos];
}

static int scanner_peek_next(const Scanner *s) {
	if (s->pos + 1 >= s->len) return EOF;
	return (unsigned char)s->src[s->pos + 1];
}

static int scanner_advance(Scanner *s) {
	if (s->pos >= s->len) return EOF;
	int c = (unsigned char)s->src[s->pos++];
	if (c == '\n') s->line++;
	return c;
}

static Token make_token(TokenKind kind, const char *start, size_t len, int line) {
	Token t;
	t.kind = kind;
	t.line = line;
	if (start && len > 0) {
		t.lexeme = (char *)xmalloc(len + 1);
		memcpy(t.lexeme, start, len);
		t.lexeme[len] = '\0';
	} else {
		t.lexeme = NULL;
	}
	return t;
}

static void token_free(Token *t) {
	if (!t) return;
	free(t->lexeme);
	t->lexeme = NULL;
}

static void skip_ws_and_comments(Scanner *s) {
	for (;;) {
		int c = scanner_peek(s);
		while (isspace(c)) {
			scanner_advance(s);
			c = scanner_peek(s);
		}
		if (c == '#') {
			while (c != '\n' && c != EOF) {
				c = scanner_advance(s);
			}
			continue;
		}
		break;
	}
}

static int is_all_caps(const char *start, size_t len) {
	int has_alpha = 0;
	for (size_t i = 0; i < len; ++i) {
		unsigned char c = (unsigned char)start[i];
		if (isalpha(c)) {
			has_alpha = 1;
			if (!isupper(c)) return 0;
		} else if (c != '_' && !isdigit(c)) {
			return 0;
		}
	}
	return has_alpha;
}

static Token scan_token(Scanner *s) {
	skip_ws_and_comments(s);
	int peek = scanner_peek(s);
	if (peek == EOF) return make_token(TOK_EOF, NULL, 0, s->line);

	int line = s->line;
	int c = scanner_advance(s);

	switch (c) {
		case ';':
			return make_token(TOK_SEMI, ";", 1, line);
		case '|':
			return make_token(TOK_BAR, "|", 1, line);
		case '(':
			return make_token(TOK_LPAREN, "(", 1, line);
		case ')':
			return make_token(TOK_RPAREN, ")", 1, line);
		case '*':
			return make_token(TOK_STAR, "*", 1, line);
		case '+':
			return make_token(TOK_PLUS, "+", 1, line);
		case '?':
			return make_token(TOK_QMARK, "?", 1, line);
		case ':':
			if (scanner_peek(s) == ':' && scanner_peek_next(s) == '=') {
				scanner_advance(s);
				scanner_advance(s);
				return make_token(TOK_COLON_EQ, "::=", 3, line);
			}
			break;
		case '\'': {
			size_t start = s->pos - 1;
			int prev = 0;
			int ch = scanner_peek(s);
			while (ch != EOF) {
				if (ch == '\'' && prev != '\\') {
					scanner_advance(s);
					break;
				}
				prev = ch;
				scanner_advance(s);
				ch = scanner_peek(s);
			}
			size_t end = s->pos;
			return make_token(TOK_LITERAL, s->src + start, end - start, line);
		}
		default:
			if (isalpha(c) || c == '_') {
				size_t start = s->pos - 1;
				int ch = scanner_peek(s);
				while (isalnum(ch) || ch == '_') {
					scanner_advance(s);
					ch = scanner_peek(s);
				}
				size_t end = s->pos;
				size_t len = end - start;
				const char *lex = s->src + start;
				TokenKind kind = is_all_caps(lex, len) ? TOK_TERMNAME : TOK_IDENT;
				return make_token(kind, lex, len, line);
			}
			break;
	}
	char buf[2] = {(char)c, '\0'};
	return make_token(TOK_INVALID, buf, 1, line);
}

/* -------------------------------------------------------------------------- */
/* Recursive-descent parser                                                   */
/* -------------------------------------------------------------------------- */

typedef struct {
	Scanner scanner;
	Token lookahead;
	int has_token;
} Parser;

static void parser_init(Parser *p, const char *src, size_t len) {
	scanner_init(&p->scanner, src, len);
	p->has_token = 0;
	p->lookahead.kind = TOK_EOF;
	p->lookahead.lexeme = NULL;
	p->lookahead.line = 1;
}

static void parser_ensure(Parser *p) {
	if (!p->has_token) {
		p->lookahead = scan_token(&p->scanner);
		p->has_token = 1;
	}
}

static TokenKind parser_peek_kind(Parser *p) {
	parser_ensure(p);
	return p->lookahead.kind;
}

static Token parser_consume(Parser *p) {
	parser_ensure(p);
	Token tok = p->lookahead;
	p->has_token = 0;
	p->lookahead.lexeme = NULL;
	return tok;
}

static int parser_expect(Parser *p, TokenKind kind, Token *out) {
	Token t = parser_consume(p);
	if (t.kind != kind) {
		fprintf(stderr, "[grammar] line %d: unexpected token\n", t.line);
		token_free(&t);
		return 0;
	}
	if (out) {
		*out = t;
	} else {
		token_free(&t);
	}
	return 1;
}

typedef struct {
	RhsNode **items;
	size_t len;
	size_t cap;
} NodeVec;

static void nodevec_push(NodeVec *vec, RhsNode *node) {
	if (!node) return;
	if (vec->len + 1 > vec->cap) {
		vec->cap = vec->cap ? vec->cap * 2 : 4;
		vec->items = (RhsNode **)xrealloc(vec->items, vec->cap * sizeof(RhsNode *));
	}
	vec->items[vec->len++] = node;
}

static RhsNode *parse_expression(Parser *p);
static RhsNode *parse_sequence(Parser *p);
static RhsNode *parse_postfix(Parser *p);
static RhsNode *parse_primary(Parser *p);

static RhsNode *parse_primary(Parser *p) {
	Token t = parser_consume(p);
	RhsNode *node = NULL;
	switch (t.kind) {
		case TOK_IDENT:
			node = rhs_node_symbol(t.lexeme, SYMBOL_NONTERM, t.line);
			break;
		case TOK_TERMNAME:
		case TOK_LITERAL:
			node = rhs_node_symbol(t.lexeme, SYMBOL_TERM, t.line);
			break;
		case TOK_LPAREN: {
			RhsNode *inner = parse_expression(p);
			Token close;
			if (!parser_expect(p, TOK_RPAREN, &close)) {
				rhs_node_free(inner);
				inner = NULL;
			}
			token_free(&close);
			node = inner;
			break;
		}
		default:
			fprintf(stderr, "[grammar] line %d: unexpected symbol in RHS\n", t.line);
			node = rhs_node_symbol("/*error*/", SYMBOL_TERM, t.line);
			break;
	}
	token_free(&t);
	return node;
}

static RhsNode *parse_postfix(Parser *p) {
	RhsNode *node = parse_primary(p);
	for (;;) {
		TokenKind kind = parser_peek_kind(p);
		if (kind == TOK_STAR || kind == TOK_PLUS || kind == TOK_QMARK) {
			Token op = parser_consume(p);
			switch (kind) {
				case TOK_STAR:
					node = rhs_node_unary(RHS_NODE_KLEENE_STAR, node, op.line);
					break;
				case TOK_PLUS:
					node = rhs_node_unary(RHS_NODE_KLEENE_PLUS, node, op.line);
					break;
				case TOK_QMARK:
					node = rhs_node_unary(RHS_NODE_OPTIONAL, node, op.line);
					break;
				default:
					break;
			}
			token_free(&op);
		} else {
			break;
		}
	}
	return node;
}

static RhsNode *parse_sequence(Parser *p) {
	NodeVec vec = {0};
	for (;;) {
		TokenKind kind = parser_peek_kind(p);
		if (kind == TOK_BAR || kind == TOK_SEMI || kind == TOK_RPAREN || kind == TOK_EOF) {
			break;
		}
		RhsNode *item = parse_postfix(p);
		nodevec_push(&vec, item);
	}
	if (vec.len == 0) {
		int line = p->has_token ? p->lookahead.line : p->scanner.line;
		RhsNode *epsilon = rhs_node_sequence(NULL, 0, line);
		free(vec.items);
		return epsilon;
	}
	return rhs_node_sequence(vec.items, vec.len, vec.items[0]->line);
}

static RhsNode *parse_expression(Parser *p) {
	NodeVec vec = {0};
	RhsNode *first = parse_sequence(p);
	nodevec_push(&vec, first);
	while (parser_peek_kind(p) == TOK_BAR) {
		Token bar = parser_consume(p);
		token_free(&bar);
		RhsNode *rhs = parse_sequence(p);
		nodevec_push(&vec, rhs);
	}
	int line = first ? first->line : (p->has_token ? p->lookahead.line : p->scanner.line);
	return rhs_node_alternation(vec.items, vec.len, line);
}

/* -------------------------------------------------------------------------- */
/* Grammar loader                                                             */
/* -------------------------------------------------------------------------- */

Grammar *grammar_read_file(const char *path) {
	FILE *fp = fopen(path, "rb");
	if (!fp) {
		fprintf(stderr, "cfg2yacc: failed to open %s: %s\n", path, strerror(errno));
		return NULL;
	}
	if (fseek(fp, 0, SEEK_END) != 0) {
		fclose(fp);
		return NULL;
	}
	long n = ftell(fp);
	if (n < 0) {
		fclose(fp);
		return NULL;
	}
	if (fseek(fp, 0, SEEK_SET) != 0) {
		fclose(fp);
		return NULL;
	}
	char *buf = (char *)xmalloc((size_t)n + 1);
	size_t read = fread(buf, 1, (size_t)n, fp);
	fclose(fp);
	buf[read] = '\0';

	Grammar *grammar = grammar_create();
	Parser parser;
	parser_init(&parser, buf, read);

	for (;;) {
		TokenKind look_kind = parser_peek_kind(&parser);
		if (look_kind == TOK_EOF) {
			break;
		}
		if (look_kind == TOK_INVALID) {
			Token bad = parser_consume(&parser);
			fprintf(stderr, "[grammar] line %d: invalid token '%s'\n", bad.line,
					bad.lexeme ? bad.lexeme : "");
			token_free(&bad);
			continue;
		}

		Token lhs_tok;
		if (!parser_expect(&parser, TOK_IDENT, &lhs_tok)) {
			for (;;) {
				Token skip = parser_consume(&parser);
				TokenKind kind = skip.kind;
				token_free(&skip);
				if (kind == TOK_SEMI || kind == TOK_EOF) break;
			}
			continue;
		}

		if (!parser_expect(&parser, TOK_COLON_EQ, NULL)) {
			token_free(&lhs_tok);
			continue;
		}

		RhsNode *expr = parse_expression(&parser);
		Token semi;
		if (!parser_expect(&parser, TOK_SEMI, &semi)) {
			rhs_node_free(expr);
			token_free(&lhs_tok);
			continue;
		}
		token_free(&semi);

		RawProduction raw;
		raw.lhs = xstrdup(lhs_tok.lexeme);
		raw.expr = expr;
		raw.line = lhs_tok.line;
		grammar_append_raw(grammar, &raw);
		if (!grammar->start_symbol) grammar->start_symbol = xstrdup(lhs_tok.lexeme);
		token_free(&lhs_tok);
	}

	free(buf);
	return grammar;
}
 




