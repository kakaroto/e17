#ifndef ELICIT_PALETTE_H
#define ELICIT_PALETTE_H

#include <Eina.h>
#include "color.h"

typedef struct Palette Palette;

struct Palette
{
  const char *name;
  const char *filename;
  Eina_List *colors;
  int columns;
};

Palette *palette_new();
void palette_free(Palette *palette);

void palette_name_set(Palette *palette, const char *name);
const char *palette_name_get(Palette *palette);

void palette_filename_set(Palette *palette, const char *filename);
const char *palette_filename_get(Palette *palette);

void palette_columns_set(Palette *palette, int columns);
int palette_columns_get(Palette *palette);

int palette_load(Palette *palette, const char *filename);
int palette_save(Palette *palette);
Eina_List *palette_colors_get(Palette *palette);

void palette_clear(Palette *palette);
void palette_color_append(Palette *palette, Color *color);
void palette_color_prepend(Palette *palette, Color *color);
void palette_color_remove(Palette *palette, Color *color);

#endif
