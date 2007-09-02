# This file is included verbatim by c_ecore.pyx

cdef int animator_cb(void *_td):
    cdef Animator obj
    cdef int r

    obj = <Animator>_td

    try:
        r = bool(obj._exec())
    except Exception, e:
        import traceback
        traceback.print_exc()
        r = 0

    if not r:
        obj.delete()
    return r


cdef class Animator:
    def __init__(self, func, *args, **kargs):
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        self.func = func
        self.args = args
        self.kargs = kargs
        if self.obj == NULL:
            self.obj = ecore_animator_add(animator_cb, <void *>self)
            python.Py_INCREF(self)

    def __str__(self):
        return "%s(func=%s, args=%s, kargs=%s)" % \
               (self.__class__.__name__, self.func, self.args, self.kargs)

    def __repr__(self):
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, Ecore_Animator=0x%x, "
                "refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_animator_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef _exec(self):
        return self.func(*self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_animator_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()


def animator_add(func, *args, **kargs):
    return Animator(func, *args, **kargs)
