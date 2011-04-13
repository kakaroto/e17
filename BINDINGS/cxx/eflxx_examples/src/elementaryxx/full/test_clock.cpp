#include "test.h"

void test_clock (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("clock", ELM_WIN_BASIC);
  win->setTitle ("Clock");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  Clock *ck = Clock::factory (*win);
  bx->packEnd (*ck);
  ck->show ();

  Clock *ck2 = Clock::factory (*win);
  ck2->setShowAmPm (true);
  bx->packEnd (*ck2);
  ck2->show ();

  Clock *ck3 = Clock::factory (*win);
  ck3->setShowSeconds (true);
  bx->packEnd (*ck3);
  ck3->show ();

  Clock *ck4 = Clock::factory (*win);
  ck4->setShowSeconds (true);
  ck4->setShowAmPm (true);
  bx->packEnd (*ck4);
  ck4->show ();

  Clock *ck5 = Clock::factory (*win);
  ck5->setEdit (true);
  ck5->setShowSeconds (true);
  ck5->setShowAmPm (true);
  ck5->setTime (Clock::Time (10, 11, 12));
  bx->packEnd (*ck5);
  ck5->show ();

  win->show ();
}