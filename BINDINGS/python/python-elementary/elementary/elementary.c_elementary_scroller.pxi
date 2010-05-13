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

    def content_set(self, c_evas.Object child):
        elm_scroller_content_set(self.obj, child.obj)

    def content_min_limit(self, w, h):
        elm_scroller_content_min_limit(self.obj, w, h)

    def region_show(self, x, y, w, h):
        elm_scroller_region_show(self.obj, x, y, w, h)

    def region_get(self):
        cdef c_evas.Evas_Coord x
        cdef c_evas.Evas_Coord y
        cdef c_evas.Evas_Coord w
        cdef c_evas.Evas_Coord h
        elm_scroller_region_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def policy_set(self, policy_h, policy_v):
        elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def child_size_get(self):
        cdef c_evas.Evas_Coord w
        cdef c_evas.Evas_Coord h
        elm_scroller_child_size_get(self.obj, &w, &h)
        return (w, h)

    def bounce_set(self, h, v):
        elm_scroller_bounce_set(self.obj, h, v)

    def page_relative_set(self, h_pagerel, v_pagerel):
        elm_scroller_page_relative_set(self.obj, h_pagerel, v_pagerel)

    def page_size_set(self, h_pagesize, v_pagesize):
        elm_scroller_page_size_set(self.obj, h_pagesize, v_pagesize)

    def region_bring_in(self, x, y, w, h):
        elm_scroller_region_bring_in(self.obj, x, y, w, h)


_elm_widget_type_register("scroller", Scroller)
