#ifndef LOCAL_UTIL_H
#define LOCAL_UTIL_H

/* STL */
#include <map>

namespace Eflxx {

// some generic template functions for delete algorithms
template <typename T>
void delete_one (T *t)
{
  delete t;
  t = NULL;
}

template <typename T>
void delete_array (T *t)
{
  delete [] t;
  t = NULL;
}

template <typename T>
void delete_stl_container (T &cnt)
{
  for (typename T::iterator it = cnt.begin(); it != cnt.end(); ++it)
  {
    delete *it;
  }
  cnt.clear();
}

template <typename Key, typename T>
void delete_stl_container (std::map<Key, T> &cnt)
{
  for (typename std::map<Key, T>::iterator it = cnt.begin(); it != cnt.end(); ++it)
  {
    delete it->second;
  }
  cnt.clear ();
} 

} // end namespace Eflxx

#endif // LOCAL_UTIL_H
