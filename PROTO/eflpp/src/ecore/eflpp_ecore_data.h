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
    /*!
     *  Create a new EcoreList.
     */
    EcoreList();
  
    /*!  
     *  Create a EcoreList from by using a existing Ecore_List C object.
     *  Notice that the destructor deletes the C object list.
     */
    EcoreList(Ecore_List *list);
  
    virtual ~EcoreList();

    /*!
     *  return The internal Ecore_List object
     */
    Ecore_List *obj();
  
    /* Adding items to the list */
    bool append( T *data );
    bool prepend( T *data );
    bool insert( T *data );
    bool appendList( EcoreList <T> *append);
    bool prependList( EcoreList <T> *prepend);

    /* Removing items from the list */
    // TODO: how to integrate this with destructors?
    //int ecore_list_remove_destroy(Ecore_List *list);
    T *remove();
    T *removeFirst();
    T *removeLast();

    /* Retrieve the current position in the list */
    T *current();
    T *first();
    T *last();
    int index();
    int count();

    /* Traversing the list */
    // not wrapped! -> std::for_each exist. -> #include <algorithm>
    // TODO: need to implement an iterator to get this working
    //EAPI int ecore_list_for_each(Ecore_List *list, Ecore_For_Each function,
	//			void *user_data);
    T *gotoFirst();
    T *gotoLast();
    T *gotoIndex(int index);
    T *gotoElement(const T *data);

    /* Traversing the list and returning data */
    T *next();
    // not wrapped! -> std::find* exist. -> #include <algorithm>
    // TODO: need to implement an iterator to get this working
    //EAPI void *ecore_list_find(Ecore_List *list, Ecore_Compare_Cb function,
    //    const void *user_data);

    /* Sorting the list */
    // TODO: it may help to implement operators <,>,== to wrap it
    /*EAPI int ecore_list_sort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);
    EAPI int ecore_list_mergesort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);
    EAPI int ecore_list_heapsort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);*/

    bool isEmpty();
    bool clear();
  
    /*class forward_iterator
    {
      public:
        forward_iterator ()
        {
            list = m_list;
        }
        
        // Prefix
        forward_iterator& operator++ ()
        {
            list = list->next ();
            return *this;          
        }
      
        // Postfix
        forward_iterator operator++ (int)
        {
            forward_iterator oldIterator =* this;
            list = list->next ();
            return oldIterator;
        }
      
      private:
        EcoreList<T> list;        

    };*/
    
    //forward_iterator begin ();
    //forward_iterator end ();

    private:
      Ecore_List *m_list;
};



template <typename T>
class EcoreDList
{
  public:
    /*!
     *  Create a new EcoreList.
     */
    EcoreDList();
  
    /*!
     *  Create a EcoreList from by using a existing Ecore_List C object.
     *  Notice that the destructor deletes the C object list.
     */
    EcoreDList(Ecore_DList *list);
  
    virtual ~EcoreDList();

    /*!
     *  return The internal Ecore_DList object
     */
    Ecore_DList *obj();
  
    /* Adding items to the list */
    bool append( T *data );
    bool prepend( T *data );
    bool insert( T *data );
    bool appendList( EcoreDList <T> *append);
    bool prependList( EcoreDList <T> *prepend);
    // not wrapped! -> std::for_each exist. -> #include <algorithm>
    // need to implement an iterator to get this working
    /* Removing items from the list */
    // TODO: how to integrate this with destructors?
    //int ecore_dlist_remove_destroy(Ecore_List *list);
    T *remove();
    T *removeFirst();
    T *removeLast();

    /* Retrieve the current position in the list */
    T *current();
    // TODO: Why no first and last in Ecore_DList?
    //T *first();
    //T *last();
    int index();
    int count();

    /* Traversing the list */
    // not wrapped! -> std::for_each exist. -> #include <algorithm>
    // need to implement an iterator to get this working
    //EAPI int ecore_dlist_for_each(Ecore_List *list, Ecore_For_Each function,
	//			void *user_data);
    T *gotoFirst();
    T *gotoLast();
    T *gotoIndex(int index);
    T *gotoElement(const T *data);


    /* Traversing the list and returning data */
    T *next();
    T *previous();
    // not wrapped! -> std::find* exist. -> #include <algorithm>
    // TODO: need to implement an iterator to get this working
    //EAPI void *ecore_dlist_find(Ecore_List *list, Ecore_Compare_Cb function,
    //    const void *user_data);

    /* Sorting the list */
    /*EAPI int ecore_dlist_sort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);
    EAPI int ecore_dlist_mergesort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);
    EAPI int ecore_dlist_heapsort(Ecore_List *list, Ecore_Compare_Cb compare,
                                   char order);*/

    bool isEmpty();
    bool clear();

  private:
    Ecore_DList *m_list;
};

/** Implementaion **/

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
EcoreList<T>::EcoreList(Ecore_List *list)
{
    m_list = list;
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
bool EcoreList<T>::append( T *data )
{
    return ecore_list_append( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::prepend( T *data )
{
   return ecore_list_prepend( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::insert( T *data )
{
   return ecore_list_insert( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreList<T>::appendList( EcoreList <T> *append)
{
    return ecore_list_append_list( m_list, append.m_list);
}

template <typename T>
bool EcoreList<T>::prependList( EcoreList <T> *prepend)
{
    return ecore_list_prepend_list( m_list, prepend.m_list);
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
T* EcoreList<T>::gotoLast()
{
    return static_cast <T*> ( ecore_list_last_goto( m_list ) );
}

template <typename T>
T *EcoreList<T>::gotoIndex(int index)
{
    return static_cast <T*> ( ecore_list_index_goto( m_list, index ) );
}

template <typename T>
T *EcoreList<T>::gotoElement(const T *data)
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

/*template <typename T>
forward_iterator EcoreList<T>::begin ()
{
  //forward_iterator fi;
  
}

template <typename T>
EcoreList<T>::forward_iterator EcoreList<T>::end ()
{
  
}*/


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
EcoreDList<T>::EcoreDList(Ecore_DList *list)
{
    m_list = list;
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
bool EcoreDList<T>::append( T *data )
{
    return ecore_dlist_append( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::prepend( T *data )
{
   return ecore_dlist_prepend( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::insert( T *data )
{
   return ecore_dlist_insert( m_list, static_cast <void*> (data) );
}

template <typename T>
bool EcoreDList<T>::appendList( EcoreDList <T> *append)
{
    return ecore_dlist_append_list( m_list, append.m_list);
}

template <typename T>
bool EcoreDList<T>::prependList( EcoreDList <T> *prepend)
{
    return ecore_dlist_prepend_list( m_list, prepend.m_list);
}

template <typename T>
T *EcoreDList<T>::remove()
{
    return static_cast <T*> (ecore_dlist_remove( m_list ) );
}

template <typename T>
T* EcoreDList<T>::removeFirst()
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
T EcoreDList<T>::first()
{
    return static_cast <T> ( ecore_dlist_first( m_list ) );
}

template <typename T>
T EcoreDList<T>::last()
{
    return static_cast <T> ( ecore_dlist_last( m_list) );
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
T *EcoreDList<T>::gotoElement(const T *data)
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

#endif // ECORE_BASE_H
