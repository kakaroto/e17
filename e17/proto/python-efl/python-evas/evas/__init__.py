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

EVAS_TEXTURE_REFLECT = 0
EVAS_TEXTURE_REPEAT = 1
EVAS_TEXTURE_RESTRICT = 2
EVAS_TEXTURE_RESTRICT_REFLECT = 3
EVAS_TEXTURE_RESTRICT_REPEAT = 4
EVAS_TEXTURE_PAD = 5

EVAS_COLOR_SPACE_ARGB = 0
EVAS_COLOR_SPACE_AHSV = 1

EVAS_FONT_HINTING_NONE = 0
EVAS_FONT_HINTING_AUTO = 1
EVAS_FONT_HINTING_BYTECODE = 2

EVAS_TEXT_STYLE_PLAIN = 0
EVAS_TEXT_STYLE_SHADOW = 1
EVAS_TEXT_STYLE_OUTLINE = 2
EVAS_TEXT_STYLE_SOFT_OUTLINE = 3
EVAS_TEXT_STYLE_GLOW = 4
EVAS_TEXT_STYLE_OUTLINE_SHADOW = 5
EVAS_TEXT_STYLE_FAR_SHADOW = 6
EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW = 7
EVAS_TEXT_STYLE_SOFT_SHADOW = 8
EVAS_TEXT_STYLE_FAR_SOFT_SHADOW = 9

EvasLoadError = c_evas.EvasLoadError
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
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj


class Rectangle(c_evas.Rectangle):
    def __new__(type, canvas, size=None, pos=None, geometry=None, color=None,
                name=None):
        obj = c_evas.Rectangle.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj


class Line(c_evas.Line):
    def __new__(type, canvas, start=None, end=None, size=None, pos=None,
                geometry=None, color=None, name=None):
        obj = c_evas.Line.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(start=start, end=end, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj


class Image(c_evas.Image):
    def __new__(type, canvas, file=None, size=None, pos=None, geometry=None,
                color=None, name=None):
        obj = c_evas.Image.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(file=file, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj


class Gradient(c_evas.Gradient):
    def __new__(type, canvas, size=None, pos=None, geometry=None, color=None,
                name=None):
        obj = c_evas.Gradient.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj


class Polygon(c_evas.Polygon):
    def __new__(type, canvas, points=None, size=None, pos=None, geometry=None,
                color=None, name=None):
        obj = c_evas.Polygon.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(points=points, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj


class Text(c_evas.Text):
    def __new__(type, canvas, text=None, font=None, font_source=None,
                style=None, shadow_color=None, glow_color=None,
                glow2_color=None, outline_color=None, size=None, pos=None,
                geometry=None, color=None, name=None):
        obj = c_evas.Text.__new__(type, canvas)
        obj._new_obj()
        obj._set_common_params(text=text, font=font, font_source=font_source,
                               style=style, shadow_color=shadow_color,
                               glow_color=glow_color, glow2_color=glow2_color,
                               outline_color=outline_color, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj
