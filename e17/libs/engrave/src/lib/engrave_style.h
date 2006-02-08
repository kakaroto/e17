#ifndef ENGRAVE_STYLE_H
#define ENGRAVE_STYLE_H

/**
 * @file engrave_style.h Engrave_Style object functions.
 * @brief Contains all of the functions to manipulate Engrave_Style objects.
 */

/**
 * @defgroup Engrave_Style Engrave_Style: Functions to work with Engrave_Style blocks.
 *
 * @{
 */

/**
 * The Engrave_Style typedef
 */
typedef struct _Engrave_Style Engrave_Style;
typedef struct _Engrave_Tag Engrave_Tag;

/**
 * Contains the needed style values
 */
struct _Engrave_Style
{
  char *name;               /**< The style name */
  char *base;               /**< The style base */
  Evas_List * tags;		/**< The style tags */

  void *parent;     /**< Pointer to parent */
};

struct _Engrave_Tag
{
  char * key;
  char * val;
};

Engrave_Style *engrave_style_new();
Engrave_Tag *engrave_tag_new(const char * key, const char * val);
void engrave_style_name_set(Engrave_Style *es, char *name);
void engrave_style_base_set(Engrave_Style *es, char *base);
void engrave_style_tag_add(Engrave_Style *es, Engrave_Tag *tag);

void engrave_style_parent_set(Engrave_Style *es, void *parent);


const char * engrave_style_name_get(Engrave_Style *es);
const char * engrave_style_base_get(Engrave_Style *es);
Evas_List * engrave_style_tag_get(Engrave_Style *es);

void engrave_tag_free(Engrave_Tag *tag);

/**
 * @}
 */

#endif

