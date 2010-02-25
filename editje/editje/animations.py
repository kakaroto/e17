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
import evas

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_combo import WidgetCombo
from prop import Property, PropertyTable
from floater import Wizard
from clist import CList
from groupselector import NameEntry


class AnimationsList(CList):
    def __init__(self, parent, new_anim_cb, anims_list_cb):
        CList.__init__(self, parent)
        self.e = parent.e

        self._new_anim_cb = new_anim_cb
        self._anims_list_cb = anims_list_cb

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
        anim_wiz = NewAnimationWizard(
            self._parent, new_anim_cb=self._new_anim_cb,
            anims_list_cb=self._anims_list_cb)
        anim_wiz.open()

    def _remove_cb(self, obj, emission, source):
        for i in self.selected:
            self.e.animation_del(i[0])


class NewAnimationWizard(Wizard):
    def __init__(self, parent, new_anim_cb=None, anims_list_cb=None):
        if not new_anim_cb or not anims_list_cb:
            raise TypeError("You must set callbacks for animations retrieval"
                            " and creation on NewAnimationWizard objects.")

        Wizard.__init__(self, parent)

        self.page_add("default", "New Animation",
                      "Name the new animation to be created.",
                      separator=True)

        self._anim_name_entry = NameEntry(
            self, changed_cb=self._name_changed_cb,
            weight_hints=(evas.EVAS_HINT_EXPAND, 0.0),
            align_hints=(evas.EVAS_HINT_FILL, 0.5))
        self.content_add("default", self._anim_name_entry)
        self._anim_name_entry.show()

        self.alternate_background_set(True)

        self.action_add("default", "Cancel", self._cancel)
        self.action_add("default", "Add", self._add)
        self.action_disabled_set("default", "Add", True)

        self._new_anim_cb = new_anim_cb
        self._anims_list_cb = anims_list_cb

    def _name_changed_cb(self, obj):
        error_msg = "This animation name already exists in this group"

        def good():
            self._anim_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", False)

        def bad():
            self._anim_name_entry.status_label = error_msg
            self.action_disabled_set("default", "Add", True)

        def ugly():
            self._anim_name_entry.status_label = ""
            self.action_disabled_set("default", "Add", True)

        name = self._anim_name_entry.entry
        if not name:
            ugly()
            return

        if name in self._anims_list_cb():
            bad()
            return

        good()

    def _add(self):
        name = self._anim_name_entry.entry
        success = self._new_anim_cb(name)
        if success:
            self.close()
        else:
            self.notify("Error creating new animation.")

    def _cancel(self):
        self.close()


class AnimationDetails(EditjeDetails):
    def __init__(self, parent):
        EditjeDetails.__init__(self, parent,
                               group="editje/collapsable/part_properties")

        self.title_set("animation")

        self._transitions = ['None', 'Linear', 'Sinusoidal', 'Accelerate',
                             'Decelerate']

        self._header_table = PropertyTable(parent)

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "length")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("l", wid)
        self._header_table.property_add(prop)

        self.content_set("part_name.swallow", self._header_table)

        prop = Property(parent, "current")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("c", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "previous")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("p", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "next")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        prop.widget_add("n", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "transition")
        wid = WidgetCombo(self)
        for null, i in enumerate(self._transitions):
            wid.item_add(i)
        prop.widget_add("type", wid)
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        prop.widget_add("length", wid)
        self["main"].property_add(prop)

        self._parent.main_edje.signal_callback_add("timestop", "*",
                                                   self._timeline_cb)
        self.e.callback_add("animation.removed", self._removed)
        self.e.animation.callback_add("animation.changed", self._update)
        self.e.animation.callback_add("animation.unselected", self._removed)
        self.e.animation.callback_add("state.added", self._timestop_add)
        self.e.animation.callback_add("state.changed", self._update_states)

    def _removed(self, emissor, data):
        self._header_table["name"].value = None
        self._header_table["length"].value = None
        self["main"]["current"].hide_value()
        self["main"]["previous"].hide_value()
        self["main"]["next"].hide_value()
        self["main"]["transition"].hide_value()
        self._timeline_clear()

    def _update(self, emissor, data):
        self._header_table["name"].value = data
        self._header_table["length"].value = "%.1gs" % self.e.animation.length
        self._last_timestamp = 0.0
        self._timeline_update()
        self.e.animation.state = 0.0

    def _timeline_cb(self, obj, emission, source):
        t = float(source)
        if not t in self.e.animation.timestops:
            self.e.animation.state_add(t)
        self.e.animation.state = t

    def _timeline_clear(self):
        for i in range(0, 11):
            sig = "ts,%.1g," % (i/10.0)
            self._parent.main_edje.signal_emit(sig + "disable", "editje")
            self._parent.main_edje.signal_emit(sig + "unselected", "editje")

    def _timeline_update(self):
        self._timeline_clear()
        for s in self.e.animation.timestops:
            sig = "ts,%.1g,enable" % s
            self._parent.main_edje.signal_emit(sig, "editje")

    def _timestop_add(self, emissor, data):
        self._parent.main_edje.signal_emit("ts,%.1g,enable" % data, "editje")
        self._header_table["length"].value = "%.1gs" % self.e.animation.length

    def _update_states(self, emissor, data):
        step = self.e.animation.state
        self["main"]["current"].show_value()
        self["main"]["current"].value = str(step)

        prev = self.e.animation.state_prev()
        self["main"]["previous"].show_value()
        if prev is None:
            prev = 0.0
            self["main"]["previous"].value = "None"
        else:
            self["main"]["previous"].value = str(prev)

        next = self.e.animation.state_next()
        self["main"]["next"].show_value()
        if next is None:
            self["main"]["next"].value = "None"
        else:
            self["main"]["next"].value = str(next)

        t = self._transitions[self.e.animation.program.transition]
        self["main"]["transition"].show_value()
        self["main"]["transition"].value = (t, str(step - prev))

        sig = "ts,%.1g,selected" % self.e.animation.state
        self._parent.main_edje.signal_emit(sig, "editje")
        sig = "ts,%.1g,unselected" % self._last_timestamp
        if self._last_timestamp != self.e.animation.state:
            self._parent.main_edje.signal_emit(sig, "editje")
        self._last_timestamp = self.e.animation.state

    def prop_value_changed(self, prop, value, group):
        if prop == "transition":
            t = self["main"]["transition"]["type"]
            self.e.animation.program.transition = self._transitions.index(t)
