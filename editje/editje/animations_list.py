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
import edje
import elementary

from clist import CList
import popups


class AnimationsList(CList):
    def __init__(self, parent):
        CList.__init__(self, parent)
        self.e = parent.e

        self._options_load()
        self.options = True

        self.e.callback_add("animations.changed", self._animations_update)
        self.e.callback_add("animation.added", self._animation_added)
        self.e.callback_add("animation.removed", self._animation_removed)

        self.e.animation.callback_add("animation.changed", self._animation_changed)
        self.e.animation.callback_add("animation.unselected", self._animation_changed)

    def _animations_update(self, emissor, data):
        self.clear()
        for i in data:
            self.add(i)
        self.go()

    def _animation_added(self, emissor, data):
        self.add(data)
        self.go()
        self.open = True
        self.select(data)

    def _animation_removed(self, emissor, data):
        self.remove(data)

    def _animation_changed(self, emissor, data):
        self.selection_clear()
        self.select(data)

    # Selection
    def _selected_cb(self, li, it):
        CList._selected_cb(self, li, it)
        name = it.label_get()
        self.e.animation.name = name
        self._options_edje.signal_emit("remove,enable", "")

    def _unselected_cb(self, li, it):
        CList._unselected_cb(self, li, it)
        if not self._selected:
            self._options_edje.signal_emit("remove,disable", "")

    # Options
    def _options_load(self):
        self._options_edje = edje.Edje(self.edje_get().evas,
                        file=self._theme_file,
                        group="editje/collapsable/list/options/animations")
        self._options_edje.signal_callback_add("new",
                                "editje/collapsable/list/options",
                                self._new_cb)
        self._options_edje.signal_callback_add("remove",
                                "editje/collapsable/list/options",
                                self._remove_cb)
        self._options_edje.signal_emit("remove,disable", "")
        self.content_set("options", self._options_edje)
        self._options = False

    def _new_cb(self, obj, emission, source):
        popups.NewAnimationNamePopUp(self._parent).open()

    def _remove_cb(self, obj, emission, source):
        for i in self.selected:
            self.e.animation_del(i[0])
