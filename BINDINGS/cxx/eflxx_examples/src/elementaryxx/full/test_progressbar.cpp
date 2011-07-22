#include "test.h"

#include <ecorexx/Ecorexx.h>

typedef struct _MyProgressbar
{
  Progressbar *pb1;
  Progressbar *pb2;
  Progressbar *pb3;
  Progressbar *pb4;
  Progressbar *pb5;
  Progressbar *pb6;
  Progressbar *pb7;
  bool run;
  Ecorexx::Timer *timer;
} MyProgressbar;

static MyProgressbar _test_progressbar;

bool _my_progressbar_value_set (Ecorexx::Timer &timer)
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

  if (progress > 1.0)
  {
    _test_progressbar.run = false;
    return false; // don't run again and delete timer
  }

  return true;
}

static void my_progressbar_test_start (Evasxx::Object &obj, void *event_info)
{
  _test_progressbar.pb2->pulse (true);
  _test_progressbar.pb5->pulse (true);
  _test_progressbar.pb7->pulse (true);

  if (!_test_progressbar.run)
  {
    sigc::slot <bool, Ecorexx::Timer&> timerSlot = sigc::ptr_fun (&_my_progressbar_value_set);

    _test_progressbar.timer = Ecorexx::Timer::factory (0.1, timerSlot);

    _test_progressbar.run = true;
  }
}

static void _test_stop ()
{
  _test_progressbar.pb2->pulse (false);
  _test_progressbar.pb5->pulse (false);
  _test_progressbar.pb7->pulse (false);

  if (_test_progressbar.run)
  {
     _test_progressbar.timer->destroy ();
     _test_progressbar.run = false;
  }
}

static void my_progressbar_test_stop (Evasxx::Object &obj, void *event_info)
{
  _test_stop ();
}

static void my_progressbar_destroy (Evasxx::Object &obj, void *event_info)
{
  _test_stop ();
  delete &obj;
}

void test_progressbar(void *data, Evas_Object *obj, void *event_info)
{
  Progressbar *pb = NULL;
  Icon *ic1 = NULL;
  Icon *ic2 = NULL;
  Button *bt = NULL;

  // start in stop mode
  _test_progressbar.run = false;

  Window *win = Window::factory ("progressbar", ELM_WIN_BASIC);
  win->setTitle ("Progressbar");
  win->getEventSignal ("delete,request")->connect (sigc::ptr_fun (&my_progressbar_destroy));

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  win->addObjectResize (*bx);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->show ();

  pb = Progressbar::factory (*win);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb1 = pb;

  pb = Progressbar::factory (*win);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setLabel ("Infinite bounce");
  pb->setPulse (true);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb2 = pb;

  ic1 = Icon::factory (*win);
  ic1->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic1->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Size (1, 1));

  pb = Progressbar::factory (*win);
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

  Box *hbx = Box::factory (*win);
  hbx->setOrientation (Box::Horizontal);
  hbx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  hbx->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*hbx);
  hbx->show ();

  pb = Progressbar::factory (*win);
  pb->setOrientation (Progressbar::Vertical);
  pb->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  hbx->packEnd (*pb);
  pb->setSpanSize (60);
  pb->setLabel ("percent");
  pb->show ();
  _test_progressbar.pb4 = pb;

  pb = Progressbar::factory (*win);
  pb->setOrientation (Progressbar::Vertical);
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  pb->setSpanSize (80);
  pb->hideUnitFormat ();
  pb->setLabel ("Infinite bounce");
  hbx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb5 = pb;

  ic2 = Icon::factory (*win);
  ic2->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
  ic2->setAspectHintSize (EVAS_ASPECT_CONTROL_HORIZONTAL, Size (1, 1));

  pb = Progressbar::factory (*win);
  pb->setOrientation (Progressbar::Vertical);
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

  pb = Progressbar::factory (*win);
  pb->setStyle ("wheel");
  pb->setLabel ("Style: wheel");
  pb->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  pb->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*pb);
  pb->show ();
  _test_progressbar.pb7 = pb;

  Box *bt_bx = Box::factory (*win);
  bt_bx->setOrientation (Box::Horizontal);
  bt_bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*bt_bx);
  bt_bx->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Start");
  bt->getEventSignal ("clicked")->connect (sigc::ptr_fun (&my_progressbar_test_start));
  bt_bx->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Stop");
  bt->getEventSignal ("clicked")->connect (sigc::ptr_fun (&my_progressbar_test_stop));
  bt_bx->packEnd (*bt);
  bt->show ();

  win->show ();
}
