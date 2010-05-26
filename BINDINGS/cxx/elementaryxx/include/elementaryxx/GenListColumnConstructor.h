#ifndef ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR
#define ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR

/* STD */
#include <string>

namespace Elmxx {

/* forward declarations */
class GenList;
class GenListDataModel;
  
class GenListColumnConstructor
{
public:
  friend class GenList;
  friend class GenListDataModel;
  
private:
  GenListDataModel *mDataModel;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_COLUMN_CONSTRUCTOR
