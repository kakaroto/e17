#ifndef EFLXX_EWL_EMBED_H
#define EFLXX_EWL_EMBED_H

/* EFLxx */
#include "EwlOverlay.h"

/* EFL */
#include <Ewl.h>

namespace efl {

class EwlEmbed : public EwlOverlay
{
public:
  EwlEmbed( EwlObject* parent = 0, const char* name = 0 );
  virtual ~EwlEmbed();

  void setFocus( bool b );
};

} // end namespace efl

#endif // EFLXX_EWL_EMBED_H
