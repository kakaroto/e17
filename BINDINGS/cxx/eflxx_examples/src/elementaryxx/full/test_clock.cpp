#include "test.h"

void test_clock (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("clock", ELM_WIN_BASIC);
  win->setTitle ("Clock");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  ElmClock *ck = ElmClock::factory (*win);
  bx->packEnd (*ck);
  ck->show ();
  
  ElmClock *ck2 = ElmClock::factory (*win);
  ck2->setShowAmPm (true);
  bx->packEnd (*ck2);
  ck2->show ();

  ElmClock *ck3 = ElmClock::factory (*win);
  ck3->setShowSeconds (true);
  bx->packEnd (*ck3);
  ck3->show ();

  ElmClock *ck4 = ElmClock::factory (*win);
  ck4->setShowSeconds (true);
  ck4->setShowAmPm (true);
  bx->packEnd (*ck4);
  ck4->show ();

  ElmClock *ck5 = ElmClock::factory (*win);
  ck5->setEdit (true);
  ck5->setShowSeconds (true);
  ck5->setShowAmPm (true);
  ck5->setTime (ElmClock::Time (10, 11, 12));
  bx->packEnd (*ck5);
  ck5->show ();

  win->show ();
}