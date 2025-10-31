#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammar.h"
#include "transform.h"
#include "first_follow.h"
#include "emitter.h"

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s -i in.ebnf -o out.y [--emit-lex out.l]\n", prog);
}

int main(int argc, char **argv) {
    const char *input = NULL;
    const char *out_y = NULL;
    const char *out_l = NULL;
    int no_ff = 0;
    int ff_summary = 0;

    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) && i + 1 < argc) {
            input = argv[++i];
        } else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && i + 1 < argc) {
            out_y = argv[++i];
        } else if (strcmp(argv[i], "--emit-lex") == 0 && i + 1 < argc) {
            out_l = argv[++i];
        } else if (strcmp(argv[i], "--no-ff") == 0) {
            no_ff = 1;
        } else if (strcmp(argv[i], "--ff-summary") == 0) {
            ff_summary = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
    }

    if (!input || !out_y) {
        usage(argv[0]);
        return 1;
    }

    fprintf(stderr, "[cfg2yacc] reading grammar: %s\n", input);
    fflush(stderr);
    Grammar *g = grammar_read_file(input);
    if (!g) {
        fprintf(stderr, "Failed to read grammar from %s\n", input);
        return 2;
    }

    fprintf(stderr, "[cfg2yacc] expanding EBNF -> CFG...\n");
    fflush(stderr);
    transform_expand_ebnf(g);

    fprintf(stderr, "[cfg2yacc] normalising grammar (LR removal, factoring)...\n");
    fflush(stderr);
    transform_fix_grammar(g);

    if (!no_ff) {
        fprintf(stderr, "[cfg2yacc] computing FIRST/FOLLOW...\n");
        fflush(stderr);
        if (ff_summary) {
            compute_first_follow_summary(g);
        } else {
            compute_first_follow(g);
        }
    }

    fprintf(stderr, "[cfg2yacc] emitting yacc/flex...\n");
    fflush(stderr);
    emit_yacc(g, out_y, out_l, "ast.h", "ast.c");

    fprintf(stderr, "[cfg2yacc] done -> %s%s%s\n",
            out_y ? out_y : "(none)", out_l ? ", " : "",
            out_l ? out_l : "");
    fflush(stderr);

    grammar_free(g);
    return 0;
}
