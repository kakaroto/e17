#ifndef EFLPP_ETKTOPLEVEL_H
#define EFLPP_ETKTOPLEVEL_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include "eflpp_etkcontainer.h"

using std::string;

namespace efl {

class EtkTopLevel : public EtkContainer
{
    public:
    EtkTopLevel( EtkObject* parent = 0, const char* type = "TopLevelWidget", const char* name = 0 );
    virtual ~EtkTopLevel();
};

} // end namespace efl

#endif // EFLPP_ETKTOPLEVEL_H
