# This file is included verbatim by c_evas.pyx

cdef class EventPoint:
    cdef void _set_obj(self, Evas_Point *obj):
        self.obj = obj

    def __str__(self):
        return "%s(%d, %d)" % (self.__class__.__name__, self.obj.x, self.obj.y)

    property x:
        def __get__(self):
            return self.obj.x

    property y:
        def __get__(self):
            return self.obj.y

    property xy:
        def __get__(self):
            return (self.obj.x, self.obj.y)

    def __len__(self):
        return 2

    def __getitem__(self, int index):
        if index == 0:
            return self.obj.x
        elif index == 1:
            return self.obj.y
        else:
            raise IndexError("list index out of range")



cdef class EventPosition:
    cdef void _set_objs(self, void *output, void *canvas):
        cdef EventPoint o, c
        o = <EventPoint>EventPoint()
        o._set_obj(<Evas_Point*>output)
        self._output = o
        c = <EventPoint>EventPoint()
        c._set_obj(<Evas_Point*>canvas)
        self._canvas = c

    def __str__(self):
        return "%s(output=(%d, %d), canvas=(%d, %d))" % \
               (self.__class__.__name__, self._output.x, self._output.y,
                self._canvas.x, self._canvas.y)

    property output:
        def __get__(self):
            return self._output

    property canvas:
        def __get__(self):
            return self._canvas



cdef class EventMouseIn:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_In *obj
        obj = <Evas_Event_Mouse_In*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.obj.buttons,
                 self.obj.output.x, self.obj.output.y,
                 self.obj.canvas.x, self.obj.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property buttons:
        def __get__(self):
            return self.obj.buttons

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventMouseOut:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Out *obj
        obj = <Evas_Event_Mouse_Out*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.obj.buttons,
                 self.obj.output.x, self.obj.output.y,
                 self.obj.canvas.x, self.obj.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property buttons:
        def __get__(self):
            return self.obj.buttons

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventMouseDown:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Down *obj
        obj = <Evas_Event_Mouse_Down*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(button=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.obj.button,
                 self.obj.output.x, self.obj.output.y,
                 self.obj.canvas.x, self.obj.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property button:
        def __get__(self):
            return self.obj.button

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventMouseUp:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Up *obj
        obj = <Evas_Event_Mouse_Up*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(button=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.obj.button,
                 self.obj.output.x, self.obj.output.y,
                 self.obj.canvas.x, self.obj.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property button:
        def __get__(self):
            return self.obj.button

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventMouseMove:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Move *obj
        obj = <Evas_Event_Mouse_Move*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.cur.output, &obj.cur.canvas)
        self._prev = <EventPosition>EventPosition()
        self._prev._set_objs(&obj.prev.output, &obj.prev.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "prev_output=(%d, %d), prev_canvas=(%d, %d), timestamp=%d)") %\
                (self.__class__.__name__, self.obj.buttons,
                 self.obj.cur.output.x, self.obj.cur.output.y,
                 self.obj.cur.canvas.x, self.obj.cur.canvas.y,
                 self.obj.prev.output.x, self.obj.prev.output.y,
                 self.obj.prev.canvas.x, self.obj.prev.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property prev_position:
        def __get__(self):
            return self._prev

    property buttons:
        def __get__(self):
            return self.obj.buttons

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventMouseWheel:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Wheel *obj
        obj = <Evas_Event_Mouse_Wheel*>ptr
        self.obj = obj
        self._pos = <EventPosition>EventPosition()
        self._pos._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(direction=%d, z=%d, output=(%d, %d), "
                "canvas=(%d, %d), timestamp=%d)") % \
                (self.__class__.__name__, self.obj.direction, self.obj.z,
                 self.obj.output.x, self.obj.output.y,
                 self.obj.canvas.x, self.obj.canvas.y,
                 self.obj.timestamp)

    property position:
        def __get__(self):
            return self._pos

    property timestamp:
        def __get__(self):
            return self.obj.timestamp

    property direction:
        def __get__(self):
            return self.obj.direction

    property z:
        def __get__(self):
            return self.obj.z


cdef class EventKeyDown:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Key_Down *obj
        obj = <Evas_Event_Key_Down*>ptr
        self.obj = obj

    def __str__(self):
        return ("%s(keyname=%r, key=%r, string=%r, compose=%r, "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.keyname,
                 self.key, self.string, self.compose,
                 self.obj.timestamp)

    property keyname:
        def __get__(self):
            if self.obj.keyname == NULL:
                return None
            else:
                return self.obj.keyname

    property key:
        def __get__(self):
            if self.obj.key == NULL:
                return None
            else:
                return self.obj.key

    property string:
        def __get__(self):
            if self.obj.string == NULL:
                return None
            else:
                return self.obj.string

    property compose:
        def __get__(self):
            if self.obj.compose == NULL:
                return None
            else:
                return self.obj.compose

    property timestamp:
        def __get__(self):
            return self.obj.timestamp


cdef class EventKeyUp:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Key_Up *obj
        obj = <Evas_Event_Key_Up*>ptr
        self.obj = obj

    def __str__(self):
        return ("%s(keyname=%r, key=%r, string=%r, compose=%r, "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.keyname,
                 self.key, self.string, self.compose,
                 self.obj.timestamp)

    property keyname:
        def __get__(self):
            if self.obj.keyname == NULL:
                return None
            else:
                return self.obj.keyname

    property key:
        def __get__(self):
            if self.obj.key == NULL:
                return None
            else:
                return self.obj.key

    property string:
        def __get__(self):
            if self.obj.string == NULL:
                return None
            else:
                return self.obj.string

    property compose:
        def __get__(self):
            if self.obj.compose == NULL:
                return None
            else:
                return self.obj.compose

    property timestamp:
        def __get__(self):
            return self.obj.timestamp

