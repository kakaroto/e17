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
import edje


class PartViewport(evas.Rectangle):

    def __init__(self, canvas, manager, part):
        evas.Rectangle.__init__(self, canvas, color=(0,0,0,0))
        self._manager = manager
        self._part = part
        self._part.on_resize_add(self._resize)
        self._part.on_move_add(self._move)
        self._part.on_restack_add(self._restack)
        self._part.on_del_add(self._del)
        self._part.on_show_add(self._show)
        self._part.on_hide_add(self._hide)
        self.pointer_mode = evas.EVAS_OBJECT_POINTER_MODE_NOGRAB
        self.on_mouse_up_add(self._select)

        self.geometry = self._part.geometry
        if self._part.visible:
            self.show()

    def _resize(self, part):
        self.size = part.size

    def _move(self, part):
        self.pos = part.pos

    def _restack(self, part):
        above = self._manager.viewports.get(self._part.above)
        if above:
            self.stack_below(above)
        below = self._manager.viewports.get(self._part.below)
        if below:
            self.stack_above(below)

    def stack_update(self):
        self._resize(self)

    def delete(self):
        self._part.on_resize_add(self._resize)
        self._part.on_move_add(self._move)
        self._part.on_resize_add(self._restack)
        self._part.on_del_add(self._del)
        self._part.on_show_add(self._show)
        self._part.on_hide_add(self._hide)
        evas.Rectangle.delete(self)

    def _del(self, part):
        self.hide()
        evas.Rectangle.delete(self)

    def _show(self, part):
        self.show()

    def _hide(self, part):
        self.hide()

    def _select(self, part, event):
        self._manager.select(self._manager.parts_name[self])


class PartsManager(evas.ClippedSmartObject):

    def __init__(self, canvas):
        self._canvas = canvas
        evas.ClippedSmartObject.__init__(self, canvas)

        self._edje = None
        self.parts = {}
        self.parts_name = {}
        self.viewports = {}
        self.select = self._select_cb

    def resize(self, w, h):
        return

    def move(self, x, y):
        if self._group is None:
            return
        ox, oy = self.pos
        dx = x - ox
        dy = y - oy
        self._group.move_relative(dx, dy)

    def delete(self):
        evas.ClippedSmartObject.delete(self)

    def _edje_set(self, edje):
        if self._edje == edje:
            return
        self._edje = edje
        if edje:
            self._parts_reload()
            self.show()
        else:
            self.hide()

    def _edje_get(self):
        return self._edje

    edje = property(_edje_get, _edje_set)

    def _parts_del(self):
        for viewport in self.viewports.values():
            self.member_del(viewport)
            viewport.hide()
            viewport.delete()
        self.parts = {}
        self.parts_name = {}
        self.viewports = {}
        return

    def _parts_reload(self):
        self._parts_del()
        if not self._edje:
            return
        for part in self._edje.parts:
            part_obj = self._edje.part_object_get(part)
            self.parts[part] = part_obj
            viewport = PartViewport(self._canvas, self, part_obj)
            self.viewports[part_obj] = viewport
            self.parts_name[viewport] = part
            self.member_add(viewport)

    def part_load(self, part):
        if not self.parts.get(part):
            part_obj = self._edje.part_object_get(part)
            self.parts[part] = part_obj
            viewport = PartViewport(self._canvas, self, part_obj)
            viewport.stack_update()
            self.viewports[part_obj] = viewport
            self.parts_name[viewport] = part
            self.member_add(viewport)

    def _select_cb(self, part):
        print part

    def _select_set(self, select):
        self._select = select

    def _select_get(self):
        return self._select

    select = property(_select_get, _select_set)
