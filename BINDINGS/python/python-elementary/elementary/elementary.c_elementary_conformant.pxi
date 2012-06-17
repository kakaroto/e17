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

cdef public class Conformant(LayoutClass) [object PyElementaryConformant, type PyElementaryConformant_Type]:

    """The aim is to provide a widget that can be used in elementary apps to
    account for space taken up by the indicator, virtual keypad & softkey
    windows when running the illume2 module of E17.

    So conformant content will be sized and positioned considering the
    space required for such stuff, and when they popup, as a keyboard
    shows when an entry is selected, conformant content won't change.

    This widget emits the signals sent from L{Layout}.

    Available styles for it:
        - C{"default"}

    Default content parts of the conformant widget that you can use for are:
        - "default" - A content of the conformant

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_conformant_add(parent.obj))

_elm_widget_type_register("conformant", Conformant)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryConformant_Type # hack to install metaclass
_install_metaclass(&PyElementaryConformant_Type, ElementaryObjectMeta)
