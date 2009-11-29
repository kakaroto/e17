#ifndef EFLXX_EWL_OVERLAY_H
#define EFLXX_EWL_OVERLAY_H

/* EFLxx */
#include "EwlContainer.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlOverlay : public EwlContainer
{
public:
  EwlOverlay( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlOverlay();
};

} // end namespace efl

#endif // EFLXX_EWL_OVERLAY_H
