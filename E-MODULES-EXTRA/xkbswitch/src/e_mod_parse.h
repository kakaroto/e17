/*
 * XML parsing abstraction interface header.
 * Contains public structs and lists externs which are further used.
 */

#ifndef E_MOD_PARSE_H
#define E_MOD_PARSE_H

#include <e.h>

typedef struct _E_XKB_Model
{
    const char *name;
    const char *description;
} E_XKB_Model;

typedef struct _E_XKB_Variant
{
    const char *name;
    const char *description;
} E_XKB_Variant;

typedef struct _E_XKB_Layout
{
    const char *name;
    const char *description;

    Eina_List *variants;
} E_XKB_Layout;

int  parse_rules();
void clear_rules();
void  find_rules();

int layout_sort_cb          (const void *data1, const void *data2);
int model_sort_cb           (const void *data1, const void *data2);
int variant_sort_cb         (const void *data1, const void *data2);
int model_sort_by_name_cb   (const void *data1, const void *data2);
int variant_sort_by_name_cb (const void *data1, const void *data2);
int layout_sort_by_name_cb  (const void *data1, const void *data2);
int model_sort_by_label_cb  (const void *data1, const void *data2);
int variant_sort_by_label_cb(const void *data1, const void *data2);
int layout_sort_by_label_cb (const void *data1, const void *data2);

extern Eina_List *models;
extern Eina_List *layouts;

#endif
