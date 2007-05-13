# This file is included verbatim by c_ecore.pyx

cdef void fd_handler_prepare_cb(void *_td, Ecore_Fd_Handler *fdh):
    cdef int r
    cdef FdHandler obj

    obj = <FdHandler>_td
    if obj._prepare_callback is None:
        return
    func, args, kargs = obj._prepare_callback
    func(obj, *args, **kargs)


cdef class FdHandler:
    def __new__(self, func, args, kargs):
        self.func = func
        self.args = args
        self.kargs = kargs
        self.obj = NULL
        self._prepare_callback = None

    def __str__(self):
        if self.obj == NULL:
            fd = None
            flags = ""
        else:
            fd = self.fd_get()
            v = self.active_get(7)
            flags = []
            if v & ECORE_FD_READ:
                flags.append("READ")
            if v & ECORE_FD_WRITE:
                flags.append("WRITE")
            if v & ECORE_FD_ERROR:
                flags.append("ERROR")
            flags = ", ".join(flags)
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, fd=%s, flags=[%s], "
                "Ecore_Fd_Handler=0x%x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs, fd, flags,
                <unsigned long>self.obj, python.REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_main_fd_handler_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef int _set_obj(self, Ecore_Fd_Handler *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        python.Py_INCREF(self)
        return 1

    def _exec(self):
        return self.func(self, *self.args, **self.kargs)

    def delete(self):
        if self.obj != NULL:
            ecore_main_fd_handler_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        self.delete()

    def fd_get(self):
        return ecore_main_fd_handler_fd_get(self.obj)

    property fd:
        def __get__(self):
            return self.fd_get()

    def active_get(self, int flags_query):
        cdef Ecore_Fd_Handler_Flags v
        v = <Ecore_Fd_Handler_Flags>flags_query
        return ecore_main_fd_handler_active_get(self.obj, v)


    def active_set(self, int flags):
        cdef Ecore_Fd_Handler_Flags v
        v = <Ecore_Fd_Handler_Flags>flags_query
        ecore_main_fd_handler_active_set(self.obj, v)

    def can_read(self):
        return ecore_main_fd_handler_active_get(self.obj, ECORE_FD_READ)

    def can_write(self):
        return ecore_main_fd_handler_active_get(self.obj, ECORE_FD_WRITE)

    def has_error(self):
        return ecore_main_fd_handler_active_get(self.obj, ECORE_FD_ERROR)

    def prepare_callback_set(self, func, *args, **kargs):
        if func is None:
            self._prepare_callback = None
            ecore_main_fd_handler_prepare_callback_set(self.obj, NULL, NULL)
        elif callable(func):
            self._prepare_callback = (func, args, kargs)
            ecore_main_fd_handler_prepare_callback_set(self.obj,
                                                       fd_handler_prepare_cb,
                                                       <void *>self)
        else:
            raise TypeError("Parameter 'func' must be callable")


cdef int fd_handler_cb(void *_td, Ecore_Fd_Handler *fdh):
    cdef int r

    obj = <FdHandler>_td
    r = bool(obj._exec())
    if not r:
        obj.delete()
    return r


def fd_handler_add(int fd, int flags, func, *args, **kargs):
    cdef Ecore_Fd_Handler *h
    cdef Ecore_Fd_Handler_Flags v
    cdef FdHandler obj

    if not callable(func):
        raise TypeError("Parameter 'func' must be callable")

    obj = FdHandler(func, args, kargs)

    v = <Ecore_Fd_Handler_Flags>flags
    h = ecore_main_fd_handler_add(fd, v, fd_handler_cb, <void *>obj,
                                  NULL, NULL)
    obj._set_obj(h)
    return obj
