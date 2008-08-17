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

cdef int cb_dispatcher(Object self, event, Evas_Callback_Type type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = list(self._callbacks[<int>type])
    for func, args, kargs in lst:
        try:
            func(self, event, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef int cb_dispatcher2(Object self, Evas_Callback_Type type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = list(self._callbacks[<int>type])
    for func, args, kargs in lst:
        try:
            func(self, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef void cb_mouse_in(void *data, Evas *e,
                      Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseIn event
    event = EventMouseIn()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_IN)
    event._unset_obj()


cdef void cb_mouse_out(void *data, Evas *e,
                       Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseOut event
    event = EventMouseOut()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_OUT)
    event._unset_obj()


cdef void cb_mouse_down(void *data, Evas *e,
                        Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseDown event
    event = EventMouseDown()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_DOWN)
    event._unset_obj()


cdef void cb_mouse_up(void *data, Evas *e,
                      Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseUp event
    event = EventMouseUp()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_UP)
    event._unset_obj()


cdef void cb_mouse_move(void *data, Evas *e,
                        Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseMove event
    event = EventMouseMove()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_MOVE)
    event._unset_obj()


cdef void cb_mouse_wheel(void *data, Evas *e,
                         Evas_Object *obj, void *e_inf) with gil:
    cdef EventMouseWheel event
    event = EventMouseWheel()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_MOUSE_WHEEL)
    event._unset_obj()


cdef void cb_free(void *data, Evas *e,
                  Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FREE)


cdef void cb_key_down(void *data, Evas *e,
                      Evas_Object *obj, void *e_inf) with gil:
    cdef EventKeyDown event
    event = EventKeyDown()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_DOWN)
    event._unset_obj()


cdef void cb_key_up(void *data, Evas *e,
                    Evas_Object *obj, void *e_inf) with gil:
    cdef EventKeyUp event
    event = EventKeyUp()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_KEY_UP)
    event._unset_obj()


cdef void cb_focus_in(void *data, Evas *e,
                      Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_IN)


cdef void cb_focus_out(void *data, Evas *e,
                       Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_FOCUS_OUT)


cdef void cb_show(void *data, Evas *e, Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_SHOW)


cdef void cb_hide(void *data, Evas *e, Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_HIDE)


cdef void cb_move(void *data, Evas *e, Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_MOVE)


cdef void cb_resize(void *data, Evas *e,
                    Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_RESIZE)


cdef void cb_restack(void *data, Evas *e,
                     Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_RESTACK)


cdef void cb_del(void *data, Evas *e,
                 Evas_Object *obj, void *e_inf) with gil:
    cb_dispatcher2(<Object>data, EVAS_CALLBACK_DEL)


cdef void cb_hold(void *data, Evas *e,
                  Evas_Object *obj, void *e_inf) with gil:
    cdef EventHold event
    event = EventHold()
    event._set_obj(e_inf)
    cb_dispatcher(<Object>data, event, EVAS_CALLBACK_HOLD)
    event._unset_obj()


cdef int evas_event_callbacks_len
cdef evas_event_callback_t evas_event_callbacks[18]
evas_event_callbacks_len = 18
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
evas_event_callbacks[<int>EVAS_CALLBACK_DEL] = cb_del
evas_event_callbacks[<int>EVAS_CALLBACK_HOLD] = cb_hold
