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

cdef public class Table(Object) [object PyElementaryTable, type PyElementaryTable_Type]:

    """A container widget to arrange other widgets in a table where items can
    span multiple columns or rows - even overlap (and then be raised or
    lowered accordingly to adjust stacking if they do overlap).

    The row and column count is not fixed. The table widget adjusts itself when
    subobjects are added to it dynamically.

    The most common way to use a table is::
        table = Table(win)
        table.show()
        table.padding_set(table, space_between_columns, space_between_rows)
        table.pack(table_content_object, x_coord, y_coord, colspan, rowspan)
        table.pack(table_content_object, next_x_coord, next_y_coord, colspan, rowspan)
        table.pack(table_content_object, other_x_coord, other_y_coord, colspan, rowspan)

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_table_add(parent.obj))

    def homogeneous_set(self, homogeneous):
        """Set the homogeneous layout in the table

        @param homogeneous: A boolean to set if the layout is homogeneous in
            the table (True = homogeneous,  False = no homogeneous)
        @type homogeneous: bool

        """
        elm_table_homogeneous_set(self.obj, homogeneous)

    def homogeneous_get(self):
        """Get the current table homogeneous mode.

        @return: A boolean to indicating if the layout is homogeneous in the
            table (True = homogeneous,  False = no homogeneous)
        @rtype: bool

        """
        return elm_table_homogeneous_get(self.obj)

    property homogeneous:
        """The homogeneous layout in the table

        @type: bool

        """
        def __get__(self):
            return elm_table_homogeneous_get(self.obj)

        def __set__(self, homogeneous):
            elm_table_homogeneous_set(self.obj, homogeneous)

    def padding_set(self, horizontal, vertical):
        """Set padding between cells.

        Default value is 0.

        @param horizontal: set the horizontal padding.
        @type horizontal: Evas_Coord (int)
        @param vertical: set the vertical padding.
        @type vertical: Evas_Coord (int)

        """
        elm_table_padding_set(self.obj, horizontal, vertical)

    def padding_get(self):
        """Get padding between cells.

        @return: The horizontal and vertical padding
        @rtype: tuple of Evas_Coords (int)

        """
        cdef Evas_Coord horizontal, vertical
        elm_table_padding_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property padding:
        """Padding between cells.

        Default value is 0.

        @type: (int, int)

        """
        def __get__(self):
            cdef Evas_Coord horizontal, vertical
            elm_table_padding_get(self.obj, &horizontal, &vertical)
            return (horizontal, vertical)

        def __set__(self, value):
            horizontal, vertical = value
            elm_table_padding_set(self.obj, horizontal, vertical)

    def pack(self, evasObject subobj, x, y, w, h):
        """pack(subobj, x, y, w, h)

        Add a subobject on the table with the coordinates passed

        @note: All positioning inside the table is relative to rows and
            columns, so a value of 0 for x and y, means the top left cell of
            the table, and a value of 1 for w and h means C{subobj} only
            takes that 1 cell.

        @param subobj: The subobject to be added to the table
        @type subobj: L{Object}
        @param x: Row number
        @type x: int
        @param y: Column number
        @type y: int
        @param w: colspan
        @type w: int
        @param h: rowspan
        @type h: int

        """
        elm_table_pack(self.obj, subobj.obj, x, y, w, h)

    def unpack(self, evasObject subobj):
        """unpack(subobj)

        Remove child from table.

        @param subobj: The subobject
        @type subobj: L{Object}

        """
        elm_table_unpack(self.obj, subobj.obj)

    def clear(self, clear):
        """clear(clear)

        Faster way to remove all child objects from a table object.

        @param clear: If True, will delete children, else just remove from table.
        @type clear: bool

        """
        elm_table_clear(self.obj, clear)

    def pack_set(evasObject subobj, x, y, w, h):
        """pack_set(subobj, x, y, w, h)

        Set the packing location of an existing child of the table

        Modifies the position of an object already in the table.

        @note: All positioning inside the table is relative to rows and
            columns, so a value of 0 for x and y, means the top left cell of
            the table, and a value of 1 for w and h means C{subobj} only
            takes that 1 cell.

        @param subobj: The subobject to be modified in the table
        @type subobj: L{Object}
        @param x: Row number
        @type x: int
        @param y: Column number
        @type y: int
        @param w: rowspan
        @type w: int
        @param h: colspan
        @type h: int

        """
        elm_table_pack_set(subobj.obj, x, y, w, h)

    def pack_get(evasObject subobj):
        """pack_get(subobj)

        Get the packing location of an existing child of the table

        @see: L{pack_set()}

        @param subobj: The subobject to be modified in the table
        @type subobj: L{Object}
        @return: Row number, Column number, rowspan, colspan
        @rtype: tuple of ints

        """
        cdef int x, y, w, h
        elm_table_pack_get(subobj.obj, &x, &y, &w, &h)
        return (x, y, w, h)

_elm_widget_type_register("table", Table)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryTable_Type # hack to install metaclass
_install_metaclass(&PyElementaryTable_Type, ElementaryObjectMeta)
