#include "common.h"
#include <X11/Xlib.h>
#include "image.h"
#include <freetype.h>
#include "font.h"

static ImlibFont *fonts = NULL;
static DATA8 rend_lut[9] = 
{ 0, 64, 128, 192, 255, 255, 255, 255, 255};

static TT_Raster_Map *
create_font_raster(int width, int height)
{
   TT_Raster_Map      *rmap;
   
   rmap = malloc(sizeof(TT_Raster_Map));
   rmap->width = (width + 3) & -4;
   rmap->rows = height;
   rmap->flow = TT_Flow_Down;
   rmap->cols = rmap->width;
   rmap->size = rmap->rows * rmap->width;
   rmap->bitmap = malloc(rmap->size);
   memset(rmap->bitmap, 0, rmap->size);
   return rmap;
}

static TT_Raster_Map *
duplicate_raster(TT_Raster_Map * rmap)
{
   TT_Raster_Map      *new_rmap;
   
   new_rmap = malloc(sizeof(TT_Raster_Map));
   *new_rmap = *rmap;
   new_rmap->bitmap = malloc(new_rmap->size);
   memcpy(new_rmap->bitmap, rmap->bitmap, new_rmap->size);
   return new_rmap;
}

static void
clear_raster(TT_Raster_Map * rmap)
{
   memset(rmap->bitmap, 0, rmap->size);
}

static void
destroy_font_raster(TT_Raster_Map * rmap)
{
   free(rmap->bitmap);
   free(rmap);
}

