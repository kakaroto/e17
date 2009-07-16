#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <elementaryxx/Elementaryxx.h>

using namespace std;
using namespace efl;

static void testFunc (void *data, Evas_Object *obj, void *event_info)
{
  cout << "Button pressed" << endl;
}

int main (int argc, char **argv)
{
  ElmApplication elmApp (argc, argv);

  ElmWindow *elmWin = ElmWindow::factory ("window1", ELM_WIN_BASIC);
  ElmBackground *bg = ElmBackground::factory (*elmWin);

  ElmButton *elmButton = ElmButton::factory (*elmWin);
  ElmClock *elmClock = ElmClock::factory (*elmWin);
  ElmToggle *elmToggle = ElmToggle::factory (*elmWin);
  ElmScroller *elmScroller = ElmScroller::factory (*elmWin);
  ElmEntry *elmEntry = ElmEntry::factory (*elmWin);
  ElmSlider *elmSlider = ElmSlider::factory (*elmWin);


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

  elmToggle->getEventSignal ("sub-object-del")->connect (sigc::ptr_fun (&testFunc));
  
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
