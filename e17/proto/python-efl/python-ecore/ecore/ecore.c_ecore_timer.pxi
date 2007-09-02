# This file is included verbatim by c_ecore.pyx

cdef int timer_cb(void *_td):
    cdef Timer obj
    cdef int r

    obj = <Timer>_td

    try:
        r = bool(obj._exec())
    except Exception, e:
        import traceback
        traceback.print_exc()
        r = 0

    if not r:
        obj.delete()
    return r


cdef class Timer:
    def __init__(self, double interval, func, *args, **kargs):
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        self._interval = interval
        self.func = func
        self.args = args
        self.kargs = kargs
        if self.obj == NULL:
            self.obj = ecore_timer_add(interval, timer_cb, <void *>self)
            python.Py_INCREF(self)

    def __str__(self):
        return "%s(interval=%f, func=%s, args=%s, kargs=%s)" % \
               (self.__class__.__name__, self._interval,
                self.func, self.args, self.kargs)

    def __repr__(self):
        return ("%s(0x%x, interval=%f, func=%s, args=%s, kargs=%s, "
                "Ecore_Timer=0x%x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self, self._interval,
                self.func, self.args, self.kargs,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_timer_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef object _exec(self):
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


def timer_add(double t, func, *args, **kargs):
    return Timer(t, func, *args, **kargs)
