# Copyright (C) 2007-2008 Caio Marcelo de Oliveira Filho
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

cdef public class VSeparator(Separator) [object PyEtk_VSeparator, type PyEtk_VSeparator_Type]:
    def __init__(self, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_vseparator_new())
        self._set_common_params(**kargs)


cdef public class HSeparator(Separator) [object PyEtk_HSeparator, type PyEtk_HSeparator_Type]:
    def __init__(self, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_hseparator_new())
        self._set_common_params(**kargs)


cdef public class Separator(Widget) [object PyEtk_Separator, type PyEtk_Separator_Type]:
    pass


class SeparatorEnums:
    pass
