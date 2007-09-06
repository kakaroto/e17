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

    /*!
     *  return The internal Ecore_List object
     */
    Ecore_List *obj();
  
    /* Adding items to the list */
    bool append( T &data );
    bool prepend( T &data );
    bool insert( T &data );
    bool appendList( EcoreList <T> &append);
    bool prependList( EcoreList <T> &prepend);

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
    T *gotoElement(const T &data);

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

  private:
    Ecore_List *m_list;
};


template <typename T>
class EcoreDList
{
  public:
    EcoreDList();
    ~EcoreDList();

    /*!
     *  return The internal Ecore_DList object
     */
    Ecore_DList *obj();
  
    /* Adding items to the list */
    bool append( T &data );
    bool prepend( T &data );
    bool insert( T &data );
    bool appendList( EcoreDList <T> &append);
    bool prependList( EcoreDList <T> &prepend);
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
    T *gotoElement(const T &data);


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

} // end namespace efl

#endif // ECORE_BASE_H
