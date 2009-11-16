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

from clist import CList, CListView
import animations


class AnimationsList(CList):
    def __init__(self, parent):
        CList.__init__(self, parent)
        self.e = parent.e
        self.options = True

        self.e.event_callback_add("animations.changed", self._animations_update)
        self.e.event_callback_add("animation.added", self._animation_added)
        self.e.event_callback_add("animation.removed", self._animation_removed)

        self.e.animation.event_callback_add("animation.changed", self._animation_changed)
        self.e.animation.event_callback_add("animation.unselected", self._animation_changed)

        self.event_callback_add("item.selected", self._item_changed)
        self.event_callback_add("item.unselected", self._item_changed)

    def _view_load(self):
        self._selected = None
        self._view = AnimationsListView(self, self.parent.view)

    def _animations_update(self, emissor, data):
        self.populate(data)
#        self.select("")

    def _animation_added(self, emissor, data):
        self.add(data)
        self.open = True
        self.select(data)

    def _animation_removed(self, emissor, data):
        self.view.remove(data)

    def _animation_changed(self, emissor, data):
        if data != self._selected:
            if data:
                self.select(data)
            else:
                self.unselect()

    def _item_changed(self, emissor, data):
        if self.e.animation.name != data:
            self.e.animation.name = data

    def new(self):
        obj = self._view
        x, y, w, h = obj.geometry
        ch, cw = obj.evas.size
        ow, oh = 200, 120
        ox = x - (ow - w) / 2
        oy = y - (oh - h) / 2
        if ox < 0:
            ox = 0
        elif ox + ow >= cw:
            ox = cw - ow
        if oy < 0:
            oy = 0
        elif oy + oh >= ch:
            oy = ch - oh

        pop = animations.NewAnimationPopUp(self.parent)
        pop.move(ox, oy)
        pop.resize(ow, oh)
        pop.show()

    def remove(self):
        if self._selected:
            anim = self._selected
            self.select("")
            self.e.animation_del(anim)


class AnimationsListView(CListView):
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
        self.content_set("options", self._options_edje)
        self._options = False

    def _new_cb(self, obj, emission, source):
        self.controller.new()

    def _remove_cb(self, obj, emission, source):
        self.controller.remove()
