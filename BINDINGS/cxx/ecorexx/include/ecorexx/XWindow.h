#ifndef ECORE_X_WINDOW_H
#define ECORE_X_WINDOW_H

/* EFLxx */
#include "Application.h"

/* EFL */
#include <Ecore_X.h>

//===============================================================================================
// XWindow
//===============================================================================================

/*!
 * This is an initial wrapper for Ecore_X.
 */

namespace Ecorexx {

class XWindow //: public Trackable
{
public:
  enum XWindowType
  {
    Desktop = ECORE_X_WINDOW_TYPE_DESKTOP,
    Dock = ECORE_X_WINDOW_TYPE_DOCK,
    Toolbar = ECORE_X_WINDOW_TYPE_TOOLBAR,
    Menu = ECORE_X_WINDOW_TYPE_MENU,
    Utility = ECORE_X_WINDOW_TYPE_UTILITY,
    Spash = ECORE_X_WINDOW_TYPE_SPLASH,
    Dialog = ECORE_X_WINDOW_TYPE_DIALOG,
    Normal = ECORE_X_WINDOW_TYPE_NORMAL,
    Unknown = ECORE_X_WINDOW_TYPE_UNKNOWN
  };

public:
  //XWindow () {};
  XWindow( Ecore_X_Window exw ); // wrap?
  virtual ~XWindow();

  void setNetWMWindowType( XWindowType type );
  //void getNetWMWindowTypePrefetch();
  //void getNetWMWindowTypeFetc ();
  bool getNetWMWindowType( XWindowType &outType );

  // TODO: not sure where to put DPMS and all other functions
  static bool getDPMSEnabled ();
  static void setDPMSEnabled (bool enabled);

protected:

private:
  //bool operator=( const XWindow& );
  bool operator==( const XWindow& );

  Ecore_X_Window _exwin;
};


} // end namespace Ecorexx

#endif // ECORE_X_WINDOW_H
