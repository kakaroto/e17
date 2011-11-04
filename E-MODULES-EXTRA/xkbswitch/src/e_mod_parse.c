/*
 * This file parses the XML file with rules into a tree with which we can work.
 * It's supposed to serve as simple interface to layouts and variants.
 * 
 * Reader example on libxml2 page used for reference.
 */

#include "e_mod_parse.h"

Eina_List *layouts = NULL;
Eina_List *models  = NULL;

void parse_rules(const char *fname)
{
    E_XKB_Model *model = NULL;
    E_XKB_Layout *layout = NULL;
    E_XKB_Variant *variant = NULL;

    char buf[512];

    FILE *f = fopen(fname, "r");
    if  (!f) return;

    /* move on to next line, the first one is useless */
    fgets(buf, sizeof(buf), f);

    /* read models here */
    for (;;) if (fgets(buf, sizeof(buf), f))
    {
        char *n = strchr(buf, '\n');
        if   (n) *n = '\0';

        /* means end of section */
        if (!buf[0]) break;

        /* get rid of initial 2 spaces here */
        char *p   = buf + 2;
        char *tmp = strdup(p);

        model = E_NEW(E_XKB_Model, 1);
        model->name = eina_stringshare_add(strtok(tmp, " "));

        free(tmp);

        p += strlen(model->name);
        while (p[0] == ' ') ++p;

        model->description = eina_stringshare_add(p);

        models = eina_list_append(models, model);
    } else break;

    /* move on again */
    fgets(buf, sizeof(buf), f);

    /* read layouts here */
    for (;;) if (fgets(buf, sizeof(buf), f))
    {
        char *n = strchr(buf, '\n');
        if   (n) *n = '\0';

        if (!buf[0]) break;

        char *p   = buf + 2;
        char *tmp = strdup(p);

        layout = E_NEW(E_XKB_Layout, 1);
        layout->name = eina_stringshare_add(strtok(tmp, " "));

        free(tmp);

        p += strlen(layout->name);
        while (p[0] == ' ') ++p;

        variant = E_NEW(E_XKB_Variant, 1);
        variant->name = "basic";
        variant->description = "Default layout variant";

        layout->description = eina_stringshare_add(p);
        layout->used        = EINA_FALSE;
        layout->model       = NULL;
        layout->variant     = NULL;
        layout->variants    = eina_list_append(layout->variants, variant);

        layouts = eina_list_append(layouts, layout);
    } else break;

    fgets(buf, sizeof(buf), f);

    /* read variants here */
    for (;;) if (fgets(buf, sizeof(buf), f))
    {
        char *n = strchr(buf, '\n');
        if   (n) *n = '\0';

        if (!buf[0]) break;


        char *p   = buf + 2;
        char *tmp = strdup(p);

        variant = E_NEW(E_XKB_Variant, 1);
        variant->name = eina_stringshare_add(strtok(tmp, " "));

        char   *tok = strtok(NULL, " ");
        *strchr(tok, ':') = '\0';

        layout =
            eina_list_search_unsorted(layouts, layout_sort_by_name_cb, tok);
        layout->variants = eina_list_append(layout->variants, variant);

        p += strlen(variant->name);
        while (p[0] == ' ') ++p;
        p += strlen(tok);
        p += 2;

        free(tmp);

        variant->description = eina_stringshare_add(p);
    } else break;

    /* Sort layouts */
    layouts =
        eina_list_sort(layouts, eina_list_count(layouts), layout_sort_cb);
}

void clear_rules()
{
    E_XKB_Variant *v  = NULL;
    E_XKB_Layout  *la = NULL;
    E_XKB_Model   *m  = NULL;
    Eina_List     *ll = NULL;
    Eina_List     *l  = NULL;

    EINA_LIST_FOREACH(layouts, l, la)
    {
        EINA_LIST_FOREACH(la->variants, ll, v)
            free(v);
        eina_list_free(la->variants);
        free(la);
    }
    eina_list_free(layouts);

    EINA_LIST_FOREACH(models, l, m)
        free(m);
    eina_list_free(models);
}

int layout_sort_cb(const void *data1, const void *data2)
{
    const E_XKB_Layout *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int model_sort_cb(const void *data1, const void *data2)
{
    const E_XKB_Model *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int variant_sort_cb(const void *data1, const void *data2)
{
    const E_XKB_Variant *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int model_sort_by_name_cb(const void *data1, const void *data2)
{
    const E_XKB_Model *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int variant_sort_by_name_cb(const void *data1, const void *data2)
{
    const E_XKB_Variant *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int layout_sort_by_name_cb(const void *data1, const void *data2)
{
    const E_XKB_Layout *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int model_sort_by_label_cb(const void *data1, const void *data2)
{
    const E_XKB_Model *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    return strcmp(l1->description, l2);
}

int variant_sort_by_label_cb(const void *data1, const void *data2)
{
    const E_XKB_Variant *l1 = NULL;
    const char *l2 = NULL;
    char buf[128];

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    /* XXX This is nasty, see below */
    snprintf(buf, sizeof(buf), "%s (%s)", l1->name, l1->description);
    return strcmp(buf, l2);
}

int layout_sort_by_label_cb(const void *data1, const void *data2)
{
    const E_XKB_Layout *l1 = NULL;
    const char *l2 = NULL;
    char buf[128];

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    /* XXX This is nasty, but it's definitely better than iterating
     * over the whole list. User-defined property for ilist item would
     * solve, but E widget system lacks this currently.
     */
    snprintf(buf, sizeof(buf), "%s (%s)", l1->description, l1->name);
    return strcmp(buf, l2);
}
