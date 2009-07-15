#ifndef EFLXX_EWL_WINDOW_H
#define EFLXX_EWL_WINDOW_H

/* EFLxx */
#include "EwlEmbed.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlWindow : public EwlEmbed
{
public:
  EwlWindow( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlWindow();

  void setTitle( const char* title );
};

} // end namespace efl

#endif // EFLXX_EWL_WINDOW_H
