#ifndef EFLPP_ETK_H
#define EFLPP_ETK_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include <eflpp_evascanvas.h>

#include "eflpp_etkobject.h"
#include "eflpp_etkwidget.h"
#include "eflpp_etkcontainer.h"
#include "eflpp_etkimage.h"
#include "eflpp_etkbox.h"
#include "eflpp_etktoplevel.h"
#include "eflpp_etkbutton.h"
#include "eflpp_etkapplication.h"

/* EFL */
#include <etk/Etk.h>

using std::string;

namespace efl {

class EtkHBox : public EtkBox
{
    public:
    EtkHBox( EtkObject* parent = 0, const char* type = "HBox", const char* name = 0 );
    virtual ~EtkHBox();
};

class EtkVBox : public EtkBox
{
    public:
    EtkVBox( EtkObject* parent = 0, const char* type = "VBox", const char* name = 0 );
    virtual ~EtkVBox();
};

class EtkEmbed : public EtkTopLevel
{
    public:
    EtkEmbed( EvasCanvas*, EtkObject* parent = 0, const char* type = "Embed", const char* name = 0 );
    virtual ~EtkEmbed();

    void setFocus( bool b );
};

class EvasEtk : public EvasObject
{
    public:
    EvasEtk( EtkEmbed* ewlobj, const char* name = 0 );
    ~EvasEtk();
};

}

#endif // EFLPP_ETK_H
