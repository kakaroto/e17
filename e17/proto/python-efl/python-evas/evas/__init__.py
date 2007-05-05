#!/usr/bin/env python2

import c_evas

init = c_evas.init
shutdown = c_evas.shutdown
render_method_lookup = c_evas.render_method_lookup
render_method_list = c_evas.render_method_list
color_argb_premul = c_evas.color_argb_premul
color_argb_unpremul = c_evas.color_argb_unpremul
color_rgb_to_hsv = c_evas.color_rgb_to_hsv
color_hsv_to_rgb = c_evas.color_hsv_to_rgb

EVAS_CALLBACK_MOUSE_IN = 0
EVAS_CALLBACK_MOUSE_OUT = 1
EVAS_CALLBACK_MOUSE_DOWN = 2
EVAS_CALLBACK_MOUSE_UP = 3
EVAS_CALLBACK_MOUSE_MOVE = 4
EVAS_CALLBACK_MOUSE_WHEEL = 5
EVAS_CALLBACK_FREE = 6
EVAS_CALLBACK_KEY_DOWN = 7
EVAS_CALLBACK_KEY_UP = 8
EVAS_CALLBACK_FOCUS_IN = 9
EVAS_CALLBACK_FOCUS_OUT = 10
EVAS_CALLBACK_SHOW = 11
EVAS_CALLBACK_HIDE = 12
EVAS_CALLBACK_MOVE = 13
EVAS_CALLBACK_RESIZE = 14
EVAS_CALLBACK_RESTACK = 15

EVAS_BUTTON_NONE = 0
EVAS_BUTTON_DOUBLE_CLICK = 1
EVAS_BUTTON_TRIPLE_CLICK = 2

EVAS_RENDER_BLEND = 0
EVAS_RENDER_BLEND_REL = 1
EVAS_RENDER_COPY = 2
EVAS_RENDER_COPY_REL = 3
EVAS_RENDER_ADD = 4
EVAS_RENDER_ADD_REL = 5
EVAS_RENDER_SUB = 6
EVAS_RENDER_SUB_REL = 7
EVAS_RENDER_TINT = 8
EVAS_RENDER_TINT_REL = 9
EVAS_RENDER_MASK = 10
EVAS_RENDER_MUL = 11

EVAS_COLOR_SPACE_ARGB = 0
EVAS_COLOR_SPACE_AHSV = 1

EVAS_FONT_HINTING_NONE = 0
EVAS_FONT_HINTING_AUTO = 1
EVAS_FONT_HINTING_BYTECODE = 2


Rect = c_evas.Rect

class Canvas(c_evas.Canvas):
    def __new__(type, method=None, size=None, viewport=None):
        obj = c_evas.Canvas.__new__(type)
        obj._new_evas()
        if method:
            obj.output_method_set(method)
        if size:
            obj.size_set(*size)
        if viewport:
            obj.viewport_set(*viewport)
        return obj

class SmartObject(c_evas.SmartObject):
    def __new__(type, canvas, size=None, pos=None, geometry=None, color=None,
                name=None, *args, **kargs):
        if type is SmartObject or type is c_evas.SmartObject:
            raise TypeError("Must not instantiate SmartObject, but subclasses")
        obj = c_evas.SmartObject.__new__(type, canvas)
        obj._new_obj()
        if size:
            obj.size_set(*size)
        if pos:
            obj.pos_set(*pos)
        if geometry:
            obj.geometry_set(*geometry)
        if color:
            obj.color_set(*c_evas.color_parse(color))
        if name:
            obj.name_set(name)
        return obj


class Rectangle(c_evas.Rectangle):
    def __new__(type, canvas, size=None, pos=None, geometry=None, color=None,
                name=None):
        obj = c_evas.Rectangle.__new__(type, canvas)
        obj._new_obj()
        if size:
            obj.size_set(*size)
        if pos:
            obj.pos_set(*pos)
        if geometry:
            obj.geometry_set(*geometry)
        if color:
            obj.color_set(*c_evas.color_parse(color))
        if name:
            obj.name_set(name)
        return obj


class Line(c_evas.Line):
    def __new__(type, canvas, start=None, end=None, size=None, pos=None,
                geometry=None, color=None, name=None):
        obj = c_evas.Line.__new__(type, canvas)
        obj._new_obj()

        if start and end:
            x1 = start[0]
            y1 = start[1]
            x2 = end[0]
            y2 = end[1]

            w = x2 - x1
            h = y2 - y1
            if w < 0:
                w = -w
                x = x2
            else:
                x = x1

            if h < 0:
                h = -h
                y = y2
            else:
                y = y1

            obj.xy_set(x1, y1, x2, y2)

            if not geometry:
                if not size:
                    obj.size_set(w, h)
                if not pos:
                    obj.pos_set(x, y)

        elif start:
            obj.start_set(*start)
        elif end:
            obj.end_set(*end)

        if size:
            obj.size_set(*size)
        if pos:
            obj.pos_set(*pos)
        if geometry:
            obj.geometry_set(*geometry)
        if color:
            obj.color_set(*c_evas.color_parse(color))
        if name:
            obj.name_set(name)
        return obj


class Image(c_evas.Image):
    def __new__(type, canvas, file=None, size=None, pos=None, geometry=None,
                color=None, name=None):
        obj = c_evas.Image.__new__(type, canvas)
        obj._new_obj()
        if size:
            obj.size_set(*size)
        if pos:
            obj.pos_set(*pos)
        if geometry:
            obj.geometry_set(*geometry)
        if color:
            obj.color_set(*c_evas.color_parse(color))
        if name:
            obj.name_set(name)
        if file:
            if isinstance(file, basestring):
                file = (file, None)
            obj.file_set(*file)
        return obj
