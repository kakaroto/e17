/*
 * The following code example is taken from the book
 * The C++ Standard Library - A Tutorial and Reference
 * by Nicolai M. Josuttis, Addison-Wesley, 1999
 * © Copyright Nicolai M. Josuttis 1999
 */

#ifndef COUNTED_PTR_H
#define COUNTED_PTR_H

namespace Eflxx {

/* class for counted reference semantics
 * - deletes the object to which it refers when the last CountedPtr
 *   that refers to it is destroyed
 */
template <class T>
class CountedPtr {
  private:
    T* ptr;        // pointer to the value
    long* count;   // shared number of owners

  public:
    template <typename S> friend class CountedPtr;
    
    // initialize pointer with existing pointer
    // - requires that the pointer p is a return value of new
    explicit CountedPtr (T* p=0)
     : ptr(p), count(new long(1)) 
    {
    }

    // copy pointer (one more owner)
    CountedPtr (const CountedPtr<T>& p)
     : ptr(p.ptr), count(p.count) 
    {
        ++*count;
    }
    
    template <class S>
    CountedPtr (const CountedPtr<S>& p)
      : ptr(p.ptr), count(p.count) 
    {
        ++*count;
    }
    
    template <class S>
    static CountedPtr<T> cast_static (const CountedPtr<S>& p)
    {
        T *obj = static_cast <T*> (&(*p));
        
        return CountedPtr<T> (obj);
    }
    
    template <class S>
    static CountedPtr<T> cast_dynamic (const CountedPtr<S>& p)
    {
        T *obj = dynamic_cast <T*> (&(*p));
        
        return CountedPtr<T> (obj);
    }
    
    // destructor (delete value if this was the last owner)
    ~CountedPtr ()
    {
        dispose();
    }

    // assignment (unshare old and share new value)
    CountedPtr<T>& operator= (const CountedPtr<T>& p) 
    {
        if (this != &p) {
            dispose();
            ptr = p.ptr;
            count = p.count;
            ++*count;
        }
        return *this;
    }

    // access the value to which the pointer refers
    T& operator*() const
    {
        return *ptr;
    }
    T* operator->() const 
    {
        return ptr;
    }
    
  private:
    void dispose() 
    {
        if (--*count == 0) 
        {
             delete count;
             delete ptr;
        }
    }
    
};

} // end namespace elf

#endif /*COUNTED_PTR_H*/
