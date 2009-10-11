#include "test.h"

void test_table (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("table", ELM_WIN_BASIC);
  win->setTitle ("Table");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  ElmTable *tb = ElmTable::factory (*win);
  win->addObjectResize (*tb);
  tb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  tb->show ();
  
  ElmButton *bt = ElmButton::factory (*win);
  bt->setLabel ("Button 1");
  bt->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt, Rect (0, 0, 1, 1));
  bt->show ();
  
  ElmButton *bt2 = ElmButton::factory (*win);
  bt2->setLabel ("Button 2");
  bt2->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt2, Rect (1, 0, 1, 1));
  bt2->show ();

  ElmButton *bt3 = ElmButton::factory (*win);
  bt3->setLabel ("Button 3");
  bt3->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt3->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt3, Rect (2, 0, 1, 1));
  bt3->show ();

  ElmButton *bt4 = ElmButton::factory (*win);
  bt4->setLabel ("Button 4");
  bt4->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt4->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt4, Rect (0, 1, 2, 1));
  bt4->show ();

  ElmButton *bt5 = ElmButton::factory (*win);
  bt5->setLabel ("Button 5");
  bt5->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt5->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt5, Rect (2, 1, 1, 3));
  bt5->show ();

  ElmButton *bt6 = ElmButton::factory (*win);
  bt6->setLabel ("Button 6");
  bt6->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bt6->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  tb->pack (*bt6, Rect (0, 2, 2, 2));
  bt6->show ();

  win->show ();
}