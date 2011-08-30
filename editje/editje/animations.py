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

import edje
import evas
import elementary

from details import EditjeDetails
from details_widget_entry import WidgetEntry
from details_widget_combo import WidgetCombo
from prop import Property, PropertyTable
from floater import Wizard
from clist import CList
from parts import PartsList
from groupselector import NameEntry
from operation import Operation
from error_notify import ErrorNotify
import objects_data


class AnimationsList(CList):
    def __init__(self, parent, new_anim_cb, anims_list_cb, parts_list_cb,
                 op_stack_cb):
        CList.__init__(self, parent)
        self._edit_grp = parent.e

        self._new_anim_cb = new_anim_cb
        self._anims_list_cb = anims_list_cb
        self._parts_list_cb = parts_list_cb
        self._operation_stack_cb = op_stack_cb

        self._options_load()
        self.options = True

        self._edit_grp.callback_add(
            "animations.changed", self._animations_update)
        self._edit_grp.callback_add(
            "animation.added", self._animation_added)
        self._edit_grp.callback_add(
            "animation.removed", self._animation_removed)

        self._edit_grp.animation.callback_add(
            "animation.changed", self._animation_changed)
        self._edit_grp.animation.callback_add(
            "animation.unselected", self._animation_changed)

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
        self._edit_grp.animation.name = name
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
            anims_list_cb=self._anims_list_cb,
            parts_list_cb=self._parts_list_cb)
        anim_wiz.open()

    def _remove_cb(self, obj, emission, source):

        def anim_restore(anim_save):
            name = anim_save.name
            if self._edit_grp.animation_add(name):
                anim_save.apply_to(self._edit_grp)

        def sigs_restore(anim_name, sigs_save):
            curr_sig = self._edit_grp.signal.name
            self._edit_grp.signal.name = None

            pname = "@%s@0.00" % anim_name

            for sig in sigs_save:
                prog = self._edit_grp.program_get(sig)
                prog.afters_clear()
                prog.after_add(pname)

            self._edit_grp.signal.name = curr_sig

        def relative_signals_anim_clear(anim_name):
            sigs = []

            curr_sig = self._edit_grp.signal.name
            self._edit_grp.signal.name = None

            pname = "@%s@0.00" % anim_name

            for sig in self._edit_grp.signals:
                prog = self._edit_grp.program_get(sig)
                afters = prog.afters
                if afters and pname != afters[0]:
                    continue

                prog.afters_clear()
                sigs.append(sig)

            self._edit_grp.signal.name = curr_sig

            return sigs

        for s in self.selected:
            anim_name = s[0]
            pname = "@%s@stop" % anim_name
            stop_prog = self._edit_grp.edje.program_get(pname)
            anim_data = objects_data.Animation(stop_prog)

            r = self._edit_grp.animation_del(anim_name)
            if not r:
                del anim_data
                continue

            sigs_save = relative_signals_anim_clear(anim_name)

            op = Operation("animation deletion: %s" % anim_name)
            op.redo_callback_add(relative_signals_anim_clear, anim_name)
            op.redo_callback_add(self._edit_grp.animation_del, anim_name)
            op.undo_callback_add(anim_restore, anim_data)
            op.undo_callback_add(sigs_restore, anim_name, sigs_save)

            self._operation_stack_cb(op)


class AnimationsPartsList(PartsList):
    def __init__(self, parent, editable_grp, operation_stack_cb):
        PartsList.__init__(self, parent, editable_grp, operation_stack_cb)

        self._remove_confirmation = None

        self._edit_grp.animation.callback_add("animation.changed",
                                              self._animation_changed_cb)
        self._edit_grp.animation.callback_add("animation.unselected",
                                              self._animation_changed_cb)
        self._blocked_parts = []

    def _animation_changed_cb(self, emissor, data):
        # Nasty hack
        self._parts_update_cb(None, self._edit_grp.parts)

    def _parts_update_cb(self, emissor, data):
        if not self._edit_grp.animation.name:
            self.clear()
            self._edit_grp.animation.event_emit("parts.blocked.changed", [])
            return

        parts = data
        self.clear()
        self._blocked_parts = []
        self._edit_grp.part.name = None
        for p in parts[::-1]:
            chk = elementary.Check(self._parent)
            chk.state_set(self._edit_grp.animation.parts.get(p))
            if not chk.state:
                self._blocked_parts.append(p)
            chk.callback_changed_add(self._check_changed_cb, p)
            chk.show()
            self.add_full(p, end=chk)
        self.go()
        self._edit_grp.animation.event_emit("parts.blocked.changed",
                                            self._blocked_parts)

    def _part_added_cb(self, emissor, data):
        if not self._edit_grp.animation.name:
            PartsList._part_added_cb(self, emissor, data)
            return

        chk = elementary.Check(self._parent)
        chk.state_set(False)
        self._blocked_parts.append(data)
        self._edit_grp.animation.event_emit("parts.blocked.changed",
                                            self._blocked_parts)
        chk.callback_changed_add(self._check_changed_cb, data)
        chk.show()
        self.add_full(data, end=chk)
        self.go()

    def _check_changed_cb(self, obj, part):
        if obj.state:
            self._part_add(part, self._edit_grp.animation.name)

            op = Operation("part (%s) addition into animation (%s)" % \
                                (part, self._edit_grp.animation.name))
            op.redo_callback_add(self._part_add, part,
                                 self._edit_grp.animation.name)
            op.undo_callback_add(self._part_remove, part,
                                 self._edit_grp.animation.name)
            self._operation_stack_cb(op)
        else:
            # FIXME: Take the confirmation out of this function
            self._notification = ErrorNotify(
                self, elementary.ELM_NOTIFY_ORIENT_CENTER)
            self._notification.title = "Part Removal"
            lb = elementary.Label(self._notification)
            lb.text_set("Are you sure you want to remove<br>"
                         "this part from the animation?")
            lb.show()
            self._notification.pack_end(lb)
            self._notification.action_add("Remove",
                                          self._confirm_remove_cb, data=part)
            self._notification.action_add("Cancel",
                                          self._cancel_remove_cb, data=obj)
            self._notification.show()

    def _context_recall(self, **kargs):
        if "animation" in kargs:
            self._edit_grp.animation.name = kargs["animation"]
            if "time" in kargs:
                self._edit_grp.animation.state = kargs["time"]
        if "part" in kargs:
            self._edit_grp.part.name = kargs["part"]
            if "state" in kargs:
                self._edit_grp.part.state.name = kargs["state"]

    def _part_add(self, part, anim_name, saved_states=None):

        def frame_readd(part_name, saved_states):
            part = self._edit_grp.part_get(part_name)
            self._edit_grp.part.name = part_name
            for t, state in saved_states:
                if not part.state_exist(state.name):
                    prog = self._edit_grp.program_get(state.name)
                    prog.target_add(part_name)
                    part.state_add(state.name, 0.0)
                st = part.state_get(state.name)
                state.apply_to(st)
            if self._edit_grp.animation.program:
                statename = self._edit_grp.animation.program.name
                part.state_selected_set(statename)

        self._context_recall(animation=anim_name)
        self._edit_grp.animation.part_add(part)
        if saved_states:
            frame_readd(part, saved_states)
        self._parts_update_cb(None, self._edit_grp.parts)

    def _part_remove(self, part, anim_name):
        self._context_recall(animation=anim_name)
        self._edit_grp.animation.part_remove(part)
        self._parts_update_cb(None, self._edit_grp.parts)

    def _confirm_remove_cb(self, btn, part):
        saved_states = []
        p = self._edit_grp.part_get(part)
        for t in self._edit_grp.animation.timestops:
            prog = "@%s@%.2f" % (self._edit_grp.animation.name, t)
            st_obj = p.state_get(prog)
            if not st_obj:
                continue
            st_class = objects_data.state_class_from_part_type_get(p)
            state_save = st_class(st_obj)
            saved_states.append([t, state_save])

        self._part_remove(part, self._edit_grp.animation.name)
        self._notification_delete()
        op = Operation("part (%s) deletion from animation (%s)" % \
                            (part, self._edit_grp.animation.name))
        op.redo_callback_add(self._part_remove, part,
                             self._edit_grp.animation.name)
        op.undo_callback_add(self._part_add, part,
                             self._edit_grp.animation.name,
                             saved_states)
        self._operation_stack_cb(op)

    def _cancel_remove_cb(self, btn, chk):
        self._notification_delete()
        chk.state = True

    def _notification_delete(self):
        self._notification.delete()
        self._notification = None

    def _selected_cb(self, list_, list_item):
        if not self._edit_grp.animation.name:
            return

        part_name = list_item.label_get()
        if self._edit_grp.animation.parts.get(part_name):
            CList._selected_cb(self, list_, list_item)
            self._edit_grp.part.name = part_name
        else:
            self._edit_grp.part.name = None
            CList._unselected_cb(self, list_, list_item)


class NewAnimationWizard(Wizard):
    def __init__(self, parent, new_anim_cb=None, anims_list_cb=None,
                 parts_list_cb=None):
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

        lbl = elementary.Label(self)
        lbl.text_set("Select the parts to use in the animation."
                      " An empty selection will use all parts.")
        self.content_add("default", lbl)
        lbl.show()

        self._parts_list = elementary.List(self)
        self._parts_list.size_hint_weight_set(evas.EVAS_HINT_EXPAND,
                                              evas.EVAS_HINT_EXPAND)
        self._parts_list.size_hint_align_set(evas.EVAS_HINT_FILL,
                                             evas.EVAS_HINT_FILL)
        if parts_list_cb:
            for p in parts_list_cb():
                self._parts_list.item_append(p)
        self._parts_list.multi_select = True
        self._parts_list.go()
        self.content_add("default", self._parts_list)
        self._parts_list.show()

        self.alternate_background_set(True)

        self.action_add("default", "Cancel", self._cancel, key="Escape")
        self.action_add("default", "Add", self._add, key="Return")
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
        its = self._parts_list.selected_items_get()
        if not its:
            parts = None
        else:
            parts = []
            for it in its:
                parts.append(it.label_get())
        success = self._new_anim_cb(name, parts)
        if success:
            self.close()
        else:
            self.notify("Error creating new animation.")

    def _cancel(self):
        self.close()


class AnimationDetails(EditjeDetails):
    def __init__(self, parent, operation_stack_cb):
        EditjeDetails.__init__(
            self, parent, operation_stack_cb,
            group="editje/collapsable/part_properties")

        self.title = "animation"

        self._transitions = ['None', 'Linear', 'Sinusoidal', 'Accelerate',
                             'Decelerate']

        self._header_table = PropertyTable(
            parent, "animation name", self.header_prop_value_changed)
        self.content_set("part_name.swallow", self._header_table)
        self.focus_custom_chain_set([self._header_table, self._box])

        prop = Property(parent, "name")
        wid = WidgetEntry(self)
        wid.tooltip_set("Unique animation name.")
        prop.widget_add("n", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "length")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Time lenght of animation<br>in seconds.")
        prop.widget_add("l", wid)
        self._header_table.property_add(prop)

        prop = Property(parent, "current")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("Time of current keyframe<br>in seconds.")
        prop.widget_add("c", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "previous")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("Time of previous keyframe<br>in seconds.")
        prop.widget_add("p", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "next")
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.tooltip_set("Time of next keyframe<br>in seconds.")
        prop.widget_add("n", wid)
        self["main"].property_add(prop)

        prop = Property(parent, "transition")
        wid = WidgetCombo(parent)
        for null, i in enumerate(self._transitions):
            wid.item_add(i)
        wid.tooltip_set("Transition effect to interpolation<br>"
                        "between the previous keyframe<br>and this.")
        prop.widget_add("type", wid)
        wid = WidgetEntry(self)
        wid.disabled_set(True)
        wid.type_float()
        #wid.parser_in = lambda x: str(x)
        #wid.parser_out = lambda x: float(x)
        wid.tooltip_set("Transition time between the<br>"
                        "previous keyframe and this,<br>in seconds.")
        prop.widget_add("length", wid)
        self["main"].property_add(prop)

        self._parent.main_edje.signal_callback_add("timestop", "*",
                                                   self._timeline_cb)
        self._parent.main_edje.signal_callback_add("timeremove", "*",
                                                   self._timeremove_cb)
        self.e.callback_add("animation.removed", self._removed)
        self.e.animation.callback_add("animation.changed", self._update)
        self.e.animation.callback_add("animation.unselected", self._removed)
        self.e.animation.callback_add("state.added", self._timestop_add)
        self.e.animation.callback_add("frame.changed", self._update_states)

    def _removed(self, emissor, data):
        self.open_disable = True
        self._header_table["name"].value = "Unselected"
        self._header_table["name"].value_obj._values_dict["n"].disabled_set(
            True)
        self._header_table["length"].value = None
        self["main"]["current"].hide_value()
        self["main"]["previous"].hide_value()
        self["main"]["next"].hide_value()
        self["main"]["transition"].hide_value()
        self["main"]["transition"].value_obj._values_dict["type"].hover_end()
        self._timeline_clear()
        self._parent.main_edje.signal_emit("timeline,block", "")
        self.open = False
        self.open_disable = True

    def _update(self, emissor, data):
        self.open_disable = False
        self._header_table["name"].value = data
        self._header_table["name"].value_obj._values_dict["n"].disabled_set(
            False)
        self._header_table["length"].value = "%.1gs" % self.e.animation.length
        self._last_timestamp = 0.0
        self._timeline_update()
        self._parent.main_edje.signal_emit("timeline,unblock", "")

        self.open_disable = False
        self.open = True
        self.show()

    def _timeline_cb(self, obj, emission, source):
        t = float(source)
        if not t in self.e.animation.timestops:
            anim_name = self.e.animation.name
            anim_frame = self.e.animation.state
            self.e.animation.state_add(t)
            op = Operation("animation (%s) frame (%s) creation" % \
                               (self.e.animation.name, t))
            op.redo_callback_add(self._state_add, t, anim_name)
            op.redo_callback_add(setattr, self.e.animation, "state", t)
            op.undo_callback_add(self._remove_time_point,
                                 t, anim_name, anim_frame)
            self._operation_stack_cb(op)

        self.e.animation.state = t

    def _state_add(self, t, anim_name):
        self._context_recall(animation=anim_name)
        self.e.animation.state_add(t)

    def _remove_time_point(self, t, anim_name, anim_frame):
        self._context_recall(animation=anim_name, time=anim_frame)
        prog = "@%s@%.2f" % (self.e.animation.name, t)
        self.e.animation.state_prev_goto()
        self.e.animation.state_del(t)
        self.e.program_del(prog)
        self._parent.main_edje.signal_emit("ts,%.1g,disable" % t, "editje")
        self._parent.main_edje.signal_emit("ts,%.1g,unselected" % t, "editje")

    def _frame_readd(self, t, anim_name, saved_states, trans):
        self.e.animation.name = anim_name
        self.e.animation.state_add(t)
        for pname, state in saved_states:
            self.e.part.name = pname
            st = self.e.part_get(pname).state_get(state.name)
            state.apply_to(st)
        self.e.animation.state = t
        self.e.animation.program.transition = trans
        self.e.animation.event_emit("frame.changed")

    def _timeremove_cb(self, obj, emission, source):
        t = float(source)
        anim_name = self.e.animation.name
        anim_frame = self.e.animation.state
        prog = "@%s@%.2f" % (anim_name, t)

        saved_states = []
        for part_name in self.e.animation.parts:
            part = self.e.part_get(part_name)
            st_obj = part.state_get(prog)
            st_class = objects_data.state_class_from_part_type_get(part)
            state_save = st_class(st_obj)
            saved_states.append([part.name, state_save])

        trans = self.e.animation.program.transition

        def agree(bt, notification):
            self._remove_time_point(t, anim_name, anim_frame)
            notification.hide()
            notification.delete()
            op = Operation("animation (%s) frame (%s) deletion" % \
                               (anim_name, t))
            op.redo_callback_add(self._remove_time_point, t, anim_name,
                                 anim_frame)
            op.undo_callback_add(
                self._frame_readd, t, anim_name, saved_states, trans)
            self._operation_stack_cb(op)

        def disagree(bt, notification):
            notification.hide()
            notification.delete()

        if t in self.e.animation.timestops:
            notification = ErrorNotify(
                self._parent, orient=elementary.ELM_NOTIFY_ORIENT_CENTER)
            notification.title = "Do you really want to delete this " \
                "animation frame?"
            notification.action_add("Yes", agree, None, notification)
            notification.action_add("No", disagree, None,  notification)
            notification.show()

    def _timeline_clear(self):
        for i in range(0, 11):
            sig = "ts,%.1g," % (i / 10.0)
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

        next_state = self.e.animation.state_next()
        self["main"]["next"].show_value()
        if next_state is None:
            self["main"]["next"].value = "None"
        else:
            self["main"]["next"].value = str(next_state)

        t = self._transitions[self.e.animation.program.transition]
        self["main"]["transition"].show_value()
        self["main"]["transition"].value = (t, str(step - prev))

        sig = "ts,%.1g,selected" % self.e.animation.state
        self._parent.main_edje.signal_emit(sig, "editje")
        if self.e.animation.state != 0.0:
            sig = "ts,rm,%.1g,selected" % self.e.animation.state
            self._parent.main_edje.signal_emit(sig, "editje")
        sig = "ts,%.1g,unselected" % self._last_timestamp
        if self._last_timestamp != self.e.animation.state:
            self._parent.main_edje.signal_emit(sig, "editje")
            sig = "ts,rm,%.1g,unselected" % self._last_timestamp
            self._parent.main_edje.signal_emit(sig, "editje")
        self._last_timestamp = self.e.animation.state

    def header_prop_value_changed(self, prop, value, group):
        if prop == "name":
            if not self.e.animation.name_set(value):
                self._header_table["name"].value = self.e.animation.name

    def _context_recall(self, **kargs):
        if "animation" in kargs:
            self.e.animation.name = kargs["animation"]
            if "time" in kargs:
                self.e.animation.state = kargs["time"]
        if "part" in kargs:
            self.e.part.name = kargs["part"]
            if "state" in kargs:
                self.e.part.state.name = kargs["state"]

    def _prop_object_get(self):
        return self.e.animation.program

    def prop_value_changed(self, prop, value, group):
        if prop == "transition":
            args = [["main"], [prop], [self._transitions.index(value[0])],
                    [None], [False],
                    [lambda x: (self._transitions[x], value[1])]]

            self._prop_change_do("animation state transition", *args)
