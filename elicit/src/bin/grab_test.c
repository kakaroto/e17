#include <stdlib.h>
#include <stdio.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Imlib2.h>
#include "grab.h"

static int grab = 0;
static int win_x = 0, win_y = 0;

void
test_grab_region(Evas_Object *img, int x, int y, int w, int h, int grab)
{
  void *data;

  evas_object_image_size_set(img, w, h);
  data = evas_object_image_data_get(img, 1);
  elicit_grab_region(x, y, w, h, grab, data);
  evas_object_image_data_set(img, data);
  evas_object_image_data_update_add(img, 0, 0, w, h);
  evas_object_image_fill_set(img, 0, 0, w, h);
}

void
test_imlib_grab(Evas_Object *img, int x, int y, int w, int h)
{
  Imlib_Image *im;

  evas_object_image_size_set(img, w, h);

  /* setup the imlib context */
  imlib_context_set_display(ecore_x_display_get());
  imlib_context_set_drawable(RootWindow(ecore_x_display_get(),0));
  imlib_context_set_visual( DefaultVisual(ecore_x_display_get(), DefaultScreen(ecore_x_display_get() ) ));

  /* copy the correct part of the screen */
  im = imlib_create_image_from_drawable(0, x, y, w, h, 1);
  imlib_context_set_image(im);
  imlib_image_set_format("argb");

  evas_object_image_data_copy_set(img,imlib_image_get_data_for_reading_only());
  evas_object_image_data_update_add(img, 0, 0, w, h);
  evas_object_image_fill_set(img, 0, 0, w, h);

  imlib_free_image();
}

void
cb_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  grab = 1;
}

void
cb_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  grab = 0;
}

void
cb_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Move *ev = event_info;
  Evas_Object *img = data;

  if (grab)
    test_grab_region(img, ev->cur.output.x + win_x - 25, ev->cur.output.y + win_y - 25, 50, 50, 1);

}

void
cb_win_move(Ecore_Evas *ee)
{
  ecore_evas_geometry_get(ee, &win_x, &win_y, NULL, NULL);
}

int
main(int argc, char **argv)
{
  Ecore_Evas *ee;
  Evas *evas;
  Evas_Object *bg, *img;

  double start, end;
  int x, y;

  ecore_evas_init();

  ee = ecore_evas_software_x11_new(0,0,0,0,100,100);
  if (!ee)
  {
    printf("ick.\n");
    exit(1);
  }
  evas = ecore_evas_get(ee);
 
  bg = evas_object_rectangle_add(evas);
  evas_object_color_set(bg, 100, 100, 200, 255);
  evas_object_move(bg, 0,0);
  evas_object_resize(bg, 100,100);
  evas_object_show(bg);

  img = evas_object_image_add(evas);
  test_grab_region(img, 100, 100, 50, 50, 0);

  evas_object_move(img, 25,25);
  evas_object_resize(img, 50,50);
  evas_object_show(img);

  evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_DOWN, cb_down, img);
  evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_UP, cb_up, img);
  evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_MOVE, cb_move, img);

  ecore_evas_callback_move_set(ee, cb_win_move);


  ecore_evas_show(ee);

#if 0
  start = ecore_time_get();
  for (x = 0; x <= 1550; x += 50)
    for (y = 0; y <= 1150; y+= 50)
      test_grab_region(img, x, y, 50, 50, 0);
  end = ecore_time_get();

  printf("time taken: %f\n", end - start);
/*
  start = ecore_time_get();
  for (x = 0; x <= 1550; x += 50)
    for (y = 0; y <= 1150; y+= 50)
      test_grab_region(img, x, y, 50, 50, 1);
  end = ecore_time_get();

  printf("time taken: %f\n", end - start);

  start = ecore_time_get();
  for (x = 0; x <= 1550; x += 50)
    for (y = 0; y <= 1150; y+= 50)
      test_imlib_grab(img, x, y, 50, 50);
  end = ecore_time_get();
  printf("time taken: %f\n", end - start);
*/
#else
  ecore_main_loop_begin();
#endif

  return 0;
}

