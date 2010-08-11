#ifndef EINAXX_LIST_H
#define EINAXX_LIST_H

/* EFL */
#include <Eina.h>

namespace Einaxx {

template <typename T>
class List
{
public:
  class Iterator
  {
  public:
    Iterator (const Eina_List *list); // TODO: friend and private
      
    ~Iterator ();
  
    //T getContainer ();
      
    bool next (T *data);
      
/*

EAPI void *eina_iterator_container_get (Eina_Iterator *iterator);

EAPI void eina_iterator_foreach        (Eina_Iterator *iterator,
                                        Eina_Each_Cb callback,
                                        const void *fdata);

*/
      
      
  protected:
    Eina_Iterator *mIterator;
  };
  
  
  /*!
   *  Create a new List.
   */
  List ();
  
  ~List ();
  
  void append (const T data);
  
  void prepend (const T data);
  
  void appendRelative (const T data, const T relative);
  
  //void appendRelative (const T &data, ListIterator it);
  //Eina_List *eina_list_append_relative_list (Eina_List *list, const void *data, Eina_List *relative);
  
  void prependRelative (const T data, const T relative);

  //void prependRelativeList (const T &data, ListIterator it);
  //Eina_List *eina_list_prepend_relative_list (Eina_List *list, const void *data, Eina_List *relative);

  //Eina_List *eina_list_sorted_insert(Eina_List *list, Eina_Compare_Cb func, const void *data);
  
  void remove (const T data);

  //Eina_List *eina_list_remove_list (Eina_List *list, Eina_List *remove_list);
  
  /*
  Eina_List *eina_list_promote_list (Eina_List *list, Eina_List *move_list);
  Eina_List *eina_list_demote_list (Eina_List *list, Eina_List *move_list);*/
  
  // TODO: wrap or with stl::find
  //void *eina_list_data_find(const Eina_List *list, const void *data);
  
  //Eina_List *eina_list_data_find_list (const Eina_List *list, const void *data);

  T at (unsigned int n); // TODO: [] operator

  void reverse ();

  /*
  Eina_List *eina_list_reverse_clone(const Eina_List *list);
  Eina_List *eina_list_clone(const Eina_List *list);
  Eina_List *eina_list_sort (Eina_List *list, unsigned int size, Eina_Compare_Cb func);
  Eina_List *eina_list_merge (Eina_List *left, Eina_List *right);
  Eina_List *eina_list_sorted_merge(Eina_List *left, Eina_List *right, Eina_Compare_Cb func);
  Eina_List *eina_list_split_list(Eina_List *list, Eina_List *relative, Eina_List **right);

  Eina_List *eina_list_search_sorted_near_list(const Eina_List *list, Eina_Compare_Cb func, const void *data, int *result_cmp);
  Eina_List *eina_list_search_sorted_list(const Eina_List *list, Eina_Compare_Cb func, const void *data);
  void *eina_list_search_sorted(const Eina_List *list, Eina_Compare_Cb func, const void *data);
  Eina_List *eina_list_search_unsorted_list(const Eina_List *list, Eina_Compare_Cb func, const void *data);
  void *eina_list_search_unsorted(const Eina_List *list, Eina_Compare_Cb func, const void *data);

  static inline Eina_List *eina_list_last (const Eina_List *list);
  static inline Eina_List *eina_list_next (const Eina_List *list);
  static inline Eina_List *eina_list_prev (const Eina_List *list);
  */
  
  T getData (); // TODO: * operator

  unsigned int count ();

  Iterator createIterator ();

  //Eina_Accessor *eina_list_accessor_new(const Eina_List *list);
  
private:
  Eina_List *mList;
};

/** Implementation **/

/*************/
/* Iterator  */
/*************/

template <typename T>
List<T>::Iterator::Iterator (const Eina_List *list)
{
  mIterator = eina_list_iterator_new (list);

  // TODO: support reverse operator
  // with switch or with more classes...
  //Eina_Iterator *eina_list_iterator_reversed_new(const Eina_List *list);
}

template <typename T>
List<T>::Iterator::~Iterator ()
{
  eina_iterator_free (mIterator);
}

template <typename T>
bool List<T>::Iterator::next (T *data)
{
  return eina_iterator_next (mIterator, reinterpret_cast <void**> (data));
}


/*************/
/* List      */
/*************/

template <typename T>
List<T>::List () :
  mList (NULL)
{
}

template <typename T>
List<T>::~List ()
{
  /*Eina_List *list = eina_list_free (mList);
  if (!list)
  {
    // good case: do nothing
  }
  else
  {
    // TODO: error handling
  }*/
}

template <typename T>
void List<T>::append (const T data)
{
  Eina_List *list = eina_list_append (mList, static_cast <const void*> (data));
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
void List<T>::prepend (const T data)
{
  Eina_List *list = eina_list_prepend (mList, static_cast <const void*> (data));
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
void List<T>::appendRelative (const T data, const T relative)
{
  Eina_List *list = eina_list_append_relative (mList, static_cast <void*> (data), static_cast <void*> (relative));
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
void List<T>::prependRelative (const T data, const T relative)
{
  Eina_List *list = eina_list_prepend_relative (mList, static_cast <void*> (data), static_cast <void*> (relative));
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
void List<T>::remove (const T data)
{
  Eina_List *list = eina_list_remove (mList, static_cast <void*> (data));
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
void List<T>::reverse ()
{
  Eina_List *list =  eina_list_reverse (mList);
  if (list)
  {
    mList = list;
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
T List<T>::at (unsigned int n)
{
  void *data = eina_list_nth (mList, n);

  if (data)
  {
    return static_cast <T> (data);
  }
  else
  {
    // TODO: error handling
  }
}

template <typename T>
unsigned int List<T>::count ()
{
  return eina_list_count (mList);
}

template <typename T>
T List<T>::getData ()
{
  return static_cast <T> (eina_list_data_get (mList));
}

template <typename T>
typename List<T>::Iterator List<T>::createIterator ()
{
  Iterator i (mList);
  
  return i;
}

} // end namespace Einaxx

#endif // EINAXX_LIST_H
