#include "transform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Utility helpers                                                            */
/* -------------------------------------------------------------------------- */

static void
fatal_oom(const char *what)
{
fprintf(stderr, "cfg2yacc: out of memory (%s)\n", what);
exit(EXIT_FAILURE);
}

static char *
dupstr(const char *s)
{
if (!s) return NULL;
size_t n = strlen(s);
char *copy = (char *)malloc(n + 1);
if (!copy) fatal_oom("string dup");
memcpy(copy, s, n + 1);
return copy;
}

static char *
make_generated_name(Grammar *g, const char *base)
{
char buf[256];
const char *stem = (base && *base) ? base : "gen";
snprintf(buf, sizeof(buf), "%s__gen_%lu", stem, (unsigned long)(++g->gen_counter));
return dupstr(buf);
}

/* -------------------------------------------------------------------------- */
/* EBNF expansion                                                             */
/* -------------------------------------------------------------------------- */

static void
symbol_vec_push(Symbol **arr, size_t *len, size_t *cap, Symbol sym)
{
if (*len + 1 > *cap) {
size_t new_cap = *cap ? (*cap * 2) : 4;
Symbol *resized = (Symbol *)realloc(*arr, new_cap * sizeof(Symbol));
if (!resized) fatal_oom("symbol list");
*arr = resized;
*cap = new_cap;
}
(*arr)[(*len)++] = sym;
}

static void
free_symbol_vec(Symbol *arr, size_t len)
{
if (!arr) return;
for (size_t i = 0; i < len; ++i) symbol_free(&arr[i]);
free(arr);
}

static void expand_expr_into_lhs(Grammar *g, const char *lhs, const RhsNode *expr);
static Symbol ensure_expr_symbol(Grammar *g, const char *context, const RhsNode *expr);

static Symbol
make_optional_symbol(Grammar *g, const char *context, const RhsNode *expr)
{
char *gen = make_generated_name(g, context);
grammar_add_production(g, gen, NULL, 0);
expand_expr_into_lhs(g, gen, expr);
Symbol sym = symbol_make(gen, SYMBOL_NONTERM);
free(gen);
return sym;
}

static Symbol
make_star_symbol(Grammar *g, const char *context, const RhsNode *expr)
{
char *gen = make_generated_name(g, context);
grammar_add_production(g, gen, NULL, 0);
Symbol inner = ensure_expr_symbol(g, gen, expr);

Symbol seq[2];
seq[0] = symbol_make(inner.text, inner.kind);
seq[1] = symbol_make(gen, SYMBOL_NONTERM);
grammar_add_production(g, gen, seq, 2);
symbol_free(&seq[0]);
symbol_free(&seq[1]);
symbol_free(&inner);

Symbol sym = symbol_make(gen, SYMBOL_NONTERM);
free(gen);
return sym;
}

static Symbol
make_plus_symbol(Grammar *g, const char *context, const RhsNode *expr)
{
char *gen = make_generated_name(g, context);
Symbol head = ensure_expr_symbol(g, gen, expr);
Symbol tail = make_star_symbol(g, gen, expr);

Symbol seq[2];
seq[0] = symbol_make(head.text, head.kind);
seq[1] = symbol_make(tail.text, tail.kind);
grammar_add_production(g, gen, seq, 2);
symbol_free(&seq[0]);
symbol_free(&seq[1]);
symbol_free(&head);
symbol_free(&tail);

Symbol sym = symbol_make(gen, SYMBOL_NONTERM);
free(gen);
return sym;
}

static Symbol
ensure_expr_symbol(Grammar *g, const char *context, const RhsNode *expr)
{
if (!expr) {
char *gen = make_generated_name(g, context);
grammar_add_production(g, gen, NULL, 0);
Symbol sym = symbol_make(gen, SYMBOL_NONTERM);
free(gen);
return sym;
}
switch (expr->kind) {
case RHS_NODE_SYMBOL:
return symbol_make(expr->data.symbol.name, expr->data.symbol.kind);
case RHS_NODE_SEQUENCE:
case RHS_NODE_ALTERNATION: {
char *gen = make_generated_name(g, context);
expand_expr_into_lhs(g, gen, expr);
Symbol sym = symbol_make(gen, SYMBOL_NONTERM);
free(gen);
return sym;
}
case RHS_NODE_OPTIONAL:
return make_optional_symbol(g, context, expr->data.unary);
case RHS_NODE_KLEENE_STAR:
return make_star_symbol(g, context, expr->data.unary);
case RHS_NODE_KLEENE_PLUS:
return make_plus_symbol(g, context, expr->data.unary);
}
return symbol_make("/*error*/", SYMBOL_TERM);
}

static void
append_expr_to_sequence(Grammar *g, const char *context, const RhsNode *expr,
    Symbol **out, size_t *len, size_t *cap)
{
if (!expr) return;
switch (expr->kind) {
case RHS_NODE_SEQUENCE:
for (size_t i = 0; i < expr->data.list.len; ++i) {
append_expr_to_sequence(g, context, expr->data.list.items[i], out, len, cap);
}
break;
case RHS_NODE_SYMBOL: {
Symbol sym = symbol_make(expr->data.symbol.name, expr->data.symbol.kind);
symbol_vec_push(out, len, cap, sym);
break;
}
default: {
Symbol sym = ensure_expr_symbol(g, context, expr);
symbol_vec_push(out, len, cap, symbol_make(sym.text, sym.kind));
symbol_free(&sym);
break;
}
}
}

static void
expand_expr_into_lhs(Grammar *g, const char *lhs, const RhsNode *expr)
{
if (!expr) {
grammar_add_production(g, lhs, NULL, 0);
return;
}
switch (expr->kind) {
case RHS_NODE_ALTERNATION:
for (size_t i = 0; i < expr->data.list.len; ++i) {
expand_expr_into_lhs(g, lhs, expr->data.list.items[i]);
}
break;
case RHS_NODE_SEQUENCE: {
if (expr->data.list.len == 0) {
grammar_add_production(g, lhs, NULL, 0);
break;
}
Symbol *seq = NULL;
size_t len = 0, cap = 0;
for (size_t i = 0; i < expr->data.list.len; ++i) {
append_expr_to_sequence(g, lhs, expr->data.list.items[i], &seq, &len, &cap);
}
grammar_add_production(g, lhs, seq, len);
free_symbol_vec(seq, len);
break;
}
case RHS_NODE_SYMBOL: {
Symbol sym = symbol_make(expr->data.symbol.name, expr->data.symbol.kind);
grammar_add_production(g, lhs, &sym, 1);
symbol_free(&sym);
break;
}
case RHS_NODE_OPTIONAL:
case RHS_NODE_KLEENE_STAR:
case RHS_NODE_KLEENE_PLUS: {
Symbol sym = ensure_expr_symbol(g, lhs, expr);
grammar_add_production(g, lhs, &sym, 1);
symbol_free(&sym);
break;
}
}
}

void
transform_expand_ebnf(Grammar *g)
{
if (!g) return;
grammar_clear_productions(g);
for (size_t i = 0; i < g->raw_len; ++i) {
RawProduction *rp = &g->raw[i];
expand_expr_into_lhs(g, rp->lhs, rp->expr);
}
}

/* -------------------------------------------------------------------------- */
/* Grammar normalisation helpers                                             */
/* -------------------------------------------------------------------------- */

typedef struct {
char *lhs;
Production *items;
size_t len;
size_t cap;
} ProdVec;

static void
production_free(Production *p)
{
if (!p) return;
free(p->lhs);
symbol_array_free(p->rhs, p->rhs_len);
p->lhs = NULL;
p->rhs = NULL;
p->rhs_len = 0;
}

static Production
production_clone(const Production *src)
{
Production p;
p.lhs = dupstr(src->lhs);
p.rhs_len = src->rhs_len;
if (src->rhs_len) {
p.rhs = (Symbol *)malloc(src->rhs_len * sizeof(Symbol));
if (!p.rhs) fatal_oom("production clone");
for (size_t i = 0; i < src->rhs_len; ++i) {
p.rhs[i] = symbol_make(src->rhs[i].text, src->rhs[i].kind);
}
} else {
p.rhs = NULL;
}
return p;
}

static void
prodvec_init(ProdVec *vec, const char *lhs)
{
vec->lhs = dupstr(lhs);
vec->items = NULL;
vec->len = 0;
vec->cap = 0;
}

static void
prodvec_free(ProdVec *vec)
{
if (!vec) return;
for (size_t i = 0; i < vec->len; ++i) production_free(&vec->items[i]);
free(vec->items);
free(vec->lhs);
}

static void
prodvec_push(ProdVec *vec, Production prod)
{
if (vec->len + 1 > vec->cap) {
size_t new_cap = vec->cap ? (vec->cap * 2) : 4;
Production *resized = (Production *)realloc(vec->items, new_cap * sizeof(Production));
if (!resized) fatal_oom("prodvec push");
vec->items = resized;
vec->cap = new_cap;
}
vec->items[vec->len++] = prod;
}

static void
prodvec_append_clone(ProdVec *vec, const Production *src)
{
prodvec_push(vec, production_clone(src));
}

static void
prodvec_add_symbols(ProdVec *vec, const Symbol *rhs, size_t rhs_len)
{
Production p;
p.lhs = dupstr(vec->lhs);
p.rhs_len = rhs_len;
if (rhs_len) {
p.rhs = (Symbol *)malloc(rhs_len * sizeof(Symbol));
if (!p.rhs) fatal_oom("prodvec symbols");
for (size_t i = 0; i < rhs_len; ++i) {
p.rhs[i] = symbol_make(rhs[i].text, rhs[i].kind);
}
} else {
p.rhs = NULL;
}
prodvec_push(vec, p);
}

static void
append_order(char ***order, size_t *len, size_t *cap, const char *name)
{
for (size_t i = 0; i < *len; ++i) {
if (strcmp((*order)[i], name) == 0) return;
}
if (*len + 1 > *cap) {
size_t new_cap = *cap ? (*cap * 2) : 8;
char **resized = (char **)realloc(*order, new_cap * sizeof(char *));
if (!resized) fatal_oom("order list");
*order = resized;
*cap = new_cap;
}
(*order)[(*len)++] = dupstr(name);
}

static size_t
append_vec(ProdVec **vecs, size_t *count, size_t *cap, const char *lhs)
{
if (*count + 1 > *cap) {
size_t new_cap = *cap ? (*cap * 2) : 8;
ProdVec *resized = (ProdVec *)realloc(*vecs, new_cap * sizeof(ProdVec));
if (!resized) fatal_oom("prodvec list");
*vecs = resized;
*cap = new_cap;
}
prodvec_init(&(*vecs)[*count], lhs);
return (*count)++;
}

static void
substitute_indirect_left_recursion(ProdVec *Ai, const ProdVec *Aj)
{
Production *result = NULL;
size_t len = 0, cap = 0;

for (size_t i = 0; i < Ai->len; ++i) {
Production *p = &Ai->items[i];
if (p->rhs_len > 0 && p->rhs[0].kind == SYMBOL_NONTERM && strcmp(p->rhs[0].text, Aj->lhs) == 0) {
size_t tail_len = p->rhs_len - 1;
for (size_t j = 0; j < Aj->len; ++j) {
const Production *q = &Aj->items[j];
size_t new_len = q->rhs_len + tail_len;
Production np;
np.lhs = dupstr(Ai->lhs);
np.rhs_len = new_len;
if (new_len) {
np.rhs = (Symbol *)malloc(new_len * sizeof(Symbol));
if (!np.rhs) fatal_oom("indirect recursion");
size_t idx = 0;
for (size_t r = 0; r < q->rhs_len; ++r) {
np.rhs[idx++] = symbol_make(q->rhs[r].text, q->rhs[r].kind);
}
for (size_t r = 1; r < p->rhs_len; ++r) {
np.rhs[idx++] = symbol_make(p->rhs[r].text, p->rhs[r].kind);
}
} else {
np.rhs = NULL;
}
if (len + 1 > cap) {
size_t new_cap = cap ? (cap * 2) : 8;
Production *resized = (Production *)realloc(result, new_cap * sizeof(Production));
if (!resized) fatal_oom("indirect result");
result = resized;
cap = new_cap;
}
result[len++] = np;
}
} else {
if (len + 1 > cap) {
size_t new_cap = cap ? (cap * 2) : 8;
Production *resized = (Production *)realloc(result, new_cap * sizeof(Production));
if (!resized) fatal_oom("indirect keep");
result = resized;
cap = new_cap;
}
result[len++] = production_clone(p);
}
}

for (size_t i = 0; i < Ai->len; ++i) production_free(&Ai->items[i]);
free(Ai->items);
Ai->items = result;
Ai->len = len;
Ai->cap = cap;
}

static void
eliminate_immediate_left_recursion(Grammar *g, ProdVec **vecs, size_t *vec_count,
    size_t *vec_cap, char ***order, size_t *order_len, size_t *order_cap, size_t ai_index)
{
ProdVec *array = *vecs;
ProdVec *Ai = &array[ai_index];

Production *betas = NULL;
Production *alphas = NULL;
size_t beta_len = 0, beta_cap = 0;
size_t alpha_len = 0, alpha_cap = 0;

for (size_t i = 0; i < Ai->len; ++i) {
Production *p = &Ai->items[i];
if (p->rhs_len > 0 && p->rhs[0].kind == SYMBOL_NONTERM && strcmp(p->rhs[0].text, Ai->lhs) == 0) {
if (alpha_len + 1 > alpha_cap) {
size_t new_cap = alpha_cap ? (alpha_cap * 2) : 4;
Production *resized = (Production *)realloc(alphas, new_cap * sizeof(Production));
if (!resized) fatal_oom("alpha list");
alphas = resized;
alpha_cap = new_cap;
}
alphas[alpha_len++] = production_clone(p);
} else {
if (beta_len + 1 > beta_cap) {
size_t new_cap = beta_cap ? (beta_cap * 2) : 4;
Production *resized = (Production *)realloc(betas, new_cap * sizeof(Production));
if (!resized) fatal_oom("beta list");
betas = resized;
beta_cap = new_cap;
}
betas[beta_len++] = production_clone(p);
}
}

if (alpha_len == 0) {
for (size_t i = 0; i < alpha_len; ++i) production_free(&alphas[i]);
for (size_t i = 0; i < beta_len; ++i) production_free(&betas[i]);
free(alphas);
free(betas);
return;
}

char *tmp = make_generated_name(g, Ai->lhs);
append_order(order, order_len, order_cap, tmp);
size_t new_index = append_vec(vecs, vec_count, vec_cap, tmp);
free(tmp);

array = *vecs;
Ai = &array[ai_index];
ProdVec *Aprime_vec = &array[new_index];
const char *Aprime = Aprime_vec->lhs;

Production *new_items = NULL;
size_t new_len = 0, new_cap = 0;

if (beta_len == 0) {
Production np;
np.lhs = dupstr(Ai->lhs);
np.rhs_len = 1;
np.rhs = (Symbol *)malloc(sizeof(Symbol));
if (!np.rhs) fatal_oom("left recursion fallback");
np.rhs[0] = symbol_make(Aprime, SYMBOL_NONTERM);
if (new_len + 1 > new_cap) {
size_t cap2 = new_cap ? (new_cap * 2) : 4;
Production *resized = (Production *)realloc(new_items, cap2 * sizeof(Production));
if (!resized) fatal_oom("left recursion new");
new_items = resized;
new_cap = cap2;
}
new_items[new_len++] = np;
} else {
for (size_t i = 0; i < beta_len; ++i) {
Production *beta = &betas[i];
size_t rhs_len = beta->rhs_len + 1;
Symbol *rhs = NULL;
if (rhs_len) {
rhs = (Symbol *)malloc(rhs_len * sizeof(Symbol));
if (!rhs) fatal_oom("beta rhs");
size_t idx = 0;
for (size_t k = 0; k < beta->rhs_len; ++k) {
rhs[idx++] = symbol_make(beta->rhs[k].text, beta->rhs[k].kind);
}
rhs[idx++] = symbol_make(Aprime, SYMBOL_NONTERM);
}
Production np;
np.lhs = dupstr(Ai->lhs);
np.rhs = rhs;
np.rhs_len = rhs_len;
if (new_len + 1 > new_cap) {
size_t cap2 = new_cap ? (new_cap * 2) : 4;
Production *resized = (Production *)realloc(new_items, cap2 * sizeof(Production));
if (!resized) fatal_oom("left recursion new");
new_items = resized;
new_cap = cap2;
}
new_items[new_len++] = np;
}
}

for (size_t i = 0; i < alpha_len; ++i) {
Production *alpha = &alphas[i];
if (alpha->rhs_len <= 1) continue;
size_t tail_len = alpha->rhs_len - 1;
Symbol *rhs = (Symbol *)malloc((tail_len + 1) * sizeof(Symbol));
if (!rhs) fatal_oom("alpha rhs");
size_t idx = 0;
for (size_t k = 1; k < alpha->rhs_len; ++k) {
rhs[idx++] = symbol_make(alpha->rhs[k].text, alpha->rhs[k].kind);
}
rhs[idx++] = symbol_make(Aprime, SYMBOL_NONTERM);
prodvec_add_symbols(Aprime_vec, rhs, tail_len + 1);
free_symbol_vec(rhs, tail_len + 1);
}
prodvec_add_symbols(Aprime_vec, NULL, 0);

for (size_t i = 0; i < Ai->len; ++i) production_free(&Ai->items[i]);
free(Ai->items);
Ai->items = new_items;
Ai->len = new_len;
Ai->cap = new_cap;

for (size_t i = 0; i < alpha_len; ++i) production_free(&alphas[i]);
for (size_t i = 0; i < beta_len; ++i) production_free(&betas[i]);
free(alphas);
free(betas);
}

static int
apply_left_factoring(Grammar *g, ProdVec **vecs, size_t *vec_count, size_t *vec_cap,
    char ***order, size_t *order_len, size_t *order_cap, size_t ai_index)
{
ProdVec *array = *vecs;
ProdVec *Ai = &array[ai_index];
if (Ai->len < 2) return 0;

// 1) Choose a group of productions that share the same FIRST symbol (appearing >= 2 times)
size_t chosen = (size_t)-1;
SymbolKind chosen_kind = SYMBOL_TERM;
const char *chosen_text = NULL;
size_t group_count = 0;
size_t *group_idx = NULL;

for (size_t i = 0; i < Ai->len; ++i) {
    Production *p = &Ai->items[i];
    if (p->rhs_len == 0) continue;
    size_t count = 1;
    // First pass: count matches
    for (size_t j = i + 1; j < Ai->len; ++j) {
        Production *q = &Ai->items[j];
        if (q->rhs_len == 0) continue;
        if (q->rhs[0].kind == p->rhs[0].kind && strcmp(q->rhs[0].text, p->rhs[0].text) == 0) {
            count++;
        }
    }
    if (count >= 2) {
        chosen = i;
        chosen_kind = p->rhs[0].kind;
        chosen_text = p->rhs[0].text;
        // Collect exact indices for this group
        group_idx = (size_t *)malloc(count * sizeof(size_t));
        if (!group_idx) fatal_oom("factor group indices");
        group_count = 0;
        group_idx[group_count++] = i;
        for (size_t j = i + 1; j < Ai->len; ++j) {
            Production *q = &Ai->items[j];
            if (q->rhs_len == 0) continue;
            if (q->rhs[0].kind == p->rhs[0].kind && strcmp(q->rhs[0].text, p->rhs[0].text) == 0) {
                group_idx[group_count++] = j;
            }
        }
        break;
    }
}

if (chosen == (size_t)-1) return 0;

char *tmp = make_generated_name(g, Ai->lhs);
append_order(order, order_len, order_cap, tmp);
size_t helper_index = append_vec(vecs, vec_count, vec_cap, tmp);
free(tmp);

array = *vecs;
Ai = &array[ai_index];
ProdVec *helper_vec = &array[helper_index];
const char *helper_name = helper_vec->lhs;

Production *new_items = NULL;
size_t new_len = 0, new_cap = 0;

// 2) Compute the longest common prefix length among the group
size_t lcp_len = (size_t)-1;
for (size_t gi = 0; gi < group_count; ++gi) {
    Production *p = &Ai->items[group_idx[gi]];
    if (lcp_len == (size_t)-1 || p->rhs_len < lcp_len) lcp_len = p->rhs_len;
}
// We already know first symbol matches; now extend as far as all match
size_t pos = 0;
for (; pos < lcp_len; ++pos) {
    SymbolKind kind0 = Ai->items[group_idx[0]].rhs[pos].kind;
    const char *text0 = Ai->items[group_idx[0]].rhs[pos].text;
    int all_same = 1;
    for (size_t gi = 1; gi < group_count; ++gi) {
        Production *p = &Ai->items[group_idx[gi]];
        if (p->rhs[pos].kind != kind0 || strcmp(p->rhs[pos].text, text0) != 0) {
            all_same = 0;
            break;
        }
    }
    if (!all_same) break;
}
size_t prefix_len = pos; // >= 1 by construction

// 3) Build helper productions: remainders after the common prefix
for (size_t gi = 0; gi < group_count; ++gi) {
    Production *p = &Ai->items[group_idx[gi]];
    size_t remainder_len = (p->rhs_len > prefix_len) ? (p->rhs_len - prefix_len) : 0;
    if (remainder_len == 0) {
        prodvec_add_symbols(helper_vec, NULL, 0);
    } else {
        Symbol *rhs = (Symbol *)malloc(remainder_len * sizeof(Symbol));
        if (!rhs) fatal_oom("factor remainder (lcp)");
        for (size_t k = 0; k < remainder_len; ++k) {
            rhs[k] = symbol_make(p->rhs[prefix_len + k].text, p->rhs[prefix_len + k].kind);
        }
        prodvec_add_symbols(helper_vec, rhs, remainder_len);
        free_symbol_vec(rhs, remainder_len);
    }
}

// 4) Keep the productions not in the chosen group
for (size_t i = 0; i < Ai->len; ++i) {
    int in_group = 0;
    for (size_t gi = 0; gi < group_count; ++gi) {
        if (group_idx[gi] == i) { in_group = 1; break; }
    }
    if (!in_group) {
        if (new_len + 1 > new_cap) {
            size_t cap2 = new_cap ? (new_cap * 2) : 4;
            Production *resized = (Production *)realloc(new_items, cap2 * sizeof(Production));
            if (!resized) fatal_oom("factor keep");
            new_items = resized;
            new_cap = cap2;
        }
        new_items[new_len++] = production_clone(&Ai->items[i]);
    }
}

// 5) Add the factored production: common_prefix helper
Production np;
np.lhs = dupstr(Ai->lhs);
np.rhs_len = prefix_len + 1;
np.rhs = (Symbol *)malloc(np.rhs_len * sizeof(Symbol));
if (!np.rhs) fatal_oom("factor new production (lcp)");
for (size_t k = 0; k < prefix_len; ++k) {
    np.rhs[k] = symbol_make(Ai->items[group_idx[0]].rhs[k].text, Ai->items[group_idx[0]].rhs[k].kind);
}
np.rhs[prefix_len] = symbol_make(helper_name, SYMBOL_NONTERM);
if (new_len + 1 > new_cap) {
    size_t cap2 = new_cap ? (new_cap * 2) : 4;
    Production *resized = (Production *)realloc(new_items, cap2 * sizeof(Production));
    if (!resized) fatal_oom("factor np");
    new_items = resized;
    new_cap = cap2;
}
new_items[new_len++] = np;

for (size_t i = 0; i < Ai->len; ++i) production_free(&Ai->items[i]);
free(Ai->items);
Ai->items = new_items;
Ai->len = new_len;
Ai->cap = new_cap;

free(group_idx);

return 1;
}

static void
rebuild_grammar_from_vecs(Grammar *g, ProdVec *vecs, size_t vec_count)
{
grammar_clear_productions(g);
for (size_t i = 0; i < vec_count; ++i) {
ProdVec *vec = &vecs[i];
for (size_t j = 0; j < vec->len; ++j) {
Production *p = &vec->items[j];
grammar_add_production(g, p->lhs, p->rhs, p->rhs_len);
}
}
}

void
transform_fix_grammar(Grammar *g)
{
if (!g) return;

size_t order_len = g->nonterm_len;
size_t order_cap = order_len ? order_len : 8;
char **order = (char **)malloc(order_cap * sizeof(char *));
if (!order) fatal_oom("order init");
for (size_t i = 0; i < order_len; ++i) order[i] = dupstr(g->nonterms[i]);

size_t vec_cap = order_cap;
ProdVec *vecs = (ProdVec *)malloc(vec_cap * sizeof(ProdVec));
if (!vecs) fatal_oom("vecs init");
for (size_t i = 0; i < order_len; ++i) prodvec_init(&vecs[i], order[i]);
size_t vec_count = order_len;

for (size_t i = 0; i < g->prod_len; ++i) {
Production *p = &g->prods[i];
size_t idx = 0;
for (; idx < vec_count; ++idx) {
if (strcmp(vecs[idx].lhs, p->lhs) == 0) break;
}
if (idx == vec_count) {
append_order(&order, &order_len, &order_cap, p->lhs);
size_t new_index = append_vec(&vecs, &vec_count, &vec_cap, p->lhs);
prodvec_append_clone(&vecs[new_index], p);
} else {
prodvec_append_clone(&vecs[idx], p);
}
}

    fprintf(stderr, "[transform] initial nonterms: %zu, productions: %zu\n", order_len, g->prod_len);
    // Only perform indirect left recursion substitution against the ORIGINAL nonterminals.
    // Helpers introduced during normalisation should not be used as Aj sources; this avoids
    // combinatorial explosion.
    size_t original_count = vec_count;
    for (size_t i = 0; i < vec_count; ++i) {
        if (i < 50 || (i % 200 == 0)) {
            fprintf(stderr, "[transform] normalising %s (%zu prods) [i=%zu/%zu]\n",
                    vecs[i].lhs, vecs[i].len, i + 1, vec_count);
        }
        size_t limit = (i < original_count) ? i : original_count;
        for (size_t j = 0; j < limit; ++j) {
            substitute_indirect_left_recursion(&vecs[i], &vecs[j]);
        }
        eliminate_immediate_left_recursion(g, &vecs, &vec_count, &vec_cap,
            &order, &order_len, &order_cap, i);
        // Left factoring is optional for Yacc/LALR parsers and was causing
        // explosive helper generation on some EBNF-derived patterns.
        // To ensure termination and keep grammars compact, we disable
        // left factoring here. If needed in the future, re-enable with
        // strict guards or only for terminal-prefix groups.
        (void)apply_left_factoring; // silence unused warning if optimized out
    }

rebuild_grammar_from_vecs(g, vecs, vec_count);

for (size_t i = 0; i < vec_count; ++i) prodvec_free(&vecs[i]);
free(vecs);
for (size_t i = 0; i < order_len; ++i) free(order[i]);
free(order);
}
