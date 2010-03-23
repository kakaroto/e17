# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
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

cdef object _event_type_mapping = dict()

def _event_mapping_register(int type, cls):
    "Register the ecore event type with the given event class."
    if type in _event_type_mapping:
        raise ValueError("event type '%d' already registered." % type)
    if not issubclass(cls, Event):
        raise TypeError("cls (%s) must be subclass of Event" % cls)
    _event_type_mapping[type] = cls


def _event_mapping_unregister(int type):
    "Forget about previous registration of this event type."
    _event_type_mapping.pop(type)


cdef int event_handler_cb(void *data, int type, void *event) with gil:
    cdef EventHandler handler
    cdef int r

    assert event != NULL
    assert data != NULL
    handler = <EventHandler>data
    assert type == handler.type

    try:
        r = handler._exec(event)
    except Exception, e:
        traceback.print_exc()
        r = 0

    if not r:
        handler.delete()
    return r


cdef public class Event [object PyEcoreEvent, type PyEcoreEvent_Type]:
    def __init__(self):
        if type(self) is Event:
            raise TypeError("Must not instantiate Event, but subclasses")

    def __str__(self):
        attrs = []
        for attr in dir(self):
            if not attr.startswith("_"):
                attrs.append("%s=%r" % (attr, getattr(self, attr)))
        return "%s(%s)" % (self.__class__.__name__, ", ".join(attrs))

    cdef int _set_obj(self, void *obj) except 0:
        raise NotImplementedError("Event._set_obj() not implemented.")


cdef class EventHandler:
    """Creates an event handler for ecore events.

       This class represents an event handler that will call the given B{func}
       when the event of given B{type} happens. The function will be passed
       any extra parameters given to constructor.

       When the handler B{func} is called, it must return a value of either
       True or False (remember that Python returns None if no value is
       explicitly returned and None evaluates to False). If it returns
       B{True}, it will be called again for the next event, or if it returns
       B{False} it will be deleted automatically making any references/handles
       for it invalid.

       Handlers should be stopped/deleted by means of L{delete()} or
       returning False from B{func}, otherwise they'll continue alive, even
       if the current python context delete it's reference to it.
    """

    def __init__(self, int type, func, *args, **kargs):
        "@parm: B{type} event type, as registered with ecore_event_type_new()."
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        event_cls = _event_type_mapping.get(type, None)
        if event_cls is None:
            raise ValueError("Unknow Ecore_Event type %d" % type)
        self.type = type
        self.event_cls = event_cls
        self.func = func
        self.args = args
        self.kargs = kargs
        self._set_obj(ecore_event_handler_add(type, event_handler_cb,
                                              <void *>self))

    def __str__(self):
        return "%s(type=%d, func=%s, args=%s, kargs=%s, event_cls=%s)" % \
               (self.__class__.__name__, self.type,
                self.func, self.args, self.kargs, self.event_cls)

    def __repr__(self):
        return ("%s(%#x, type=%d, func=%s, args=%s, kargs=%s, event_cls=%s, "
                "Ecore_Event_Handler=%#x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self.type, self.func, self.args, self.kargs, self.event_cls,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            ecore_event_handler_del(self.obj)

    cdef int _set_obj(self, Ecore_Event_Handler *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        assert obj != NULL, "Cannot set NULL as object"
        self.obj = obj
        Py_INCREF(self)
        return 1

    cdef int _unset_obj(self) except 0:
        if self.obj != NULL:
            ecore_event_handler_del(self.obj)
            self.obj = NULL
            self.event_cls = None
            self.func = None
            self.args = None
            self.kargs = None
            Py_DECREF(self)
        return 1

    cdef int _exec(self, void *event) except 2:
        cdef Event e
        e = self.event_cls()
        e._set_obj(event)
        return bool(self.func(e, *self.args, **self.kargs))

    def delete(self):
        "Stop handling events"
        if self.obj != NULL:
            self._unset_obj()

    def stop(self):
        "Alias for L{delete()}"
        self.delete()


def event_handler_add(int type, func, *args, **kargs):
    """L{EventHandler} factory, for C-api compatibility.

       @rtype: L{EventHandler}
    """
    return EventHandler(type, func, *args, **kargs)


cdef class EventSignalUser(Event):
    """Represents SIGUSR1 or SIGUSR2.

    See property C{number} to detect if it was SIGUSR1 or SIGUSR2

    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Signal_User *obj
        obj = <Ecore_Event_Signal_User*>o
        self.number = obj.number
        return 1

    def __str__(self):
        return "%s(number=%d)" % (self.__class__.__name__, self.number)

    def __repr__(self):
        return "%s(number=%d)" % (self.__class__.__name__, self.number)


cdef class EventSignalUser1(EventSignalUser):
    """Represents SIGUSR1."""
    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


cdef class EventSignalUser2(EventSignalUser):
    """Represents SIGUSR2."""
    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


cdef class EventHandlerSignalUser(EventHandler):
    """Specialized event handler that creates specialized event instances.

    Unlike EventHandler, this class will create specialized event of
    classes L{EventSignalUser1} or L{EventSignalUser2}
    instead of generic L{EventSignalUser}.
    """
    def __init__(self, func, *args, **kargs):
        EventHandler.__init__(self, ECORE_EVENT_SIGNAL_USER,
                              func, *args, **kargs)

    cdef int _exec(self, void *event) except 2:
        cdef Ecore_Event_Signal_User *obj = <Ecore_Event_Signal_User *>event
        cdef EventSignalUser e
        if obj.number == 1:
            e = EventSignalUser1()
        elif obj.number == 2:
            e = EventSignalUser2()
        else:
            e = EventSignalUser()
        e._set_obj(event)
        return bool(self.func(e, *self.args, **self.kargs))


def on_signal_user(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_SIGNAL_USER

       @see: L{EventHandler}
       @see: L{EventHandlerSignalUser}
    """
    return EventHandlerSignalUser(func, *args, **kargs)


cdef class EventSignalHup(Event):
    """"Represents SIGHUP."""
    cdef int _set_obj(self, void *o) except 0:
        return 1

    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


def on_signal_hup(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_SIGNAL_HUP

       @see: L{EventHandler}
    """
    return EventHandler(ECORE_EVENT_SIGNAL_HUP, func, *args, **kargs)


cdef class EventSignalExit(Event):
    """"Represents exit request signals: SIGINT, SIGQUIT and SIGTERM.

    See properties C{interrupt}, C{quit} and C{terminate} to detect
    which signal happened.
    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Signal_Exit *obj
        obj = <Ecore_Event_Signal_Exit*>o
        self.interrupt = bool(obj.interrupt)
        self.quit = bool(obj.quit)
        self.terminate = bool(obj.terminate)
        return 1

    def __str__(self):
        return "%s(interrupt=%s, quit=%s, terminate=%s)" % \
            (self.__class__.__name__, self.interrupt, self.quit, self.terminate)

    def __repr__(self):
        return "%s(interrupt=%s, quit=%s, terminate=%s)" % \
            (self.__class__.__name__, self.interrupt, self.quit, self.terminate)


cdef class EventSignalQuit(EventSignalExit):
    """Represents SIGQUIT."""
    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


cdef class EventSignalInterrupt(EventSignalExit):
    """Represents SIGINT."""
    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


cdef class EventSignalTerminate(EventSignalExit):
    """Represents SIGTERM."""
    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


cdef class EventHandlerSignalExit(EventHandler):
    """Specialized event handler that creates specialized event instances.

    Unlike EventHandler, this class will create specialized event of
    classes L{EventSignalQuit}, L{EventSignalInterrupt} or
    L{EventSignalTerminate} instead of generic L{EventSignalExit}.
    """
    def __init__(self, func, *args, **kargs):
        EventHandler.__init__(self, ECORE_EVENT_SIGNAL_EXIT,
                              func, *args, **kargs)

    cdef int _exec(self, void *event) except 2:
        cdef Ecore_Event_Signal_Exit *obj = <Ecore_Event_Signal_Exit *>event
        cdef EventSignalUser e
        if obj.terminate:
            e = EventSignalTerminate()
        elif obj.interrupt:
            e = EventSignalInterrupt()
        elif obj.quit:
            e = EventSignalQuit()
        else:
            e = EventSignalExit()
        e._set_obj(event)
        return bool(self.func(e, *self.args, **self.kargs))


def on_signal_exit(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_SIGNAL_EXIT

       @see: L{EventHandler}
       @see: L{EventHandlerSignalExit}
    """
    return EventHandlerSignalExit(func, *args, **kargs)


cdef class EventSignalPower(Event):
    """"Represents SIGPWR."""
    cdef int _set_obj(self, void *o) except 0:
        return 1

    def __str__(self):
        return "%s()" % (self.__class__.__name__,)

    def __repr__(self):
        return "%s()" % (self.__class__.__name__,)


def on_signal_power(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_SIGNAL_POWER

       @see: L{EventHandler}
    """
    return EventHandler(ECORE_EVENT_SIGNAL_POWER, func, *args, **kargs)


cdef class EventSignalRealtime(Event):
    """"Represents SIGRTMIN+N.

    See property C{num} for the exact number that happened.
    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Signal_Realtime *obj
        obj = <Ecore_Event_Signal_Realtime*>o
        self.num = obj.num
        return 1

    def __str__(self):
        return "%s(num=%d)" % (self.__class__.__name__, self.num)

    def __repr__(self):
        return "%s(num=%d)" % (self.__class__.__name__, self.num)


def on_signal_realtime(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_SIGNAL_REALTIME

       @see: L{EventHandler}
    """
    return EventHandler(ECORE_EVENT_SIGNAL_REALTIME, func, *args, **kargs)


cdef class CustomEvent(Event):
    cdef int _set_obj(self, void *obj):
        self.obj = <object>obj
        return 1

def event_type_new(cls):
    cdef int type

    type = ecore_event_type_new()

    _event_mapping_register(type, cls)

    return type


cdef void _event_free_cb(void *data, void *event) with gil:
    cdef QueuedEvent ev

    ev = <QueuedEvent>data
    ev._free()

cdef class QueuedEvent:
    def __init__(self, int type, *args):
        self.args = args
        self._set_obj(ecore_event_add(type, <void *>self.args, _event_free_cb,
                      <void*>self))

    cdef int _set_obj(self, Ecore_Event *ev):
        assert self.obj == NULL, "Object must be clean"
        assert ev != NULL, "Cannot set NULL as object"
        self.obj = ev
        Py_INCREF(self)
        return 1

    cdef int _unset_obj(self):
        if self.obj != NULL:
            self.obj = NULL
            self.args = None
            Py_DECREF(self)
        return 1

    def _free(self):
        self._unset_obj()

    def delete(self):
        ecore_event_del(self.obj)

def event_add(int type, *args):
    return QueuedEvent(type, args)
