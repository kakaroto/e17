cdef extern from "Ecore.h":

    cdef struct Ecore_Timer
    cdef struct Ecore_Animator

    int ecore_init()
    int ecore_shutdown()

    void ecore_main_loop_iterate()
    void ecore_main_loop_begin()
    void ecore_main_loop_quit()

    double ecore_time_get()

    Ecore_Timer *ecore_timer_add(double t, int (*func) (void *data), void *data)
    void *ecore_timer_del(Ecore_Timer *timer)
    void ecore_timer_interval_set(Ecore_Timer *timer, double t)

    Ecore_Animator *ecore_animator_add(int (*func) (void *data), void *data)
    void *ecore_animator_del(Ecore_Animator *animator)
    void ecore_animator_frametime_set(double frametime)
    double ecore_animator_frametime_get()


cdef class Timer:
    cdef Ecore_Timer *obj
    cdef double _interval
    cdef object func
    cdef object args
    cdef object kargs

    cdef int _set_obj(self, Ecore_Timer *obj) except 0


cdef class Animator:
    cdef Ecore_Animator *obj
    cdef object func
    cdef object args
    cdef object kargs

    cdef int _set_obj(self, Ecore_Animator *obj) except 0

