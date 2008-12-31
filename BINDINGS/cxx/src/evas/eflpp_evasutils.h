#ifndef EVAS_UTILS_H
#define EVAS_UTILS_H

/* EFL++ */
#include <eflpp_sys.h>

/* EFL */
#include <Evas.h>

/**
 * C++ Wrapper for the Enlightenment Canvas Library (EVAS)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

class EvasObject;

template<typename T> class EvasList;
template<typename T> class EvasListIterator;
template<typename T> class EvasHash;
template<typename T> class EvasHashIterator;

typedef class EvasList<EvasObject> EvasObjectList;
typedef class EvasHash<EvasObject> EvasObjectHash;

/**
 * An Evas List Iterator
 */
template<typename T> class EvasListIterator
{
  public:
    EvasListIterator( const EvasList<T>& list ) : o( list.o ) {};
    EvasListIterator( Evas_List* list ) : o( list ) {};
    ~EvasListIterator() {};

    EvasListIterator<T>* next()
    {
        o = evas_list_next( o );
        return this;
    }
    EvasListIterator<T>* prev()
    {
        o = evas_list_prev( o );
        return this;
    }
    T* data() const
    {
        return static_cast<T*>( evas_list_data( o ) );
    }
    EvasListIterator<T>* operator++()
    {
        return next();
    }
    EvasListIterator<T>* operator--()
    {
        return prev();
    }
    T* operator*() const
    {
        return data();
    }
  private:
    Evas_List* o;
};

/**
 * An Evas List Wrapper
 */
template<typename T> class EvasList
{
  friend class EvasListIterator<T>;

  public:
    EvasList():o(0) {};
    ~EvasList() { evas_list_free( o ); };

    EvasList<T>* append( const T* element, const T* after = 0 )
    {
        if ( !after )
            o = evas_list_append( o, element );
        else
            o = evas_list_append_relative( o, element, after );
        return this;
    }

    EvasList<T>* prepend( const T* element, const T* before = 0 )
    {
        if ( !before )
            o = evas_list_prepend( o, element );
        else
            o = evas_list_prepend_relative( o, element, before );
        return this;
    }

    EvasList<T>* remove( const T* element )
    {
        o = evas_list_remove( o, element );
        return this;
    }

    T* find( const T* element ) const
    {
        return static_cast<T*>( evas_list_find( o, element ) );
    }
    bool contains( const T* element ) const
    {
        return static_cast<T*>( evas_list_find( o, element ) ) == element;
    }
    EvasListIterator<T> iter( const T* element ) const
    {
        return EvasListIterator<T>( evas_list_find_list( o, element ) );
    }
    int count() const
    {
        return evas_list_count( o );
    }
    T* nth( int n ) const
    {
        return static_cast<T*>( evas_list_nth( o, n ) );
    }
    T* operator[]( int n ) const
    {
        return nth( n );
    }
    // nth_list
    EvasList<T>* reverse()
    {
        o = evas_list_reverse( o );
        return this;
    }
    // sort
  private:
    Evas_List* o;
};

/**
 * An Evas Hash Wrapper
 */
template<typename T> class EvasHash
{
  friend class EvasHashIterator<T>;

  public:
    EvasHash():o(0) {};
    ~EvasHash() { evas_hash_free( o ); };

    EvasHash<T>* add( const char* key, const T* value )
    {
        o = evas_hash_add( o, key, value );
        return this;
    }

    EvasHash<T>* remove( const T* element )
    {
        o = evas_list_remove( o, element );
        return this;
    }

    EvasHash<T>* del( const char* key )
    {
        o = evas_hash_del( o, key, 0 ); //FIXME: Add support for deleting by data!?
    }

    int size() const
    {
        return evas_hash_size( o );
    }

    void foreach(Evas_Bool(*func)(Evas_Hash*, const char*, void *data, void *fdata)) const
    {
        evas_hash_foreach( o, func, 0 );
    }

    T* find( const char* key ) const
    {
        if ( !o )
            return 0;
        else
            return static_cast<T*>( evas_hash_find( o, key ) );
    }
    T* operator[]( const char* key ) const
    {
        return find( key );
    }

  private:
    Evas_Hash* o;
};

}

#endif
