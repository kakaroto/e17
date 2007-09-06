#include "eflpp_ecore_data.h"

namespace efl {

/*************/
/* EcoreList */
/*************/

template <typename T>
EcoreList<T>::EcoreList()
{
    m_list = ecore_list_new();
    // todo: how to handle error return? -> Exception?
}
  
template <typename T>
EcoreList<T>::~EcoreList()
{
    ecore_list_destroy( m_list );
}

template <typename T>
Ecore_List *EcoreList<T>::obj()
{
    return m_list;
}

template <typename T>
bool EcoreList<T>::append( T &data )
{
    return ecore_list_append( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::prepend( T &data )
{
   return ecore_list_prepend( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::insert( T &data )
{
   return ecore_list_insert( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::appendList( EcoreList <T> &append)
{
    return ecore_list_append_list( m_list, append.m_list);
}

template <typename T>
bool EcoreList<T>::prependList( EcoreList <T> &prepend)
{
    return ecore_list_prepend_list( m_list, &prepend.m_list);
}

template <typename T>
T *EcoreList<T>::remove()
{
    return static_cast <T*> (ecore_list_remove( m_list ) );
}

template <typename T>
T *EcoreList<T>::removeFirst()
{
    return static_cast <T*> ( ecore_list_first_remove( m_list ) );
}

template <typename T>
T *EcoreList<T>::removeLast()
{
    return static_cast <T*> ( ecore_list_last_remove( m_list ) );
}

template <typename T>
T *EcoreList<T>::current()
{
    return static_cast <T*> ( ecore_list_current( m_list ) );
}

template <typename T>
T *EcoreList<T>::first()
{
    return static_cast <T*> ( ecore_list_first( m_list ) );
}

template <typename T>
T *EcoreList<T>::last()
{
    return static_cast <T*> ( ecore_list_last( m_list) );
}

template <typename T>
int EcoreList<T>::index()
{
    return ecore_list_index( m_list );
}

template <typename T>
int EcoreList<T>::count()
{
    return ecore_list_count( m_list );
}

template <typename T>
T *EcoreList<T>::gotoFirst()
{
    return static_cast <T*> ( ecore_list_first_goto( m_list ) );
}

template <typename T>
T *EcoreList<T>::gotoLast()
{
    return static_cast <T*> ( ecore_list_last_goto( m_list ) );
}

template <typename T>
T *EcoreList<T>::gotoIndex(int index)
{
    return static_cast <T*> ( ecore_list_index_goto( m_list, index ) );
}

template <typename T>
T *EcoreList<T>::gotoElement(const T &data)
{
    return static_cast <T*> ( ecore_list_goto( m_list, 
                             static_cast <const void*> ( data ) ));
}

template <typename T>
T *EcoreList<T>::next()
{
    return static_cast <T*> ( ecore_list_next( m_list ));
}

template <typename T>
bool EcoreList<T>::isEmpty()
{
    return ecore_list_empty_is ( m_list );
}

template <typename T>
bool EcoreList<T>::clear()
{
    return ecore_list_clear( m_list );
}

/**************/
/* EcoreDList */
/**************/

template <typename T>
EcoreDList<T>::EcoreDList()
{
    m_list = ecore_dlist_new();
    // todo: how to handle error return? -> Exception?
}
  
template <typename T>
EcoreDList<T>::~EcoreDList()
{
    ecore_dlist_destroy( m_list );
}

template <typename T>
Ecore_DList *EcoreDList<T>::obj()
{
    return m_list;
}

template <typename T>
bool EcoreDList<T>::append( T &data )
{
    return ecore_dlist_append( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::prepend( T &data )
{
   return ecore_dlist_prepend( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::insert( T &data )
{
   return ecore_dlist_insert( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::appendList( EcoreDList <T> &append)
{
    return ecore_dlist_append_list( m_list, append.m_list);
}

template <typename T>
bool EcoreDList<T>::prependList( EcoreDList <T> &prepend)
{
    return ecore_dlist_prepend_list( m_list, &prepend.m_list);
}

template <typename T>
T *EcoreDList<T>::remove()
{
    return static_cast <T*> (ecore_dlist_remove( m_list ) );
}

template <typename T>
T *EcoreDList<T>::removeFirst()
{
    return static_cast <T*> ( ecore_dlist_first_remove( m_list ) );
}

template <typename T>
T *EcoreDList<T>::removeLast()
{
    return static_cast <T*> ( ecore_dlist_last_remove( m_list ) );
}

template <typename T>
T *EcoreDList<T>::current()
{
    return static_cast <T*> ( ecore_dlist_current( m_list ) );
}

/*template <typename T>
T *EcoreDList<T>::first()
{
    return static_cast <T*> ( ecore_dlist_first( m_list ) );
}

template <typename T>
T *EcoreDList<T>::last()
{
    return static_cast <T*> ( ecore_dlist_last( m_list) );
}*/

template <typename T>
int EcoreDList<T>::index()
{
    return ecore_dlist_index( m_list );
}

template <typename T>
int EcoreDList<T>::count()
{
    return ecore_dlist_count( m_list );
}

template <typename T>
T *EcoreDList<T>::gotoFirst()
{
    return static_cast <T*> ( ecore_dlist_first_goto( m_list ) );
}

template <typename T>
T *EcoreDList<T>::gotoLast()
{
    return static_cast <T*> ( ecore_dlist_last_goto( m_list ) );
}

template <typename T>
T *EcoreDList<T>::gotoIndex(int index)
{
    return static_cast <T*> ( ecore_dlist_index_goto( m_list, index ) );
}

template <typename T>
T *EcoreDList<T>::gotoElement(const T &data)
{
    return static_cast <T*> ( ecore_dlist_goto( m_list, 
                             static_cast <const void*> ( data ) ));
}

template <typename T>
T *EcoreDList<T>::next()
{
    return static_cast <T*> ( ecore_dlist_next( m_list ));
}

template <typename T>
T *EcoreDList<T>::previous()
{
    return static_cast <T*> ( ecore_dlist_previous( m_list ));
}

template <typename T>
bool EcoreDList<T>::isEmpty()
{
    return ecore_dlist_empty_is ( m_list );
}

template <typename T>
bool EcoreDList<T>::clear()
{
    return ecore_dlist_clear( m_list );
}


} // end namespace efl
