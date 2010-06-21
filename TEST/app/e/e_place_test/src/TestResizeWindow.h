#ifndef TEST_RESIZE_WINDOW_H
#define TEST_RESIZE_WINDOW_H

/* Gtk */
#include <gtkmm.h>

/* Project */
#include "TestWindow.h"

class TestResizeWindow : public TestWindow
{
public:
  TestResizeWindow (const std::string &helpImageName);
  virtual ~TestResizeWindow ();
  
protected:
  virtual void onButtonUpSize ();
  virtual void onButtonDownSize ();
  
  Gtk::HButtonBox mButtonBox;
  Gtk::Button mButtonUpSize;
  Gtk::Button mButtonDownSize;
};

#endif // TEST_RESIZE_WINDOW_H
