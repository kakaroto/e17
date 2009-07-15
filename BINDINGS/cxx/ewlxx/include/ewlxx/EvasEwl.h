#ifndef EVAS_EWL_H
#define EVAS_EWL_H

/* EFLxx */
#include <evasxx/EvasCanvas.h>

/* EFL */
#include <Ewl.h>

namespace efl {

/* forward declarations */
class EwlEmbed;

class EvasEwl : public EvasObject
{
public:
  EvasEwl( EvasCanvas &canvas, EwlEmbed* ewlobj, const char* name = 0 );
  ~EvasEwl();
};

} // end namespace efl

#endif // EVAS_EWL_H
