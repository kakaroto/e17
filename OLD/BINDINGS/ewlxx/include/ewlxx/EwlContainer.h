#ifndef EFLXX_EWL_CONTAINER_H
#define EFLXX_EWL_CONTAINER_H

/* EFLxx */
#include "EwlWidget.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlContainer : public EwlWidget
{
public:
  EwlContainer( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlContainer();

  void appendChild( EwlWidget* child );
};

} // end namespace efl

#endif // EFLXX_EWL_CONTAINER_H
