#ifndef EFLXX_EWL_BUTTON_H
#define EFLXX_EWL_BUTTON_H

/* EFLxx */
#include "EwlBox.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlButton : public EwlBox
{
public:
  EwlButton( EwlObject* parent = 0, const char* name = 0 );
  EwlButton( const char* text, EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlButton();

  void setText( const char* text );
};

} // end namespace efl

#endif // EFLXX_EWL_BUTTON_H
