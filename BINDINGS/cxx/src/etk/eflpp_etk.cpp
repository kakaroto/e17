#include "eflpp_etk.h"

/* EFL */
#include <Ecore_Evas.h>

/* STD */
#include <iostream>
#include <cstring>
#include <assert.h>

using namespace std;

namespace efl {

//==========================================================================//
// EtkEmbed
//==========================================================================//

EtkEmbed::EtkEmbed( EvasCanvas* canvas, EtkObject* parent, const char* type, const char* name )
    :EtkTopLevel( parent, type, name )
{
    init( );
    _o = ETK_OBJECT( etk_embed_new( canvas->obj()) );
}

EtkEmbed::~EtkEmbed()
{
}

void EtkEmbed::setFocus( bool b )
{
    //ewl_embed_focus_set( EWL_EMBED( _o ), b );
}

//==========================================================================//
// EvasEtk
//==========================================================================//

EvasEtk::EvasEtk( EtkEmbed* ewlobj, const char* name )

{
    o = etk_embed_object_get( ETK_EMBED(ewlobj->obj()) );
}

EvasEtk::~EvasEtk()
{
}

//==========================================================================//
// EtkHBox
//==========================================================================//

EtkHBox::EtkHBox( EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    init( );
    //ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_HORIZONTAL );
}

EtkHBox::~EtkHBox()
{
}

//==========================================================================//
// EtkVBox
//==========================================================================//

EtkVBox::EtkVBox( EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    init( );
    //ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_VERTICAL );
}

EtkVBox::~EtkVBox()
{
}

} // end namespace efl
