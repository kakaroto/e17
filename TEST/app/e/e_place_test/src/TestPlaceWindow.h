#ifndef TEST_PLACE_WINDOW_H
#define TEST_PLACE_WINDOW_H

/* Gtk */
#include <gtkmm.h>

/* Project */
#include "TestWindow.h"

class TestPlaceWindow : public TestWindow
{
public:
  enum _Border
  {
    Left,
    Right,
    Top,
    Bottom,
    LeftOff,
    RightOff,
    TopOff,
    BottomOff,
    Oversize,
    Center
  };
  typedef _Border Border;
  
  TestPlaceWindow (Border border, const std::string &helpImageName);
  virtual ~TestPlaceWindow ();

protected:
  
};

#endif // TEST_PLACE_WINDOW_H
