#include "test.h"

void test_spinner(void *data, Evas_Object *obj, void *event_info)
{
  ElmSpinner *sp = NULL;
  
  ElmWindow *win = ElmWindow::factory ("spinner", ELM_WIN_BASIC);
  win->setTitle ("Spinner");
  win->setAutoDel (true);

  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  ElmBox *bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  sp = ElmSpinner::factory (*win);
  sp->setLabelFormat ("%1.1f units");
  sp->setStep (1.3);
  sp->setWrap (true);
  sp->setMinMax (-50.0, 250.0);
  sp->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  sp->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*sp);
  sp->show ();
  
  sp = ElmSpinner::factory (*win);
  sp->setLabelFormat ("%1.1f units");
  sp->setStep (1.3);
  sp->setWrap (true);
  sp->setStyle ("vertical");
  sp->setMinMax (-50.0, 250.0);
  sp->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  sp->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*sp);
  sp->show ();
  
  sp = ElmSpinner::factory (*win);
  sp->setLabelFormat ("Disabled %.0f");
  sp->setDisabled (true);
  sp->setMinMax (-50.0, 250.0);
  sp->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  sp->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*sp);
  sp->show ();
  
  win->show ();
}
