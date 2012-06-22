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

cdef public class Icon(Image) [object PyElementaryIcon, type PyElementaryIcon_Type]:

    """An icon object is used to display standard icon images ("delete",
    "edit", "arrows", etc.) or images coming from a custom file (PNG, JPG,
    EDJE, etc.), on icon contexts.

    The icon image requested can be in the Elementary theme in use, or in
    the C{freedesktop.org} theme paths. It's possible to set the order of
    preference from where an image will be fetched.

    This widget inherits from the L{Image} one, so that all the functions
    acting on it also work for icon objects.

    You should be using an icon, instead of an image, whenever one of the
    following apply:
        - you need a B{thumbnail} version of an original image
        - you need freedesktop.org provided icon images
        - you need theme provided icon images (Edje groups)

    Default images provided by Elementary's default theme are described below.

    These are names for icons that were first intended to be used in
    toolbars, but can be used in many other places too:
        - C{"home"}
        - C{"close"}
        - C{"apps"}
        - C{"arrow_up"}
        - C{"arrow_down"}
        - C{"arrow_left"}
        - C{"arrow_right"}
        - C{"chat"}
        - C{"clock"}
        - C{"delete"}
        - C{"edit"}
        - C{"refresh"}
        - C{"folder"}
        - C{"file"}

    These are names for icons that were designed to be used in menus
    (but again, you can use them anywhere else):
        - C{"menu/home"}
        - C{"menu/close"}
        - C{"menu/apps"}
        - C{"menu/arrow_up"}
        - C{"menu/arrow_down"}
        - C{"menu/arrow_left"}
        - C{"menu/arrow_right"}
        - C{"menu/chat"}
        - C{"menu/clock"}
        - C{"menu/delete"}
        - C{"menu/edit"}
        - C{"menu/refresh"}
        - C{"menu/folder"}
        - C{"menu/file"}

    And these are names for some media player specific icons:
        - C{"media_player/forward"}
        - C{"media_player/info"}
        - C{"media_player/next"}
        - C{"media_player/pause"}
        - C{"media_player/play"}
        - C{"media_player/prev"}
        - C{"media_player/rewind"}
        - C{"media_player/stop"}

    This widget emits the following signals, besides the ones sent from
    L{Image}:
        - C{"thumb,done"} - L{thumb_set()} has completed with success
        - C{"thumb,error"} - L{thumb_set()} has failed

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_icon_add(parent.obj))

    def thumb_set(self, filename, group = None):
        """Set the file that will be used, but use a generated thumbnail.

        This functions like L{Image.file_set()} but requires the Ethumb library
        support to be enabled successfully with elm_need_ethumb(). When set
        the file indicated has a thumbnail generated and cached on disk for
        future use or will directly use an existing cached thumbnail if it
        is valid.

        @see: L{Image.file_set()}

        @param file: The path to file that will be used as icon image
        @type file: string
        @param group: The group that the icon belongs to an edje file
        @type group: string

        """
        if group == None:
            elm_icon_thumb_set(self.obj, _cfruni(filename), NULL)
        else:
            elm_icon_thumb_set(self.obj, _cfruni(filename), _cfruni(group))

    property thumb:
        """Set the file (and edje group) that will be used, but use a
        generated thumbnail.

        This functions like L{Image.file_set()} but requires the Ethumb
        library support to be enabled successfully with elm_need_ethumb().
        When set the file indicated has a thumbnail generated and cached on
        disk for future use or will directly use an existing cached
        thumbnail if it is valid.

        @see: L{Image.file}

        @type: string C{file} or tuple(string C{file}, string C{group})

        """
        def __set__(self, value):
            if isinstance(value, tuple):
                filename, group = value
            else:
                filename = value
                group = None
            # TODO: check return value
            elm_icon_thumb_set(self.obj, _cfruni(filename), _cfruni(group))

    def standard_set(self, name):
        """Set the icon by icon standards names.

        For example, freedesktop.org defines standard icon names such as "home",
        "network", etc. There can be different icon sets to match those icon
        keys. The C{name} given as parameter is one of these "keys", and will be
        used to look in the freedesktop.org paths and elementary theme. One can
        change the lookup order with L{order_lookup_set()}.

        If name is not found in any of the expected locations and it is the
        absolute path of an image file, this image will be used.

        @note: The icon image set by this function can be changed by
            L{Image.file_set()}.

        @see: L{standard_get()}
        @see: L{Image.file_set()}

        @param name: The icon name
        @type name: string
        @return: (C{True} = success, C{False} = error)
        @rtype: bool

        """
        return bool(elm_icon_standard_set(self.obj, _cfruni(name)))

    def standard_get(self):
        """Get the icon name set by icon standard names.

        If the icon image was set using L{Image.file_set()} instead of
        L{standard_set()}, then this function will return C{None}.

        @see: L{standard_set()}

        @return: The icon name
        @rtype: string

        """
        return _ctouni(elm_icon_standard_get(self.obj))

    property standard:
        """The icon standards name.

        For example, freedesktop.org defines standard icon names such as "home",
        "network", etc. There can be different icon sets to match those icon
        keys. The C{name} given as parameter is one of these "keys", and will be
        used to look in the freedesktop.org paths and elementary theme. One can
        change the lookup order with L{order_lookup_set()}.

        If name is not found in any of the expected locations and it is the
        absolute path of an image file, this image will be used.

        @note: The icon image set can be changed by L{Image.file}.

        @see: L{Image.file}

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_icon_standard_get(self.obj))
        def __set__(self, name):
            # TODO: check return value
            elm_icon_standard_set(self.obj, _cfruni(name))

    def order_lookup_set(self, order):
        """Sets the icon lookup order used by L{standard_set()}.

        @see: L{order_lookup_get()}

        @param order: The icon lookup order (can be one of
            ELM_ICON_LOOKUP_FDO_THEME, ELM_ICON_LOOKUP_THEME_FDO, ELM_ICON_LOOKUP_FDO
            or ELM_ICON_LOOKUP_THEME)
        @type order: Elm_Icon_Lookup_Order

        """
        elm_icon_order_lookup_set(self.obj, order)

    def order_lookup_get(self):
        """Gets the icon lookup order.

        @see: L{order_lookup_set()}

        @return: The icon lookup order
        @rtype: Elm_Icon_Lookup_Order

        """
        return elm_icon_order_lookup_get(self.obj)

    property order_lookup:
        """The icon lookup order used by L{standard_set()}.

        @type: Elm_Icon_Lookup_Order

        """
        def __get__(self):
            return elm_icon_order_lookup_get(self.obj)
        def __set__(self, order):
            elm_icon_order_lookup_set(self.obj, order)

    def callback_thumb_done_add(self, func, *args, **kwargs):
        """L{thumb_set()} has completed with success."""
        self._callback_add("thumb,done", func, *args, **kwargs)

    def callback_thumb_done_del(self, func):
        self._callback_del("thumb,done", func)

    def callback_thumb_error_add(self, func, *args, **kwargs):
        """L{thumb_set()} has failed."""
        self._callback_add("thumb,error", func, *args, **kwargs)

    def callback_thumb_error_del(self, func):
        self._callback_del("thumb,error", func)

_elm_widget_type_register("icon", Icon)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryIcon_Type # hack to install metaclass
_install_metaclass(&PyElementaryIcon_Type, ElementaryObjectMeta)
