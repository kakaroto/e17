#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <evasxx/Evasxx.h>
#include <elementaryxx/Elementaryxx.h>
#include "../../common/searchFile.h"

using namespace std;
using namespace Eflxx;
using namespace Elmxx;

static void
my_win_del(Evasxx::Object &obj, void *event_info)
{
  // TODO
  /* called when my_win_main is requested to be deleted */
  Application::exit(); /* exit the program's main loop that runs in elm_run() */
}

static void testFunc (Evasxx::Object &obj, void *event_info)
{
  cout << "sub-object-del" << endl;
}

int main (int argc, const char **argv)
{
  Application elmApp (argc, argv);

  Window *elmWin = Window::factory ("window1", ELM_WIN_BASIC);
  elmWin->getEventSignal ("delete-request")->connect (sigc::ptr_fun (&my_win_del));

  Background *bg = Background::factory (*elmWin);

  //Evas *e = evas_object_evas_get(bg->obj ());

/*  Evasxx::Canvas ec (e);

  Evasxx::Image ei (ec);
  ei.setFile (searchPixmapFile ("elementaryxx/plant_01.jpg"));
  ei.setGeometry (Rect (100,100, 100, 100));
  ei.show ();*/

  Button *elmButton = Button::factory (*elmWin);
  Clock *elmClock = Clock::factory (*elmWin);
  //elmWin->addMember (ei);
  Toggle *elmToggle = Toggle::factory (*elmWin);
  Scroller *elmScroller = Scroller::factory (*elmWin);
  Entry *elmEntry = Entry::factory (*elmWin);
  Slider *elmSlider = Slider::factory (*elmWin);

  bg->setWeightHintSize (1.0, 1.0);
  elmWin->addObjectResize (*bg);

  /* set size hints. a minimum size for the bg. this should propagate back
   * to the window thus limiting its size based off the bg as the bg is one
   * of the window's resize objects. */
  bg->setMinHintSize (Size (160, 160));
  /* and set a maximum size. not needed very often. normally used together
   * with evas_object_size_hint_min_set() at the same size to make a
   * window not resizable */
  bg->setMaxHintSize (Size (640, 640));

  elmWin->resize (Size (500, 500));
  elmButton->resize (Size (100, 50));
  elmClock->setGeometry (Rect (100, 50, 200, 50));
  elmToggle->setGeometry (Rect (150, 80, 100, 50));
  elmScroller->setGeometry (Rect (250, 120, 200, 200));
  elmEntry->setGeometry (Rect (150, 200, 100, 50));
  elmSlider->setGeometry (Rect (200, 200, 100, 50));

  elmWin->setTitle ("Elementaryxx Simple Example");

  elmButton->setLabel ("Text");

  elmClock->setEdit (true);

  elmWin->setAutoDel (true);

  //elmToggle->getEventSignal ("sub-object-del")->connect (sigc::ptr_fun (&testFunc));

  elmWin->show ();
  bg->show ();
  elmButton->show ();
  elmClock->show ();
  elmToggle->show ();
  elmScroller->show ();
  elmEntry->show ();
  elmSlider->show ();

  elmApp.run ();
}

