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

import evas
import ecore
import edje


class Handler(edje.Edje):
    def __init__(self, parent, group):
        self._parent = parent
        edje.Edje.__init__(self, parent.evas, file=parent.theme, group=group)
        self.on_mouse_down_add(self.__mouse_down_cb)
        self.on_mouse_up_add(self.__mouse_up_cb)
        self._move_animator = None

    @evas.decorators.del_callback
    def _on_del(self):
        if self._move_animator is not None:
            self._move_animator.delete()
            self._move_animator = None

    def __mouse_down_cb(self, obj, event):
        self._parent.parent_view.scroll_hold_push()
        self._start_region = self._parent.parent_view.region_get()
        self._start = event.position.output.xy
        self._last = self._start
        self._move_animator = ecore.animator_add(self.__move_animator_do)
        self.down(*event.position.output)

    def __move_animator_do(self):
        cur = self.evas.pointer_output_xy_get()
        if cur == self._last:
            return True
        self._last = cur
        x, y = cur
        sx, sy, sw, sh = self._parent.parent_view.region_get()
        dw = x - self._start[0]
        dw += sx - self._start_region[0]
        dh = y - self._start[1]
        dh += sy - self._start_region[1]
        self.move(dw, dh)
        return True

    def __mouse_up_cb(self, obj, event):
        sx, sy, sw, sh = self._parent.parent_view.region_get()
        dw = event.position.output[0] - self._start[0]
        dw += sx - self._start_region[0]
        dh = event.position.output[1] - self._start[1]
        dh += sy - self._start_region[1]
        self.up(dw, dh)

        self._parent.parent_view.scroll_hold_pop()

        del self._start
        del self._last
        self._move_animator.delete()
        self._move_animator = None
