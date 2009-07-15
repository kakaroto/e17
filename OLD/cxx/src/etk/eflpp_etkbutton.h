#ifndef EFLPP_ETKBUTTON_H
#define EFLPP_ETKBUTTON_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

#include "eflpp_etkbox.h"
#include "eflpp_etkimage.h"

#include <etk/etk_button.h> // TODO: Wrap C enum types to not use C header (e.g. for Etk_Stock_Id)!

using std::string;

namespace efl {

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



} // end namespace efl

#endif // EFLPP_ETKBUTTON_H
