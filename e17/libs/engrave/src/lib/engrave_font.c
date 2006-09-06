#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_font_new - create a new Engrave_Font object.
 * @param path: The path to the font
 * @param name: The name for the font
 *
 * @return Returns a pointer to a newly allocated Engrave_Font on success,
 * or NULL on failure.
 */
EAPI Engrave_Font *
engrave_font_new(const char *path, const char *name)
{
  Engrave_Font *ef;
  ef = NEW(Engrave_Font, 1);
  if (!ef) return NULL;

  ef->name = (name ? strdup(name) : NULL);
  ef->path = (path ? strdup(path) : NULL);
  return ef;
}

/**
 * engrave_font_free - free the memory
 * @param ef: The Engrave_Font to free
 *
 * @return Returns no value
 */
EAPI void
engrave_font_free(Engrave_Font *ef)
{
  if (!ef) return;

  IF_FREE(ef->name);
  IF_FREE(ef->path);
  FREE(ef);
}

/**
 * engrave_font_name_get - get the font name
 * @param ef: The Engrave_Font to get the name from
 *
 * @return Returns the name of the font on success or NULL on failure.
 */
EAPI const char *
engrave_font_name_get(Engrave_Font *ef)
{
  return (ef ? ef->name : NULL);
}

/**
 * engrave_font_path_get - get the font path
 * @param ef: The Engrave_Font to get the path from 
 *
 * @return Returns the path of the font on success or NULL on failure.
 */
EAPI const char *
engrave_font_path_get(Engrave_Font *ef)
{
  return (ef ? ef->path : NULL);
}

/**
 * engrave_font_parent_set - set the parent of the font node
 * @param font: The Engrave_Font to set the parent into
 * @param ef: The Engrave_File to set as parent
 *
 * @return Retruns no value.
 */
EAPI void
engrave_font_parent_set(Engrave_Font *font, void *ef)
{
    if (!font) return;
    font->parent = ef;
}

/**
 * engrave_font_parent_get - get the parent pointer
 * @param ef: The Engrave_Font to get the parent from
 * 
 * @return Returns the pointer to the parent or NULL if none set
 */
EAPI void *
engrave_font_parent_get(Engrave_Font *ef)
{
    return (ef ? ef->parent : NULL);
}




