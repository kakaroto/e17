#ifndef ENTICE_IMAGE_H
#define ENTICE_IMAGE_H

/**
 * Entice_Image - goal is to make a zoomable evas image that keeps aspect
 * for easy swallowing into an edje with an api that allows us to modify it
 * externally.
 */
#include <Evas.h>
#include <Ecore.h>
#define X_DISPLAY_MISSING
#include <Imlib2.h>
#undef X_DISPLAY_MISSING

typedef struct _Entice_Image Entice_Image;
typedef enum _Entice_Image_Scroll_Direction Entice_Scroll_Direction;
typedef enum _Entice_Image_Moving_State Entice_Image_Moving_State;

enum _Entice_Image_Moving_State
{
   ENTICE_IMAGE_MOVE_DEFAULT = 0,
   ENTICE_IMAGE_MOVE_DRAGGING,
   ENTICE_IMAGE_MOVE_FIXING
};

enum _Entice_Image_Scroll_Direction
{
   ENTICE_SCROLL_NORTH = 0,
   ENTICE_SCROLL_EAST,
   ENTICE_SCROLL_SOUTH,
   ENTICE_SCROLL_WEST
};

struct _Entice_Image
{
   int fit;
   int quality;                 /* int value for saving quality */
   double zoom;                 /* the current zoom percentage */
   struct
   {
      int button;               /* the drag button */
      int dragging;             /* whether we're currently dragging */
      Evas_Coord off_x, off_y;
   } mouse;

   struct
   {
      double x, y;
   } align;
   char *filename;              /* we need to keep track of this */
   char *format;                /* we need to keep track of this too */
   Entice_Image_Moving_State state;
   /* current moving state */
   Evas_Coord x, y, w, h;       /* geometry */
   int iw, ih;                  /* image size */
   Evas_Coord dx, dy;
   Evas_Object *obj;            /* the image object */
   Evas_Object *clip;           /* clip to this area when we swallow */
};


/* entice_image_new - generate a new image from an existing thumb
 * @o - an E_Thumb object
 * we can get all the image info we'll need from o
 */
Evas_Object *entice_image_new(Evas_Object * o);

/*
void entice_image_free(Entice_Image *im);
*/

int entice_image_zoom_fit_get(Evas_Object * o);
void entice_image_zoom_fit(Evas_Object * o);
void entice_image_zoom_reset(Evas_Object * o);
void entice_image_zoom_in(Evas_Object * o);
void entice_image_zoom_out(Evas_Object * o);
double entice_image_zoom_get(Evas_Object * o);
void entice_image_zoom_set(Evas_Object * o, double zoom);
void entice_image_zoom_focused_set(Evas_Object * o, double zoom, double x,
                                   double y);
const char *entice_image_file_get(Evas_Object * o);
const char *entice_image_format_get(Evas_Object * o);
int entice_image_rotate(Evas_Object * o, int direction);
int entice_image_flip(Evas_Object * o, int direction);
int entice_image_save(Evas_Object * o);
void entice_image_file_set(Evas_Object * o, const char *filename);
void entice_image_format_set(Evas_Object * o, const char *format);
void entice_image_dragable_set(Evas_Object * o, int dragging);
void entice_image_geometry_get(Evas_Object * o, Evas_Coord * x,
                               Evas_Coord * y, Evas_Coord * w,
                               Evas_Coord * h);
void entice_image_x_align_set(Evas_Object * o, double align);
void entice_image_y_align_set(Evas_Object * o, double align);
double entice_image_x_align_get(Evas_Object * o);
double entice_image_y_align_get(Evas_Object * o);
void entice_image_save_quality_set(Evas_Object * o, int quality);

#endif
