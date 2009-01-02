#ifndef EFLPP_ETKWINDOW_H
#define EFLPP_ETKWINDOW_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include "eflpp_etktoplevel.h"

using std::string;

namespace efl {

class EtkWindow : public EtkTopLevel
{
    public:
    EtkWindow( EtkObject* parent = 0, const char* type = "Window", const char* name = 0 );
    virtual ~EtkWindow();

    void setTitle( const char* title );
};

} // end namespace efl

#endif // EFLPP_ETKWINDOW_H
