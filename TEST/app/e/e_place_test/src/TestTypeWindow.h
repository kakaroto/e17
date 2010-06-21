#ifndef TEST_TYPE_WINDOW_H
#define TEST_TYPE_WINDOW_H

/* Gtk */
#include <gtkmm.h>

/* Project */
#include "TestWindow.h"

class TestTypeWindow : public TestWindow
{
public:
  TestTypeWindow (Gdk::WindowTypeHint typeHint, const std::string &helpImageName);
  virtual ~TestTypeWindow ();
  
protected:
  //void TestTypeWindow::onButtonUpSize ()
  
  Gtk::HButtonBox mButtonBox;
  Gtk::Button mButtonUpSize;
  Gtk::Button mButtonDownSize;
};

#endif // TEST_TYPE_WINDOW_H
