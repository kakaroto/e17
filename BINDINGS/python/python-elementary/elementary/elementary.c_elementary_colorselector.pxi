# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
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

cdef class ColorselectorPaletteItem(ObjectItem):
    def __init__(self, c_evas.Object colorselector, r, g, b, a):
        self.item = elm_colorselector_palette_color_add(colorselector.obj, r, g, b, a)

    def color_get(self):
        cdef int r, g, b, a
        elm_colorselector_palette_item_color_get(self.item, &r, &g, &b, &a)
        return (r, g, b, a)

    def color_set(self, r, g, b, a):
        elm_colorselector_palette_item_color_set(self.item, r, g, b, a)

    property color:
        def __get__(self):
            return self.color_get()
        def __set__(self, value):
            self.color_set(value)

def _colorselector_item_conv(long addr):
    cdef Elm_Object_Item *it = <Elm_Object_Item *>addr
    cdef void *data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    else:
        prm = <object>data
        return prm[2]

cdef public class Colorselector(Object) [object PyElementaryColorselector, type PyElementaryColorselector_Type]:

    """A Colorselector is a color selection widget.

    It allows application to set a series of colors. It also allows to load/save
    colors from/to config with a unique identifier, by default, the colors are
    loaded/saved from/to config using "default" identifier. The colors
    can be picked by user from the color set by clicking on individual
    color item on the palette or by selecting it from selector.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
      - @c "changed" - When the color value changes on selector
      - @c "color,item,selected" - When user clicks on color item.
        The event_info parameter of the callback will be the selected color item.
      - @c "color,item,longpressed" - When user long presses on color item.
        The event_info parameter of the callback will be the selected color item.

    """

    def __init__(self, c_evas.Object parent):
        """Add a new colorselector to the parent

        @param parent: The parent object
        @type parent: L{Object}
        @return: The new object or None if it cannot be created
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_colorselector_add(parent.obj))

    def color_set(self, r, g, b, a):
        """Set color to colorselector

        @param r: r-value of color
        @type r: int
        @param g: g-value of color
        @type g: int
        @param b: b-value of color
        @type b: int
        @param a: a-value of color
        @type a: int

        """
        elm_colorselector_color_set(self.obj, r, g, b, a)

    def color_get(self):
        """Get the current color from colorselector

        @return: Tuple of the current colors
        @rtype: tuple of ints

        """
        cdef int r, g, b, a
        elm_colorselector_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    property color:
        """The current color (r, g, b, a).

        @type: tuple of ints

        """
        def __get__(self):
            return self.color_get()
        def __set__(self, value):
            self.color_set(value)

    def mode_set(self, mode):
        """Set Colorselector's mode.

        Colorselector supports three modes palette only, selector only and both.

        @param mode: The current mode of colorselector
        @type mode: Elm_Colorselector_Mode

        """
        elm_colorselector_mode_set(self.obj, mode)

    def mode_get(self):
        """Get Colorselector's mode.

        @return: The current mode of colorselector
        @rtype: Elm_Colorselector_Mode

        """
        return elm_colorselector_mode_get(self.obj)

    property mode:
        """Colorselector's mode.

        Colorselector supports three modes palette only, selector only and both.

        @type: Elm_Colorselector_Mode

        """
        def __get__(self):
            return self.mode_get()
        def __set__(self, mode):
            self.mode_set(mode)

    def palette_color_add(self, r, g, b, a):
        """Add a new color item to palette.

        @param r: r-value of color
        @type r: int
        @param g: g-value of color
        @type g: int
        @param b: b-value of color
        @type b: int
        @param a: a-value of color
        @type a: int
        @return: A new color palette Item.
        @rtype: L{ColorSelectorPaletteItem}

        """
        return ColorselectorPaletteItem(self, r, g, b, a)

    def palette_clear(self):
        """Clear the palette items."""
        elm_colorselector_palette_clear(self.obj)

    def palette_name_set(self, palette_name):
        """Set current palette's name

        When colorpalette name is set, colors will be loaded from and saved to config
        using the set name. If no name is set then colors will be loaded from or
        saved to "default" config.

        @param palette_name: Name of palette
        @type palette_name: string

        """
        elm_colorselector_palette_name_set(self.obj, palette_name)

    def palette_name_get(self):
        """Get current palette's name

        Returns the currently set palette name using which colors will be
        saved/loaded in to config.

        @return: Name of palette
        @rtype: string

        """
        return elm_colorselector_palette_name_get(self.obj)

    property palette_name:
        """Current palette's name.

        @type: string

        """
        def __get__(self):
            return self.palette_name_get()
        def __set__(self, palette_name):
            self.palette_name_set(palette_name)

    def callback_selected_add(self, func, *args, **kwargs):
        """When the color value changes on selector"""
        self._callback_add("selected", func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del("selected", func)

    def callback_color_item_selected_add(self, func, *args, **kwargs):
        """When user clicks on color item.
        The event_info parameter of the callback will be the selected color item.

        """
        self._callback_add_full("color,item,selected", _colorselector_item_conv, func, *args, **kwargs)

    def callback_color_item_selected_del(self, func):
        self._callback_del_full("color,item,selected", _colorselector_item_conv, func)

    def callback_color_item_longpressed_add(self, func, *args, **kwargs):
        """When user long presses on color item.
        The event_info parameter of the callback will be the selected color item.

        """
        self._callback_add_full("color,item,longpressed", _colorselector_item_conv, func, *args, **kwargs)

    def callback_color_item_longpressed_del(self, func):
        self._callback_del_full("color,item,longpressed", _colorselector_item_conv, func)

_elm_widget_type_register("colorselector", Colorselector)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryColorselector_Type # hack to install metaclass
_install_metaclass(&PyElementaryColorselector_Type, ElementaryObjectMeta)
