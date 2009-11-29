#ifndef EFLXX_EWL_WIDGET_H
#define EFLXX_EWL_WIDGET_H

/* EFLxx */
#include "EwlObject.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlWidget : public EwlObject
{
public:
  EwlWidget( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlWidget();

  void show();
};


} // end namespace efl

#endif // EFLXX_EWL_WIDGET_H
