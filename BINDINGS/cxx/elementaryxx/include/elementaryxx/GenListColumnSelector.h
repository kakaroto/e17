#ifndef ELMXX_GEN_LIST_COLUMN_SELECTOR
#define ELMXX_GEN_LIST_COLUMN_SELECTOR

/* STD */
#include <string>

namespace Elmxx {

/* forward declarations */
class GenList;
  
class GenListColumnSelector
{
public:
  friend class GenList;
  
private:
  GenList *mGenList;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_COLUMN_SELECTOR
