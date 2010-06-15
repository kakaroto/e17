#include "TestStateWindow.h"

TestStateWindow::TestStateWindow (State state, const std::string &helpImageName) :
  TestWindow ("TestStateWindow", helpImageName)
{
  switch (state)
  {
    case Iconify:
      iconify ();
      break;
    
    case Stick:
      stick ();
      break;
      
    case Maximize:
      maximize ();
      break;
      
    case Fullscreen:
      fullscreen ();
      break;
  };

  show ();
}

TestStateWindow::~TestStateWindow ()
{

}
