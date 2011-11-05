#ifndef ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR
#define ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR

/* STD */
#include <string>

namespace Elmxx {

/* forward declarations */
class GenList;
class GenDataModel;
class GenItem;
  
class GenListColumnConstructor
{
public:
  friend class GenList;
  friend class GenDataModel;

  GenListColumnConstructor ();
  
private:
  GenDataModel *mDataModel;
  GenItem *mGenItem;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR
