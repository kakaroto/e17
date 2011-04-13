#include "test.h"



void test_box_vert (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("box-vert", ELM_WIN_BASIC);
  win->setTitle ("Box Vert");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  Icon *ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ic->setAlignHintSize (0.5, 0.5);
  bx->packEnd (*ic);
  ic->show ();

  Icon *ic2 = Icon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ic2->setAlignHintSize (0.0, 0.5);
  bx->packEnd (*ic2);
  ic2->show ();

  Icon *ic3 = Icon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ic3->setAlignHintSize (EVAS_HINT_EXPAND, 0.5);
  bx->packEnd (*ic3);
  ic3->show ();

  win->show ();
}

void test_box_horiz (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("box-horiz", ELM_WIN_BASIC);
  win->setTitle ("Box Horiz");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setOrientation (Box::Horizontal);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  Icon *ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ic->setAlignHintSize (0.5, 0.5);
  bx->packEnd (*ic);
  ic->show ();

  Icon *ic2 = Icon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ic2->setAlignHintSize (0.5, 0.0);
  bx->packEnd (*ic2);
  ic2->show ();

  Icon *ic3 = Icon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ic3->setAlignHintSize (0.0, EVAS_HINT_EXPAND);
  bx->packEnd (*ic3);
  ic3->show ();

  win->show ();
}
