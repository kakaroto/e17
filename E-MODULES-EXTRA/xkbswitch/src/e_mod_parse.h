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

extern Eina_List *models;
extern Eina_List *layouts;

#endif
