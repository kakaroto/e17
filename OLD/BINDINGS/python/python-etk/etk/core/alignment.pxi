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

cdef public class Alignment(Bin) [object PyEtk_Alignment, type PyEtk_Alignment_Type]:
    def __init__(self, xalign=0, yalign=0, xscale=0, yscale=0, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_alignment_new(xalign, yalign,
                                                         xscale, yscale))
        self._set_common_params(**kargs)

    def get(self):
        cdef float xalign
        cdef float yalign
        cdef float xscale
        cdef float yscale
        etk_alignment_get(<Etk_Alignment*>self.obj, &xalign, &yalign, &xscale, &yscale)
        return (xalign, yalign, xscale, yscale)

    def set(self, float xalign, float yalign, float xscale, float yscale):
        etk_alignment_set(<Etk_Alignment*>self.obj, xalign, yalign, xscale, yscale)


class AlignmentEnums:
    pass
