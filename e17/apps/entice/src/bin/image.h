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
   double zoom;                 /* the current zoom percentage */
   struct
   {
      Ecore_Timer *timer;
      double velocity, start_time, x, y;
      Entice_Scroll_Direction direction;
   } scroll;
   char *filename;              /* we need to keep track of this */
   char *format;                /* we need to keep track of this too */
   int x, y, w, h, iw, ih;      /* geometry */
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
void entice_image_scroll_stop(Evas_Object * o);
void entice_image_scroll_start(Evas_Object * o, Entice_Scroll_Direction d);
void entice_image_scroll(Evas_Object * o, Entice_Scroll_Direction d, int val);
const char *entice_image_file_get(Evas_Object * o);
const char *entice_image_format_get(Evas_Object * o);
int entice_image_rotate(Evas_Object * o, int direction);
int entice_image_flip(Evas_Object * o, int direction);
int entice_image_save(Evas_Object * o);
void entice_image_file_set(Evas_Object * o, const char *filename);
void entice_image_format_set(Evas_Object * o, const char *format);
void entice_image_edje_set(Evas_Object * o, Evas_Object * edje);

#endif
