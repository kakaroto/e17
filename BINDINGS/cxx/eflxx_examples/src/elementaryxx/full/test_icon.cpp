#include "test.h"

// TODO: update from C example

void test_icon (void *data, Evas_Object *obj, void *event_info)
{
  ElmWindow *win = ElmWindow::factory ("icon-transparent", ELM_WIN_BASIC);
  win->setTitle ("Icon Transparent");
  win->setAutoDel (true);
  win->setAlpha (true);

  ElmIcon *ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo.png"));
  ic->setScale (false, false);
  win->addObjectResize (*ic);
  ic->show ();

  win->show ();
}
