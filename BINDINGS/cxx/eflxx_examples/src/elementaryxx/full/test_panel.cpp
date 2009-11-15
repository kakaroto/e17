#include "test.h"

void test_panel (void *data, Evas_Object *obj, void *event_info) 
{
  ElmWindow *win = ElmWindow::factory ("panel", ELM_WIN_BASIC);
  win->setTitle ("Panel");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmPanel *panel = ElmPanel::factory (*win);
  panel->setOrientation (ELM_PANEL_ORIENT_LEFT);
  panel->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  panel->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  panel->show ();

  cout << "TODO: wrap elm_genlist!" << endl;
#warning TODO: wrap elm_genlist!
  
  /*
  list = elm_genlist_add(win);
  evas_object_resize(list, 100, 100);
  evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(list);
  elm_panel_content_set(panel, list);*/

  win->resize (Size (300, 300));
  win->show ();
}

