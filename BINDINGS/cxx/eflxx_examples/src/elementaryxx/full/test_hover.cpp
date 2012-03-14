#include "test.h"



static void my_hover_bt (Evasxx::Object &obj, void *event_info, Hover *hv)
{
  hv->show ();
}

void test_hover (void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;
  Box *bx = NULL;

  Window *win = Window::factory ("hover", ELM_WIN_BASIC);
  win->setTitle ("Hover");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  Hover *hv = Hover::factory (*win);

  bt = Button::factory (*win);
  bt->setTextPart ("Button");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_hover_bt), hv));
  bx->packEnd (*bt);
  bt->show ();
  hv->setParent (*win);
  hv->setTarget (*bt);

  bt = Button::factory (*win);
  bt->setTextPart ("Popup");
  hv->setContentPart ("middle", *bt);
  bt->show ();

  bx = Box::factory (*win);

  Icon *ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  bx->packEnd (*ic);
  ic->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 1");
  bx->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 2");
  bx->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 3");
  bx->packEnd (*bt);
  bt->show ();

  bx->show ();

  hv->setContentPart ("top", *bx);

  bt = Button::factory (*win);
  bt->setTextPart ("Bottom");
  hv->setContentPart ("bottom", *bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Left");
  hv->setContentPart ("left", *bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Right");
  hv->setContentPart ("right", *bt);
  bt->show ();

  bg->setMinHintSize (size160x160);
  bg->setMaxHintSize (size640x640);
  win->resize (size320x320);
  win->show ();
}

void test_hover2 (void *data, Evas_Object *obj, void *event_info)
{
  Box *bx = NULL;
  Button *bt = NULL;

  Window *win = Window::factory ("hover2", ELM_WIN_BASIC);
  win->setTitle ("Hover 2");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  Hover *hv = Hover::factory (*win);
  hv->setStyle ("popout");

  bt = Button::factory (*win);
  bt->setTextPart ("Button");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_hover_bt), hv));
  bx->packEnd (*bt);
  bt->show ();
  hv->setParent (*win);
  hv->setTarget (*bt);

  bt = Button::factory (*win);
  bt->setTextPart ("Popup");
  hv->setContentPart ("middle", *bt);
  bt->show ();

  bx = Box::factory (*win);

  Icon *ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setNoScale (true);
  bx->packEnd (*ic);
  ic->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 1");
  bx->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 2");
  bx->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Top 3");
  bx->packEnd (*bt);
  bt->show ();

  bx->show ();

  hv->setContentPart ("top", *bx);

  bt = Button::factory (*win);
  bt->setTextPart ("Bot");
  hv->setContentPart ("bottom", *bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Left");
  hv->setContentPart ("left", *bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setTextPart ("Right");
  hv->setContentPart ("right", *bt);
  bt->show ();

  bg->setMinHintSize (size160x160);
  bg->setMaxHintSize (size640x640);
  win->resize (size320x320);
  win->show ();
}
