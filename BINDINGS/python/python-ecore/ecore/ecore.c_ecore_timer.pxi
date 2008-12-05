# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_ecore.pyx

import traceback

cdef int timer_cb(void *_td) with gil:
    cdef Timer obj
    cdef int r

    obj = <Timer>_td

    try:
        r = bool(obj._exec())
    except Exception, e:
        traceback.print_exc()
        r = 0

    if not r:
        obj.delete()
    return r


cdef class Timer:
    """Creates a timer to call the given function in the given period of time.

       This class represents a timer that will call the given B{func} every
       B{interval} seconds. The function will be passed any extra
       parameters given to constructor.

       When the timer B{func} is called, it must return a value of either
       True or False (remember that Python returns None if no value is
       explicitly returned and None evaluates to False). If it returns
       B{True}, it will be called again at the next interval, or if it returns
       B{False} it will be deleted automatically making any references/handles
       for it invalid.

       Timers should be stopped/deleted by means of L{delete()} or
       returning False from B{func}, otherwise they'll continue alive, even
       if the current python context delete it's reference to it.

       B{func} signature: C{func(*args, **kargs): bool}
    """
    def __init__(self, double interval, func, *args, **kargs):
        """Constructor.

           @parm: B{interval} interval in seconds (float).
           @parm: B{func} function to callback when timer expires.
        """
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        self._interval = interval
        self.func = func
        self.args = args
        self.kargs = kargs
        if self.obj == NULL:
            self.obj = ecore_timer_add(interval, timer_cb, <void *>self)
            if self.obj != NULL:
                python.Py_INCREF(self)

    def __str__(self):
        return "%s(interval=%f, func=%s, args=%s, kargs=%s)" % \
               (self.__class__.__name__, self._interval,
                self.func, self.args, self.kargs)

    def __repr__(self):
        return ("%s(%#x, interval=%f, func=%s, args=%s, kargs=%s, "
                "Ecore_Timer=%#x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self._interval, self.func, self.args, self.kargs,
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
        "Stop callback emission and free internal resources."
        if self.obj != NULL:
            ecore_timer_del(self.obj)
            self.obj = NULL
            python.Py_DECREF(self)

    def stop(self):
        "Alias for L{delete()}."
        self.delete()

    def interval_set(self, double t):
        "Change interval to call function."
        self._interval = t
        ecore_timer_interval_set(self.obj, t)

    def interval_get(self):
        "@rtype: float"
        return self._interval

    property interval:
        def __get__(self):
            return self._interval

        def __set__(self, double t):
            self._interval = t
            ecore_timer_interval_set(self.obj, t)


def timer_add(double t, func, *args, **kargs):
    """L{Timer} factory, for C-api compatibility.

       B{func} signature: C{func(*args, **kargs): bool}

       @parm: B{interval} interval in seconds (float).
       @parm: B{func} function to callback when timer expires.

       @rtype: L{Timer}
    """
    return Timer(t, func, *args, **kargs)
