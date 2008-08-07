# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_ecore_evas.pyx

cdef class FB(EcoreEvas):
    "Framebuffer window."
    def __init__(self, char *display=NULL, int rotation=0, int w=320,
                 int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_fb_new(display, rotation, w, h)
            self._set_obj(obj)
