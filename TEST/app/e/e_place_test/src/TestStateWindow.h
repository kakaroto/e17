#ifndef TEST_STATE_WINDOW_H
#define TEST_STATE_WINDOW_H

/* Gtk */
#include <gtkmm.h>

/* Project */
#include "TestWindow.h"

class TestStateWindow : public TestWindow
{
public:
  enum _State
  {
    Iconify,
    Stick,
    Maximize,
    Fullscreen
  };
  typedef _State State;
  
  TestStateWindow (State state, const std::string &helpImageName);
  virtual ~TestStateWindow ();
};

#endif // TEST_STATE_WINDOW_H