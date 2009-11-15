#include "test.h"

#include <ecorexx/Ecorexx.h>

typedef struct Progressbar
{
  ElmProgressbar *pb1;
  ElmProgressbar *pb2;
  ElmProgressbar *pb3;
  ElmProgressbar *pb4;
  ElmProgressbar *pb5;
  ElmProgressbar *pb6;
  ElmProgressbar *pb7;
  bool run;
  Ecore_Timer *timer; // TODO: port to EcoreTimer
} Progressbar;

static Progressbar _test_progressbar;

static int _my_progressbar_value_set (void *data)
{
  double progress;

  progress = _test_progressbar.pb1->getValue ();
  
  if (progress < 1.0)
  {
    progress += 0.0123;
  }
  else 
  {
    progress = 0.0;
  }
  
  
  _test_progressbar.pb1->setValue (progress);
  _test_progressbar.pb4->setValue (progress);
  _test_progressbar.pb3->setValue (progress);
  _test_progressbar.pb6->setValue (progress);
  
  if (progress < 1.0)
  {
    return ECORE_CALLBACK_RENEW;
  }
  
  _test_progressbar.run = false;

  return ECORE_CALLBACK_CANCEL;
}

static void my_progressbar_test_start (Evas_Object *obj, void *event_info)
{
  _test_progressbar.pb2->pulse (true);
  _test_progressbar.pb5->pulse (true);
  _test_progressbar.pb7->pulse (true);
  
  if (!_test_progressbar.run)
  {
    _test_progressbar.timer = ecore_timer_add (0.1, _my_progressbar_value_set, NULL);
    _test_progressbar.run = true;
  }
}

static void my_progressbar_test_stop (Evas_Object *obj, void *event_info)
{
  _test_progressbar.pb2->pulse (false);
  _test_progressbar.pb5->pulse (false);
  _test_progressbar.pb7->pulse (false);
   
  if (_test_progressbar.run)
  {
     ecore_timer_del(_test_progressbar.timer);
     _test_progressbar.run = false;
  }
}

static void my_progressbar_destroy (Evas_Object *obj, void *event_info)
{
  my_progressbar_test_stop (NULL, NULL);
  evas_object_del (obj);
}

void test_progressbar(void *data, Evas_Object *obj, void *event_info)
{
  ElmProgressbar *pb = NULL;
  ElmIcon *ic1 = NULL;
  ElmIcon *ic2 = NULL;
  ElmButton *bt = NULL;
  
  // start in stop mode
  _test_progressbar.run = false;
  
  ElmWindow *win = ElmWindow::factory ("progressbar", ELM_WIN_BASIC);
  win->setTitle ("Progressbar");
  win->getEventSignal ("delete,request")->connect (sigc::ptr_fun (&my_progressbar_destroy));
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();
  
  pb = ElmProgressbar::factory (*win);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb1 = pb;
  
  pb = ElmProgressbar::factory (*win);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setLabel ("Infinite bounce");
  pb->setPulse (true);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb2 = pb;
  
  ic1 = ElmIcon::factory (*win);
  ic1->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic1->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));
  
  pb = ElmProgressbar::factory (*win);
  pb->setLabel ("Label");
  pb->setIcon (*ic1);
  pb->setInverted (true);
  pb->setUnitFormat ("%1.1f units");
  pb->setSpanSize (200);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*pb);
  ic1->show ();
  pb->show ();
  _test_progressbar.pb3 = pb;

  ElmBox *hbx = ElmBox::factory (*win);
  hbx->setHorizontal (true);
  hbx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  hbx->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*hbx);
  hbx->show ();
  
  pb = ElmProgressbar::factory (*win);
  pb->setHorizontal (false);
  pb->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  hbx->packEnd (*pb);
  pb->setSpanSize (60);
  pb->setLabel ("percent");
  pb->show ();
  _test_progressbar.pb4 = pb;
  
  pb = ElmProgressbar::factory (*win);
  pb->setHorizontal (false);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setSpanSize (80);
  pb->hideUnitFormat ();
  pb->setLabel ("Infinite bounce");
  hbx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb5 = pb;

  ic2 = ElmIcon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setAspectHintSize (EVAS_ASPECT_CONTROL_HORIZONTAL, Size (1, 1));
  
  pb = ElmProgressbar::factory (*win);
  pb->setHorizontal (false);
  pb->setLabel ("Label");
  pb->setIcon (*ic2);
  pb->setInverted (true);
  pb->setUnitFormat ("%1.2f%%");
  pb->setSpanSize (200);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  hbx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb6 = pb;
  
  pb = ElmProgressbar::factory (*win);
  pb->setStyle ("wheel");
  pb->setLabel ("Style: wheel");
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb7 = pb;
  
  ElmBox *bt_bx = ElmBox::factory (*win);
  bt_bx->setHorizontal (true);
  bt_bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*bt_bx);
  bt_bx->show ();
  
  bt = ElmButton::factory (*win);
  bt->setLabel ("Start");
  bt->getEventSignal ("clicked")->connect (sigc::ptr_fun (&my_progressbar_test_start));
  bt_bx->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Stop");
  bt->getEventSignal ("clicked")->connect (sigc::ptr_fun (&my_progressbar_test_stop));
  bt_bx->packEnd (*bt);
  bt->show ();

  win->show ();
}
