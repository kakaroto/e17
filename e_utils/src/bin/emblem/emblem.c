#include "Emblem.h"

#include <stdlib.h>

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
    if (em->theme) free(em->theme);
    free(em);
}

