# Copyright (c) 2008-2009 Simon Busch
# Copyright (c) 2010-2010 ProFUSION embedded systems
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

cdef class NaviframeItem(ObjectItem):
    def item_pop_to(self):
        elm_naviframe_item_pop_to(self.obj)

    def item_promote(self):
        elm_naviframe_item_promote(self.obj)

    def style_set(self, style):
        elm_naviframe_item_style_set(self.obj, style)

    def style_get(self):
        return elm_naviframe_item_style_get(self.obj)

    def title_visible_set(self, visible):
        elm_naviframe_item_title_visible_set(self.obj, visible)

    def title_visible_get(self):
        return bool(elm_naviframe_item_title_visible_get(self.obj))

cdef _elm_naviframe_item_to_python(Elm_Object_Item *it):
    cdef void *data
    cdef object prm
    if it == NULL:
        return None
    data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    prm = <object>data
    return prm[2]

cdef class Naviframe(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_naviframe_add(parent.obj))

    def item_push(self, title_label, c_evas.Object prev_btn, c_evas.Object next_btn, c_evas.Object content, const_char_ptr item_style):
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_push(self.obj, title_label, prev_btn.obj, next_btn.obj, content.obj, item_style)
        if item != NULL:
            ret.obj = item
            return ret
        else:
            return None

    def item_insert_before(self, NaviframeItem before, title_label, c_evas.Object prev_btn, c_evas.Object next_btn, c_evas.Object content, const_char_ptr item_style):
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_insert_before(self.obj, before.obj, title_label, prev_btn.obj, next_btn.obj, content.obj, item_style)
        if item != NULL:
            ret.obj = item
            return ret
        else:
            return None

    def item_insert_after(self, NaviframeItem after, title_label, c_evas.Object prev_btn, c_evas.Object next_btn, c_evas.Object content, const_char_ptr item_style):
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_insert_after(self.obj, after.obj, title_label, prev_btn.obj, next_btn.obj, content.obj, item_style)
        if item != NULL:
            ret.obj = item
            return ret
        else:
            return None

    def item_pop(self):
        cdef c_evas.Evas_Object *obj = elm_naviframe_item_pop(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def content_preserve_on_pop_set(self, preserve):
        elm_naviframe_content_preserve_on_pop_set(self.obj, preserve)

    def content_preserve_on_pop_get(self):
        return bool(elm_naviframe_content_preserve_on_pop_get(self.obj))

    property content_preserve_on_pop:
        def __get__(self):
            return self.content_preserve_on_pop_get()
        def __set__(self, preserve):
            self.content_preserve_on_pop_set(preserve)

    def top_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_naviframe_top_item_get(self.obj)
        return _elm_naviframe_item_to_python(it)

    def bottom_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_naviframe_bottom_item_get(self.obj)
        return _elm_naviframe_item_to_python(it)

    def prev_btn_auto_pushed_set(self, auto_pushed):
        elm_naviframe_prev_btn_auto_pushed_set(self.obj, auto_pushed)

    def prev_btn_auto_pushed_get(self):
        return bool(elm_naviframe_prev_btn_auto_pushed_get(self.obj))

    property prev_btn_auto_pushed:
        def __get__(self):
            return self.prev_btn_auto_pushed_get()
        def __set__(self, auto_pushed):
            self.prev_btn_auto_pushed_set(auto_pushed)

    def items_get(self):
        cdef Elm_Object_Item *it
        cdef c_evas.const_Eina_List *lst

        lst = elm_naviframe_items_get(self.obj)
        ret = []
        ret_append = ret.append
        while lst:
            it = <Elm_Object_Item *>lst.data
            lst = lst.next
            o = _elm_naviframe_item_to_python(it)
            if o is not None:
                ret_append(o)
        return ret

    def event_enabled_set(self, enabled):
        elm_naviframe_event_enabled_set(self.obj, enabled)

    def event_enabled_get(self):
        return bool(elm_naviframe_event_enabled_get(self.obj))

    property event_enabled:
        def __get__(self):
            return self.event_enabled_get()
        def __set__(self, enabled):
            self.event_enabled_set(enabled)

    def item_simple_push(self, c_evas.Object content):
        cdef NaviframeItem ret = NaviframeItem()
        cdef Elm_Object_Item *item

        item = elm_naviframe_item_simple_push(self.obj, content.obj)
        if item != NULL:
            ret.obj = item
            return ret
        else:
            return None

    def item_simple_promote(self, c_evas.Object content):
        elm_naviframe_item_simple_promote(self.obj, content.obj)

    def callback_transition_finished_add(self, func, *args, **kwargs):
        self._callback_add("transition,finished", func, *args, **kwargs)

    def callback_transition_finished_del(self, func):
        self._callback_add("transition,finished", func)

    def callback_title_clicked_add(self, func, *args, **kwargs):
        self._callback_add("title,clicked", func, *args, **kwargs)

    def callback_title_clicked_add(self, func):
        self._callback_add("title,clicked", func)

_elm_widget_type_register("naviframe", Naviframe)
