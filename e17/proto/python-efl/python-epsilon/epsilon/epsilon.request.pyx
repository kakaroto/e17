cimport evas.python as python
import traceback

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def shutdown():
    epsilon_shutdown()

def init():
    epsilon_thumb_init()

EPSILON_THUMB_NORMAL = 0
EPSILON_THUMB_LARGE = 1

cdef class Request:
    """Request epsilon_thumbd the thumbnail for the given image at given size.

    This is an asynchronous request to the thumbnailer, that will take care
    of checking if thumbnail needs to be generated and if so, generate it.

    After it's processed, it will be dispatched using callback set with
    L{event_done_handler_set()} and then B{deleted} (it will become shallow
    and no attribute will work, so don't store it).

    The thumbnailer daemon 'epsilon_thumbd' is automatically started.
    """
    def __init__(self, char *path, dest=None, int size=EPSILON_THUMB_NORMAL):
        """Epsilon Request constructor.

        @parm path: file to process.
        @path dest: if provided (not None), specify where to store the thumb.
        @path size: EPSILON_THUMB_NORMAL or EPSILON_THUMB_LARGE
        """
        cdef char *d
        if self.obj == NULL:
            if dest is not None:
                d = dest
            else:
                d = NULL
            self.obj = epsilon_add(path, d, size, <void*>self)
            if self.obj != NULL:
                python.Py_INCREF(self)

    def __str__(self):
        return "%s(path=%r, dest=%r, size=%r, request_id=%r, status=%r)" % \
               (self.__class__.__name__, self.path, self.dest, self.size,
                self.request_id, self.status)

    def __repr__(self):
        return ("%s(0x%x, path=%r, dest=%r, size=%r, request_id=%r, status=%r, "
                "Epsilon_Request=%#x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.path, self.dest, self.size, self.request_id, self.status,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            epsilon_del(self.obj)
            self.obj = NULL

    def delete(self):
        "Stop thumbnail request, free resources."
        if self.obj != NULL:
            epsilon_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        "Alias for L{delete()}."
        self.delete()

    property path:
        def __get__(self):
            if self.obj != NULL and self.obj.path:
                return self.obj.path

    property dest:
        def __get__(self):
            if self.obj != NULL and self.obj.dest:
                return self.obj.dest

    property size:
        def __get__(self):
            if self.obj != NULL:
                return self.obj.size

    property request_id:
        def __get__(self):
            if self.obj != NULL:
                return self.obj.id

    property status:
        def __get__(self):
            if self.obj != NULL:
                return self.obj.status


init()
cdef object _epsilon_handler_py = None
cdef Ecore_Event_Handler *_epsilon_event_handler = NULL

cdef int _epsilon_event_handler_cb(void *data, int type, void *event) with GIL:
    cdef Epsilon_Request *er
    cdef Request obj
    global _epsilon_handler_py

    er = <Epsilon_Request *>event
    obj = <Request>er.data # this may bring problems if Epsilon is used
                           # by other non-python lib in this same process.

    if _epsilon_handler_py is not None:
        func, args, kargs = _epsilon_handler_py
        try:
            r = func(obj, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
            r = False

        if not r:
            _epsilon_handler_py = None

    obj.delete()
    return 1

def event_done_handler_set(func, *args, **kargs):
    """Set callback to call when requests are processed.

    Signature: C{function(request, *args, **kargs)}
    """
    global _epsilon_handler_py
    if func is None:
        _epsilon_handler_py = None
    elif callable(func):
        _epsilon_handler_py = (func, args, kargs)
    else:
        raise TypeError("Parameter 'func' must be callable")

def event_done_handler_unset():
    "Do not deliver requests anymore."
    _epsilon_handler_py = None


# Call after init()
# Always execute to clean up generated objects
_epsilon_event_handler = ecore_event_handler_add(EPSILON_EVENT_DONE, _epsilon_event_handler_cb, NULL)
