#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkcontainer.h"

/* EFL */
#include <etk/etk_container.h>

namespace efl {
  
EtkContainer::EtkContainer( EtkObject* parent, const char* type, const char* name )
    :EtkWidget( parent, type, name )
{
    init( );
}

EtkContainer::EtkContainer (Etk_Object *o)
{
  _o = o;
  _managed = false;
}

EtkContainer::~EtkContainer()
{
}

void EtkContainer::add( EtkWidget* child )
{
   etk_container_add( ETK_CONTAINER(_o), ETK_WIDGET(child->obj()) );
}

void EtkContainer::remove (EtkWidget *widget)
{
    etk_container_remove (ETK_WIDGET (widget->obj ()));
}

void EtkContainer::removeAll ()
{
    etk_container_remove_all (ETK_CONTAINER(_o));
}

void EtkContainer::setBorderWidth( int width )
{
   etk_container_border_width_set( ETK_CONTAINER (_o), width);
}

int EtkContainer::getBorderWidth ()
{
    return etk_container_border_width_get (ETK_CONTAINER (_o));
}

Eina_List *EtkContainer::getChildren ()
{
    return etk_container_children_get (ETK_CONTAINER (_o));
}

bool EtkContainer::isChild (EtkWidget *widget)
{
    return etk_container_is_child(ETK_CONTAINER (_o), ETK_WIDGET (widget->obj ()));
}

void EtkContainer::fillChildSpace (EtkWidget *child, Etk_Geometry &out_child_space, bool hfill, bool vfill, float xalign, float yalign)
{
    etk_container_child_space_fill (ETK_WIDGET (child->obj ()), &out_child_space, hfill, vfill, xalign, yalign);
}

EtkContainer *EtkContainer::wrap( Etk_Object* o )
{
    return new EtkContainer (o);
}

} // end namespace efl
