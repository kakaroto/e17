#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkbox.h"

namespace efl {

EtkBox::EtkBox( EtkObject* parent, const char* type, const char* name )
    :EtkContainer( parent, type, name )
{
    //_o = ETK_OBJECT( etk_box_new() );
}

EtkBox::~EtkBox()
{
}

void EtkBox::prepend (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fillPolicy, int padding)
{
    etk_box_prepend (ETK_BOX (_o), ETK_WIDGET (child.obj ()), group, fillPolicy, padding);
}

void EtkBox::append (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fillPolicy, int padding)
{
    etk_box_append (ETK_BOX (_o), ETK_WIDGET (child.obj ()), group, fillPolicy, padding);
}

void EtkBox::insert (EtkWidget &child, Etk_Box_Group group, EtkWidget &after, Etk_Box_Fill_Policy fillPolicy, int padding)
{
    etk_box_insert (ETK_BOX (_o), ETK_WIDGET (child.obj ()), group, ETK_WIDGET (after.obj ()), fillPolicy, padding);
}

void EtkBox::insertAt (EtkWidget &child, Etk_Box_Group group, int pos, Etk_Box_Fill_Policy fillPolicy, int padding)
{
    etk_box_insert_at (ETK_BOX (_o), ETK_WIDGET (child.obj ()), group, pos, fillPolicy, padding);
}

/*EtkWidget *EtkBox::getChildAt (Etk_Box_Group group, int pos)
{
  //~ Etk_Widget *etk_box_child_get_at(ETK_BOX (_o), Etk_Box_Group group, int pos);
}*/

void EtkBox::setChildPosition (EtkWidget &child, Etk_Box_Group group, int pos)
{
    etk_box_child_position_set(ETK_BOX (_o), ETK_WIDGET (child.obj ()), group, pos);
}

bool EtkBox::getChildPosition (EtkWidget &child, Etk_Box_Group &outGroup, int &outPos)
{
    return etk_box_child_position_get(ETK_BOX (_o), ETK_WIDGET (child.obj ()), &outGroup, &outPos);
}

void EtkBox::setChildPacking (EtkWidget &child, Etk_Box_Fill_Policy fillPolicy, int padding)
{
    etk_box_child_packing_set(ETK_BOX (_o), ETK_WIDGET (child.obj ()), fillPolicy, padding);
}

bool EtkBox::getChildPacking (EtkWidget &child, Etk_Box_Fill_Policy &outFillPolicy, int &outPadding)
{
    return etk_box_child_packing_get(ETK_BOX (_o), ETK_WIDGET (child.obj ()), &outFillPolicy, &outPadding);
}

void EtkBox::setSpacing (int spacing)
{
    etk_box_spacing_set(ETK_BOX (_o), spacing);
}

int EtkBox::getSpacing ()
{
    return etk_box_spacing_get(ETK_BOX (_o));
}

void EtkBox::setHomogeneous (bool homogeneous)
{
    etk_box_homogeneous_set(ETK_BOX (_o), homogeneous);
}

bool EtkBox::getHomogeneous ()
{
    return etk_box_homogeneous_get(ETK_BOX (_o));
}

} // end namespace efl
