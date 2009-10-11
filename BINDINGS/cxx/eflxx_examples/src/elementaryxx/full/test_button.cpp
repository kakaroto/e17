  #include "test.h"

void test_button (void *data, Evas_Object *obj, void *event_info)
{
  ElmIcon *ic = NULL;
  ElmButton *bt = NULL;
  
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

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  bt = ElmButton::factory (*win);
  bt->setLabel ("Icon sized to button");
  bt->setIcon (*ic);
  bx->packEnd (*bt);
  bt->show ();
  ic->show ();
  
  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  bt = ElmButton::factory (*win);
  bt->setLabel ("Disabled Button");
  bt->setIcon (*ic);
  bt->setDisabled (true);
  bx->packEnd (*bt);
  bt->show ();
  ic->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  bt = ElmButton::factory (*win);
  bt->setLabel ("Icon no scale");
  bt->setIcon (*ic);
  bx->packEnd (*bt);
  bt->show ();
  ic->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Label Only");
  bx->packEnd (*bt);
  bt->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  bt = ElmButton::factory (*win);
  bt->setLabel ("Icon no scale");
  bt->setIcon (*ic);
  bx->packEnd (*bt);
  bt->show ();
  ic->show ();
  
  win->show ();
}