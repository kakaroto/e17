# This file is included verbatim by c_evas.pyx

cdef class EventPoint:
    cdef void _set_obj(self, Evas_Point *obj):
        self.x = obj.x
        self.y = obj.y

    def __str__(self):
        return "%s(%d, %d)" % (self.__class__.__name__, self.x, self.y)

    property xy:
        def __get__(self):
            return (self.x, self.y)

    def __len__(self):
        return 2

    def __getitem__(self, int index):
        if index == 0:
            return self.x
        elif index == 1:
            return self.y
        else:
            raise IndexError("list index out of range")



cdef class EventPosition:
    cdef void _set_objs(self, void *output, void *canvas):
        self.output = EventPoint()
        self.output._set_obj(<Evas_Point*>output)
        self.canvas = EventPoint()
        self.canvas._set_obj(<Evas_Point*>canvas)

    def __str__(self):
        return "%s(output=(%d, %d), canvas=(%d, %d))" % \
               (self.__class__.__name__, self.output.x, self.output.y,
                self.canvas.x, self.canvas.y)



cdef class EventMouseIn:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_In *obj
        obj = <Evas_Event_Mouse_In*>ptr
        self.buttons = obj.buttons
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.buttons,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.timestamp)


cdef class EventMouseOut:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Out *obj
        obj = <Evas_Event_Mouse_Out*>ptr
        self.buttons = obj.buttons
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.buttons,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.timestamp)


cdef class EventMouseDown:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Down *obj
        obj = <Evas_Event_Mouse_Down*>ptr
        self.button = obj.button
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(button=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.button,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.timestamp)


cdef class EventMouseUp:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Up *obj
        obj = <Evas_Event_Mouse_Up*>ptr
        self.button = obj.button
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(button=%d, output=(%d, %d), canvas=(%d, %d), "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.button,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.timestamp)


cdef class EventMouseMove:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Move *obj
        obj = <Evas_Event_Mouse_Move*>ptr
        self.buttons = obj.buttons
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.cur.output, &obj.cur.canvas)
        self.prev_position = EventPosition()
        self.prev_position._set_objs(&obj.prev.output,
                                     &obj.prev.canvas)

    def __str__(self):
        return ("%s(buttons=%d, output=(%d, %d), canvas=(%d, %d), "
                "prev_output=(%d, %d), prev_canvas=(%d, %d), timestamp=%d)") %\
                (self.__class__.__name__, self.buttons,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.prev_position.output.x, self.prev_position.output.y,
                 self.prev_position.canvas.x, self.prev_position.canvas.y,
                 self.timestamp)


cdef class EventMouseWheel:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Mouse_Wheel *obj
        obj = <Evas_Event_Mouse_Wheel*>ptr
        self.z = obj.z
        self.direction = obj.direction
        self.timestamp = obj.timestamp
        self.position = EventPosition()
        self.position._set_objs(&obj.output, &obj.canvas)

    def __str__(self):
        return ("%s(direction=%d, z=%d, output=(%d, %d), "
                "canvas=(%d, %d), timestamp=%d)") % \
                (self.__class__.__name__, self.direction, self.z,
                 self.position.output.x, self.position.output.y,
                 self.position.canvas.x, self.position.canvas.y,
                 self.timestamp)


cdef class EventKeyDown:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Key_Down *obj
        obj = <Evas_Event_Key_Down*>ptr
        self.timestamp = obj.timestamp
        if obj.keyname != NULL:
            self.keyname = obj.keyname
        else:
            self.keyname = None

        if obj.key != NULL:
            self.key = obj.key
        else:
            self.key = None

        if obj.string != NULL:
            self.string = obj.string
        else:
            self.string = None

        if obj.compose != NULL:
            self.compose = obj.compose
        else:
            self.compose = None

    def __str__(self):
        return ("%s(keyname=%r, key=%r, string=%r, compose=%r, "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.keyname,
                 self.key, self.string, self.compose,
                 self.timestamp)


cdef class EventKeyUp:
    cdef void _set_obj(self, void *ptr):
        cdef Evas_Event_Key_Up *obj
        obj = <Evas_Event_Key_Up*>ptr
        self.timestamp = obj.timestamp
        if obj.keyname != NULL:
            self.keyname = obj.keyname
        else:
            self.keyname = None

        if obj.key != NULL:
            self.key = obj.key
        else:
            self.key = None

        if obj.string != NULL:
            self.string = obj.string
        else:
            self.string = None

        if obj.compose != NULL:
            self.compose = obj.compose
        else:
            self.compose = None

    def __str__(self):
        return ("%s(keyname=%r, key=%r, string=%r, compose=%r, "
                "timestamp=%d)") % \
                (self.__class__.__name__, self.keyname,
                 self.key, self.string, self.compose,
                 self.timestamp)

