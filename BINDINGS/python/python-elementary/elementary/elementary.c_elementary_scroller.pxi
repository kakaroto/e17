# Copyright (c) 2008-2009 Simon Busch
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

cdef class Scroller(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_scroller_add(parent.obj))

    def custom_widget_base_theme_set(self, widget, base):
        elm_scroller_custom_widget_base_theme_set(self.obj, widget, base)

    def content_min_limit(self, w, h):
        elm_scroller_content_min_limit(self.obj, w, h)

    def region_show(self, x, y, w, h):
        elm_scroller_region_show(self.obj, x, y, w, h)

    def policy_set(self, policy_h, policy_v):
        elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def policy_get(self):
        cdef Elm_Scroller_Policy policy_h, policy_v
        elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
        return (policy_h, policy_v)

    property policy:
        def __get__(self):
            return self.policy_get()
    
        def __set__(self, value):
            self.policy_set(*value)
    
    def region_get(self):
        cdef c_evas.Evas_Coord x, y, w, h
        elm_scroller_region_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def child_size_get(self):
        cdef c_evas.Evas_Coord w, h
        elm_scroller_child_size_get(self.obj, &w, &h)
        return (w, h)

    def bounce_set(self, h, v):
        elm_scroller_bounce_set(self.obj, h, v)

    def bounce_get(self):
        cdef c_evas.Eina_Bool h, v
        elm_scroller_bounce_get(self.obj, &h, &v)
        return (h, v)

    property bounce:
        def __get__(self):
            return self.bounce_get()
    
        def __set__(self, value):
            self.bounce_set(*value)
    
    def page_relative_set(self, h_pagerel, v_pagerel):
        elm_scroller_page_relative_set(self.obj, h_pagerel, v_pagerel)

    def page_size_set(self, h_pagesize, v_pagesize):
        elm_scroller_page_size_set(self.obj, h_pagesize, v_pagesize)

    def current_page_get(self):
        cdef int h_pagenumber, v_pagenumber
        elm_scroller_current_page_get(self.obj, &h_pagenumber, &v_pagenumber)
        return (h_pagenumber, v_pagenumber)

    def last_page_get(self):
        cdef int h_pagenumber, v_pagenumber
        elm_scroller_last_page_get(self.obj, &h_pagenumber, &v_pagenumber)
        return (h_pagenumber, v_pagenumber)

    def page_show(self, h_pagenumber, v_pagenumber):
        elm_scroller_page_show(self.obj, h_pagenumber, v_pagenumber)

    def page_bring_in(self, h_pagenumber, v_pagenumber):
        elm_scroller_page_bring_in(self.obj, h_pagenumber, v_pagenumber)

    def region_bring_in(self, x, y, w, h):
        elm_scroller_region_bring_in(self.obj, x, y, w, h)

    def propagate_events_set(self, propagation):
        elm_scroller_propagate_events_set(self.obj, propagation)

    def propagate_events_get(self):
        return bool(elm_scroller_propagate_events_get(self.obj))

    property propagate_events:
        def __get__(self):
            return self.propagate_events_get()

        def __set__(self, propagation):
            self.propagate_events_set(propagation)

    def gravity_set(self, x, y):
        elm_scroller_gravity_set(self.obj, x, y)

    def gravity_get(self):
        cdef double x, y
        elm_scroller_gravity_get(self.obj, &x, &y)
        return (x, y)

    property gravity:
        def __get__(self):
            return self.gravity_get()
    
        def __set__(self, value):
            self.gravity_set(*value)

    def callback_edge_left_add(self, func, *args, **kwargs):
        self._callback_add("edge,left", func, *args, **kwargs)

    def callback_edge_left_del(self, func):
        self._callback_del("edge,left", func)

    def callback_edge_right_add(self, func, *args, **kwargs):
        self._callback_add("edge,right", func, *args, **kwargs)

    def callback_edge_right_del(self, func):
        self._callback_del("edge,right", func)

    def callback_edge_top_add(self, func, *args, **kwargs):
        self._callback_add("edge,top", func, *args, **kwargs)

    def callback_edge_top_del(self, func):
        self._callback_del("edge,top", func)

    def callback_edge_bottom_add(self, func, *args, **kwargs):
        self._callback_add("edge,bottom", func, *args, **kwargs)

    def callback_edge_bottom_del(self, func):
        self._callback_del("edge,bottom", func)

    def callback_scroll_add(self, func, *args, **kwargs):
        self._callback_add("scroll", func, *args, **kwargs)

    def callback_scroll_del(self, func):
        self._callback_del("scroll", func)

    def callback_scroll_anim_start_add(self, func, *args, **kwargs):
        self._callback_add("scroll,anim,start", func, *args, **kwargs)

    def callback_scroll_anim_start_del(self, func):
        self._callback_del("scroll,anim,start", func)

    def callback_scroll_anim_stop_add(self, func, *args, **kwargs):
        self._callback_add("scroll,anim,stop", func, *args, **kwargs)

    def callback_scroll_anim_stop_del(self, func):
        self._callback_del("scroll,anim,stop", func)

    def callback_scroll_drag_start_add(self, func, *args, **kwargs):
        self._callback_add("scroll,drag,start", func, *args, **kwargs)

    def callback_scroll_drag_start_del(self, func):
        self._callback_del("scroll,drag,start", func)

    def callback_scroll_drag_stop_add(self, func, *args, **kwargs):
        self._callback_add("scroll,drag,stop", func, *args, **kwargs)

    def callback_scroll_drag_stop_del(self, func):
        self._callback_del("scroll,drag,stop", func)


_elm_widget_type_register("scroller", Scroller)
