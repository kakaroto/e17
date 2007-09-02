# This file is included verbatim by c_ecore.pyx

cdef void fd_handler_prepare_cb(void *_td, Ecore_Fd_Handler *fdh):
    cdef FdHandler obj
    cdef int r

    obj = <FdHandler>_td
    if obj._prepare_callback is None:
        return
    func, args, kargs = obj._prepare_callback
    try:
        func(obj, *args, **kargs)
    except Exception, e:
        import traceback
        traceback.print_exc()


cdef flags2str(int value):
    flags = []
    if value & <int>ECORE_FD_READ:
        flags.append("READ")
    if value & <int>ECORE_FD_WRITE:
        flags.append("WRITE")
    if value & <int>ECORE_FD_ERROR:
        flags.append("ERROR")
    return ", ".join(flags)


cdef int fd_handler_cb(void *_td, Ecore_Fd_Handler *fdh):
    cdef FdHandler obj
    cdef int r

    obj = <FdHandler>_td

    try:
        r = bool(obj._exec())
    except Exception, e:
        import traceback
        traceback.print_exc()
        r = 0

    if not r:
        obj.delete()
    return r


cdef class FdHandler:
    def __init__(self, fd, int flags, func, *args, **kargs):
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        self.func = func
        self.args = args
        self.kargs = kargs
        self._prepare_callback = None
        if self.obj == NULL:
            if not isinstance(fd, (int, long)):
                try:
                    fd = fd.fileno()
                except AttributeError, e:
                    raise ValueError("fd must be integer or have fileno()")

            self.obj = ecore_main_fd_handler_add(fd,
                                                 <Ecore_Fd_Handler_Flags>flags,
                                                 fd_handler_cb, <void *>self,
                                                 NULL, NULL)
            python.Py_INCREF(self)

    def __str__(self):
        if self.obj == NULL:
            fd = None
            flags = ""
        else:
            fd = self.fd_get()
            flags = flags2str(self.active_get(7))
        return "%s(func=%s, args=%s, kargs=%s, fd=%s, flags=[%s])" % \
               (self.__class__.__name__, self.func, self.args, self.kargs,
                fd, flags)

    def __repr__(self):
        if self.obj == NULL:
            fd = None
            flags = ""
        else:
            fd = self.fd_get()
            flags = flags2str(self.active_get(7))
        return ("%s(0x%x, func=%s, args=%s, kargs=%s, fd=%s, flags=[%s], "
                "Ecore_Fd_Handler=0x%x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.func, self.args, self.kargs, fd, flags,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_main_fd_handler_del(self.obj)
            self.obj = NULL
        self.func = None
        self.args = None
        self.kargs = None

    cdef object _exec(self):
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


def fd_handler_add(fd, int flags, func, *args, **kargs):
    return FdHandler(fd, flags, func, *args, **kargs)
