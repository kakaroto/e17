#ifndef ETK_BASE
#define ETK_BASE

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include <eflpp_evascanvas.h>

#include "eflpp_etkcontainer.h"
#include "eflpp_etkimage.h"

/* EFL */
#include <etk/Etk.h>

#define etkApp EtkApplication::application()

using std::string;

namespace efl {

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
    
    /**
     * @brief Sets the text of the button's label
     * @param label the text to set to the button's label
     */
    void setLabel( const string &label );
  
    /**
     * @brief Gets the text of the button's label
     * @return Returns the text of the button's label
     */
    const string getLabel( );
    
    /**
     * @brief Sets the image of the button
     * @param image the image to set
     * @note If the button already has an image, the current image will just be unpacked, it will not be destroyed
     * @note The image will be automatically shown, but you can still manually hide it with calling etk_widget_hide()
     * after having called etk_button_image_set()
     * @todo Change usage to CountedPtr!
     */
    void setImage( EtkImage *image );

    /**
     * @brief Gets the image of the button
     * @return Returns the image of the button, or NULL if the button has no image
     * @todo Change usage to CountedPtr!
     */
    EtkImage *getImage( );
    
    /**
     * @brief Sets the label and the image of the button from a stock-id
     * @param stock_id the stock-id to use
     * @note For some stock-id, the label is empty
     */
    void setFromStock( Etk_Stock_Id stock_id );
    
    /**
     * @brief Sets the style of the button (icon, text, both vertically, both horizontally)
     * @param style the style to give to the button
     */
    void setStyle( Etk_Button_Style style );
    
    /**
     * @brief Gets the style of the button
     * @return Returns the button's style
     */
    Etk_Button_Style getStyle( );
    
    /**
     * @brief Sets the stock-size of the button's image
     * @param size the stock-size
     */
    void setStockSize( Etk_Stock_Size size );
    
    /**
     * @brief Gets the stock-size of the button's image
     * @return Returns the stock-size of the button's image
     */
    Etk_Stock_Size getStockSize( );
    
    /**
     * @brief Sets the alignment of the child of the button
     * @param xalign the horizontal alignment (0.0 = left, 0.5 = center, 1.0 = right, ...)
     * @param yalign the vertical alignment (0.0 = top, 0.5 = center, 1.0 = bottom, ...)
     * @note It only has effect if the child is a label or an alignment
     */
    void setAlignment( float xalign, float yalign );
    
    /**
     * @brief Gets the alignment of the button's child
     * @param xalign the location where to store the horizontal alignment
     * @param yalign the location where to store the vertical alignment
     */
    void getAlignment( float &xalign, float &yalign );
    
    /**
     * @brief C object wrapper factory method
     * Only for internal usage!
     */
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
