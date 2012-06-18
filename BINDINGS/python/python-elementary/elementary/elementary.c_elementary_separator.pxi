# Copyright (c) 2009 Boris Faure
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

cdef public class Separator(LayoutClass) [object PyElementarySeparator, type PyElementarySeparator_Type]:

    """Separator is a very thin object used to separate other objects.

    A separator can be vertical or horizontal.

    This widget emits the signals coming from L{Layout}.

    """

    def __init__(self, evasObject parent):
        self._set_obj(elm_separator_add(parent.obj))

    def horizontal_set(self, b):
        """Set the horizontal mode of a separator object

        @param horizontal: If True, the separator is horizontal
        @type horizontal: bool

        """
        elm_separator_horizontal_set(self.obj, b)

    def horizontal_get(self):
        """Get the horizontal mode of a separator object

        @see: L{horizontal_set()}

        @return: If True, the separator is horizontal
        @rtype: bool

        """
        return elm_separator_horizontal_get(self.obj)

    property horizontal:
        """The horizontal mode of a separator object

        @type: bool

        """
        def __get__(self):
            return elm_separator_horizontal_get(self.obj)

        def __set__(self, b):
            elm_separator_horizontal_set(self.obj, b)

_elm_widget_type_register("separator", Separator)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementarySeparator_Type # hack to install metaclass
_install_metaclass(&PyElementarySeparator_Type, ElementaryObjectMeta)
