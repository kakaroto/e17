/*
 * XML parsing abstraction interface header.
 * Contains public structs and lists externs which are further used.
 */

#ifndef E_MOD_PARSE_H
#define E_MOD_PARSE_H

#include <e.h>
#include <libxml/xmlreader.h>

typedef struct _e_xkb_model
{
    const char *name;
    const char *description;
    const char *vendor;
} e_xkb_model;

typedef struct _e_xkb_layout
{
    const char *name;
    const char *description;
    const char *short_descr;
    Eina_List *variants;
} e_xkb_layout;

typedef struct _e_xkb_variant
{
    const char *name;
    const char *description;
} e_xkb_variant;

void parse_rules(const char *fname);
void clear_rules();

int _layout_sort_cb        (const void *data1, const void *data2);
int _model_sort_cb         (const void *data1, const void *data2);
int _variant_sort_cb       (const void *data1, const void *data2);
int _layout_sort_bylabel_cb(const void *data1, const void *data2);

extern Eina_List *models;
extern Eina_List *layouts;

#endif
