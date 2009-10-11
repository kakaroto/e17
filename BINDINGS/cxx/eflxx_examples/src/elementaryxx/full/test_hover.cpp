#include "test.h"

static void my_hover_bt (Evas_Object *obj, void *event_info, ElmHover *hv)
{
  hv->show ();
}

void test_hover (void *data, Evas_Object *obj, void *event_info)
{
  ElmButton *bt = NULL;
  ElmBox *bx = NULL;
  
  ElmWindow *win = ElmWindow::factory ("hover", ELM_WIN_BASIC);
  win->setTitle ("Hover");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  ElmHover *hv = ElmHover::factory (*win);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Button");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_hover_bt), hv));
  bx->packEnd (*bt);
  bt->show ();
  hv->setParent (*win);
  hv->setTarget (*bt);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Popup");
  hv->setContent ("middle", *bt);
  bt->show ();

  bx = ElmBox::factory (*win);

  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  bx->packEnd (*ic);
  ic->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 1");
  bx->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 2");
  bx->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 3");
  bx->packEnd (*bt);
  bt->show ();

  bx->show ();

  hv->setContent ("top", *bx);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Bottom");
  hv->setContent ("bottom", *bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Left");
  hv->setContent ("left", *bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Right");
  hv->setContent ("right", *bt);
  bt->show ();

  bg->setMinHintSize (size160x160);
  bg->setMaxHintSize (size640x640);
  win->resize (size320x320);
  win->show ();
}

void test_hover2 (void *data, Evas_Object *obj, void *event_info)
{
  ElmBox *bx = NULL;
  ElmButton *bt = NULL;
  
  ElmWindow *win = ElmWindow::factory ("hover2", ELM_WIN_BASIC);
  win->setTitle ("Hover 2");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  ElmHover *hv = ElmHover::factory (*win);
  hv->setStyle ("popout");

  bt = ElmButton::factory (*win);
  bt->setLabel ("Button");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_hover_bt), hv));
  bx->packEnd (*bt);
  bt->show ();
  hv->setParent (*win);
  hv->setTarget (*bt);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Popup");
  hv->setContent ("middle", *bt);
  bt->show ();

  bx = ElmBox::factory (*win);

  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  bx->packEnd (*ic);
  ic->show ();
  
  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 1");
  bx->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 2");
  bx->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Top 3");
  bx->packEnd (*bt);
  bt->show ();

  bx->show ();

  hv->setContent ("top", *bx);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Bot");
  hv->setContent ("bottom", *bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Left");
  hv->setContent ("left", *bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Right");
  hv->setContent ("right", *bt);
  bt->show ();
  
  bg->setMinHintSize (size160x160);
  bg->setMaxHintSize (size640x640);
  win->resize (size320x320);
  win->show ();
}
