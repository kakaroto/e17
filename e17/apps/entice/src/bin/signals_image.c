#include <stdio.h>
#include <stdlib.h>
#include "signals_image.h"
#include "entice.h"

#define DEBUG 0

/* EnticeImageDelete*/
void
_entice_delete_current(void *data, Evas_Object * o, const char *emission,
                       const char *source)
{
   entice_delete_current();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageRemove*/
void
_entice_remove_current(void *data, Evas_Object * o, const char *emission,
                       const char *source)
{
   entice_remove_current();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageNext */
void
_entice_image_next(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   entice_load_next();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImagePrev */
void
_entice_image_prev(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   entice_load_prev();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomIn */
void
_entice_zoom_in(void *data, Evas_Object * o, const char *emission,
                const char *source)
{
   entice_zoom_in();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomOut */
void
_entice_zoom_out(void *data, Evas_Object * o, const char *emission,
                 const char *source)
{
   entice_zoom_out();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomDefault */
void
_entice_zoom_default(void *data, Evas_Object * o, const char *emission,
                     const char *source)
{
   entice_zoom_reset();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomFit */
void
_entice_zoom_fit(void *data, Evas_Object * o, const char *emission,
                 const char *source)
{
   entice_zoom_fit();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomInFocused */
/* FIXME: get mouse coords from ecore, if image is under mouse center there */
void
_entice_zoom_in_focused(void *data, Evas_Object * o, const char *emission,
                        const char *source)
{
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageZoomOutFocused */
/* FIXME: get mouse coords from ecore, if image is under mouse center there */
void
_entice_zoom_out_focused(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* FIXME: I'm not sure how to do this with edje
 * README.theme states EnticeImageFitWindow should resize the window to
 * fit the image, for now I"m omitting this feature.
 */
void
_entice_fit_window(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageRotateLeft */
void
_entice_rotate_left(void *data, Evas_Object * o, const char *emission,
                    const char *source)
{
   entice_rotate_image_left();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageRotateRight */
void
_entice_rotate_right(void *data, Evas_Object * o, const char *emission,
                     const char *source)
{
   entice_rotate_image_right();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageFlipH */
void
_entice_flip_horizontal(void *data, Evas_Object * o, const char *emission,
                        const char *source)
{
   entice_flip_horizontal();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageFlipV */
void
_entice_flip_vertical(void *data, Evas_Object * o, const char *emission,
                      const char *source)
{
   entice_flip_vertical();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeFullScreen */
void
_entice_fullscreen(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   entice_fullscreen_toggle();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeThumbScrollNextStart */
void
_entice_thumbs_scroll_next_start(void *data, Evas_Object * o,
                                 const char *emission, const char *source)
{
   entice_thumbs_scroll_next_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeThumbScrollPrevStart */
void
_entice_thumbs_scroll_prev_start(void *data, Evas_Object * o,
                                 const char *emission, const char *source)
{
   entice_thumbs_scroll_prev_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeThumbScrollStop */
void
_entice_thumbs_scroll_stop(void *data, Evas_Object * o, const char *emission,
                           const char *source)
{
   entice_thumbs_scroll_stop();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageScrollEastStart */
void
_entice_image_scroll_east_start(void *data, Evas_Object * o,
                                const char *emission, const char *source)
{
#if DEBUG
   fprintf(stderr, "SCROLL EAST!!!\n");
#endif
   entice_main_image_scroll_east_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageScrollWestStart */
void
_entice_image_scroll_west_start(void *data, Evas_Object * o,
                                const char *emission, const char *source)
{
#if DEBUG
   fprintf(stderr, "SCROLL WEST!!!\n");
#endif
   entice_main_image_scroll_west_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageScrollNorthStart */
void
_entice_image_scroll_north_start(void *data, Evas_Object * o,
                                 const char *emission, const char *source)
{
#if DEBUG
   fprintf(stderr, "SCROLL NORTH!!!\n");
#endif
   entice_main_image_scroll_north_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageScrollSouthStart */
void
_entice_image_scroll_south_start(void *data, Evas_Object * o,
                                 const char *emission, const char *source)
{
#if DEBUG
   fprintf(stderr, "SCROLL SOUTH!!!\n");
#endif
   entice_main_image_scroll_south_start();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageScrollStop */
void
_entice_image_scroll_stop(void *data, Evas_Object * o, const char *emission,
                          const char *source)
{
   entice_main_image_scroll_stop();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeQuit */
void
_entice_quit(void *data, Evas_Object * o, const char *emission,
             const char *source)
{
   ecore_main_loop_quit();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageModified */
void
_entice_image_modified(void *data, Evas_Object * o, const char *emission,
                       const char *source)
{
#if DEBUG
   fprintf(stderr, "Image has been modified !!! :)\n");
#endif
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* EnticeImageSave */
void
_entice_image_save(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
#if DEBUG
   fprintf(stderr, "Image Save Request !!! :)\n");
#endif
   entice_save_image();
   return;
   data = NULL;
   o = NULL;
   emission = NULL;
   source = NULL;
}

/* drag,* */
void
_entice_image_drag_stop(void *data, Evas_Object * o, const char *emission,
                        const char *source)
{
#if DEBUG
   fprintf(stderr, "Drag stopped\n");
#endif
   if ((source) && !strcmp(source, "EnticeImage"))
      entice_dragable_image_set(0);
}

void
_entice_image_drag_start(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
#if DEBUG
   fprintf(stderr, "Drag started\n");
#endif
   if ((source) && !strcmp(source, "EnticeImage"))
      entice_dragable_image_set(1);
}
