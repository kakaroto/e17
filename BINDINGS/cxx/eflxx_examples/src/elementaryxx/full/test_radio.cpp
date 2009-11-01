#include "test.h"

void test_radio (void *data, Evas_Object *obj, void *event_info)
{
  ElmRadio *rd = NULL;
  ElmRadio *rdg = NULL;
  ElmIcon *ic = NULL;
  
  ElmWindow *win = ElmWindow::factory ("radio", ELM_WIN_BASIC);
  win->setTitle ("Radios");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();
  
  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  rd = ElmRadio::factory (*win);
  rd->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  rd->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  rd->setLabel ("Icon sized to radio");
  rd->setIcon (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();
  rdg = rd;

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  rd = ElmRadio::factory (*win);
  rd->setStateValue (1);
  rd->addGroup (*rdg);
  rd->setLabel ("Icon no scale");
  rd->setIcon (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();

  rd = ElmRadio::factory (*win);
  rd->setStateValue (2);
  rd->addGroup (*rdg);
  rd->setLabel ("Label Only");
  bx->packEnd (*rd);
  rd->show ();

  rd = ElmRadio::factory (*win);
  rd->setStateValue (3);
  rd->addGroup (*rdg);
  rd->setLabel ("Disabled");
  rd->setDisabled (true);
  bx->packEnd (*rd);
  rd->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  rd = ElmRadio::factory (*win);
  rd->setStateValue (4);
  rd->addGroup (*rdg);
  rd->setIcon (*ic);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();

  ic = ElmIcon::factory (*win);
  ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic->setScale (false, false);
  rd = ElmRadio::factory (*win);
  rd->setStateValue (5);
  rd->addGroup (*rdg);
  rd->setIcon (*ic);
  rd->setDisabled (true);
  bx->packEnd (*rd);
  rd->show ();
  ic->show ();
  
  rdg->setValue (2);

  win->show ();
}
