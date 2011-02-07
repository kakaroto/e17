#ifndef ECOREXX_EVAS_WINDOW_FB_H
#define ECOREXX_EVAS_WINDOW_FB_H

 /* EFLxx */
#include <evasxx/Evasxx.h>
#include "EvasWindow.h"

namespace Ecorexx {

class EvasWindowFB : public EvasWindow
{
public:
  EvasWindowFB( const Eflxx::Size &size, const char* display = 0, int rotation = 0 );
  virtual ~EvasWindowFB();

private:
  EvasWindowFB();
  bool operator=( const EvasWindowFB& );
  bool operator==( const EvasWindowFB& );
};

} // end namespace Ecorexx

#endif // ECOREXX_EVAS_WINDOW_FB_H
