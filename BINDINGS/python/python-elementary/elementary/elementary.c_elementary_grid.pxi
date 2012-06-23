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

cdef public class Grid(Object) [object PyElementaryGrid, type PyElementaryGrid_Type]:

    """A grid layout widget.

    The grid is a grid layout widget that lays out a series of children as a
    fixed "grid" of widgets using a given percentage of the grid width and
    height each using the child object.

    The Grid uses a "Virtual resolution" that is stretched to fill the grid
    widgets size itself. The default is 100 x 100, so that means the
    position and sizes of children will effectively be percentages (0 to 100)
    of the width or height of the grid widget.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_grid_add(parent.obj))

    def size_set(self, w, h):
        """Set the virtual size of the grid.

        @param w: The virtual width of the grid
        @type w: Evas_Coord (int)
        @param h: The virtual height of the grid
        @type h: Evas_Coord (int)

        """
        elm_grid_size_set(self.obj, w, h)

    def size_get(self):
        """Get the virtual size of the grid.

        @return: The virtual width and height of the grid
        @rtype: tuple of Evas_Coords (int)

        """
        cdef Evas_Coord w, h
        elm_grid_size_get(self.obj, &w, &h)
        return (w, h)

    property size:
        """The virtual size (width and height) of the grid.

        @type: tuple of Evas_Coords (int)

        """
        def __set__(self, value):
            w, h = value
            elm_grid_size_set(self.obj, w, h)

        def __get__(self):
            cdef Evas_Coord w, h
            elm_grid_size_get(self.obj, &w, &h)
            return (w, h)

    def pack(self, evasObject subobj, x, y, w, h):
        """pack(subobj, x, y, w, h)

        Pack child at given position and size

        @param subobj: The child to pack
        @type subobj: L{Object}
        @param x: The virtual x coord at which to pack it
        @type x: Evas_Coord (int)
        @param y: The virtual y coord at which to pack it
        @type y: Evas_Coord (int)
        @param w: The virtual width at which to pack it
        @type w: Evas_Coord (int)
        @param h: The virtual height at which to pack it
        @type h: Evas_Coord (int)

        """
        elm_grid_pack(self.obj, subobj.obj, x, y, w, h)

    def unpack(self, evasObject subobj):
        """unpack(subobj)

        Unpack a child from a grid object

        @param subobj: The child to unpack
        @type subobj: L{Object}

        """
        elm_grid_unpack(self.obj, subobj.obj)

    def clear(self, clear):
        """clear(clear)

        Faster way to remove all child objects from a grid object.

        @param clear: If True, it will delete just removed children
        @type clear: bool

        """
        elm_grid_clear(self.obj, clear)

    def pack_set(self, evasObject subobj, x, y, w, h):
        """pack_set(subobj, x, y, w, h)

        Set packing of an existing child at to position and size

        @param subobj: The child to set packing of
        @type subobj: L{Object}
        @param x: The virtual x coord at which to pack it
        @type x: Evas_Coord (int)
        @param y: The virtual y coord at which to pack it
        @type y: Evas_Coord (int)
        @param w: The virtual width at which to pack it
        @type w: Evas_Coord (int)
        @param h: The virtual height at which to pack it
        @type h: Evas_Coord (int)

        """
        elm_grid_pack_set(subobj.obj, x, y, w, h)

    def pack_get(self, evasObject subobj):
        """pack_get(subobj)

        Get packing of a child

        @param subobj: The child to query
        @type subobj: L{Object}

        return: The position and size
        rtype: tuple of Evas_Coords (int)

        """
        cdef Evas_Coord x, y, w, h
        elm_grid_pack_get(subobj.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def children_get(self):
        """Get the list of the children for the grid.

        @return: The list of children
        @rtype: tuple of L{Object}s

        """
        return _object_list_to_python(elm_box_children_get(self.obj))
#~         cdef Evas_Object *o
#~         cdef Object obj
#~         cdef Eina_List *lst
#~
#~         ret = []
#~         lst = elm_box_children_get(self.obj)
#~         while lst:
#~             o = <Evas_Object *> lst.data
#~             obj = <Object>evas_object_data_get(o, "python-evas")
#~             ret.append(obj)
#~             lst = lst.next
#~         return ret

    property children:
        """Get the list of the children for the grid.

        @type: tuple of L{Object}s

        """
        def __get__(self):
            return _object_list_to_python(elm_box_children_get(self.obj))

_elm_widget_type_register("grid", Grid)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryGrid_Type # hack to install metaclass
_install_metaclass(&PyElementaryGrid_Type, ElementaryObjectMeta)
