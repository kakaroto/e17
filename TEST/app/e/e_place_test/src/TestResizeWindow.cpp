#include "TestResizeWindow.h"

#include <iostream>

using namespace std;

TestResizeWindow::TestResizeWindow (const std::string &helpImageName) :
  TestWindow ("TestResizeWindow", helpImageName),
  mButtonUpSize ("Size Up"),
  mButtonDownSize ("Size Down")
{
  add (mButtonBox);

  mButtonBox.pack_start (mButtonUpSize, Gtk::PACK_SHRINK);
  mButtonBox.set_border_width (5);
  mButtonBox.set_layout (Gtk::BUTTONBOX_END);
  mButtonUpSize.signal_clicked ().connect( sigc::mem_fun (*this, &TestResizeWindow::onButtonUpSize) );
  
  mButtonBox.pack_start (mButtonDownSize, Gtk::PACK_SHRINK);
  mButtonBox.set_border_width (5);
  mButtonBox.set_layout (Gtk::BUTTONBOX_END);
  mButtonDownSize.signal_clicked ().connect( sigc::mem_fun (*this, &TestResizeWindow::onButtonDownSize) );

  show_all ();
}

TestResizeWindow::~TestResizeWindow ()
{

}

void TestResizeWindow::onButtonUpSize ()
{
  resize (get_width () * 1.5, get_height () * 1.5);
}

void TestResizeWindow::onButtonDownSize ()
{
  resize (get_width () * 0.5, get_height () * 0.5);
}
