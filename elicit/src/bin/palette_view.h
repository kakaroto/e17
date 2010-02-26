#ifndef PALETTE_VIEW_H
#define PALETTE_VIEW_H

#include <Evas.h>
#include <Ecore.h>
#include "palette.h"

typedef struct Palette_View Palette_View;

struct Palette_View
{
  Evas_Object *smart_obj;
  Palette *palette;
  Eina_List *rects;

  Color *selected;
  Evas_Object *selector;

  Evas_Coord x,y;
  Evas_Coord w,h;

  int size;
  int direction : 1;

  struct {
    char *file;
    char *group;
    int changed;
  } theme;

  Ecore_Timer *layout_timer;
};

Evas_Object *palette_view_add(Evas *evas);
void palette_view_palette_set(Evas_Object *obj, Palette *palette);
void palette_view_theme_set(Evas_Object *obj, const char *file, const char *group);
Palette *palette_view_palette_get(Evas_Object *obj);
void palette_view_changed(Evas_Object *obj);

void palette_view_select(Evas_Object *obj, Color *c);
Color *palette_view_selected(Evas_Object *obj);

#endif
