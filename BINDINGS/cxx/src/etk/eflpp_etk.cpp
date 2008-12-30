#include "eflpp_etk.h"

/* EFL */
#include <Ecore_Evas.h>

/* STD */
#include <iostream>
#include <cstring>
#include <assert.h>

using namespace std;

namespace efl {

EtkApplication* EtkApplication::_instance = 0;

//==========================================================================//
// EtkTopLevelWidget
//==========================================================================//

EtkTopLevelWidget::EtkTopLevelWidget( EtkObject* parent, const char* type, const char* name )
    :EtkContainer( parent, type, name )
{
    init( );
}

EtkTopLevelWidget::~EtkTopLevelWidget()
{
}

//==========================================================================//
// EtkEmbed
//==========================================================================//

EtkEmbed::EtkEmbed( EvasCanvas* canvas, EtkObject* parent, const char* type, const char* name )
    :EtkTopLevelWidget( parent, type, name )
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
// EtkWindow
//==========================================================================//

EtkWindow::EtkWindow( EtkObject* parent, const char* type, const char* name )
    :EtkTopLevelWidget( parent, type, name )
{
}

EtkWindow::~EtkWindow()
{
}

void EtkWindow::setTitle( const char* title )
{
    etk_window_title_set( ETK_WINDOW( _o ), strdup( title ) );
}

//==========================================================================//
// EtkBox
//==========================================================================//

EtkBox::EtkBox( EtkObject* parent, const char* type, const char* name )
    :EtkContainer( parent, type, name )
{
    //_o = ETK_OBJECT( etk_box_new() );
}

EtkBox::~EtkBox()
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
  
//==========================================================================//
// EtkButton
//==========================================================================//

EtkButton::EtkButton( EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    init( );
    setLabel( name ? name : "unnamed" );
}

EtkButton::EtkButton( const char* text, EtkObject* parent, const char* type, const char* name )
    :EtkBox( parent, type, name )
{
    setLabel( text );
}

EtkButton::EtkButton( Etk_Object *o )
{
    _o = o;
    _managed = false;
}

EtkButton::~EtkButton()
{
}

void EtkButton::setLabel( const string &label )
{
    etk_button_label_set( ETK_BUTTON(_o), label.c_str () );
}

const string EtkButton::getLabel( )
{
    return etk_button_label_get (ETK_BUTTON(_o));
}

void EtkButton::setImage( EtkImage *image )
{
    etk_button_image_set( ETK_BUTTON( _o ), ETK_IMAGE( image->obj( ) ));
}

EtkImage *EtkButton::getImage( )
{
    return EtkImage::wrap ( ETK_OBJECT( etk_button_image_get( ETK_BUTTON (_o ) )));
}

EtkButton *EtkButton::wrap( Etk_Object* o )
{
  return new EtkButton( o );
}

void EtkButton::setFromStock( Etk_Stock_Id stock_id )
{
    etk_button_set_from_stock( ETK_BUTTON( _o ), stock_id );
}

void EtkButton::setStyle( Etk_Button_Style style )
{
    etk_button_style_set( ETK_BUTTON( _o ), style );
}

Etk_Button_Style EtkButton::getStyle( )
{
    return etk_button_style_get( ETK_BUTTON( _o ));
}

void EtkButton::setStockSize( Etk_Stock_Size size )
{
    etk_button_stock_size_set( ETK_BUTTON( _o ), size );
}

Etk_Stock_Size EtkButton::getStockSize( )
{
    return etk_button_stock_size_get( ETK_BUTTON( _o ) );
}

void EtkButton::setAlignment( float xalign, float yalign )
{
    etk_button_alignment_set( ETK_BUTTON( _o ), xalign, yalign );
}

void EtkButton::getAlignment( float &xalign, float &yalign )
{
    etk_button_alignment_get( ETK_BUTTON( _o ), &xalign, &yalign);
}


//===============================================================================================
// EtkApplication
//===============================================================================================

EtkApplication::EtkApplication( int argc, const char **argv, const char* name )
    :_name( name )
{
    if ( EtkApplication::_instance )
    {
        cout << "FATAL: EtkApplication object already created" << endl;
            ::exit( -1 );
    }
    cout << "EtkApplication::EtkApplication" << endl;
    if ( !etk_init( argc, const_cast<char**>(argv) ) )
    {
            cout << "FATAL: Couldn't initialize Etk" << endl;
            ::exit( -1 );
    }
    EtkApplication::_instance = this;
}

EtkApplication::~EtkApplication()
{
    cout << "EtkApplication::~EtkApplication" << endl;
    etk_shutdown();
}

EtkApplication* EtkApplication::application()
{
    if ( !EtkApplication::_instance )
    {
        cerr << "FATAL: EtkApplication object hasn't been created yet" << endl;
            ::exit( -1 );
    }
    else
    {
        return EtkApplication::_instance;
    }
}

void EtkApplication::setMainWindow( EtkWindow* mw )
{
    _mainWindow = mw;
    mw->setTitle( _name );
}

EtkWindow* EtkApplication::mainWindow() const
{
    return _mainWindow;
}

void EtkApplication::exec()
{
    cout << "EtkApplication::exec() - >>> jumping into main loop" << endl;
    etk_main();
    cout << "EtkApplication::exec() - <<< returned from main loop" << endl;
}

void EtkApplication::exit()
{
    //ecore_main_loop_quit();
}

}
