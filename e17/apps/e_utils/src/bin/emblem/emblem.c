#include "Emblem.h"

Emblem *
emblem_new(void)
{
    Emblem *em;

    em = calloc(1, sizeof(Emblem));
    return em;
}

void
emblem_free(Emblem *em)
{
    if (!em) return;

    if (em->display) free(em->display);
    free(em);
}

