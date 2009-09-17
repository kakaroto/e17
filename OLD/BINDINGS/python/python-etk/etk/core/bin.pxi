# Copyright (C) 2007-2008 Caio Marcelo de Oliveira Filho, Gustavo Sverzut Barbieri
#
# This file is part of Python-Etk.
#
# Python-Etk is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Etk is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Etk.  If not, see <http://www.gnu.org/licenses/>.

cdef public class Bin(Container) [object PyEtk_Bin, type PyEtk_Bin_Type]:
    def child_get(self):
        __ret = Object_from_instance(<Etk_Object*>etk_bin_child_get(<Etk_Bin*>self.obj))
        return (__ret)

    def child_set(self, Widget child):
        etk_bin_child_set(<Etk_Bin*>self.obj, <Etk_Widget*>child.obj)

    property child:
        def __get__(self):
            return self.child_get()

        def __set__(self, child):
            self.child_set(child)

    def _set_common_params(self, child=None, **kargs):
        if child is not None:
            self.child_set(child)

        if kargs:
            Container._set_common_params(self, **kargs)


class BinEnums:
    pass
