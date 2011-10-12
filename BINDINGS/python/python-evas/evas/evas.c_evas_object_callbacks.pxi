# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Caio Marcelo de Oliveira Filho, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_evas.pyx

import traceback

cdef int cb_object_dispatcher(Object self, event, int type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = tuple(self._callbacks[type])
    for func, args, kargs in lst:
        try:
            func(self, event, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef int cb_object_dispatcher2(Object self, int type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = tuple(self._callbacks[type])
    for func, args, kargs in lst:
        try:
            func(self, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef void cb_object_mouse_in(void *data, Evas *e,
                             Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseIn event
    event = EventMouseIn()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_IN)
    event._unset_obj()


cdef void cb_object_mouse_out(void *data, Evas *e,
                              Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseOut event
    event = EventMouseOut()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_OUT)
    event._unset_obj()


cdef void cb_object_mouse_down(void *data, Evas *e,
                               Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseDown event
    event = EventMouseDown()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_DOWN)
    event._unset_obj()


cdef void cb_object_mouse_up(void *data, Evas *e,
                             Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseUp event
    event = EventMouseUp()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_UP)
    event._unset_obj()


cdef void cb_object_mouse_move(void *data, Evas *e,
                               Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseMove event
    event = EventMouseMove()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_MOVE)
    event._unset_obj()


cdef void cb_object_multi_down(void *data, Evas *e,
                               Evas_Object *obj, void *e_inf) with gil:
    cdef EventMultiDown event
    event = EventMultiDown()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MULTI_DOWN)
    event._unset_obj()

cdef void cb_object_multi_up(void *data, Evas *e,
                             Evas_Object *obj, void *e_inf) with gil:
    cdef EventMultiUp event
    event = EventMultiUp()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MULTI_UP)
    event._unset_obj()


cdef void cb_object_multi_move(void *data, Evas *e,
                               Evas_Object *obj, void *e_inf) with gil:
    cdef EventMultiMove event
    event = EventMultiMove()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MULTI_MOVE)
    event._unset_obj()


cdef void cb_object_mouse_wheel(void *data, Evas *e,
                                Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseWheel event
    event = EventMouseWheel()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_WHEEL)
    event._unset_obj()


cdef void cb_object_free(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_FREE)


cdef void cb_object_key_down(void *data, Evas *e,
                             Evas_Object *obj, void *e_inf) with gil:
    cdef EventKeyDown event
    event = EventKeyDown()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_DOWN)
    event._unset_obj()


cdef void cb_object_key_up(void *data, Evas *e,
                           Evas_Object *obj, void *e_inf) with gil:
    cdef EventKeyUp event
    event = EventKeyUp()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_UP)
    event._unset_obj()


cdef void cb_object_focus_in(void *data, Evas *e,
                             Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_IN)


cdef void cb_object_focus_out(void *data, Evas *e,
                              Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_OUT)


cdef void cb_object_show(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_SHOW)


cdef void cb_object_hide(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_HIDE)


cdef void cb_object_move(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_MOVE)


cdef void cb_object_resize(void *data, Evas *e,
                           Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_RESIZE)


cdef void cb_object_restack(void *data, Evas *e,
                            Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_RESTACK)


cdef void cb_object_del(void *data, Evas *e,
                        Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_DEL)


cdef void cb_object_hold(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cdef EventHold event
    event = EventHold()
    event._set_obj(e_inf)
    cb_object_dispatcher(<Object>data, event, EVAS_CALLBACK_HOLD)
    event._unset_obj()


cdef void cb_object_changed_size_hints(void *data, Evas *e,
                                       Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_CHANGED_SIZE_HINTS)


cdef void cb_object_image_preloaded(void *data, Evas *e,
                                    Evas_Object *obj, void *e_inf) with gil:
    cb_object_dispatcher2(<Object>data, EVAS_CALLBACK_IMAGE_PRELOADED)


cdef void cb_object_canvas_focus_in(void *data, Evas *e,
                                    Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_FOCUS_IN is not supported by object."


cdef void cb_object_canvas_focus_out(void *data, Evas *e,
                                     Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_FOCUS_OUT is not supported by object."


cdef void cb_object_render_flush_pre(void *data, Evas *e,
                                     Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_RENDER_FLUSH_PRE is not supported by object."


cdef void cb_object_render_flush_post(void *data, Evas *e,
                                      Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_RENDER_FLUSH_POST is not supported by object."

cdef void cb_object_canvas_object_focus_in(void *data, Evas *e,
                                           Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN is not supported by object."

cdef void cb_object_canvas_object_focus_out(void *data, Evas *e,
                                            Evas_Object *obj, void *e_inf) with gil:
    print "EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT is not supported by object."


cdef int evas_object_event_callbacks_len
cdef Evas_Object_Event_Cb evas_object_event_callbacks[31]
evas_object_event_callbacks_len = 31
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_IN] = cb_object_mouse_in
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_OUT] = cb_object_mouse_out
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_DOWN] = cb_object_mouse_down
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_UP] = cb_object_mouse_up
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_MOVE] = cb_object_mouse_move
evas_object_event_callbacks[EVAS_CALLBACK_MOUSE_WHEEL] = cb_object_mouse_wheel
evas_object_event_callbacks[EVAS_CALLBACK_MULTI_DOWN] = cb_object_multi_down
evas_object_event_callbacks[EVAS_CALLBACK_MULTI_UP] = cb_object_multi_up
evas_object_event_callbacks[EVAS_CALLBACK_MULTI_MOVE] = cb_object_multi_move
evas_object_event_callbacks[EVAS_CALLBACK_FREE] = cb_object_free
evas_object_event_callbacks[EVAS_CALLBACK_KEY_DOWN] = cb_object_key_down
evas_object_event_callbacks[EVAS_CALLBACK_KEY_UP] = cb_object_key_up
evas_object_event_callbacks[EVAS_CALLBACK_FOCUS_IN] = cb_object_focus_in
evas_object_event_callbacks[EVAS_CALLBACK_FOCUS_OUT] = cb_object_focus_out
evas_object_event_callbacks[EVAS_CALLBACK_SHOW] = cb_object_show
evas_object_event_callbacks[EVAS_CALLBACK_HIDE] = cb_object_hide
evas_object_event_callbacks[EVAS_CALLBACK_MOVE] = cb_object_move
evas_object_event_callbacks[EVAS_CALLBACK_RESIZE] = cb_object_resize
evas_object_event_callbacks[EVAS_CALLBACK_RESTACK] = cb_object_restack
evas_object_event_callbacks[EVAS_CALLBACK_DEL] = cb_object_del
evas_object_event_callbacks[EVAS_CALLBACK_HOLD] = cb_object_hold
evas_object_event_callbacks[EVAS_CALLBACK_CHANGED_SIZE_HINTS] = cb_object_changed_size_hints
evas_object_event_callbacks[EVAS_CALLBACK_IMAGE_PRELOADED] = cb_object_image_preloaded
evas_object_event_callbacks[EVAS_CALLBACK_CANVAS_FOCUS_IN] = cb_object_canvas_focus_in
evas_object_event_callbacks[EVAS_CALLBACK_CANVAS_FOCUS_OUT] = cb_object_canvas_focus_out
evas_object_event_callbacks[EVAS_CALLBACK_RENDER_FLUSH_PRE] = cb_object_render_flush_pre
evas_object_event_callbacks[EVAS_CALLBACK_RENDER_FLUSH_POST] = cb_object_render_flush_post
evas_object_event_callbacks[EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN] = cb_object_canvas_object_focus_in
evas_object_event_callbacks[EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT] = cb_object_canvas_object_focus_out
## I just made it compile, it's someone else job to support this in python-evas...
#evas_object_event_callbacks[EVAS_CALLBACK_IMAGE_UNLOADED] = cb_object_image_unloaded
#evas_object_event_callbacks[EVAS_CALLBACK_TOUCH] = cb_object_touch
