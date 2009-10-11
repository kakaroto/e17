#include "test.h"

void test_toggle (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("toggles", ELM_WIN_BASIC);
  win->setTitle ("Toggles");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (1.0, 1.0);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (1.0, 1.0);
  bx->show ();
  
  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  ElmToggle *tg = ElmToggle::factory (*win);
  tg->setWeightHintSize (1.0, 1.0);
  tg->setAlignHintSize (-1.0, 0.5);
  tg->setLabel ("Icon sized to toggle");
  tg->setIcon (*ic);
  tg->setState (true);
  tg->setLabelsStates ("Yes", "No");
  bx->packEnd (*tg);
  tg->show ();
  ic->show ();

  ElmIcon *ic2 = ElmIcon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setScale (false, false);
  ElmToggle *tg2 = ElmToggle::factory (*win);
  tg2->setLabel ("Icon no scale");
  tg2->setIcon (*ic2);
  bx->packEnd (*tg2);
  tg2->show ();
  ic2->show ();
  
  ElmToggle *tg3 = ElmToggle::factory (*win);
  tg3->setLabel ("Label Only");
  tg3->setLabelsStates ("Big long fun times label", "Small long happy fun label");
  bx->packEnd (*tg3);
  tg3->show ();

  ElmIcon *ic3 = ElmIcon::factory (*win);
  ic3->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic3->setScale (false, false);
  ElmToggle *tg4 = ElmToggle::factory (*win);
  tg4->setIcon (*ic3);
  bx->packEnd (*tg4);
  tg4->show ();
  ic3->show ();

  win->show ();
}
