#ifndef EFLXX_EWL_HBOX_H
#define EFLXX_EWL_HBOX_H

/* EFLxx */
#include "EwlBox.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlHBox : public EwlBox
{
public:
  EwlHBox( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlHBox();
};

} // end namespace efl

#endif // EFLXX_EWL_HBOX_H
