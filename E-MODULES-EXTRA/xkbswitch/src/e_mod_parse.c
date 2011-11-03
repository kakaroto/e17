/*
 * This file parses the XML file with rules into a tree with which we can work.
 * It's supposed to serve as simple interface to layouts and variants.
 * 
 * Reader example on libxml2 page used for reference.
 */

#include "e_mod_parse.h"

#define LOOP_TO(name, nt) \
while (rv == 1) \
{ \
    rv = xmlTextReaderRead(rdr); \
    if (xmlTextReaderNodeType(rdr) == nt) \
    { \
        node_retval nv = _parse_node(rdr); \
        if (!strcmp(nv.n, name)) \
        { \
            rv = xmlTextReaderRead(rdr); \
            break; \
        } \
    } \
}

#define NEXT_TO(name, catname) \
while (rv == 1) \
{ \
    rv = xmlTextReaderRead(rdr); \
    if (xmlTextReaderNodeType(rdr) == 1) \
    { \
        node_retval nv = _parse_node(rdr); \
        if (!strcmp(nv.n, name)) break; \
        if (!strcmp(nv.n, catname)) \
        { \
            m = 1; \
            break; \
        } \
    } \
}

typedef struct _node_retval
{
    const char *n;
    const char *v;
} node_retval;

node_retval _parse_node(xmlTextReaderPtr rdr);

Eina_List *layouts = NULL;
Eina_List *models  = NULL;

void parse_rules(const char *fname)
{
    e_xkb_model *model = NULL;
    e_xkb_layout *layout = NULL;
    e_xkb_variant *variant = NULL;

    xmlTextReaderPtr rdr;
    int rv = 0, i = 0, m = 0;
    char *tmp;

    rdr = xmlReaderForFile(fname, NULL, 0);
    if (!rdr) return;

    /* Get rid of useless beginning values */
    rv = xmlTextReaderRead(rdr);
    while (rv == 1)
    {
        if (!strcmp(_parse_node(rdr).n, "modelList"))
        {
            /* When we're at modelList, skip the closing block */
            rv = xmlTextReaderRead(rdr);
            break;
        }
        rv = xmlTextReaderRead(rdr);
    }

    /* Parse out models - we'll be at first 'model' */
    rv = xmlTextReaderRead(rdr);
    while (rv == 1 && !m)
    {
        model = E_NEW(e_xkb_model, 1);

        /* This moves us at 'name' value */
        LOOP_TO("name", 1)
        /* Assign the name value */
        model->name = _parse_node(rdr).v;

        /* Move to 'description' */
        LOOP_TO("description", 1)
        /* Assign the description value */
        model->description = _parse_node(rdr).v;

        /* Now the vendor */
        LOOP_TO("vendor", 1)
        model->vendor = _parse_node(rdr).v;

        /* Append the model into eina array */
        models = eina_list_append(models, model);

        /* Move to next 'model' */
        NEXT_TO("model", "layout")
    }

    /* Sort models */
    models = eina_list_sort(models, eina_list_count(models), _model_sort_cb);

    /* We're at first 'layout' here now */
    m = 0;
    while (rv == 1 && !m)
    {
        layout = E_NEW(e_xkb_layout, 1);

        LOOP_TO("name", 1)
        layout->name = _parse_node(rdr).v;

        LOOP_TO("shortDescription", 1)
        tmp = strdup(_parse_node(rdr).v);
        eina_str_toupper(&tmp);
        layout->short_descr = eina_stringshare_add(tmp);
        E_FREE(tmp);

        LOOP_TO("description", 1)
        layout->description = _parse_node(rdr).v;

        /* We got name, short description and description.
         * Now let's go on variants, which are more tricky.
         */

        /* Append 'basic' variant for every layout */
        variant = E_NEW(e_xkb_variant, 1);
        variant->name = "basic";
        variant->description = "Default layout variant";
        layout->variants = eina_list_append(layout->variants, variant);

        /* Loop at variantList */
        LOOP_TO("variantList", 1)

        /* Move forward; if we find layout there, means no variants.
         * But if we find variant there, we can add them.
         */
        rv = xmlTextReaderRead(rdr);

        /* Parse and find out. */
        node_retval nv = _parse_node(rdr);
        if (!strcmp(nv.n, "variant"))
        {
            variant = E_NEW(e_xkb_variant, 1);

            /* We got some variants (at least one), append the first one */
            LOOP_TO("name", 1)
            variant->name = _parse_node(rdr).v;

            LOOP_TO("description", 1)
            variant->description = _parse_node(rdr).v;

            layout->variants = eina_list_append(layout->variants, variant);

            /* Append more variants if present */
            while (rv == 1)
            {
                /* Go to end of current variant */
                LOOP_TO("variant", 15)

                /* See if variant is after that. */
                rv = xmlTextReaderRead(rdr);
                nv = _parse_node(rdr);

                if (!strcmp(nv.n, "variant"))
                {
                    variant = E_NEW(e_xkb_variant, 1);

                    LOOP_TO("name", 1)
                    variant->name = _parse_node(rdr).v;

                    LOOP_TO("description", 1)
                    variant->description = _parse_node(rdr).v;

                    layout->variants = eina_list_append(layout->variants, variant);
                }
                else
                {
                        /* Sort variants */
                        layout->variants =
                            eina_list_sort(
                                layout->variants,
                                eina_list_count(layout->variants),
                                _variant_sort_cb
                            );
                        break;
                }
            }
        }

        /* default values */
        layout->used    = EINA_FALSE;
        layout->model   = NULL;
        layout->variant = NULL;

        /* No matter if we found variants or not, add and move forward. */
        layouts = eina_list_append(layouts, layout);

        /* Move to next 'layout' */
        NEXT_TO("layout", "optionList")
    }

    /* Sort layouts */
    layouts = eina_list_sort(layouts, eina_list_count(layouts), _layout_sort_cb);

    xmlFreeTextReader(rdr);
}

void clear_rules()
{
    e_xkb_variant *v = NULL;
    e_xkb_layout *la = NULL;
    e_xkb_model *m = NULL;
    Eina_List *ll = NULL;
    Eina_List *l = NULL;

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

node_retval _parse_node(xmlTextReaderPtr rdr)
{
    const xmlChar *n, *v;

    n = xmlTextReaderConstName(rdr);
    if (!n) n = BAD_CAST"--";

    v = xmlTextReaderConstValue(rdr);

    return (node_retval){
        eina_stringshare_add(n),
        eina_stringshare_add(v)
    };
}

int _layout_sort_cb(const void *data1, const void *data2)
{
    const e_xkb_layout *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int _model_sort_cb(const void *data1, const void *data2)
{
    const e_xkb_model *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int _variant_sort_cb(const void *data1, const void *data2)
{
    const e_xkb_variant *l1 = NULL, *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    if (!l2->name) return -1;
    return strcmp(l1->name, l2->name);
}

int _model_sort_byname_cb(const void *data1, const void *data2)
{
    const e_xkb_model *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int _variant_sort_byname_cb(const void *data1, const void *data2)
{
    const e_xkb_variant *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int _layout_sort_byname_cb(const void *data1, const void *data2)
{
    const e_xkb_layout *l1 = NULL;
    const char *l2 = NULL;

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;
    return strcmp(l1->name, l2);
}

int _model_sort_bylabel_cb(const void *data1, const void *data2)
{
    const e_xkb_model *l1 = NULL;
    const char *l2 = NULL;
    char buf[128];

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    /* XXX This is nasty, see below */
    snprintf(buf, sizeof(buf), "%s (%s)", l1->description, l1->vendor);
    return strcmp(buf, l2);
}

int _variant_sort_bylabel_cb(const void *data1, const void *data2)
{
    const e_xkb_variant *l1 = NULL;
    const char *l2 = NULL;
    char buf[128];

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    /* XXX This is nasty, see below */
    snprintf(buf, sizeof(buf), "%s (%s)", l1->name, l1->description);
    return strcmp(buf, l2);
}

int _layout_sort_bylabel_cb(const void *data1, const void *data2)
{
    const e_xkb_layout *l1 = NULL;
    const char *l2 = NULL;
    char buf[128];

    if (!(l1 = data1)) return 1;
    if (!l1->name) return 1;
    if (!(l2 = data2)) return -1;

    /* XXX This is nasty, but it's definitely better than iterating
     * over the whole list. User-defined property for ilist item would
     * solve, but E widget system lacks this currently.
     */
    snprintf(buf, sizeof(buf), "%s (%s)", l1->description, l1->short_descr);
    return strcmp(buf, l2);
}
