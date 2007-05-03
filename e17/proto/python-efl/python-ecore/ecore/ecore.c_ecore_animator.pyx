# This file is included verbatim by c_ecore.pyx

cdef class Animator:
    def __new__(self, func, args, kargs):
        self.func = func
        self.args = args
        self.kargs = kargs
        self.obj = NULL

    def __str__(self):
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, Ecore_Animator=0x%x, "
                "refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, python.REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_animator_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef int _set_obj(self, Ecore_Animator *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        python.Py_INCREF(self)
        return 1

    def _exec(self):
        return self.func(*self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_animator_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()


cdef int animator_cb(void *_td):
    cdef int r

    obj = <Animator>_td
    r = bool(obj._exec())
    if not r:
        obj.delete()
    return r


def animator_add(func, *args, **kargs):
    cdef Ecore_Animator *h
    cdef Animator obj

    if not callable(func):
        raise TypeError("Parameter 'func' must be callable")

    obj = Animator(func, args, kargs)

    h = ecore_animator_add(animator_cb, <void *>obj)
    obj._set_obj(h)
    return obj
