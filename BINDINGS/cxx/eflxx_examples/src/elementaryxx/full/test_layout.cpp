#include "test.h"



void test_layout (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("layout", ELM_WIN_BASIC);
  win->setTitle ("Layout");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Layout *ly = Layout::factory (*win);
  ly->setFile (searchEdjeFile ("elementaryxx-test.edj"), "layout");
  ly->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*ly);
  ly->show ();

  Button *bt = Button::factory (*win);
  bt->setLabel ("Button 1");
  ly->setContent ("element1", *bt);
  bt->show ();

  Button *bt2 = Button::factory (*win);
  bt2->setLabel ("Button 2");
  ly->setContent ("element2", *bt2);
  bt2->show ();

  Button *bt3 = Button::factory (*win);
  bt3->setLabel ("Button 3");
  ly->setContent ("element3", *bt3);
  bt3->show ();

  win->show ();
}