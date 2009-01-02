#ifndef EFLPP_ETKBOX_H
#define EFLPP_ETKBOX_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

#include "eflpp_etkcontainer.h"

using std::string;

namespace efl {
  
class EtkBox : public EtkContainer
{
    public:
    EtkBox( EtkObject* parent = 0, const char* type = "Box", const char* name = 0 );
    virtual ~EtkBox();
};


} // end namespace efl

#endif // EFLPP_ETKBOX_H
