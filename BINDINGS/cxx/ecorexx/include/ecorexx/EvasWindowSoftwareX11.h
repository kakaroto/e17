#ifndef ECOREXX_EVAS_WINDOW_SOFTWARE_X11_H
#define ECOREXX_EVAS_WINDOW_SOFTWARE_X11_H

/* EFLxx */
#include <evasxx/Evasxx.h>
#include "EvasWindow.h"

namespace Ecorexx {

/* forward declarations */
class XWindow;

class EvasWindowSoftwareX11 : public EvasWindow
{
public:
  EvasWindowSoftwareX11( const Eflxx::Size &size, const char* display = 0 );
  virtual ~EvasWindowSoftwareX11();

  /*!
   * \brief Gets a pointer to the X window. The returned pointer needs to be
   *        deleted if you don't need it any longer.
   *
   * \return The XWindow.
   */
  XWindow *getXWindow();

private:
  EvasWindowSoftwareX11();
  bool operator=( const EvasWindowSoftwareX11& );
  bool operator==( const EvasWindowSoftwareX11& );

  XWindow *mExwin;
};

} // end namespace Ecorexx

#endif // ECORXX_EVAS_WINDOW_SOFTWARE_X11_H
