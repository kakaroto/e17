#ifndef EFLXX_EWL_VBOX_H
#define EFLXX_EWL_VBOX_H

/* EFLxx */
#include "EwlBox.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlVBox : public EwlBox
{
public:
  EwlVBox( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlVBox();
};

} // end namespace efl

#endif // EFLXX_EWL_VBOX_H
