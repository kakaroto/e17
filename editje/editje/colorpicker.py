#
# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.
import os
import sys

import evas
import edje
import elementary

import sysconfig
from floater import Floater


class Colorpicker(elementary.Layout):

    def __init__(self, parent):
        elementary.Layout.__init__(self, parent)
        theme_file = sysconfig.theme_file_get("default")
        self.file_set(theme_file, "colorpicker")
        self.r = elementary.Spinner(parent)
        self.g = elementary.Spinner(parent)
        self.b = elementary.Spinner(parent)
        self.a = elementary.Spinner(parent)
        bx = elementary.Box(parent)
        widgets = (("red", self.r),
                   ("green", self.g),
                   ("blue", self.b),
                   ("alpha", self.a))
        for lbl, o in widgets:
            o.label_format_set("%s: %%.0f" % lbl)
            o.min_max_set(0, 255)
            o.step_set(1)
            o.wrap_set(0)
            o.size_hint_weight_set(1.0, 0.0)
            o.size_hint_align_set(-1.0, 0.0)
            bx.pack_end(o)
            o.changed = self._color_changed
            o.show()
        bx.show()
        self.content_set("sliders", bx)
        self.current_color_set(255, 255, 255, 255)

    def current_color_set(self, r, g, b, a):
        self.r.value_set(r)
        self.g.value_set(g)
        self.b.value_set(b)
        self.a.value_set(a)
        self._update_color()

    def current_color_get(self):
        return self.current_color

    def _color_changed(self, obj, event, data):
        self._update_color()

    def _update_color(self):
        r = int(self.r.value_get())
        g = int(self.g.value_get())
        b = int(self.b.value_get())
        a = int(self.a.value_get())
        self.current_color = (r, g, b, a)
        self.edje_get().color_class_set("colorpicker.sample", r, g, b, a,
                                                              0, 0, 0, 0,
                                                              0, 0, 0, 0)
