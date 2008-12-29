#ifndef ETK_BASE
#define ETK_BASE

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

/* EFL */
#include <etk/Etk.h>

#define etkApp EtkApplication::application()

using std::string;

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
};

class EtkContainer : public EtkWidget
{
    public:
    EtkContainer( EtkObject* parent = 0, const char* type = "Container", const char* name = 0 );
    virtual ~EtkContainer();

    /**
     * @brief Adds a child to the container
     * @param widget the widget to add
     */
    void add( EtkWidget* child );
    
    /**
     * @brief Removes a child from its container. It is equivalent to setParent (NULL)
     * @param widget the widget to remove
     */
    void remove (EtkWidget *widget);
    
    /**
     * @brief Unpacks all the children of the container
     */
    void removeAll ();
    
    /**
     * @brief Sets the border width of a container. The border width is the amount of space left around the inside of
     * the container. To add free space around the outside of a container, you can use etk_widget_padding_set()
     * @param border_width the border width to set
     * @see setPadding()
     */    
    void setBorderWidth( int );
    
    /**
     * @brief Gets the border width of the container
     * @return Returns the border width of the container
     */
    int getBorderWidth ();
    
    /**
     * @brief Gets the list of the children of the container. It simply calls the "childrend_get()" method of the container
     * @return Returns the list of the container's children
     * @note The returned list will have to be freed with eina_list_free() when you no longer need it
     * @todo wrap Eina_List
     */
    Eina_List *getChildren ();
    
    /**
     * @brief Gets whether the widget is a child of the container
     * @param widget the widget you want to check if it is a child of the container
     * @return Returns true if the widget is a child of the container, false otherwise
     */
    bool isChild (EtkWidget *widget);

    /**
     * @brief Calls @a for_each_cb(child) for each child of the container
     * @param for_each_cb the function to call
     * @todo do it with sigc++
     */
    //void etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child))
        
    /**
     * @brief Calls @a for_each_cb(child, data) for each child of the container
     * @param for_each_cb the function to call
     * @param data the data to pass as the second argument of @a for_each_cb()
     * @todo do it with sigc++
     */
    //void etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data)
        
    /**
     * @brief A utility function that resizes the given space according to the specified fill-policy.
     * It is mainly used by container implementations
     * @param child a child
     * @param child_space the space for the child. It will be modified according to the fill options
     * @param hfill if @a hfill == true, the child will fill the space horizontally
     * @param vfill if @a vfill == true, the child will fill the space vertically
     * @param xalign the horizontal alignment of the child widget in the child space (has no effect if @a hfill is true)
     * @param yalign the vertical alignment of the child widget in the child space (has no effect if @a vfill is true)
     */
    static void fillChildSpace (EtkWidget *child, Etk_Geometry &out_child_space, bool hfill, bool vfill, float xalign, float yalign);

    /**
     * @brief C object wrapper factory method
     * Only for internal usage!
     */
    static EtkContainer *wrap( Etk_Object* o );
    
  private:
    EtkContainer (Etk_Object *o);
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
/*
Etk_Widget * 	etk_image_new (void)
 	Creates a new empty image.
Etk_Widget * 	etk_image_new_from_file (const char *filename, const char *key)
 	Creates a new image and loads the image from an image file.
Etk_Widget * 	etk_image_new_from_edje (const char *filename, const char *group)
 	Creates a new image and loads the image from an edje-file.
Etk_Widget * 	etk_image_new_from_stock (Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
 	Creates a new image and loads the image corresponding to the stock id.
Etk_Widget * 	etk_image_new_from_evas_object (Evas_Object *evas_object)
 	Creates a new image from the given evas object.
Etk_Widget * 	etk_image_new_from_data (int width, int height, void *data, Etk_Bool copy)
 	Creates a new image from the given pixel data.
*/
    /**
     * @brief Loads the image from a file.
     * @param filename the path to the file to load
     * @param key the key to load (only used if the file is an Eet file, otherwise you can set it to NULL)
     */
    void setFromFile( const string &filename, const string &key );
    
    /**
     * @brief Gets the path to the file used by the image.
     * @param outFilename the location where to store the path to the loaded file
     * @param outKey the location where to store the key of the loaded image
     * @param false if the file is not loaded from an Eet file
     */
    void getFile( string &outFilename, string &outKey, bool eetLoaded );
    
    /**
     * @brief Loads the image from an edje file
     * @param filename the path to the edje-file to load
     * @param group the name of the edje-group to load
     */
    void setFromEdje (const string &filename, const string &group);
    
    /**
     * @brief Gets the filename and the group of the edje-object used for the image
     * @param outFilename the location to store the path to the edje-file used
     * @param outGroup the location to store the name of the edje-group used
     */
    void getEdje (string &outFilename, string &outGroup);
    
    /**
     * @brief Loads the image corresponding to the given stock-id
     * @param stock_id the stock-id corresponding to the icon to load
     * @param stock_size the size of the stock-icon
     */
    void setFromStock (Etk_Stock_Id stock_id, Etk_Stock_Size stock_size);
    
    /**
     * @brief Gets the stock-id used by the image
     * @return stock id used by the image
     */
    Etk_Stock_Id getStockId ();
    
    /**
     * @brief Gets the stock-size used by the image
     * @return stock size used by the image
     */
    Etk_Stock_Size getStockSize ();

    
    /**
     * @brief Loads the image from an Evas object
     * @param evas_object the Evas object to use. The object can be anything (image, edje object, emotion object, ...)
     */
    void setFromEvasObject (const EvasObject &evas_object);
    
    /**
     * @brief Gets the Evas object used by the image. You can call this function even if you have not explicitly set the
     * Evas object used by this image. For example, if you have loaded the image from a file, this function will return the
     * corresponding Evas image object. You should just be careful by manipulating it (don't use Edje functions on an image
     * object for example).
     * @return Returns the EvasObject of the image
     */
    CountedPtr <EvasObject> getEvasObject ();
  
    /**
     * @brief Sets the pixels of the image
     * @param width the width of the image
     * @param height the height of the image
     * @param data a pointer to the pixels: the pixels have to be stored in the premul'ed ARGB format
     * @param copy whether the pixels should be copied or not. If you decide not to copy the pixels, you have to make sure
     * the memory area where the pixels are stored is valid during all the lifetime of the image
     * @return Returns the new image widget
     */
    void setFromData (int width, int height, void *data, bool copy);
    
    /**
     * @brief Gets a pointer to the image pixels. This function only works if the image has been loaded from a file or if
     * you have explicitely set its pixels with etk_image_set_from_data().
     * @param for_writing whether or not you want to be able to modify the pixels of the image. If so, call
     * update() once you have finished.
     * @return Returns a pointer to the location of the pixels (stored in premul'ed ARGB format)
     * @note If the image is loaded from a file, it has to be realized. Otherwise it will return NULL
     */
    void *getData (bool for_writing);
    
    /**
     * @brief Gets the source of the image (file, edje-file, stock, Evas object or pixel data)
     * @return Returns the source of the image
     */
    Etk_Image_Source getSource ();
    
    /**
     * @brief Updates all the pixels of the image (to be called after you have modified the pixel buffer for example).
     * Same as rectUpdate(0, 0, image_width, image_height)
     */
    void update ();
    
    /**
     * @brief Updates a rectangle of the pixels of the image (to be called after you have modified the pixel buffer
     * for example). It only has effect on image loaded from a pixel buffer
     * @param x the x position of the top-left corner of the rectangle to update
     * @param y the y position of the top-left corner of the rectangle to update
     * @param w the width of the rectangle to update
     * @param h the height of the rectangle to update
     */
    void rectUpdate (int x, int y, int w, int h);
    
    /**
     * @brief Gets the native width of the image. If the image is loaded from a file or from given pixels, it returns the
     * native size of the image. If the image is loaded from an Edje file or a stock-id, it returns the minimum size of the
     * Edje object (a stock image is an Edje object). Otherwise, the returned size is 0x0
     * @return native width of the image
     */
    int getWidth ();
    
    /**
     * @brief Gets the native height of the image. If the image is loaded from a file or from given pixels, it returns the
     * native size of the image. If the image is loaded from an Edje file or a stock-id, it returns the minimum size of the
     * Edje object (a stock image is an Edje object). Otherwise, the returned size is 0x0
     * @return native height of the image
     */
    int getHeight ();
    
    /**
     * @brief Sets if the image should keep its aspect ratio when it is resized
     * @param keep_aspect if @a keep_aspect is true, the image will keep its aspect ratio when it is resized
     */
    void setAspect (bool keep_aspect);
    
    /**
     * @brief Gets whether the image keeps its aspect ratio when it is resized
     * @return Returns true if the image keeps its aspect ratio when it is resized
     */
    bool getKeepAspect ();
    
     /**
     * @brief Sets (forces) the aspect ratio of the image. You can use this function for example to set the aspect-ratio
     * when you set the image from an Evas object with etk_image_set_from_evas_object().
     * @param aspect_ratio the aspect ratio to set, or 0.0 to make Etk calculates automatically the aspect ratio
     */
    void setRatio (double aspect_ratio);
    
    /**
     * @brief Gets the aspect ratio you set to the image. If no aspect ratio has been set, it will return 0.0.
     * To know the native aspect ratio, call etk_image_size_get() to get the native size of the image and calculate the
     * aspect ratio from these values.
     * @return Returns the aspect ratio you set to the image, or 0.0 if no aspect ratio has been set
     */
    double getRatio ();
   
  /**
   * @brief Copies the image @a src_image to @a dest_image
   * @param dest_image the destination image
   * @param src_image the image to copy
   * @note If @a src_image is an edje image, the current state of the edje animation won't be copied
   */
    /*
   TODO: copy constructor:
   void 	etk_image_copy (Etk_Image *dest_image, Etk_Image *src_image)
   */
  
    /**
     * @brief C object wrapper factory method
     * Only for internal usage!
     */
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
