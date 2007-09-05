#include "eflpp_ecore_data.h"

namespace efl {

template <typename T>
EcoreList<T>::EcoreList()
{
    m_list = ecore_list_new();
    if (m_list)
    {
        /*int status =*/ ecore_list_init( m_list );
        // todo: how to handle return value? -> Exception?
    }
}
  
template <typename T>
EcoreList<T>::~EcoreList()
{
    ecore_list_destroy( m_list );
}

template <typename T>
int EcoreList<T>::append( T &data )
{
    return ecore_list_append( m_list, static_cast <void*> (data) );
}

template <typename T>
int EcoreList<T>::prepend( T &data )
{
   return ecore_list_prepend( m_list, static_cast <void*> (data) );
}

template <typename T>
int EcoreList<T>::insert( T &data )
{
   return ecore_list_insert( m_list, static_cast <void*> (data) );
}

} // end namespace efl
