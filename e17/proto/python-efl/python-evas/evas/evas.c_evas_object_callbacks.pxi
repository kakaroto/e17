# This file is included verbatim by c_evas.pyx

cdef int cb_dispatcher(Object self, event, Evas_Callback_Type type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = list(self._callbacks[<int>type])
    for func, args, kargs in lst:
        func(self, event, *args, **kargs)
    return 1


cdef int cb_dispatcher2(Object self, Evas_Callback_Type type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = list(self._callbacks[<int>type])
    for func, args, kargs in lst:
        func(self, *args, **kargs)
    return 1


cdef void cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseIn event
    event = EventMouseIn()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_IN)


cdef void cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseOut event
    event = EventMouseOut()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_OUT)


cdef void cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseDown event
    event = EventMouseDown()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_DOWN)


cdef void cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseUp event
    event = EventMouseUp()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_UP)


cdef void cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseMove event
    event = EventMouseMove()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_MOVE)


cdef void cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventMouseWheel event
    event = EventMouseWheel()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_WHEEL)


cdef void cb_free(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FREE)


cdef void cb_key_down(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventKeyDown event
    event = EventKeyDown()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_DOWN)


cdef void cb_key_up(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cdef EventKeyUp event
    event = EventKeyUp()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_UP)


cdef void cb_focus_in(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_IN)


cdef void cb_focus_out(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_OUT)


cdef void cb_show(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_SHOW)


cdef void cb_hide(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_HIDE)


cdef void cb_move(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_MOVE)


cdef void cb_resize(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_RESIZE)


cdef void cb_restack(void *data, Evas *e, Evas_Object *obj, void *e_inf):
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_RESTACK)


cdef int evas_event_callbacks_len
cdef evas_event_callback_t evas_event_callbacks[16]
evas_event_callbacks_len = 16
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_IN] = cb_mouse_in
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_OUT] = cb_mouse_out
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_DOWN] = cb_mouse_down
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_UP] = cb_mouse_up
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_MOVE] = cb_mouse_move
evas_event_callbacks[<int>EVAS_CALLBACK_MOUSE_WHEEL] = cb_mouse_wheel
evas_event_callbacks[<int>EVAS_CALLBACK_FREE] = cb_free
evas_event_callbacks[<int>EVAS_CALLBACK_KEY_DOWN] = cb_key_down
evas_event_callbacks[<int>EVAS_CALLBACK_KEY_UP] = cb_key_up
evas_event_callbacks[<int>EVAS_CALLBACK_FOCUS_IN] = cb_focus_in
evas_event_callbacks[<int>EVAS_CALLBACK_FOCUS_OUT] = cb_focus_out
evas_event_callbacks[<int>EVAS_CALLBACK_SHOW] = cb_show
evas_event_callbacks[<int>EVAS_CALLBACK_HIDE] = cb_hide
evas_event_callbacks[<int>EVAS_CALLBACK_MOVE] = cb_move
evas_event_callbacks[<int>EVAS_CALLBACK_RESIZE] = cb_resize
evas_event_callbacks[<int>EVAS_CALLBACK_RESTACK] = cb_restack
