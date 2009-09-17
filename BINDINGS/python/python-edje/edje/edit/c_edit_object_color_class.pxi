# Copyright (C) 2007-2008 Tiago Falcao
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.edit.pyx


#   evas.c_evas.Eina_List * edje_edit_color_classes_list_get(evas.c_evas.Evas_Object * obj)
#   unsigned char edje_edit_color_class_colors_get(evas.c_evas.Evas_Object *obj, char *class_name, int *r, int *g, int *b, int *a, int *r2, int *g2, int *b2, int *a2, int *r3, int *g3, int *b3, int *a3)
#    unsigned char edje_edit_color_class_colors_set(evas.c_evas.Evas_Object *obj, char *class_name, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3)
#cdef class color:
#    self.r
#    self.g
#    self.b
#    self.a

cdef class Color_Class:
    cdef EdjeEdit edje
    cdef object name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self.name = name

    def rename(self, char *newname):
        cdef unsigned char r
        r = edje_edit_color_class_name_set(self.edje.obj, self.name, newname)
        if r == 0:
            return False
        self.name = newname
        return True

