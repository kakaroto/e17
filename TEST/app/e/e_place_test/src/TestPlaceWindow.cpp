#include "TestPlaceWindow.h"

#include <iostream>

using namespace std;

TestPlaceWindow::TestPlaceWindow (Border border, const std::string &helpImageName) :
  TestWindow ("TestPlaceWindow", helpImageName)
{
  Glib::RefPtr <Gdk::Screen> screen = Gdk::Screen::get_default ();
  int xres = screen->get_width ();
  int yres = screen->get_height ();
  int xpos = 0;
  int ypos = 0;
  int winWidth = getImageWidth ();
  int winHeight = getImageHeight ();
  
  switch (border)
  {
    case Left:
      xpos = 0;
      ypos = yres / 2 - winHeight / 2;
      break;

    case Right:
      xpos = xres - winWidth;
      ypos = yres / 2 - winHeight / 2;
      break;

    case Top:
      xpos = xres / 2 - winWidth / 2;
      ypos = 0;
      break;

    case Bottom:
      xpos = xres / 2 - winWidth / 2;
      ypos = yres - winHeight;
      break;

    case LeftOff:
      xpos = 0 - winWidth / 2;
      ypos = yres / 2 - winHeight / 2;
      break;

    case RightOff:
      xpos = xres + winWidth / 2;
      ypos = yres / 2 - winHeight / 2;
      break;

    case TopOff:
      xpos = xres / 2 - winWidth / 2;
      ypos = 0 - winHeight / 2;
      break;

    case BottomOff:
      xpos = xres / 2 - winWidth / 2;
      ypos = yres + winHeight / 2;
      break;

    case Oversize:
      xpos = -100;
      ypos = -100;
      winWidth = xres;
      winHeight = yres;
      break;

    case Center:
      xpos = xres / 2 - winWidth / 2;
      ypos = yres / 2 - winHeight / 2;
      break;
  };

  set_default_size (winWidth, winHeight);
  move (xpos, ypos);
  
  show ();
}

TestPlaceWindow::~TestPlaceWindow ()
{
  cout << "~TestPlaceWindow" << endl;
}
