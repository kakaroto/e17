#include "test.h"



static void icon_clicked (Evas_Object *obj, void *event_info)
{
  cout << "clicked!" << endl;
}

void test_icon (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("icon-transparent", ELM_WIN_BASIC);
  win->setTitle ("Icon Transparent");
  win->setAutoDel (true);
  win->setAlpha (true);

  Icon *ic = Icon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo.png"));
  ic->setScale (false, false);
  win->addObjectResize (*ic);
  ic->show ();

  ic->getEventSignal ("clicked")->connect (sigc::ptr_fun (&icon_clicked));

  win->show ();
}
