#ifndef ECORE_DATA_H
#define ECORE_DATA_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <Ecore_Data.h>

namespace efl {

template <typename T>
class EcoreList
{
  public:
    EcoreList();
    ~EcoreList();
  
    /* Adding items to the list */
    int append( T &data );
    int prepend( T &data );
    int insert( T &data );

  private:
    Ecore_List *m_list;
};

} // end namespace efl

#endif // ECORE_BASE_H
