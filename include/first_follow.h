#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include "grammar.h"

typedef struct {
    char **items;
    size_t len;
    size_t cap;
} Set;

void compute_first_follow(Grammar *g);
void compute_first_follow_summary(Grammar *g);

#endif


