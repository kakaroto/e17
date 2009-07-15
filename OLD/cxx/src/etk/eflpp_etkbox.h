#ifndef EFLPP_ETKBOX_H
#define EFLPP_ETKBOX_H

/* STL */
#include <string>

/* EFL */
#include <etk/etk_box.h>

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
    
    void prepend (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fillPolicy, int padding);
   
    void append (EtkWidget &child, Etk_Box_Group group, Etk_Box_Fill_Policy fillPolicy, int padding);
   
    void insert (EtkWidget &child, Etk_Box_Group group, EtkWidget &after, Etk_Box_Fill_Policy fillPolicy, int padding);

    void insertAt (EtkWidget &child, Etk_Box_Group group, int pos, Etk_Box_Fill_Policy fillPolicy, int padding);

    //EtkWidget *getChildAt (Etk_Box_Group group, int pos); // TODO: CountedPtr
    
    void setChildPosition (EtkWidget &child, Etk_Box_Group group, int pos);
    
    bool getChildPosition (EtkWidget &child, Etk_Box_Group &outGroup, int &outPos);
    
    void setChildPacking (EtkWidget &child, Etk_Box_Fill_Policy fillPolicy, int padding);
    
    bool getChildPacking (EtkWidget &child, Etk_Box_Fill_Policy &outFillPolicy, int &outPadding);

    void setSpacing (int spacing);
    
    int getSpacing ();
    
    void setHomogeneous (bool homogeneous);
    
    bool getHomogeneous (); 
};

} // end namespace efl

#endif // EFLPP_ETKBOX_H
