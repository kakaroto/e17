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

cdef int cb_canvas_dispatcher(Canvas self, event, int type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = tuple(self._callbacks[type])
    for func, args, kargs in lst:
        try:
            func(self, event, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef int cb_canvas_dispatcher2(Canvas self, int type) except 0:
    # iterate over copy since users may delete callback from callback
    lst = tuple(self._callbacks[type])
    for func, args, kargs in lst:
        try:
            func(self, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
    return 1


cdef void cb_canvas_mouse_in(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_IN is not supported by canvas."


cdef void cb_canvas_mouse_out(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_OUT is not supported by canvas."


cdef void cb_canvas_mouse_down(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_DOWN is not supported by canvas."


cdef void cb_canvas_mouse_up(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_UP is not supported by canvas."


cdef void cb_canvas_mouse_move(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_MOVE is not supported by canvas."


cdef void cb_canvas_multi_down(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MULTI_DOWN is not supported by canvas."


cdef void cb_canvas_multi_up(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MULTI_UP is not supported by canvas."


cdef void cb_canvas_multi_move(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MULTI_MOVE is not supported by canvas."


cdef void cb_canvas_mouse_wheel(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOUSE_WHEEL is not supported by canvas."


cdef void cb_canvas_free(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_FREE is not supported by canvas."


cdef void cb_canvas_key_down(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_KEY_DOWN is not supported by canvas."


cdef void cb_canvas_key_up(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_KEY_UP is not supported by canvas."


cdef void cb_canvas_focus_in(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_FOCUS_IN is not supported by canvas."


cdef void cb_canvas_focus_out(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_FOCUS_OUT is not supported by canvas."


cdef void cb_canvas_show(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_SHOW is not supported by canvas."


cdef void cb_canvas_hide(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_HIDE is not supported by canvas."


cdef void cb_canvas_move(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_MOVE is not supported by canvas."


cdef void cb_canvas_resize(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_RESIZE is not supported by canvas."


cdef void cb_canvas_restack(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_RESTACK is not supported by canvas."


cdef void cb_canvas_del(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_DEL is not supported by canvas."


cdef void cb_canvas_hold(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_HOLD is not supported by canvas."


cdef void cb_canvas_changed_size_hints(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_CHANGED_SIZE_HINTS is not supported by canvas."


cdef void cb_canvas_image_preloaded(void *data, Evas *e, void *e_inf) with gil:
    print "EVAS_CALLBACK_IMAGE_PRELOADED is not supported by canvas."


cdef void cb_canvas_canvas_focus_in(void *data, Evas *e, void *e_inf) with gil:
    cb_canvas_dispatcher2(<Canvas>data, EVAS_CALLBACK_CANVAS_FOCUS_IN)


cdef void cb_canvas_canvas_focus_out(void *data, Evas *e, void *e_inf) with gil:
    cb_canvas_dispatcher2(<Canvas>data, EVAS_CALLBACK_CANVAS_FOCUS_OUT)


cdef void cb_canvas_render_flush_pre(void *data, Evas *e, void *e_inf) with gil:
    cb_canvas_dispatcher2(<Canvas>data, EVAS_CALLBACK_RENDER_FLUSH_PRE)


cdef void cb_canvas_render_flush_post(void *data, Evas *e, void *e_inf) with gil:
    cb_canvas_dispatcher2(<Canvas>data, EVAS_CALLBACK_RENDER_FLUSH_POST)


cdef void cb_canvas_canvas_object_focus_in(void *data, Evas *e, void *e_inf) with gil:
    cdef Evas_Object *obj = <Evas_Object*>e_inf
    o = Object_from_instance(obj)
    cb_canvas_dispatcher(<Canvas>data, o, EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN)


cdef void cb_canvas_canvas_object_focus_out(void *data, Evas *e, void *e_inf) with gil:
    cdef Evas_Object *obj = <Evas_Object*>e_inf
    o = Object_from_instance(obj)
    cb_canvas_dispatcher(<Canvas>data, o, EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT)


cdef int evas_canvas_event_callbacks_len
cdef Evas_Event_Cb evas_canvas_event_callbacks[31]
evas_canvas_event_callbacks_len = 31
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_IN] = cb_canvas_mouse_in
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_OUT] = cb_canvas_mouse_out
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_DOWN] = cb_canvas_mouse_down
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_UP] = cb_canvas_mouse_up
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_MOVE] = cb_canvas_mouse_move
evas_canvas_event_callbacks[EVAS_CALLBACK_MOUSE_WHEEL] = cb_canvas_mouse_wheel
evas_canvas_event_callbacks[EVAS_CALLBACK_MULTI_DOWN] = cb_canvas_multi_down
evas_canvas_event_callbacks[EVAS_CALLBACK_MULTI_UP] = cb_canvas_multi_up
evas_canvas_event_callbacks[EVAS_CALLBACK_MULTI_MOVE] = cb_canvas_multi_move
evas_canvas_event_callbacks[EVAS_CALLBACK_FREE] = cb_canvas_free
evas_canvas_event_callbacks[EVAS_CALLBACK_KEY_DOWN] = cb_canvas_key_down
evas_canvas_event_callbacks[EVAS_CALLBACK_KEY_UP] = cb_canvas_key_up
evas_canvas_event_callbacks[EVAS_CALLBACK_FOCUS_IN] = cb_canvas_focus_in
evas_canvas_event_callbacks[EVAS_CALLBACK_FOCUS_OUT] = cb_canvas_focus_out
evas_canvas_event_callbacks[EVAS_CALLBACK_SHOW] = cb_canvas_show
evas_canvas_event_callbacks[EVAS_CALLBACK_HIDE] = cb_canvas_hide
evas_canvas_event_callbacks[EVAS_CALLBACK_MOVE] = cb_canvas_move
evas_canvas_event_callbacks[EVAS_CALLBACK_RESIZE] = cb_canvas_resize
evas_canvas_event_callbacks[EVAS_CALLBACK_RESTACK] = cb_canvas_restack
evas_canvas_event_callbacks[EVAS_CALLBACK_DEL] = cb_canvas_del
evas_canvas_event_callbacks[EVAS_CALLBACK_HOLD] = cb_canvas_hold
evas_canvas_event_callbacks[EVAS_CALLBACK_CHANGED_SIZE_HINTS] = cb_canvas_changed_size_hints
evas_canvas_event_callbacks[EVAS_CALLBACK_IMAGE_PRELOADED] = cb_canvas_image_preloaded
evas_canvas_event_callbacks[EVAS_CALLBACK_CANVAS_FOCUS_IN] = cb_canvas_canvas_focus_in
evas_canvas_event_callbacks[EVAS_CALLBACK_CANVAS_FOCUS_OUT] = cb_canvas_canvas_focus_out
evas_canvas_event_callbacks[EVAS_CALLBACK_RENDER_FLUSH_PRE] = cb_canvas_render_flush_pre
evas_canvas_event_callbacks[EVAS_CALLBACK_RENDER_FLUSH_POST] = cb_canvas_render_flush_post
evas_canvas_event_callbacks[EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_IN] = cb_canvas_canvas_object_focus_in
evas_canvas_event_callbacks[EVAS_CALLBACK_CANVAS_OBJECT_FOCUS_OUT] = cb_canvas_canvas_object_focus_out

## I just made it compile again. It's someone else job to make this work in python-evas...
#evas_canvas_event_callbacks[EVAS_CALLBACK_IMAGE_UNLOADED] = cb_canvas_image_unloaded
#evas_canvas_event_callbacks[EVAS_CALLBACK_TOUCH] = cb_canvas_touch
