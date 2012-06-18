# Copyright (c)  2012 Davide Andreoli <dave@gurumeditation.it>
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

cdef public class Mapbuf(Object) [object PyElementaryMapbuf, type PyElementaryMapbuf_Type]:

    """This holds one content object and uses an Evas Map of transformation
    points to be later used with this content. So the content will be
    moved, resized, etc as a single image. So it will improve performance
    when you have a complex interface, with a lot of elements, and will
    need to resize or move it frequently (the content object and its
    children).

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_mapbuf_add(parent.obj))

    def enabled_set(self, enabled):
        """Enable or disable the map.

        @param enabled: A boolean to set if the map should me enabled.
        @type enabled: bool

        """
        elm_mapbuf_enabled_set(self.obj, enabled)

    def enabled_get(self):
        """Get the enabled state of the map.

        @return: A boolean to indicating if the map is enabled
        @rtype: bool

        """
        return bool(elm_mapbuf_enabled_get(self.obj))

    property enabled:
        """The enabled state of the map.

        @type: bool

        """
        def __get__(self):
            return bool(elm_mapbuf_enabled_get(self.obj))
        def __set__(self, enabled):
            elm_mapbuf_enabled_set(self.obj, enabled)

    def smooth_set(self, smooth):
        """Enable or disable smooth map rendering.

        This sets smoothing for map rendering. If the object is a type that has
        its own smoothing settings, then both the smooth settings for this object
        and the map must be turned off.

        By default smooth maps are enabled.

        @param smooth: A boolean to set if the map should me enabled.
        @type smooth: bool

        """
        elm_mapbuf_smooth_set(self.obj, smooth)

    def smooth_get(self):
        """Get the smooth state of the map.

        @return: A boolean to indicating if the map will use smooth rendering
        @rtype: bool

        """
        return bool(elm_mapbuf_smooth_get(self.obj))

    property smooth:
        """Smooth map rendering.

        This sets smoothing for map rendering. If the object is a type that has
        its own smoothing settings, then both the smooth settings for this object
        and the map must be turned off.

        By default smooth maps are enabled.

        @type: bool

        """
        def __get__(self):
            return bool(elm_mapbuf_smooth_get(self.obj))
        def __set__(self, smooth):
            elm_mapbuf_smooth_set(self.obj, smooth)

    def alpha_set(self, alpha):
        """Set or unset alpha flag for map rendering.

        @param alpha: A boolean to set if the map should use transparency
        @type alpha: bool

        """
        elm_mapbuf_alpha_set(self.obj, alpha)

    def alpha_get(self):
        """Get the alpha state of the map.

        @return: A boolean to indicating if the alpha is enabled
        @rtype: bool

        """
        return bool(elm_mapbuf_alpha_get(self.obj))

    property alpha:
        """The alpha state of the map.

        @type: bool

        """
        def __get__(self):
            return bool(elm_mapbuf_alpha_get(self.obj))
        def __set__(self, alpha):
            elm_mapbuf_alpha_set(self.obj, alpha)


_elm_widget_type_register("mapbuf", Mapbuf)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryMapbuf_Type # hack to install metaclass
_install_metaclass(&PyElementaryMapbuf_Type, ElementaryObjectMeta)
