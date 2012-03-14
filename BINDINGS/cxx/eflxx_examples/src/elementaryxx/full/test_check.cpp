#include "test.h"

void test_check (void *data, Evas_Object *obj, void *event_info)
{
  Icon *ic = NULL;
  Check *ck = NULL;

  Window *win = Window::factory ("check", ELM_WIN_BASIC);
  win->setTitle ("Checks");
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
  ck = Check::factory (*win);
  ck->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  ck->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  ck->setTextPart ("Icon sized to check");
  ck->setContentPart (*ic);
  ck->setState (true);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  ck = Check::factory (*win);
  ck->setTextPart ("Icon no scale");
  ck->setContentPart (*ic);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  ck = Check::factory (*win);
  ck->setTextPart ("Label Only");
  bx->packEnd (*ck);
  ck->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  ck = Check::factory (*win);
  ck->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  ck->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  ck->setTextPart ("Disabled check");
  ck->setContentPart (*ic);
  ck->setState (true);
  bx->packEnd (*ck);
  ck->setDisabled (true);
  ck->show ();
  ic->show ();

  ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  ck = Check::factory (*win);
  ck->setContentPart (*ic);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  win->show ();
}
