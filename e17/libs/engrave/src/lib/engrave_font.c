#include <Engrave.h>

/**
 * engrave_font_new - create a new Engrave_Font object.
 * @param path: The path to the font
 * @param name: The name for the font
 *
 * @return Returns a pointer to a newly allocated Engrave_Font on success,
 * or NULL on failure.
 */
Engrave_Font *
engrave_font_new(char *path, char *name)
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
void
engrave_font_free(Engrave_Font *ef)
{
  if (!ef) return;

  if (ef->name) free(ef->name);
  ef->name = NULL;

  if (ef->path) free(ef->path);
  ef->path = NULL;

  free(ef);
  ef = NULL;
}

/**
 * engrave_font_name_get - get the font name
 * @param ef: The Engrave_Font to get the name from
 *
 * @return Returns a pointer to the name of the font
 * on success or NULL on error. This pointer must be freed by the user.
 */
char *
engrave_font_name_get(Engrave_Font *ef)
{
  if (!ef) return NULL;
  return (ef->name ? strdup(ef->name) : NULL);
}

/**
 * engrave_font_path_get - get the font path
 * @param ef: The Engrave_Font to get the path from 
 *
 * @return Returns a pointer to the path on success 
 * or NULL on error. This pointer must be freed by the user.
 */
char *
engrave_font_path_get(Engrave_Font *ef)
{
  if (!ef) return NULL;
  return (ef->path ? strdup(ef->path) : NULL);
}


