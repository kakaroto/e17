#ifndef EFLPP_ETKCONTAINER_H
#define EFLPP_ETKCONTAINER_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

#include "eflpp_etkwidget.h"

using std::string;

namespace efl {

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

} // end namespace efl

#endif // EFLPP_ETKCONTAINER_H
