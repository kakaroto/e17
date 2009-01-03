#ifndef EFLPP_ETKBOX_H
#define EFLPP_ETKBOX_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>

#include "eflpp_etkcontainer.h"

using std::string;

namespace efl {
  
class EtkBox : public EtkContainer
{
  public:
    EtkBox( EtkObject* parent = 0, const char* type = "Box", const char* name = 0 );
    virtual ~EtkBox();
    
    //void prepend (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
    //~ void        etk_box_prepend(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
    
    //void append (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
    //~ void        etk_box_append(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Box_Fill_Policy fill_policy, int padding);
    
    
    //~ void        etk_box_insert(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, Etk_Widget *after, Etk_Box_Fill_Policy fill_policy, int padding);
    //~ void        etk_box_insert_at(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos, Etk_Box_Fill_Policy fill_policy, int padding);

    //~ Etk_Widget *etk_box_child_get_at(Etk_Box *box, Etk_Box_Group group, int pos);
    //~ void        etk_box_child_position_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Group group, int pos);
    //~ Etk_Bool    etk_box_child_position_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Group *group, int *pos);
    //~ void        etk_box_child_packing_set(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy fill_policy, int padding);
    //~ Etk_Bool    etk_box_child_packing_get(Etk_Box *box, Etk_Widget *child, Etk_Box_Fill_Policy *fill_policy, int *padding);

    //~ void        etk_box_spacing_set(Etk_Box *box, int spacing);
    //~ int         etk_box_spacing_get(Etk_Box *box);
    //~ void        etk_box_homogeneous_set(Etk_Box *box, Etk_Bool homogeneous);
    //~ Etk_Bool    etk_box_homogeneous_get(Etk_Box *box);
   
    
};


} // end namespace efl

#endif // EFLPP_ETKBOX_H
