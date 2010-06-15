/* Project */
#include "TestGravityWindow.h"

TestGravityWindow::TestGravityWindow (Gdk::Gravity gravity, const std::string &helpImageName) :
  TestWindow ("TestGravityWindow", helpImageName)
{
  set_gravity (gravity);
  //move (100, 100);
/*
GRAVITY_NORTH_WEST 	
GRAVITY_NORTH 	
GRAVITY_NORTH_EAST 	
GRAVITY_WEST 	
GRAVITY_CENTER 	
GRAVITY_EAST 	
GRAVITY_SOUTH_WEST 	
GRAVITY_SOUTH 	
GRAVITY_SOUTH_EAST 	
GRAVITY_STATIC 
*/
  
  show ();
}

TestGravityWindow::~TestGravityWindow ()
{

}
