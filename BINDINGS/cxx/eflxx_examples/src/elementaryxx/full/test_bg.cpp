#include "test.h"



void test_bg_plain (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("bg-plain", ELM_WIN_BASIC);
  win->setTitle ("Bg Plain");
  win->setAutoDel(true);

  Background *bg = Background::factory (*win);
  /* allow bg to expand in x & y */
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bg);
  bg->show ();

  /* set size hints. a minimum size for the bg. this should propagate back
   * to the window thus limiting its size based off the bg as the bg is one
   * of the window's resize objects. */
  bg->setMinHintSize (size160x160);
  /* and set a maximum size. not needed very often. normally used together
   * with evas_object_size_hint_min_set() at the same size to make a
   * window not resizable */
  bg->setMaxHintSize (size640x640);
  /* and now just resize the window to a size you want. normally widgets
   * will determine the initial size though */
  win->resize (size320x320);
  /* and show the window */
  win->show ();
}

void test_bg_image (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("bg-image", ELM_WIN_BASIC);
  win->setTitle ("Bg Image");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  bg->setFile (searchPixmapFile ("elementaryxx/plant_01.jpg"));
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bg);
  bg->show ();

  bg->setMinHintSize (size160x160);
  bg->setMaxHintSize (size640x640);

  win->resize (Size (size320x320));
  win->show ();
}
