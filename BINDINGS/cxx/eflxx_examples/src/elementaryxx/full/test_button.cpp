#include "test.h"

void test_button (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("buttons", ELM_WIN_BASIC);
  win->setTitle ("Buttons");
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
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  ElmButton *bt = ElmButton::factory (*win);
  bt->setLabel ("Icon sized to button");
  bt->setIcon (*ic);
  bx->packEnd (*bt);
  bt->show ();
  ic->show ();

  ElmIcon *ic2 = ElmIcon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ElmButton *bt2 = ElmButton::factory (*win);
  bt2->setLabel ("Icon no scale");
  bt2->setIcon (*ic2);
  bx->packEnd (*bt2);
  bt2->show ();
  ic2->show ();

  ElmButton *bt3 = ElmButton::factory (*win);
  bt3->setLabel ("Label Only");
  bx->packEnd (*bt3);
  bt3->show ();

  ElmIcon *ic3 = ElmIcon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ElmButton *bt4 = ElmButton::factory (*win);
  bt4->setLabel ("Icon no scale");
  bt4->setIcon (*ic3);
  bx->packEnd (*bt4);
  bt4->show ();
  ic3->show ();
  
  win->show ();
}