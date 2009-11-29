#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/ecorexx/XWindow.h"

//===============================================================================================
// XWindow
//===============================================================================================

namespace Ecorexx {

XWindow::XWindow( Ecore_X_Window exwin )
{
  _exwin = exwin;
}

XWindow::~XWindow()
{

}

void XWindow::setNetWMWindowType( XWindowType type )
{
  ecore_x_netwm_window_type_set( _exwin, static_cast <Ecore_X_Window_Type> (type) );
}

//void XWindow::getNetWMWindowTypePrefetch()
//void XWindow::getNetWMWindowTypeFetch ()
//EAPI void                ecore_x_netwm_window_type_get_prefetch(Ecore_X_Window window);
//EAPI void                ecore_x_netwm_window_type_get_fetch(void);

bool XWindow::getNetWMWindowType( XWindowType &outType )
{
  Ecore_X_Window_Type eWinType;
  bool ret = ecore_x_netwm_window_type_get( _exwin, &eWinType );
  outType = static_cast <XWindowType> (eWinType);

  return ret;
}

bool XWindow::getDPMSEnabled ()
{
  return ecore_x_dpms_enabled_get ();
}

void XWindow::setDPMSEnabled (bool enabled)
{
  ecore_x_dpms_enabled_set (enabled);
}

} // end namespace Ecorexx
