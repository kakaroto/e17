# This file is included verbatim by c_ecore.pyx

cdef int idler_cb(void *_td):
    cdef Idler obj
    cdef int r

    obj = <Idler>_td

    try:
        r = bool(obj._exec())
    except Exception, e:
        import traceback
        traceback.print_exc()
        r = 0

    if not r:
        obj.delete()
    return r


cdef class Idler:
    def __init__(self, func, *args, **kargs):
        self.func = func
        self.args = args
        self.kargs = kargs
        if self.obj == NULL:
            self.obj = ecore_idler_add(idler_cb, <void *>self)
            python.Py_INCREF(self)

    def __str__(self):
        return "%s(func=%s, args=%s, kargs=%s)" % \
               (self.__class__.__name__, self.func, self.args, self.kargs)

    def __repr__(self):
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, Ecore_Idler=0x%x, "
                "refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_idler_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef object _exec(self):
        return self.func(*self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_idler_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()


def idler_add(func, *args, **kargs):
    return Idler(func, *args, **kargs)
