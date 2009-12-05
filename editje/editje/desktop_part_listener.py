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

from edje import Edje

class PartListener(object):
    def __init__(self):
        self._part = None

    def _part_set(self, part):
        if self._part:
            self._part.on_move_del(self.part_move)
            self._part.on_resize_del(self.part_move)
            self._part.on_del_del(self._part_del_cb)

        if part:
            part.on_move_add(self.part_move)
            part.on_resize_add(self.part_move)
            part.on_del_add(self._part_del_cb)
            self._part = part
            self.part_move(part)
        else:
            self._part = None

    part = property(fset=_part_set)

    def _part_del_cb(self, obj):
        self._part = None
        self.hide()

class PartHighlight(PartListener, Edje):
    def __init__(self, parent, group="editje/desktop/highlight"):
        self._parent = parent
        Edje.__init__(self, parent.evas, file=parent.theme, group=group)
        PartListener.__init__(self)

    def part_move(self, obj):
        if self._part:
            self.geometry = obj.geometry
            self.show()
        else:
            self.hide()

