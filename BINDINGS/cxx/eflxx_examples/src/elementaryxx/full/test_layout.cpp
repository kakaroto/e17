#include "test.h"

void test_layout (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("layout", ELM_WIN_BASIC);
  win->setTitle ("Layout");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  ElmLayout *ly = ElmLayout::factory (*win);
  ly->setFile (searchEdjeFile ("elementaryxx-test.edj"), "layout");
  ly->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*ly);
  ly->show ();

  ElmButton *bt = ElmButton::factory (*win);
  bt->setLabel ("Button 1");
  ly->setContent ("element1", *bt);
  bt->show ();

  ElmButton *bt2 = ElmButton::factory (*win);
  bt2->setLabel ("Button 2");
  ly->setContent ("element2", *bt2);
  bt2->show ();

  ElmButton *bt3 = ElmButton::factory (*win);
  bt3->setLabel ("Button 3");
  ly->setContent ("element3", *bt3);
  bt3->show ();

  win->show ();
}