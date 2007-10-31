cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Epsilon_Request.h":
    ctypedef struct Epsilon_Request:
        unsigned id
        unsigned size
        unsigned status
        char *path
        char *dest
        void *data

    int EPSILON_EVENT_DONE

    int epsilon_thumb_init()
    int epsilon_shutdown()
    Epsilon_Request *epsilon_add(char *path, char *dst, int size, void *data)
    void epsilon_del(Epsilon_Request *thumb)


cdef extern from "Ecore.h":
    ctypedef struct Ecore_Event_Handler
    Ecore_Event_Handler *ecore_event_handler_add(int type, int (*func) (void *data, int type, void *event), void *data)


cdef class Request:
    cdef Epsilon_Request *obj
