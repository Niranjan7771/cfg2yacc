#include "first_follow.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void set_init(Set *s) {
    s->items = NULL;
    s->len = 0;
    s->cap = 0;
}

static void set_clear(Set *s) {
    for (size_t i = 0; i < s->len; ++i) free(s->items[i]);
    s->len = 0;
}

static void set_free(Set *s) {
    set_clear(s);
    free(s->items);
    s->items = NULL;
    s->cap = 0;
}

static void *xrealloc_checked(void *ptr, size_t n) {
    void *p = realloc(ptr, n);
    if (!p && n != 0) {
        fprintf(stderr, "cfg2yacc: out of memory (first/follow)\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static int set_contains(const Set *s, const char *value) {
    for (size_t i = 0; i < s->len; ++i) {
        if (strcmp(s->items[i], value) == 0) return 1;
    }
    return 0;
}

static int set_add(Set *s, const char *value) {
    if (set_contains(s, value)) return 0;
    if (s->len + 1 > s->cap) {
        s->cap = s->cap ? s->cap * 2 : 8;
        s->items = (char **)xrealloc_checked(s->items, s->cap * sizeof(char *));
    }
    size_t n = strlen(value);
    s->items[s->len] = (char *)malloc(n + 1);
    if (!s->items[s->len]) {
        fprintf(stderr, "cfg2yacc: out of memory (set_add)\n");
        exit(EXIT_FAILURE);
    }
    memcpy(s->items[s->len], value, n + 1);
    s->len++;
    return 1;
}

static int set_union(Set *dst, const Set *src) {
    int changed = 0;
    for (size_t i = 0; i < src->len; ++i) {
        if (set_add(dst, src->items[i])) changed = 1;
    }
    return changed;
}

static void set_copy(Set *dst, const Set *src) {
    set_clear(dst);
    for (size_t i = 0; i < src->len; ++i) {
        set_add(dst, src->items[i]);
    }
}

static int set_intersects(const Set *a, const Set *b) {
    for (size_t i = 0; i < a->len; ++i) {
        if (set_contains(b, a->items[i])) return 1;
    }
    return 0;
}

static int nonterm_index(const Grammar *g, const char *name) {
    for (size_t i = 0; i < g->nonterm_len; ++i) {
        if (strcmp(g->nonterms[i], name) == 0) return (int)i;
    }
    return -1;
}

static void compute_nullable(const Grammar *g, int *nullable) {
    int changed;
    int iterations = 0;
    const int max_iterations = 1000;  // Safety limit to prevent infinite loops
    do {
        changed = 0;
        iterations++;
        if (iterations > max_iterations) {
            fprintf(stderr, "[first_follow] warning: NULLABLE computation exceeded %d iterations, stopping\n", max_iterations);
            break;
        }
        for (size_t i = 0; i < g->prod_len; ++i) {
            const Production *p = &g->prods[i];
            int lhs = nonterm_index(g, p->lhs);
            if (lhs < 0 || nullable[lhs]) continue;
            if (p->rhs_len == 0) {
                nullable[lhs] = 1;
                changed = 1;
                continue;
            }
            int all_nullable = 1;
            for (size_t k = 0; k < p->rhs_len; ++k) {
                const Symbol *sym = &p->rhs[k];
                if (sym->kind == SYMBOL_TERM) { all_nullable = 0; break; }
                int idx = nonterm_index(g, sym->text);
                if (idx < 0 || !nullable[idx]) { all_nullable = 0; break; }
            }
            if (all_nullable) {
                nullable[lhs] = 1;
                changed = 1;
            }
        }
    } while (changed);
}

static void compute_first_sets(const Grammar *g, const int *nullable, Set *first_sets) {
    int changed;
    int iterations = 0;
    const int max_iterations = 1000;  // Safety limit to prevent infinite loops
    do {
        changed = 0;
        iterations++;
        if (iterations > max_iterations) {
            fprintf(stderr, "[first_follow] warning: FIRST set computation exceeded %d iterations, stopping\n", max_iterations);
            break;
        }
        for (size_t i = 0; i < g->prod_len; ++i) {
            const Production *p = &g->prods[i];
            int lhs = nonterm_index(g, p->lhs);
            if (lhs < 0) continue;
            if (p->rhs_len == 0) continue;
            for (size_t k = 0; k < p->rhs_len; ++k) {
                const Symbol *sym = &p->rhs[k];
                if (sym->kind == SYMBOL_TERM) {
                    changed |= set_add(&first_sets[lhs], sym->text);
                    break;
                }
                int idx = nonterm_index(g, sym->text);
                if (idx < 0) break;
                changed |= set_union(&first_sets[lhs], &first_sets[idx]);
                if (!nullable[idx]) break;
            }
        }
    } while (changed);
}

static void compute_follow_sets(const Grammar *g, const int *nullable, const Set *first_sets, Set *follow_sets) {
    int start_idx = nonterm_index(g, g->start_symbol);
    if (start_idx >= 0) set_add(&follow_sets[start_idx], "$");

    int changed;
    int iterations = 0;
    const int max_iterations = 1000;  // Safety limit to prevent infinite loops
    do {
        changed = 0;
        iterations++;
        if (iterations > max_iterations) {
            fprintf(stderr, "[first_follow] warning: FOLLOW set computation exceeded %d iterations, stopping\n", max_iterations);
            break;
        }
        for (size_t i = 0; i < g->prod_len; ++i) {
            const Production *p = &g->prods[i];
            int lhs = nonterm_index(g, p->lhs);
            if (lhs < 0) continue;

            Set trailer;
            set_init(&trailer);
            set_copy(&trailer, &follow_sets[lhs]);

            for (size_t idx = p->rhs_len; idx > 0; --idx) {
                size_t k = idx - 1;
                const Symbol *sym = &p->rhs[k];
                if (sym->kind == SYMBOL_TERM) {
                    set_clear(&trailer);
                    set_add(&trailer, sym->text);
                    continue;
                }
                int sym_idx = nonterm_index(g, sym->text);
                if (sym_idx < 0) continue;

                changed |= set_union(&follow_sets[sym_idx], &trailer);
                if (nullable[sym_idx]) {
                    changed |= set_union(&trailer, &first_sets[sym_idx]);
                } else {
                    set_clear(&trailer);
                    set_union(&trailer, &first_sets[sym_idx]);
                }
            }

            set_free(&trailer);
        }
    } while (changed);
}

static void compute_rhs_first(const Grammar *g, const Production *p, const int *nullable, const Set *first_sets,
                              Set *out, int *is_nullable) {
    set_init(out);
    if (p->rhs_len == 0) {
        *is_nullable = 1;
        return;
    }
    *is_nullable = 0;
    for (size_t k = 0; k < p->rhs_len; ++k) {
        const Symbol *sym = &p->rhs[k];
        if (sym->kind == SYMBOL_TERM) {
            set_add(out, sym->text);
            return;
        }
        int idx = nonterm_index(g, sym->text);
        if (idx < 0) return;
        set_union(out, &first_sets[idx]);
        if (!nullable[idx]) return;
        if (k == p->rhs_len - 1) {
            *is_nullable = 1;
        }
    }
}

static void report_sets(const char *title, const Grammar *g, const Set *sets) {
    fprintf(stdout, "%s:\n", title);
    for (size_t i = 0; i < g->nonterm_len; ++i) {
        fprintf(stdout, "  %s : {", g->nonterms[i]);
        for (size_t j = 0; j < sets[i].len; ++j) {
            fprintf(stdout, "%s%s", j ? ", " : "", sets[i].items[j]);
        }
        fprintf(stdout, "}\n");
    }
}

static void report_undefined_nonterms(const Grammar *g) {
    for (size_t i = 0; i < g->nonterm_len; ++i) {
        int has_prod = 0;
        for (size_t p = 0; p < g->prod_len; ++p) {
            if (strcmp(g->prods[p].lhs, g->nonterms[i]) == 0) {
                has_prod = 1;
                break;
            }
        }
        if (!has_prod) {
            fprintf(stdout, "  [warn] nonterminal %s has no productions\n", g->nonterms[i]);
        }
    }
}

void compute_first_follow(Grammar *g) {
    if (!g) return;

    size_t n = g->nonterm_len;
    int *nullable = (int *)calloc(n, sizeof(int));
    Set *first_sets = (Set *)calloc(n, sizeof(Set));
    Set *follow_sets = (Set *)calloc(n, sizeof(Set));
    if (!nullable || !first_sets || !follow_sets) {
        fprintf(stderr, "cfg2yacc: out of memory (first/follow alloc)\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; ++i) {
        set_init(&first_sets[i]);
        set_init(&follow_sets[i]);
    }

    compute_nullable(g, nullable);
    compute_first_sets(g, nullable, first_sets);
    compute_follow_sets(g, nullable, first_sets, follow_sets);

    fprintf(stdout, "NULLABLE:\n");
    for (size_t i = 0; i < n; ++i) {
        fprintf(stdout, "  %s : %s\n", g->nonterms[i], nullable[i] ? "yes" : "no");
    }

    report_sets("FIRST sets", g, first_sets);
    report_sets("FOLLOW sets", g, follow_sets);

    fprintf(stdout, "LL(1) conflict checks:\n");
    for (size_t i = 0; i < n; ++i) {
        const char *lhs = g->nonterms[i];
        size_t prod_count = 0;
        for (size_t p = 0; p < g->prod_len; ++p) {
            if (strcmp(g->prods[p].lhs, lhs) == 0) prod_count++;
        }
        if (prod_count <= 1) continue;

        for (size_t p = 0; p < g->prod_len; ++p) {
            if (strcmp(g->prods[p].lhs, lhs) != 0) continue;
            Set first_p;
            int nullable_p;
            compute_rhs_first(g, &g->prods[p], nullable, first_sets, &first_p, &nullable_p);

            for (size_t q = p + 1; q < g->prod_len; ++q) {
                if (strcmp(g->prods[q].lhs, lhs) != 0) continue;
                Set first_q;
                int nullable_q;
                compute_rhs_first(g, &g->prods[q], nullable, first_sets, &first_q, &nullable_q);

                if (set_intersects(&first_p, &first_q)) {
                    fprintf(stdout, "  [warn] %s alternatives %zu and %zu overlap in FIRST\n", lhs, p, q);
                }
                if (nullable_p && set_intersects(&first_q, &follow_sets[i])) {
                    fprintf(stdout, "  [warn] %s production %zu nullable vs FOLLOW\n", lhs, p);
                }
                if (nullable_q && set_intersects(&first_p, &follow_sets[i])) {
                    fprintf(stdout, "  [warn] %s production %zu nullable vs FOLLOW\n", lhs, q);
                }

                set_free(&first_q);
            }

            set_free(&first_p);
        }
    }

    report_undefined_nonterms(g);

    for (size_t i = 0; i < n; ++i) {
        set_free(&first_sets[i]);
        set_free(&follow_sets[i]);
    }
    free(first_sets);
    free(follow_sets);
    free(nullable);
}

void compute_first_follow_summary(Grammar *g) {
    if (!g) return;

    size_t n = g->nonterm_len;
    int *nullable = (int *)calloc(n, sizeof(int));
    Set *first_sets = (Set *)calloc(n, sizeof(Set));
    Set *follow_sets = (Set *)calloc(n, sizeof(Set));
    if (!nullable || !first_sets || !follow_sets) {
        fprintf(stderr, "cfg2yacc: out of memory (first/follow alloc)\n");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < n; ++i) {
        set_init(&first_sets[i]);
        set_init(&follow_sets[i]);
    }

    compute_nullable(g, nullable);
    compute_first_sets(g, nullable, first_sets);
    compute_follow_sets(g, nullable, first_sets, follow_sets);

    fprintf(stdout, "FIRST/FOLLOW summary (counts only):\n");
    size_t total_first = 0, total_follow = 0, nullable_count = 0;
    for (size_t i = 0; i < n; ++i) {
        total_first += first_sets[i].len;
        total_follow += follow_sets[i].len;
        if (nullable[i]) nullable_count++;
    }
    fprintf(stdout, "  nonterminals: %zu\n", n);
    fprintf(stdout, "  nullable: %zu\n", nullable_count);
    fprintf(stdout, "  total FIRST entries: %zu\n", total_first);
    fprintf(stdout, "  total FOLLOW entries: %zu\n", total_follow);
    for (size_t i = 0; i < n; ++i) {
        fprintf(stdout, "  %s -> FIRST:%zu, FOLLOW:%zu%s\n",
                g->nonterms[i], first_sets[i].len, follow_sets[i].len,
                nullable[i] ? ", NULLABLE" : "");
    }

    for (size_t i = 0; i < n; ++i) {
        set_free(&first_sets[i]);
        set_free(&follow_sets[i]);
    }
    free(first_sets);
    free(follow_sets);
    free(nullable);
}
