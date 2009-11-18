#include "test.h"

void test_box_vert (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("box-vert", ELM_WIN_BASIC);
  win->setTitle ("Box Vert");
  win->setAutoDel (true);

  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ic->setAlignHintSize (0.5, 0.5);
  bx->packEnd (*ic);
  ic->show ();

  ElmIcon *ic2 = ElmIcon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ic2->setAlignHintSize (0.0, 0.5);
  bx->packEnd (*ic2);
  ic2->show ();
  
  ElmIcon *ic3 = ElmIcon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ic3->setAlignHintSize (EVAS_HINT_EXPAND, 0.5);
  bx->packEnd (*ic3);
  ic3->show ();

  win->show ();
}

void test_box_horiz (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("box-horiz", ELM_WIN_BASIC);
  win->setTitle ("Box Horiz");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  bx->setOrientation (ElmBox::Horizontal);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  ic->setAlignHintSize (0.5, 0.5);
  bx->packEnd (*ic);
  ic->show ();

  ElmIcon *ic2 = ElmIcon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ic2->setAlignHintSize (0.5, 0.0);
  bx->packEnd (*ic2);
  ic2->show ();
  
  ElmIcon *ic3 = ElmIcon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ic3->setAlignHintSize (0.0, EVAS_HINT_EXPAND);
  bx->packEnd (*ic3);
  ic3->show ();

  win->show ();
}
