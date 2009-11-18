#include "test.h"

static void my_notepad_bt_1 (Evas_Object *obj, void *event_info, ElmNotepad *np)
{
  // TODO
}

static void my_notepad_bt_2 (Evas_Object *obj, void *event_info, ElmNotepad *np)
{
  // TODO
}

static void my_notepad_bt_3 (Evas_Object *obj, void *event_info, ElmNotepad *np)
{
  // TODO
}

void test_notepad (void *data, Evas_Object *obj, void *event_info)
{
  ElmButton *bt = NULL;
  ElmIcon *ic = NULL;
  
  ElmWindow *win = ElmWindow::factory ("notepad", ELM_WIN_BASIC);
  win->setTitle ("Notepad");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  ElmNotepad *np = ElmNotepad::factory (*win);
  np->setFile ("note.txt", ELM_TEXT_FORMAT_PLAIN_UTF8);
  np->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  np->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*np);
  np->show ();

  ElmBox *bx2 = ElmBox::factory (*win);
  bx2->setOrientation (ElmBox::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = ElmButton::factory (*win);
  ic = ElmIcon::factory (*win);
  ic->setStandard ("arrow_left");
  ic->setScale (true, false);
  bt->setIcon (*ic);
  ic->show ();
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_notepad_bt_1), np));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  ic = ElmIcon::factory (*win);
  ic->setStandard ("close");
  ic->setScale (true, false);
  bt->setIcon (*ic);
  ic->show ();
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_notepad_bt_2), np));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  ic = ElmIcon::factory (*win);
  ic->setStandard ("arrow_right");
  ic->setScale (true, false);
  bt->setIcon (*ic);
  ic->show ();
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_notepad_bt_3), np));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);

  win->setFocus (true);
  win->show ();
}
