#include "test.h"

void test_radio (void *data, Evas_Object *obj, void *event_info)
{
  Radio *rd = NULL;
  Radio *rdg = NULL;
  Icon *ic = NULL;

  Window *win = Window::factory ("radio", ELM_WIN_BASIC);
  win->setTitle ("Radios");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  rd = Radio::factory (*win);
  rd->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  rd->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  rd->setText ("Icon sized to radio");
  rd->setContent (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();
  rdg = rd;

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  rd = Radio::factory (*win);
  rd->setStateValue (1);
  rd->addGroup (*rdg);
  rd->setText ("Icon no scale");
  rd->setContent (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();

  rd = Radio::factory (*win);
  rd->setStateValue (2);
  rd->addGroup (*rdg);
  rd->setText ("Label Only");
  bx->packEnd (*rd);
  rd->show ();

  rd = Radio::factory (*win);
  rd->setStateValue (3);
  rd->addGroup (*rdg);
  rd->setText ("Disabled");
  rd->setDisabled (true);
  bx->packEnd (*rd);
  rd->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  rd = Radio::factory (*win);
  rd->setStateValue (4);
  rd->addGroup (*rdg);
  rd->setContent (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  rd = Radio::factory (*win);
  rd->setStateValue (5);
  rd->addGroup (*rdg);
  rd->setContent (*ic);
  rd->setDisabled (true);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();

  rdg->setValue (2);

  win->show ();
}
