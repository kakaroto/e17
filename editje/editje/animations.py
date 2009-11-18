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
import elementary

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_boolean import WidgetBoolean
from details_widget_color import WidgetColor
from details_widget_button import WidgetButton
from details_widget_combo import WidgetCombo
from floater import Wizard
from prop import Property, PropertyTable

class NewAnimationPopUp(Wizard):

    def __init__(self, parent):
        Wizard.__init__(self, parent, "New Animation")
        self.page_add("default")
        self.style_set("minimal")

        self._name_init()

        self.action_add("default", "Cancel", self._cancel, icon="cancel")
        self.action_add("default", "Add", self._add, icon="confirm")
        self.goto("default")

    def _name_init(self):
        bx2 = elementary.Box(self)
        bx2.horizontal_set(True)
        bx2.size_hint_weight_set(1.0, 0.0)
        bx2.size_hint_align_set(-1.0, 0.0)
        bx2.size_hint_min_set(160, 160)
        self.content_append("default", bx2)
        bx2.show()

        lb = elementary.Label(self)
        lb.label_set("Name:")
        bx2.pack_end(lb)
        lb.show()

        scr = elementary.Scroller(self)
        scr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        scr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        scr.content_min_limit(False, True)
        scr.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                       elementary.ELM_SCROLLER_POLICY_OFF)
        scr.bounce_set(False, False)
        bx2.pack_end(scr)

        self._name = elementary.Entry(self)
        self._name.single_line_set(True)
        self._name.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        self._name.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
        self._name.entry_set("")
        self._name.show()

        scr.content_set(self._name)
        scr.show()

    def _add(self, popup, data):
        name = self._name.entry_get().replace("<br>", "")
        if name == "":
            return

        self._parent.e.animation_add(name)

    def _cancel(self, popup, data):
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
        self.e.animation.event_callback_add("animation.changed", self._update)
        self.e.animation.event_callback_add("state.added", self._timestop_add)
        self.e.animation.event_callback_add("state.changed", self._update_states)

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

    def _timeline_update(self):
        for i in range(1, 11):
            sig = "ts,%.1g," % (i/10.0)
            self._parent.main_edje.signal_emit(sig + "disable", "editje")
            self._parent.main_edje.signal_emit(sig + "unselected", "editje")
        for s in self.e.animation.timestops:
            sig = "ts,%.1g,enable" % s
            self._parent.main_edje.signal_emit(sig, "editje")

    def _timestop_add(self, emissor, data):
        self._parent.main_edje.signal_emit("ts,%.1g,enable" % data, "editje")
        self._header_table["length"].value = "%.1gs" % self.e.animation.length

    def _update_states(self, emissor, data):
        step = self.e.animation.state
        self["main"]["current"].value = str(step)
        prev = self.e.animation.state_prev()
        if prev is None:
            prev = 0.0
            self["main"]["previous"].value = "None"
        else:
            self["main"]["previous"].value = str(prev)
        next = self.e.animation.state_next()
        if next is None:
            self["main"]["next"].value = "None"
        else:
            self["main"]["next"].value = str(next)

        t = self._transitions[self.e.animation.program.transition]
        self["main"]["transition"].value = (t, str(step - prev))

        sig = "ts,%.1g,selected" % self.e.animation.state
        self._parent.main_edje.signal_emit(sig, "editje")
        sig = "ts,%.1g,unselected" % self._last_timestamp
        self._parent.main_edje.signal_emit(sig, "editje")
        self._last_timestamp = self.e.animation.state

    def prop_value_changed(self, prop, value, group):
        if prop == "transition":
            t = self["main"]["transition"]["type"]
            self.e.animation.program.transition = self._transitions.index(t)
