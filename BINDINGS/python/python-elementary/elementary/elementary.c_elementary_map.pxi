# Copyright (c) 2012 Davide Andreoli <dave@gurumeditation.it>
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

from evas.c_evas cimport eina_list_free, eina_list_append

cdef _elm_map_overlay_to_python(Elm_Map_Overlay *ov):
    cdef void *data

    if ov == NULL:
        return None
    data = elm_map_overlay_data_get(ov)
    if data == NULL:
        return None
    return <object>data

cdef void _map_overlay_get_callback(void *data, Evas_Object *map, Elm_Map_Overlay *overlay) with gil:
    cdef Object obj

    obj = <Object>evas_object_data_get(map, "python-evas")
    try:
        (func, args, kwargs) = <object>data
        func(obj, _elm_map_overlay_to_python(overlay), *args, **kwargs)
    except Exception, e:
        traceback.print_exc()

cdef void _map_overlay_del_cb(void *data, Evas_Object *map, Elm_Map_Overlay *overlay) with gil:
    ov = <object>data
    ov.__del_cb()

cdef void _map_route_callback(void *data, Evas_Object *map, Elm_Map_Route *route) with gil:
    cdef Object obj

    obj = <Object>evas_object_data_get(map, "python-evas")
    (proute, func, args, kwargs) = <object>data
    try:
        func(obj, proute, *args, **kwargs)
    except Exception, e:
        traceback.print_exc()

    Py_DECREF(<object>data)

cdef void _map_name_callback(void *data, Evas_Object *map, Elm_Map_Name *name) with gil:
    cdef Object obj

    obj = <Object>evas_object_data_get(map, "python-evas")
    (pname, func, args, kwargs) = <object>data
    try:
        func(obj, pname, *args, **kwargs)
    except Exception, e:
        traceback.print_exc()

    Py_DECREF(<object>data)


cdef class MapRoute(object):
    cdef Elm_Map_Route *route

    def __cinit__(self):
        self.route = NULL

    def __init__(self, evasObject map,
                       Elm_Map_Route_Type type, Elm_Map_Route_Method method,
                       double flon, double flat, double tlon, double tlat,
                       func, *args, **kwargs):

        if not callable(func):
            raise TypeError("func must be callable")

        data = (self, func, args, kwargs)
        self.route = elm_map_route_add(map.obj, type, method,
                                       flon, flat, tlon, tlat,
                                       _map_route_callback, <void *>data)
        Py_INCREF(data)
        Py_INCREF(self)

    def delete(self):
        elm_map_route_del(self.route)
        self.route = NULL
        Py_DECREF(self)

    def distance_get(self):
        return elm_map_route_distance_get(self.route)

    property distance:
        def __get__(self):
            return self.distance_get()

    def node_get(self):
        return elm_map_route_node_get(self.route)

    property node:
        def __get__(self):
            return self.node_get()

    def waypoint_get(self):
        return elm_map_route_waypoint_get(self.route)

    property waypoint:
        def __get__(self):
            return self.waypoint_get()


cdef class MapName(object):
    cdef Elm_Map_Name *name

    def __cinit__(self):
        self.name = NULL

    def __init__(self, evasObject map, address, double lon, double lat,
                       func, *args, **kwargs):
        if not callable(func):
            raise TypeError("func must be callable")

        data = (self, func, args, kwargs)
        if address:
            self.name = elm_map_name_add(map.obj, address, lon, lat,
                                         _map_name_callback, <void *>data)
        else:
            self.name = elm_map_name_add(map.obj, NULL, lon, lat,
                                         _map_name_callback, <void *>data)
        Py_INCREF(data)
        Py_INCREF(self)

    def delete(self):
        elm_map_name_del(self.name)
        self.name = NULL
        Py_DECREF(self)

    def address_get(self):
        return _ctouni(elm_map_name_address_get(self.name))

    property address:
        def __get__(self):
            return self.address_get()

    def region_get(self):
        cdef double lon, lat

        elm_map_name_region_get(self.name, &lon, &lat)
        return (lon, lat)

    property region:
        def __get__(self):
            return self.region_get()


cdef class MapOverlay(object):
    cdef Elm_Map_Overlay *overlay
    cdef void* cb_get_data

    def __cinit__(self):
        self.overlay = NULL
        self.cb_get_data = NULL

    def __init__(self, evasObject Map, double lon, double lat):
        self.overlay = elm_map_overlay_add(Map.obj, lon, lat)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)

    def __del_cb(self):
        self.overlay = NULL
        Py_DECREF(self)

    def delete(self):
        if self.overlay == NULL:
            raise ValueError("Object already deleted")
        elm_map_overlay_del(self.overlay)

    def type_get(self):
        return elm_map_overlay_type_get(self.overlay)

    property type:
        def __get__(self):
            return self.type_get()

    def hide_set(self, hide):
        elm_map_overlay_hide_set(self.overlay, bool(hide))

    def hide_get(self):
        return bool(elm_map_overlay_hide_get(self.overlay))

    property hide:
        def __get__(self):
            return self.hide_get()
        def __set__(self, value):
            self.hide_set(value)

    def displayed_zoom_min_set(self, zoom):
        elm_map_overlay_displayed_zoom_min_set(self.overlay, zoom)

    def displayed_zoom_min_get(self):
        return elm_map_overlay_displayed_zoom_min_get(self.overlay)

    property displayed_zoom_min:
        def __get__(self):
            return self.displayed_zoom_min_get()
        def __set__(self, value):
            self.displayed_zoom_min_set(value)

    def paused_set(self, paused):
        elm_map_overlay_paused_set(self.overlay, paused)

    def paused_get(self):
        return elm_map_overlay_paused_get(self.overlay)

    property paused:
        def __get__(self):
            return self.paused_get()
        def __set__(self, value):
            self.paused_set(value)

    def visible_get(self):
        return bool(elm_map_overlay_visible_get(self.overlay))

    property visible:
        def __get__(self):
            return self.visible_get()

    def content_set(self, Object content):
        elm_map_overlay_content_set(self.overlay, content.obj)

    def content_get(self):
        cdef Evas_Object *obj = <Evas_Object *>elm_map_overlay_content_get(self.overlay)
        return Object_from_instance(obj)

    property content:
        def __get__(self):
            return self.content_get()
        def __set__(self, value):
            self.content_set(value)

    def icon_set(self, Object icon):
        elm_map_overlay_icon_set(self.overlay, icon.obj)

    def icon_get(self):
        cdef Evas_Object *obj = <Evas_Object *>elm_map_overlay_icon_get(self.overlay)
        return Object_from_instance(obj)

    property icon:
        def __get__(self):
            return self.icon_get()
        def __set__(self, value):
            self.icon_set(value)

    def region_set(self, lon, lat):
        elm_map_overlay_region_set(self.overlay, lon, lat)

    def region_get(self):
        cdef double lon, lat
        elm_map_overlay_region_get(self.overlay, &lon, &lat)
        return (lon, lat)

    property region:
        def __get__(self):
            return self.region_get()
        def __set__(self, value):
            self.region_set(*value)

    def color_set(self, r, g, b, a):
        elm_map_overlay_color_set(self.overlay, r, g, b, a)

    def color_get(self):
        cdef int r, g, b, a
        elm_map_overlay_color_get(self.overlay, &r, &g, &b, &a)
        return (r, g, b, a)

    property color:
        def __get__(self):
            return self.color_get()
        def __set__(self, value):
            self.color_set(*value)

    def show(self):
        elm_map_overlay_show(self.overlay)

    def callback_clicked_set(self, func, *args, **kwargs):
        if not callable(func):
            raise TypeError("func must be callable")
        cb_data = (func, args, kwargs)
        elm_map_overlay_get_cb_set(self.overlay, _map_overlay_get_callback,
                                                 <void *>cb_data)
        Py_INCREF(cb_data)
        self.cb_get_data

    def callback_clicked_unset(self, func):
        elm_map_overlay_get_cb_set(self.overlay, NULL, NULL)
        cb_data = <object>self.cb_get_data
        self.cb_get_data = NULL
        Py_DECREF(cb_data)


cdef class MapOverlayClass(MapOverlay):

    def __init__(self, evasObject Map):
        self.overlay = elm_map_overlay_class_add(Map.obj)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)

    def append(self, MapOverlay overlay):
        elm_map_overlay_class_append(self.overlay, overlay.overlay)

    def remove(self, MapOverlay overlay):
        elm_map_overlay_class_remove(self.overlay, overlay.overlay)

    def zoom_max_set(self, zoom):
        elm_map_overlay_class_zoom_max_set(self.overlay, zoom)

    def zoom_max_get(self):
        return elm_map_overlay_class_zoom_max_get(self.overlay)

    property zoom_max:
        def __get__(self):
            return self.zoom_max_get()
        def __set__(self, value):
            self.zoom_max_set(value)

    def members_get(self):
        cdef Eina_List *lst
        cdef Elm_Map_Overlay *ov
        lst = elm_map_overlay_group_members_get(self.overlay)# TODO this is somehow wrong... group <> class

        ret = []
        ret_append = ret.append
        while lst:
            ov = <Elm_Map_Overlay *>lst.data
            lst = lst.next
            o = _elm_map_overlay_to_python(ov)
            if o is not None:
                ret_append(o)
        return ret

    property members:
        def __get__(self):
            return self.members_get()


cdef class MapOverlayBubble(MapOverlay):

    def __init__(self, evasObject Map):
        self.overlay = elm_map_overlay_bubble_add(Map.obj)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)

    def follow(self, MapOverlay overlay):
        elm_map_overlay_bubble_follow(self.overlay, overlay.overlay)

    def content_append(self, evasObject content):
        elm_map_overlay_bubble_content_append(self.overlay, content.obj)

    def content_clear(self):
        elm_map_overlay_bubble_content_clear(self.overlay)


cdef class MapOverlayLine(MapOverlay):

    def __init__(self, evasObject Map, flon, flat, tlot, tlat):
        self.overlay = elm_map_overlay_line_add(Map.obj, flon, flat, tlot, tlat)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)


cdef class MapOverlayPolygon(MapOverlay):

    def __init__(self, evasObject Map):
        self.overlay = elm_map_overlay_polygon_add(Map.obj)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)

    def region_add(self, lon, lat):
        elm_map_overlay_polygon_region_add(self.overlay, lon, lat)


cdef class MapOverlayCircle(MapOverlay):

    def __init__(self, evasObject Map, lon, lat, radius):
        self.overlay = elm_map_overlay_circle_add(Map.obj, lon, lat, radius)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)


cdef class MapOverlayScale(MapOverlay):

    def __init__(self, evasObject Map, x, y):
        self.overlay = elm_map_overlay_scale_add(Map.obj, x, y)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)


cdef class MapOverlayRoute(MapOverlay):

    def __init__(self, evasObject Map, MapRoute route):
        self.overlay = elm_map_overlay_route_add(Map.obj, route.route)
        elm_map_overlay_data_set(self.overlay, <void *>self)
        elm_map_overlay_del_cb_set(self.overlay, _map_overlay_del_cb, <void *>self)
        Py_INCREF(self)


cdef public class Map(Object)[object PyElementaryMap, type PyElementaryMap_Type]:

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_map_add(parent.obj))

    def zoom_set(self, zoom):
        elm_map_zoom_set(self.obj, zoom)

    def zoom_get(self):
        return elm_map_zoom_get(self.obj)

    property zoom:
        def __get__(self):
            return self.zoom_get()
        def __set__(self, value):
            self.zoom_set(value)

    def zoom_mode_set(self, mode):
        elm_map_zoom_mode_set(self.obj, mode)

    def zoom_mode_get(self):
        return elm_map_zoom_mode_get(self.obj)

    property zoom_mode:
        def __get__(self):
            return self.zoom_mode_get()
        def __set__(self, value):
            self.zoom_mode_set(value)

    def zoom_min_set(self, zoom):
        elm_map_zoom_min_set(self.obj, zoom)

    def zoom_min_get(self):
        return elm_map_zoom_min_get(self.obj)

    property zoom_min:
        def __get__(self):
            return self.zoom_min_get()
        def __set__(self, value):
            self.zoom_min_set(value)

    def zoom_max_set(self, zoom):
        elm_map_zoom_max_set(self.obj, zoom)

    def zoom_max_get(self):
        return elm_map_zoom_max_get(self.obj)

    property zoom_max:
        def __get__(self):
            return self.zoom_max_get()
        def __set__(self, value):
            self.zoom_max_set(value)

    def region_get(self):
        cdef double lon, lat
        elm_map_region_get(self.obj, &lon, &lat)
        return (lon, lat)

    property region:
        def __get__(self):
            return self.region_get()

    def region_bring_in(self, lon, lat):
        elm_map_region_bring_in(self.obj, lon, lat)

    def region_show(self, lon, lat):
        elm_map_region_show(self.obj, lon, lat)

    def canvas_to_region_convert(self, x, y):
        cdef double lon, lat
        elm_map_canvas_to_region_convert(self.obj, x, y, &lon, &lat)
        return (lon, lat)

    def region_to_canvas_convert(self, lon, lat):
        cdef Evas_Coord x, y
        elm_map_region_to_canvas_convert(self.obj, lon, lat, &x, &y)
        return (x, y)

    def paused_set(self, paused):
        elm_map_paused_set(self.obj, bool(paused))

    def paused_get(self):
        return bool(elm_map_paused_get(self.obj))

    property paused:
        def __get__(self):
            return self.paused_get()
        def __set__(self, value):
            self.paused_set(value)

    def rotate_set(self, degree, cx, cy):
        elm_map_rotate_set(self.obj, degree, cx, cy)

    def rotate_get(self):
        cdef double degree
        cdef Evas_Coord cx, cy
        elm_map_rotate_get(self.obj, &degree, &cx, &cy)
        return (degree, cx, cy)

    property rotate:
        def __get__(self):
            return self.rotate_get()
        def __set__(self, value):
            self.rotate_set(*value)

    def wheel_disabled_set(self, disabled):
        elm_map_wheel_disabled_set(self.obj, bool(disabled))

    def wheel_disabled_get(self):
        return bool(elm_map_wheel_disabled_get(self.obj))

    property wheel_disabled:
        def __get__(self):
            return self.wheel_disabled_get()
        def __set__(self, value):
            self.wheel_disabled_set(value)

    def user_agent_set(self, user_agent):
        elm_map_user_agent_set(self.obj, user_agent)

    def user_agent_get(self):
        return elm_map_user_agent_get(self.obj)

    property user_agent:
        def __get__(self):
            return self.user_agent_get()
        def __set__(self, value):
            self.user_agent_set(value)

    def overlay_add(self, lon, lat):
        return MapOverlay(self, lon, lat)

    def overlays_get(self):
        cdef Eina_List *lst
        cdef Elm_Map_Overlay *ov
        lst = elm_map_overlays_get(self.obj)

        ret = []
        ret_append = ret.append
        while lst:
            ov = <Elm_Map_Overlay *>lst.data
            lst = lst.next
            o = _elm_map_overlay_to_python(ov)
            if o is not None:
                ret_append(o)
        return ret

    property overlays:
        def __get__(self):
            return self.overlays_get()

    def overlays_show(self, overlays):
        cdef Eina_List *lst

        lst = NULL
        for overlay in overlays:
            ov = <MapOverlay>overlay
            lst = eina_list_append(lst, ov.overlay)
        elm_map_overlays_show(lst)
        eina_list_free(lst)

    def overlay_bubble_add(self):
        return MapOverlayBubble(self)

    def overlay_line_add(self, flon, flat, tlon, tlat):
        return MapOverlayLine(self, flon, flat, tlon, tlat)

    def overlay_polygon_add(self):
        return MapOverlayPolygon(self)

    def overlay_circle_add(self, lon, lat, radius):
        return MapOverlayCircle(self, lon, lat, radius)

    def overlay_scale_add(self, x, y):
        return MapOverlayScale(self, x, y)

    def overlay_class_add(self):
        return MapOverlayClass(self)

    def overlay_route_add(self, route):
        return MapOverlayRoute(self, route)

    def tile_load_status_get(self):
        cdef int try_num, finish_num

        elm_map_tile_load_status_get(self.obj, &try_num, &finish_num)
        return (try_num, finish_num)

    property tile_load_status_get:
        def __get__(self):
            return self.tile_load_status_get_get()

    def sources_get(self, type):
        cdef const_char_ptr *lst

        i = 0
        ret = []
        lst = elm_map_sources_get(self.obj, type)
        while (lst[i]):
            ret.append(_ctouni(lst[i]))
            i += 1

        return ret

    def source_set(self, type, source_name):
        elm_map_source_set(self.obj, type, _cfruni(source_name))

    def source_get(self, type):
        return _ctouni(elm_map_source_get(self.obj, type))

    def route_add(self, Elm_Map_Route_Type type, Elm_Map_Route_Method method,
                        double flon, double flat, double tlon, double tlat,
                        route_cb, *args, **kwargs):
        return MapRoute(self, type, method, flon, flat, tlon, tlat,
                        route_cb, *args, **kwargs)

    def name_add(self, address, double lon, double lat,
                       name_cb, *args, **kwargs):
        return MapName(self, address, lon, lat, name_cb, *args, **kwargs)

    # TODO elm_map_track_add

    # TODO elm_map_track_remove

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        self._callback_add("clicked,double", func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del("clicked,double", func)

    def callback_press_add(self, func, *args, **kwargs):
        self._callback_add("press", func, *args, **kwargs)

    def callback_press_del(self, func):
        self._callback_del("press", func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        self._callback_add("longpressed", func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del("longpressed", func)

    def callback_scroll_add(self, func, *args, **kwargs):
        self._callback_add("scroll", func, *args, **kwargs)

    def callback_scroll_del(self, func):
        self._callback_del("scroll", func)

    def callback_scroll_drag_start_add(self, func, *args, **kwargs):
        self._callback_add("scroll,drag,start", func, *args, **kwargs)

    def callback_scroll_drag_start_del(self, func):
        self._callback_del("scroll,drag,start", func)

    def callback_scroll_drag_stop_add(self, func, *args, **kwargs):
        self._callback_add("scroll,drag,stop", func, *args, **kwargs)

    def callback_scroll_drag_stop_del(self, func):
        self._callback_del("scroll,drag,stop", func)

    def callback_scroll_anim_start_add(self, func, *args, **kwargs):
        self._callback_add("scroll,anim,start", func, *args, **kwargs)

    def callback_scroll_anim_start_del(self, func):
        self._callback_del("scroll,anim,start", func)

    def callback_scroll_anim_stop_add(self, func, *args, **kwargs):
        self._callback_add("scroll,anim,stop", func, *args, **kwargs)

    def callback_scroll_anim_stop_del(self, func):
        self._callback_del("scroll,anim,stop", func)

    def callback_zoom_start_add(self, func, *args, **kwargs):
        self._callback_add("zoom,start", func, *args, **kwargs)

    def callback_zoom_start_del(self, func):
        self._callback_del("zoom,start", func)

    def callback_zoom_stop_add(self, func, *args, **kwargs):
        self._callback_add("zoom,stop", func, *args, **kwargs)

    def callback_zoom_stop_del(self, func):
        self._callback_del("zoom,stop", func)

    def callback_zoom_change_add(self, func, *args, **kwargs):
        self._callback_add("zoom,change", func, *args, **kwargs)

    def callback_zoom_change_del(self, func):
        self._callback_del("zoom,change", func)

    def callback_tile_load_add(self, func, *args, **kwargs):
        self._callback_add("tile,load", func, *args, **kwargs)

    def callback_tile_load_del(self, func):
        self._callback_del("tile,load", func)

    def callback_tile_loaded_add(self, func, *args, **kwargs):
        self._callback_add("tile,loaded", func, *args, **kwargs)

    def callback_tile_loaded_del(self, func):
        self._callback_del("tile,loaded", func)

    def callback_tile_loaded_fail_add(self, func, *args, **kwargs):
        self._callback_add("tile,loaded,fail", func, *args, **kwargs)

    def callback_tile_loaded_fail_del(self, func):
        self._callback_del("tile,loaded,fail", func)

    def callback_route_load_add(self, func, *args, **kwargs):
        self._callback_add("route,load", func, *args, **kwargs)

    def callback_route_load_del(self, func):
        self._callback_del("route,load", func)

    def callback_route_loaded_add(self, func, *args, **kwargs):
        self._callback_add("route,loaded", func, *args, **kwargs)

    def callback_route_loaded_del(self, func):
        self._callback_del("route,loaded", func)

    def callback_route_loaded_fail_add(self, func, *args, **kwargs):
        self._callback_add("route,loaded,fail", func, *args, **kwargs)

    def callback_route_loaded_fail_del(self, func):
        self._callback_del("route,loaded,fail", func)

    def callback_name_load_add(self, func, *args, **kwargs):
        self._callback_add("name,load", func, *args, **kwargs)

    def callback_name_load_del(self, func):
        self._callback_del("name,load", func)

    def callback_name_loaded_add(self, func, *args, **kwargs):
        self._callback_add("name,loaded", func, *args, **kwargs)

    def callback_name_loaded_del(self, func):
        self._callback_del("name,loaded", func)

    def callback_name_loaded_fail_add(self, func, *args, **kwargs):
        self._callback_add("name,loaded,fail", func, *args, **kwargs)

    def callback_name_loaded_fail_del(self, func):
        self._callback_del("name,loaded,fail", func)

    def callback_overlay_clicked_add(self, func, *args, **kwargs):
        self._callback_add("overlay,clicked", func, *args, **kwargs)

    def callback_overlay_clicked_del(self, func):
        self._callback_del("overlay,clicked", func)

    def callback_overlay_del_add(self, func, *args, **kwargs):
        self._callback_add("overlay,del", func, *args, **kwargs)

    def callback_overlay_del_del(self, func):
        self._callback_del("overlay,del", func)

    def callback_loaded_add(self, func, *args, **kwargs):
        self._callback_add("loaded", func, *args, **kwargs)

    def callback_loaded_del(self, func):
        self._callback_del("loaded", func)


_elm_widget_type_register("map", Map)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryMap_Type # hack to install metaclass
_install_metaclass(&PyElementaryMap_Type, ElementaryObjectMeta)
