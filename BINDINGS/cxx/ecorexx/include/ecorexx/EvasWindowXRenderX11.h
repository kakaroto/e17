#ifndef ECOREXX_EVAS_WINDOW_XRENDER_X11_H
#define ECOREXX_EVAS_WINDOW_XRENDER_X11_H

/* EFLxx */
#include <evasxx/Evasxx.h>
#include "EvasWindow.h"

namespace Ecorexx {

/* forward declarations */
class XWindow;
  
class EvasWindowXRenderX11 : public EvasWindow
{
public:
  EvasWindowXRenderX11( const Eflxx::Size &size, const char* display = 0 );
  virtual ~EvasWindowXRenderX11();

  /*!
   * \brief Gets a pointer to the X window. The returned pointer needs to be
   *        deleted if you don't need it any longer.
   *
   * \return The XWindow.
   */
  XWindow *getXWindow();

private:
  EvasWindowXRenderX11();
  bool operator=( const EvasWindowXRenderX11& );
  bool operator==( const EvasWindowXRenderX11& );

  XWindow *mExwin;
};

} // end namespace Ecorexx

#endif // ECOREXX_EVAS_WINDOW_XRENDER_X11_H
