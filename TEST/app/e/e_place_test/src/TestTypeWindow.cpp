#include "TestTypeWindow.h"

#include <iostream>

using namespace std;

TestTypeWindow::TestTypeWindow (Gdk::WindowTypeHint typeHint, const std::string &helpImageName) :
  TestWindow ("TestResizeWindow", helpImageName),
  mButtonUpSize ("Size Up"),
  mButtonDownSize ("Size Down")
{
  add (mButtonBox);

  /*mButtonBox.pack_start (mButtonUpSize, Gtk::PACK_SHRINK);
  mButtonBox.set_border_width (5);
  mButtonBox.set_layout (Gtk::BUTTONBOX_END);
  mButtonUpSize.signal_clicked ().connect( sigc::mem_fun (*this, &TestTypeWindow::onButtonUpSize) );*/

/*
WINDOW_TYPE_HINT_NORMAL 	
WINDOW_TYPE_HINT_DIALOG 	
WINDOW_TYPE_HINT_MENU 	
WINDOW_TYPE_HINT_TOOLBAR 	
WINDOW_TYPE_HINT_SPLASHSCREEN 	
WINDOW_TYPE_HINT_UTILITY 	
WINDOW_TYPE_HINT_DOCK 	
WINDOW_TYPE_HINT_DESKTOP 	
WINDOW_TYPE_HINT_DROPDOWN_MENU 	
WINDOW_TYPE_HINT_POPUP_MENU 	
WINDOW_TYPE_HINT_TOOLTIP 	
WINDOW_TYPE_HINT_NOTIFICATION 	
WINDOW_TYPE_HINT_COMBO 	
WINDOW_TYPE_HINT_DND 
   */
  set_type_hint (typeHint);
  show_all ();
}

TestTypeWindow::~TestTypeWindow ()
{

}

/*void TestTypeWindow::onButtonUpSize ()
{
  resize (get_width () * 1.5, get_height () * 1.5);
}*/
