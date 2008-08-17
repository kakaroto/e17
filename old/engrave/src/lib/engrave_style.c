#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_style_new - create a new Engrave_Style object.
 *
 * @return Returns a pointer to a newly allocated Engrave_Style object on
 * success or NULL on failure.
 */
EAPI Engrave_Style *
engrave_style_new()
{
  Engrave_Style *style;

  style = NEW(Engrave_Style, 1);
  if (!style) return NULL;

  style->name = NULL;
  style->base = NULL;
  style->tags = NULL;

  return style;
}

EAPI Engrave_Tag *
engrave_tag_new(const char *key, const char *val)
{
  Engrave_Tag *tag;

  tag = NEW(Engrave_Tag, 1);
  if (!tag) return NULL;

  tag->key = key ? strdup(key) : NULL;
  tag->val = val ? strdup(val) : NULL;

  return tag;  
}

void engrave_tag_free(Engrave_Tag *tag)
{
  if (!tag) return;
  IF_FREE(tag->key);
  IF_FREE(tag->val);
  IF_FREE(tag);
}

void engrave_style_name_set(Engrave_Style *es, char * name)
{

  if (!es) return;
  IF_FREE(es->name);
  es->name = (name ? strdup(name) : NULL);

}

void engrave_style_base_set(Engrave_Style *es, char * base)
{

  if (!es) return;
  IF_FREE(es->base);
  es->base = (base ? strdup(base) : NULL);

}

void engrave_style_tag_add(Engrave_Style *es, Engrave_Tag * tag)
{

  if (!es || !tag) return;
  es->tags = evas_list_append(es->tags, tag);

}


void engrave_style_parent_set(Engrave_Style *es, void *parent)
{
  if (!es) return;
  es->parent = parent;
}


const char * engrave_style_name_get(Engrave_Style *es) 
{
  return es ? es->name : NULL;
}

const char * engrave_style_base_get(Engrave_Style *es) 
{
  return es ? es->base : NULL;
}

Evas_List * engrave_style_tag_get(Engrave_Style *es) 
{
  return es ? es->tags : NULL;
}
