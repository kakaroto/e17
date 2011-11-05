#ifndef ELMXX_GEN_ITEM_H
#define ELMXX_GEN_ITEM_H

/* EFL */
#include <Elementary.h>

/* EFLxx */
#include <evasxx/Evasxx.h>
#include "GenListColumnConstructor.h"

namespace Elmxx {

/* forward declarations */
class Gen;
class GenDataModel;

class GenItem
{
public:
  friend class Gen;
  friend class GenList;

  virtual ~GenItem ();
  
  void setSelected (bool selected);
  bool getSelected () const;

protected:
  GenItem ();
  Elm_Gen_Item *mItem;
  GenDataModel *mDataModel;

};

} // end namespace Elmxx

#endif // ELMXX_GEN_ITEM_H

