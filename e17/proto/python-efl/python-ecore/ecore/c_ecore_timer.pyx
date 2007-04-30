# This file is included verbatim by c_ecore.pyx

cdef class Timer:
    def __new__(self, double interval, func, args, kargs):
        self._interval = interval
        self.func = func
        self.args = args
        self.kargs = kargs
        self.obj = NULL

    def __str__(self):
        return ("%s(0x%x, interval=%f, func=%s, args=%s, kargs=%s, "
                "Ecore_Timer=0x%x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self, self._interval,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, python.REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_timer_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef int _set_obj(self, Ecore_Timer *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        python.Py_INCREF(self)
        return 1

    def _exec(self):
        return self.func(*self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_timer_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()

    def interval_set(self, double t):
        self._interval = t
        ecore_timer_interval_set(self.obj, t)

    def interval_get(self):
        return self._interval

    property interval:
        def __get__(self):
            return self._interval

        def __set__(self, double t):
            self._interval = t
            ecore_timer_interval_set(self.obj, t)


cdef int timer_cb(void *_td):
    cdef int r

    obj = <Timer>_td
    r = bool(obj._exec())
    if not r:
        obj.delete()
    return r


def timer_add(double t, func, *args, **kargs):
    cdef Ecore_Timer *h
    cdef Timer obj

    if not callable(func):
        raise TypeError("Parameter 'func' must be callable")

    obj = Timer(t, func, args, kargs)

    h = ecore_timer_add(t, timer_cb, <void *>obj)
    obj._set_obj(h)
    return obj
