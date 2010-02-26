#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "palette.h"

#define BUF_SIZE 1024

/**
 * Create a palette
 * @return the new palette
 */
Palette *
palette_new(void)
{
  Palette *palette;
  palette = calloc(1, sizeof(Palette));
  palette->colors = NULL;

  return palette;
}

/**
 * Free a palette
 * @param palette
 */
void
palette_free(Palette *palette)
{
  Eina_List *l;
  Color *c;

  if (!palette) return;
  if (palette->name) eina_stringshare_del(palette->name);
  if (palette->filename) eina_stringshare_del(palette->filename);
  EINA_LIST_FOREACH(palette->colors, l, c)
  {
    color_unref(c);
  }

  eina_list_free(palette->colors);
  free(palette);
}

/**
 * Set the name of a palette
 * @param palette
 * @param name
 */
void 
palette_name_set(Palette *palette, const char *name)
{
  if (palette->name) eina_stringshare_del(palette->name);
  if (name)
    palette->name = eina_stringshare_add(name);
  else
    palette->name = NULL;
}

/**
 * Get the name of a palette
 * @param palette
 * @return the name or "" if none is set
 */
const char *
palette_name_get(Palette *palette)
{
  return palette->name ? palette->name : "";
}

/**
 * Set the filename to save a palette to
 * @param palette
 * @param filename
 */
void 
palette_filename_set(Palette *palette, const char *filename)
{
  if (palette->filename) eina_stringshare_del(palette->filename);
  if (filename)
    palette->filename = eina_stringshare_add(filename);
  else
    palette->filename = NULL;
}

/**
 * Get the filename that this palette was loaded from or set to be saved to
 * @param palette
 * @return the filename or "" if none is set
 */
const char *
palette_filename_get(Palette *palette)
{
  return palette->filename ? palette->filename : "";
}

/**
 * Set the number of columns to display a palette in
 * @param palette
 * @param columns
 */
void
palette_columns_set(Palette *palette, int columns)
{
  palette->columns = columns;
}

/**
 * Get the number of columns to display a palette in
 * @param palette
 * @return the number of columns (0 for 'use global preference')
 */
int
palette_columns_get(Palette *palette)
{
  return palette->columns;
}

/**
 * Load a palette from disk
 * @param palette
 * @param filename
 * @return 0 for success, or negative error code
 *
 * Error Codes:
 *  -1 unable to open file
 *  -2 invalid file format
 *  -3 other (out of memory?)
 */
int
palette_load(Palette *palette, const char *filename)
{
  FILE *f;
  char buf[BUF_SIZE];
  char name[1024];
  int r, g, b;
  int columns;

  name[0] = 0;

  palette_filename_set(palette, filename);

  f = fopen(filename, "r");
  if (!f) return -1;

  buf[0] = 0;
  ;
  if (fgets(buf, sizeof(buf), f) && strncmp("GIMP Palette", buf, 12)) return -2;

  palette_clear(palette);

  while(fgets(buf, sizeof(buf), f))
  {
    int num;
    if (buf[0] == '#' || buf[0] == '\n') continue;


    num = sscanf(buf, "%3d %3d %3d\t%1024s\n", &r, &g, &b, name);
    if (num >= 3)
    {
      Color *c = color_new();
      if (!c) goto error;
      color_rgba_set(c, r, g, b, 255);
      if (num == 4) color_name_set(c, name);
      palette_color_append(palette, c);
      color_unref(c);
    }
    else if (sscanf(buf, "Name: %[^\n]\n", name))
      palette_name_set(palette, name);
    else if (sscanf(buf, "Columns: %d\n", &columns))
      palette_columns_set(palette, columns);
  }
  fclose(f);

  return 0;
error:

  if (f) fclose(f);
  return -3;
}

/**
 * Save a palette to disk
 * @param palette
 * @return 0 on success or negative error code
 *
 * Errors:
 *  -1 filename not set
 *  -2 could not open file for writing
 */
int
palette_save(Palette *palette)
{
  Eina_List *l;
  Color *c;
  FILE *f;

  if (!palette->filename) return -1;
  f = fopen(palette->filename, "w");
  if (!f) return -2;

  fprintf(f, "GIMP Palette\nName: %s\nColumns: %d\n#\n", palette_name_get(palette), palette_columns_get(palette));

  EINA_LIST_FOREACH(palette->colors, l, c) {
    int r, g, b, a;
    color_rgba_get(c, &r, &g, &b, &a);

    fprintf(f, "%3d %3d %3d\t%s\n", r, g, b, color_name_get(c));
  }

  fclose(f);
  return 0;
}
/**
 * Get the list of colors in a palette
 * @param pallete
 * @return Eina_List of colors - modifying this list will modify the colors in the palette
 */
Eina_List *
palette_colors_get(Palette *palette)
{
  return palette->colors;
}

/**
 * Clear the colors in a palette
 * @param palette
 */
void
palette_clear(Palette *palette)
{
  Eina_List *l;
  Color *c;

  EINA_LIST_FOREACH(palette->colors, l, c)
    color_unref(c);
  eina_list_free(palette->colors);
  palette->colors = NULL;
}

/**
 * Append a color to the palette list
 * @param palette
 * @param color
 */
void
palette_color_append(Palette *palette, Color *color)
{
  color_ref(color);
  palette->colors = eina_list_append(palette->colors, color);
}

/**
 * Prepend a color to the palette list
 * @param palette
 * @param color
 */
void
palette_color_prepend(Palette *palette, Color *color)
{
  palette->colors = eina_list_prepend(palette->colors, color);
}

void
palette_color_remove(Palette *palette, Color *color)
{
  palette->colors = eina_list_remove(palette->colors, color);
  color_unref(color);
}
