# Copyright (c) 2012 Joost Albers
# Copyright (c) 2012 Gustavo Sverzut Barbieri <barbieri@profusion.mobi>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

def _web_double_conv(long addr):
    cdef double *info = <double *>addr
    if info == NULL:
        return None
    return info[0]

def _web_load_frame_error_conv(long addr):
    cdef Elm_Web_Frame_Load_Error *err = <Elm_Web_Frame_Load_Error *>addr
    if err == NULL:
        return None
    ret = {
        "code": err.code,
        "is_cancellation": bool(err.is_cancellation),
        }
    if err.domain:
        ret["domain"] = err.domain
    else:
        ret["domain"] = None

    if err.description:
        ret["description"] = err.description
    else:
        ret["description"] = None

    if err.failing_url:
        ret["failing_url"] = err.failing_url
    else:
        ret["failing_url"] = None

    if err.frame:
        ret["frame"] = Object_from_instance(err.frame)
    else:
        ret["frame"] = None

    return ret


def _web_link_hover_in_conv(long addr):
    cdef char **info = <char **>addr
    if info == NULL:
        url = title = None
    else:
        if info[0] == NULL:
           url = None
        else:
           url = info[0]

        if info[1] == NULL:
           title = None
        else:
           title = info[1]
    return (url, title)


cdef void _web_console_message_hook(void *data, Evas_Object *obj, const_char_ptr message, unsigned int line_number, const_char_ptr source_id) with gil:
    cdef Web self = <Web>data

    if message == NULL:
        m = None
    else:
        m = message

    if source_id == NULL:
        s = None
    else:
        s = source_id

    try:
        self._console_message_hook(self, m, line_number, s)
    except Exception, e:
        traceback.print_exc()

cdef public class Web(Object) [object PyElementaryWeb, type PyElementaryWeb_Type]:
    cdef object _console_message_hook

    def __init__(self,evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_web_add(parent.obj))

    # XXX TODO: complete all callbacks from elm_web.h
    def callback_uri_changed_add(self, func, *args, **kwargs):
        self._callback_add_full("uri,changed", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_uri_changed_del(self, func):
        self._callback_del_full("uri,changed", _cb_string_conv, func)

    def callback_title_changed_add(self, func, *args, **kwargs):
        self._callback_add_full("title,changed", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_title_changed_del(self, func):
        self._callback_del_full("title,changed", _cb_string_conv, func)

    def callback_link_hover_in_add(self, func, *args, **kwargs):
        self._callback_add_full("link,hover,in", _web_link_hover_in_conv,
                                func, *args, **kwargs)

    def callback_link_hover_in_del(self, func):
        self._callback_del_full("link,hover,in", _web_link_hover_in_conv, func)

    def callback_link_hover_out_add(self, func, *args, **kwargs):
        self._callback_add("link,hover,out", func, *args, **kwargs)

    def callback_link_hover_out_del(self, func):
        self._callback_del("link,hover,out", func)

    def callback_load_error_add(self, func, *args, **kwargs):
        self._callback_add_full("load,error", _web_load_frame_error_conv,
                                func, *args, **kwargs)

    def callback_load_error_del(self, func):
        self._callback_del_full("load,error", _web_load_frame_error_conv, func)

    def callback_load_finished_add(self, func, *args, **kwargs):
        self._callback_add_full("load,finished", _web_load_frame_error_conv,
                                func, *args, **kwargs)

    def callback_load_finished_del(self, func):
        self._callback_del_full("load,finished",
                                _web_load_frame_error_conv, func)

    def callback_load_progress_add(self, func, *args, **kwargs):
        self._callback_add_full("load,progress", _web_double_conv,
                                func, *args, **kwargs)

    def callback_load_progress_del(self, func):
        self._callback_del_full("load,progress", _web_double_conv, func)

    def callback_load_provisional_add(self, func, *args, **kwargs):
        self._callback_add("load,provisional", func, *args, **kwargs)

    def callback_load_provisional_del(self, func):
        self._callback_del("load,provisional", func)

    def callback_load_started_add(self, func, *args, **kwargs):
        self._callback_add("load,started", func, *args, **kwargs)

    def callback_load_started_del(self, func):
        self._callback_del("load,started", func)

    def history_enabled_get(self):
        return bool(elm_web_history_enabled_get(self.obj))

    def webkit_view_get(self):
        cdef Evas_Object *obj = elm_web_webkit_view_get(self.obj)
        return Object_from_instance(obj)

    def uri_set(self, uri):
        return bool(elm_web_uri_set(self.obj, uri))

    def uri_get(self):
        cdef const_char_ptr l
        l = elm_web_uri_get(self.obj)
        if l == NULL:
            return None
        return l

    property uri:
        def __get__(self):
            return self.uri_get()

        def __set__(self, value):
            self.uri_set(value)

    def useragent_get(self):
        cdef const_char_ptr l
        l = elm_web_useragent_get(self.obj)
        if l == NULL:
            return None
        return l

    def zoom_get(self):
        return elm_web_zoom_get(self.obj)

    def zoom_mode_get(self):
        return elm_web_zoom_mode_get(self.obj)

    def back(self):
        return bool(elm_web_back(self.obj))

    def console_message_hook_set(self, func):
        self._console_message_hook = func
        if func:
            elm_web_console_message_hook_set(self.obj,
                                             _web_console_message_hook,
                                             <void *>self)
        else:
            elm_web_console_message_hook_set(self.obj, NULL, NULL)

_elm_widget_type_register("web", Web)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryWeb_Type # hack to install metaclass
_install_metaclass(&PyElementaryWeb_Type, ElementaryObjectMeta)
