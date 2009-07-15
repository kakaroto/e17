#include "eflpp_ewl.h"

/* EFL */
#include <Ecore_Evas.h>

/* STD */
#include <iostream>
#include <cstring>
using namespace std;

namespace efl {

EwlApplication* EwlApplication::_instance = 0;

//==========================================================================//
// EwlObject
//==========================================================================//

EwlObject::EwlObject( EwlObject* parent, const char* name )
{
    cout << "EwlObject::EwlObject - generating object '" << ( name ? name : "<unknown>" ) << "' (" << /*type <<*/ ")" << endl;
}

EwlObject::~EwlObject()
{
}

void EwlObject::resize( const Size& size )
{
    ewl_object_size_request( _o, size.width(), size.height() );
}

void EwlObject::setFillPolicy( FillPolicy p )
{
    ewl_object_fill_policy_set( EWL_OBJECT( _o ), p );
}

void EwlObject::setAlignment( Alignment a )
{
    ewl_object_alignment_set( EWL_OBJECT( _o ), a );
}

//==========================================================================//
// EwlWidget
//==========================================================================//

EwlWidget::EwlWidget( EwlObject* parent, const char* name )
    :EwlObject( parent, name )
{
}

EwlWidget::~EwlWidget()
{
}

void EwlWidget::show()
{
    ewl_widget_show( EWL_WIDGET( _o ) );
}

//==========================================================================//
// EwlContainer
//==========================================================================//

EwlContainer::EwlContainer( EwlObject* parent, const char* name )
    :EwlWidget( parent, name )
{
}

EwlContainer::~EwlContainer()
{
}

void EwlContainer::appendChild( EwlWidget* child )
{
    ewl_container_child_append( EWL_CONTAINER( _o ), EWL_WIDGET( child->obj() ) );
}

//==========================================================================//
// EwlOverlay
//==========================================================================//

EwlOverlay::EwlOverlay( EwlObject* parent, const char* name )
    :EwlContainer( parent, name )
{
}

EwlOverlay::~EwlOverlay()
{
}

//==========================================================================//
// EwlEmbed
//==========================================================================//

EwlEmbed::EwlEmbed( EwlObject* parent, const char* name )
    :EwlOverlay( parent, name )
{
    _o = EWL_OBJECT( ewl_embed_new() );
}

EwlEmbed::~EwlEmbed()
{
}

void EwlEmbed::setFocus( bool b )
{
    ewl_embed_focus_set( EWL_EMBED( _o ), b );
}

//==========================================================================//
// EvasEwl
//==========================================================================//

EvasEwl::EvasEwl( EwlEmbed* ewlobj, EvasCanvas* canvas, const char* name )
{
#warning not sure whether this belongs here
#if 0 // [audifahrer]
    ewl_object_fill_policy_set( EWL_OBJECT(ewlobj->obj() ), EWL_FLAG_FILL_ALL );
    o = ewl_embed_evas_set( EWL_EMBED( ewlobj->obj() ), canvas->obj(), EWL_EMBED_EVAS_WINDOW(ecore_evas_software_x11_window_get(canvas->obj())));
#endif
}

EvasEwl::~EvasEwl()
{
}

//==========================================================================//
// EwlWindow
//==========================================================================//

EwlWindow::EwlWindow( EwlObject* parent, const char* name )
    :EwlEmbed( parent, name )
{
    _o = EWL_OBJECT( ewl_window_new() );

    ewl_window_name_set( EWL_WINDOW( _o ), "EWL_WINDOW" );
    ewl_window_class_set( EWL_WINDOW( _o ), "EWLWindow" );
}

EwlWindow::~EwlWindow()
{
}

void EwlWindow::setTitle( const char* title )
{
    ewl_window_title_set( EWL_WINDOW( _o ), strdup( title ) );
}

//==========================================================================//
// EwlBox
//==========================================================================//

EwlBox::EwlBox( EwlObject* parent, const char* name )
    :EwlContainer( parent, name )
{
    _o = EWL_OBJECT( ewl_box_new() );
}

EwlBox::~EwlBox()
{
}

//==========================================================================//
// EwlHBox
//==========================================================================//

EwlHBox::EwlHBox( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
    ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_HORIZONTAL );
}

EwlHBox::~EwlHBox()
{
}

//==========================================================================//
// EwlVBox
//==========================================================================//

EwlVBox::EwlVBox( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
    ewl_box_orientation_set( EWL_BOX(_o), EWL_ORIENTATION_VERTICAL );
}

EwlVBox::~EwlVBox()
{
}

//==========================================================================//
// EwlButton
//==========================================================================//

EwlButton::EwlButton( EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
    _o = EWL_OBJECT( ewl_button_new() );
    setText( name ? name : "unnamed" );
}

EwlButton::EwlButton( const char* text, EwlObject* parent, const char* name )
    :EwlBox( parent, name )
{
    _o = EWL_OBJECT( ewl_button_new() );
    setText( text );
}

EwlButton::~EwlButton()
{
}

void EwlButton::setText( const char* text )
{
    ewl_button_label_set( EWL_BUTTON(_o), const_cast<char*>( text ) );
}

//===============================================================================================
// EwlApplication
//===============================================================================================

EwlApplication::EwlApplication( int argc, const char **argv, const char* name )
    :_name( name )
{
    if ( EwlApplication::_instance )
    {
        cout << "FATAL: EwlApplication object already created" << endl;
            ::exit( -1 );
    }
    cout << "EwlApplication::EwlApplication" << endl;
    ewl_init( &argc, const_cast<char**>( argv ) );
    EwlApplication::_instance = this;
}

EwlApplication::~EwlApplication()
{
    cout << "EwlApplication::~EwlApplication" << endl;
    ewl_shutdown();
}

EwlApplication* EwlApplication::application()
{
    if ( !EwlApplication::_instance )
    {
        cerr << "FATAL: EwlApplication object hasn't been created yet" << endl;
            ::exit( -1 );
    }
    else
    {
        return EwlApplication::_instance;
    }
}

void EwlApplication::setMainWindow( EwlWindow* mw )
{
    _mainWindow = mw;
    mw->setTitle( _name );
}

EwlWindow* EwlApplication::mainWindow() const
{
    return _mainWindow;
}

void EwlApplication::exec()
{
    cout << "EwlApplication::exec() - >>> jumping into main loop" << endl;
    ewl_main();
    cout << "EwlApplication::exec() - <<< returned from main loop" << endl;
}

void EwlApplication::exit()
{
    //ecore_main_loop_quit();
}

}
