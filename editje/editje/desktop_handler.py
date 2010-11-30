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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

import evas
import ecore
import elementary
from elementary import cursors


class Handler(elementary.Layout):
    cursor = cursors.ELM_CURSOR_X

    def __init__(self, editable_grp, desktop_scroller, theme_file=None,
                 group=None, op_stack_cb=None):
        if not editable_grp or not desktop_scroller or \
                not theme_file or not group or not op_stack_cb:
            raise TypeError(
                "You must provide valid editable_grp, desktop_scroller, "
                "theme_file, group and operation_stack_callback "
                "parameters to Handler objects.")
        elementary.Layout.__init__(self, desktop_scroller)
        self.file_set(theme_file, group)
        self.size = self.edje.size_min_get()
        self.cursor_set(self.cursor)
        self.on_mouse_down_add(self.__mouse_down_cb)
        self._move_animator = None
        self._desktop_scroller = desktop_scroller
        self._operation_stack_cb = op_stack_cb
        self._edit_grp = editable_grp

        self.modifier_ctrl = False
        self.modifier_shift = False

    @evas.decorators.del_callback
    def _on_del(self):
        if self._move_animator is not None:
            self._move_animator.delete()
            self._move_animator = None

    def __mouse_down_cb(self, obj, event):
        self._desktop_scroller.scroll_hold_push()
        self._start_region = self._desktop_scroller.region_get()
        self._start = event.position.output.xy
        self._last = self._start
        self._move_animator = ecore.animator_add(self.__move_animator_do)
        self._modifier_control = event.modifier_is_set("Control")
        self._modifier_shift = event.modifier_is_set("Shift")
        self.down(*event.position.output)
        self.on_mouse_up_add(self.__mouse_up_cb)

    def down(self, x, y):
        return

    def __move_animator_do(self):
        self._modifier_control = self.evas.key_modifier_is_set("Control")
        self._modifier_shift = self.evas.key_modifier_is_set("Shift")

        cur = self.evas.pointer_output_xy_get()
        if cur == self._last:
            return True
        self._last = cur
        x, y = cur
        sx, sy, sw, sh = self._desktop_scroller.region_get()
        dw = x - self._start[0]
        dw += sx - self._start_region[0]
        dh = y - self._start[1]
        dh += sy - self._start_region[1]
        self.move(dw, dh)
        return True

    def move(self, dw, dh):
        return

    def __mouse_up_cb(self, obj, event):
        self.on_mouse_up_del(self.__mouse_up_cb)

        self._modifier_control = event.modifier_is_set("Control")
        self._modifier_shift = event.modifier_is_set("Shift")

        sx, sy, sw, sh = self._desktop_scroller.region_get()
        dw = event.position.output[0] - self._start[0]
        dw += sx - self._start_region[0]
        dh = event.position.output[1] - self._start[1]
        dh += sy - self._start_region[1]
        self.up(dw, dh)

        self._desktop_scroller.scroll_hold_pop()

        del self._start
        del self._last
        if self._move_animator:
            self._move_animator.delete()
            self._move_animator = None

    def up(self, dw, dh):
        return
