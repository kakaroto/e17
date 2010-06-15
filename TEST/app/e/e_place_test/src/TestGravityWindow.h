#ifndef TEST_GRAVITY_OFFSCREEN_H
#define TEST_GRAVITY_OFFSCREEN_H

/* Gtk */
#include <gtkmm.h>

/* Project */
#include "TestWindow.h"

class TestGravityWindow : public TestWindow
{
public:
  TestGravityWindow (Gdk::Gravity gravity, const std::string &helpImageName);
  virtual ~TestGravityWindow ();
  
};

#endif // TEST_GRAVITY_OFFSCREEN_H
