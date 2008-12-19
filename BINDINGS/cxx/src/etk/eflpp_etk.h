#ifndef ETK_BASE
#define ETK_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>

/* EFL */
#include <etk/Etk.h>

#define etkApp EtkApplication::application()

namespace efl {

class EtkObject;
class EtkWidget;

class EtkObject
{
  public:
    EtkObject( EtkObject* parent = 0, const char* type = "<unknown>", const char* name = 0 );
    virtual ~EtkObject();

    void setFillPolicy(  );
    void setAlignment(  );

    void resize( const Size& size );

  public:
    Etk_Object* obj() const { return _o; };

  protected:
    void init( );
  
    Etk_Object* _o;
    const char* _type;
    bool _managed;
};

class EtkWidget : public EtkObject
{
    public:
    EtkWidget( EtkObject* parent = 0, const char* type = "Widget", const char* name = 0 );
    virtual ~EtkWidget();

    void show();
    void showAll();
    void hide();
    void hideAll();

    bool isVisible() const;
    void setVisibilityLock( bool );
    bool visibilityLock() const;
};

class EtkContainer : public EtkWidget
{
    public:
    EtkContainer( EtkObject* parent = 0, const char* type = "Container", const char* name = 0 );
    virtual ~EtkContainer();

    void appendChild( EtkWidget* child );
    void setBorderWidth( int );
};

class EtkTopLevelWidget : public EtkContainer
{
    public:
    EtkTopLevelWidget( EtkObject* parent = 0, const char* type = "TopLevelWidget", const char* name = 0 );
    virtual ~EtkTopLevelWidget();
};

class EtkBox : public EtkContainer
{
    public:
    EtkBox( EtkObject* parent = 0, const char* type = "Box", const char* name = 0 );
    virtual ~EtkBox();
};

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

class EtkImage : public EtkWidget
{
  public:
    static EtkImage *wrap( Etk_Object* o );
  
  private:
    EtkImage( Etk_Object *o );
};

/// \todo wrap C enum types to C++
class EtkButton : public EtkBox
{
    public:
    EtkButton( EtkObject* parent = 0, const char* type = "Button", const char* name = 0 );
    EtkButton( const char* text, EtkObject* parent = 0, const char* type = "Button", const char* name = 0 );
    virtual ~EtkButton();

    /*
   TODO:
Etk_Widget * 	etk_button_new (void)
 	Creates a new button.
Etk_Widget * 	etk_button_new_with_label (const char *label)
 	Creates a new button with a label.
Etk_Widget * 	etk_button_new_from_stock (Etk_Stock_Id stock_id)
 	Creates a new button with a label and an icon defined by a stock-id.
void 	etk_button_press (Etk_Button *button)
 	Presses the button if it wasn't already pressed.
void 	etk_button_release (Etk_Button *button)
 	Releases the button if it was pressed.
void 	etk_button_click (Etk_Button *button)
 	Clicks on the button.
      */
    
    /*!
     *  Sets the text of the button's label.
     */
    void setText( const char* text );
  
    /*!
     *  Gets the text of the button's label.
     */
    const char *getText( );
    
    /*!
     *  Sets the image of the button.
     */
    void setImage( EtkImage *image );
    
    /*!
     *  Gets the image of the button.
     */
    EtkImage *getImage( );
    
    /*!
     *  Sets the label and the image of the button from a stock-id.
     */
    void setFromStock( Etk_Stock_Id stock_id );
    
    /*!
     *  Sets the style of the button (icon, text, both vertically, both horizontally).
     */
    void setStyle( Etk_Button_Style style );
    
    /*!
     *  Gets the style of the button.
     */
    Etk_Button_Style getStyle( );
    
    /*!
     *  Sets the stock-size of the button's image.
     */
    void setStockSize( Etk_Stock_Size size );
    
    /*!
     *  Gets the stock-size of the button's image.
     */
    Etk_Stock_Size getStockSize( );
    
    /*!
     *  Sets the alignment of the child of the button.
     */
    void setAlignment( float xalign, float yalign );
    
    /*!
     *  Gets the alignment of the button's child.
     */
    void getAlignment( float &xalign, float &yalign );
    
    static EtkButton *wrap( Etk_Object* o );
    
  private:
    EtkButton( Etk_Object *o );
};

class EtkEmbed : public EtkTopLevelWidget
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

class EtkWindow : public EtkTopLevelWidget
{
    public:
    EtkWindow( EtkObject* parent = 0, const char* type = "Window", const char* name = 0 );
    virtual ~EtkWindow();

    void setTitle( const char* title );
};

class EtkApplication
{
    public:
    EtkApplication( int argc, const char** argv, const char* name );
    ~EtkApplication();

    static EtkApplication* application();
    const char* name() const { return _name; };

    /* Main Window */
    void setMainWindow( EtkWindow* );
    EtkWindow* mainWindow() const;

    /* Main Loop */
    void exec();
    void exit();

    private:
    const char* _name;
    static EtkApplication* _instance;
    EtkWindow* _mainWindow;

};

}
#endif
