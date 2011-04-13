#include "test.h"

void test_toggle (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("toggles", ELM_WIN_BASIC);
  win->setTitle ("Toggles");
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
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  Toggle *tg = Toggle::factory (*win);
  tg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  tg->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  tg->setLabel ("Icon sized to toggle");
  tg->setIcon (*ic);
  tg->setState (true);
  tg->setLabelsStates ("Yes", "No");
  bx->packEnd (*tg);
  tg->show ();
  ic->show ();

  Icon *ic2 = Icon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  Toggle *tg2 = Toggle::factory (*win);
  tg2->setLabel ("Icon no scale");
  tg2->setIcon (*ic2);
  bx->packEnd (*tg2);
  tg2->show ();
  ic2->show ();

  Toggle *tg3 = Toggle::factory (*win);
  tg3->setLabel ("Label Only");
  tg3->setLabelsStates ("Big long fun times label", "Small long happy fun label");
  bx->packEnd (*tg3);
  tg3->show ();

  Icon *ic3 = Icon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  Toggle *tg4 = Toggle::factory (*win);
  tg4->setIcon (*ic3);
  bx->packEnd (*tg4);
  tg4->show ();
  ic3->show ();

  win->show ();
}
