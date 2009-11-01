#include "test.h"

void test_check (void *data, Evas_Object *obj, void *event_info)
{
  ElmIcon *ic = NULL;
  ElmCheck *ck = NULL;

  ElmWindow *win = ElmWindow::factory ("check", ELM_WIN_BASIC);
  win->setTitle ("Checks");
  win->setAutoDel (true);

  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  ck = ElmCheck::factory (*win);
  ck->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  ck->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  ck->setLabel ("Icon sized to check");
  ck->setIcon (*ic);
  ck->setState (true);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ck = ElmCheck::factory (*win);
  ck->setLabel ("Icon no scale");
  ck->setIcon (*ic);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  ck = ElmCheck::factory (*win);
  ck->setLabel ("Label Only");
  bx->packEnd (*ck);
  ck->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  ck = ElmCheck::factory (*win);
  ck->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  ck->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  ck->setLabel ("Disabled check");
  ck->setIcon (*ic);
  ck->setState (true);
  bx->packEnd (*ck);
  ck->setDisabled (true);
  ck->show ();
  ic->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ck = ElmCheck::factory (*win);
  ck->setIcon (*ic);
  bx->packEnd (*ck);
  ck->show ();
  ic->show ();

  win->show ();
}
