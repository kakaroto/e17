# This file is included verbatim by c_ecore.pyx

cdef class IdleExiter:
    def __new__(self, func, args, kargs):
        self.func = func
        self.args = args
        self.kargs = kargs
        self.obj = NULL

    def __str__(self):
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, Ecore_Idle_Exiter=0x%x, "
                "refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, python.REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_idle_exiter_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef int _set_obj(self, Ecore_Idle_Exiter *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        python.Py_INCREF(self)
        return 1

    def _exec(self):
        return self.func(*self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_idle_exiter_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()


cdef int idle_exiter_cb(void *_td):
    cdef int r

    obj = <IdleExiter>_td
    r = bool(obj._exec())
    if not r:
        obj.delete()
    return r


def idle_exiter_add(func, *args, **kargs):
    cdef Ecore_Idle_Exiter *h
    cdef IdleExiter obj

    if not callable(func):
        raise TypeError("Parameter 'func' must be callable")

    obj = IdleExiter(func, args, kargs)

    h = ecore_idle_exiter_add(idle_exiter_cb, <void *>obj)
    obj._set_obj(h)
    return obj
