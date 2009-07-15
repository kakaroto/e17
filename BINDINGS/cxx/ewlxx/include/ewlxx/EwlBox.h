#ifndef EFLXX_EWL_BOX_H
#define EFLXX_EWL_BOX_H

/* EFLxx */
#include "EwlContainer.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlBox : public EwlContainer
{
public:
  EwlBox( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlBox();
};

} // end namespace efl

#endif // EFLXX_EWL_BOX_H
