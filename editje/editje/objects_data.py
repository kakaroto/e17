# Copyright (C) 2010 Samsung Electronics.
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

from editable_animation import re_anim_program, re_anim_program_stop


class Object(dict):
    def __init__(self, name):
        self._name = name

    def _name_get(self):
        return self._name

    def _name_set(self, value):
        self._name = value

    # name setting only useful for object copying et al, where different names
    # got to be used
    name = property(fget=_name_get, fset=_name_set)

    def apply_to(self, obj):
        return False

    def source_get(self, indent=""):
        return ""


class Group(Object):
    pass


class Part(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)
        st_class = state_class_from_part_type_get(obj)

        self["above"] = obj.above_get()
        self["below"] = obj.below_get()

        self._type = obj.type
        self["mouse_events"] = obj.mouse_events
        self["repeat_events"] = obj.repeat_events
        self["scale"] = obj.scale
        self["clip_to"] = obj.clip_to
        self["source"] = obj.source
        self["effect"] = obj.effect
        self["api"] = obj.api

        dragable = dict()
        self["dragable"] = dragable
        x, y = obj.drag
        dragable["x"] = x
        dragable["y"] = y
        dragable["confine"] = obj.drag_confine
        dragable["events"] = obj.drag_event

        self["state"] = obj.state_selected_get()
        states = []
        self["states"] = states
        for st in obj.states:
            state_name, state_val = st.split(None, 1)
            state_val = float(state_val)
            state = obj.state_get(state_name, state_val)
            states.append(st_class(state))

    def _type_get(self):
        return self._type

    type = property(_type_get)

    def apply_to(self, obj):

        # Simple Restack. TODO: relative restack in edje_edit
        above = obj.above_get()
        while above:
            obj.restack_above()
            above = obj.above_get()

        above = self["above"]
        if above:
            below = obj.below_get()
            while below != above:
                obj.restack_below()
                below = obj.below_get()
            obj.restack_below()

        # type is imutable
        obj.mouse_events = self["mouse_events"]
        obj.repeat_events = self["repeat_events"]
        obj.scale = self["scale"]
        obj.clip_to = self["clip_to"]
        obj.source = self["source"]
        obj.effect = self["effect"]
        obj.api = self["api"]

        #dragable = self["dragable"]
        #obj.drag = (dragable["x"], dragable["y"])
        #obj.drag_confine = dragable["confine"]
        #obj.drag_event = dragable["events"]

        for state in self["states"]:
            name, val = state.name, state.value
            if not obj.state_exist(name, val):
                if not obj.state_add(name, val):
                    return False
            state.apply_to(obj.state_get(name, val))

        obj.state_selected_set(*self["state"])

        return True

    def source_get(self, indent=""):
        ret = indent + 'part { name: "%s";\n' % self.name
        types = ["NONE", "RECT", "TEXT", "IMAGE", "SWALLOW", "TEXTBLOCK",
                 "GRADIENT", "GROUP", "BOX", "TABLE", "EXTERNAL"]
        ret += indent + '   type: %s;\n' % types[self.type]
        ret += indent + '   mouse_events: %d;\n' % int(self["mouse_events"])
        ret += indent + '   repeat_events: %d;\n' % int(self["repeat_events"])
        ret += indent + '   scale: %d;\n' % int(self["scale"])
        ret += indent + '   clip_to: "%s";\n' % self["clip_to"]
        ret += indent + '   source: "%s";\n' % self["source"]
        effects = ["NONE", "PLAIN", "OUTLINE", "SOFT_OUTLINE", "SHADOW",
                   "SOFT_SHADOW", "OUTLINE_SHADOW", "OUTLINE_SOFT_SHADOW ",
                   "FAR_SHADOW ", "FAR_SOFT_SHADOW", "GLOW"]
        ret += indent + '   effect: %s;\n' % effects[self["effect"]]
        for state in self["states"]:
            ret += state.source_get("   ")
        ret += indent + '}\n'
        return ret


class State(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)

        self.value = obj.value

        self["visible"] = obj.visible
        self["align"] = obj.align
        self["min"] = obj.min
        self["max"] = obj.max
        self["aspect"] = (obj.aspect_min_get(), obj.aspect_max_get())
        self["aspect_preference"] = obj.aspect_pref_get()
        self["color"] = obj.color_get()
        self["color2"] = obj.color2_get()
        self["color3"] = obj.color3_get()

        rel = dict()
        self["rel1"] = rel
        rel["relative"] = obj.rel1_relative_get()
        rel["offset"] = obj.rel1_offset_get()
        x, y = obj.rel1_to_get()
        if not x:
            x = ""
        if not y:
            y = ""
        rel["to"] = (x, y)

        rel = dict()
        self["rel2"] = rel
        rel["relative"] = obj.rel2_relative_get()
        rel["offset"] = obj.rel2_offset_get()
        x, y = obj.rel2_to_get()
        if not x:
            x = ""
        if not y:
            y = ""
        rel["to"] = (x, y)

    def apply_to(self, obj):
        obj.visible = self["visible"]
        obj.align = self["align"]
        obj.min = self["min"]
        obj.max = self["max"]
        min, max = self["aspect"]
        obj.aspect_min_set(min)
        obj.aspect_max_set(max)
        obj.aspect_pref_set(self["aspect_preference"])
        obj.color_set(*self["color"])
        obj.color2_set(*self["color2"])
        obj.color3_set(*self["color3"])

        rel = self["rel1"]
        obj.rel1_relative_set(*rel["relative"])
        obj.rel1_offset_set(*rel["offset"])
        obj.rel1_to_set(*rel["to"])

        rel = self["rel2"]
        obj.rel2_relative_set(*rel["relative"])
        obj.rel2_offset_set(*rel["offset"])
        obj.rel2_to_set(*rel["to"])

        return True

    def source_get(self, indent=""):
        ret = indent + 'description { state: "%s" %.f;\n' % (self.name, self.value)
        ret += indent + '   visible: %d;\n' % int(self["visible"])
        ret += indent + '   align: %f %f;\n' % self["align"]
        ret += indent + '   min: %d %d;\n' % self["min"]
        ret += indent + '   max: %d %d;\n' % self["max"]
        ret += indent + '   aspect: %f %f;\n' % self["aspect"]
        prefs = ["NONE", "VERTICAL", "HORIZONTAL", "BOTH"]
        ret += indent + '   aspect_preference: %s;\n' % \
            prefs[self["aspect_preference"]]
        ret += indent + '   color: %d %d %d %d;\n' % self["color"]
        ret += indent + '   color2: %d %d %d %d;\n' % self["color2"]
        ret += indent + '   color3: %d %d %d %d;\n' % self["color3"]

        rel = self["rel1"]
        ret += indent + '   rel1 {\n'
        ret += indent + '      relative: %f %f;\n' % rel["relative"]
        ret += indent + '      offset: %d %d;\n' % rel["offset"]
        x, y = rel["to"]
        ret += indent + '      to_x: "%s";\n' % x
        ret += indent + '      to_y: "%s";\n' % y
        ret += indent + '   }\n'

        rel = self["rel2"]
        ret += indent + '   rel2 {\n'
        ret += indent + '      relative: %f %f;\n' % rel["relative"]
        ret += indent + '      offset: %d %d;\n' % rel["offset"]
        x, y = rel["to"]
        ret += indent + '      to_x: "%s";\n' % x
        ret += indent + '      to_y: "%s";\n' % y
        ret += indent + '   }\n'

        ret += self._source_get(indent)
        ret += indent + '}\n'
        return ret

    def _source_get(self, indent=""):
        return ""


class StateText(State):
    def __init__(self, obj):
        State.__init__(self, obj)

        text = dict()
        self["text"] = text
        text["text"] = obj.text_get()
        text["font"] = obj.font_get()
        text["size"] = obj.text_size_get()
        #TODO: text["text_class"] =
        text["fit"] = obj.text_fit_get()
        text["align"] = obj.text_align_get()
        text["elipsis"] = obj.text_elipsis_get()

    def apply_to(self, obj):
        if obj.part_get().type != edje.EDJE_PART_TYPE_TEXT:
            return False

        State.apply_to(self, obj)

        text = self["text"]
        obj.text_set(text["text"])
        obj.font_set(text["font"])
        obj.text_size_set(text["size"])
        #TODO: text["text_class"] =
        obj.text_fit_set(*text["fit"])
        obj.text_align_set(*text["align"])
        obj.text_elipsis_set(text["elipsis"])

        return True

    def _source_get(self, indent=""):
        text = self["text"]
        ret = indent + '   text {\n'
        ret += indent + '      text: "%s";\n' % text["text"]
        ret += indent + '      font: "%s";\n' % text["font"]
        ret += indent + '      size: %d;\n' % text["size"]
        ret += indent + '      fit: %d %d;\n' % text["fit"]
        ret += indent + '      align: %f %f;\n' % text["align"]
        ret += indent + '      elipsis: %f;\n' % text["elipsis"]
        ret += indent + '   }\n'
        return ret


class StateImage(State):
    def __init__(self, obj):
        State.__init__(self, obj)

        image = dict()
        self["image"] = image
        normal = obj.image_get()
        if not normal:
            normal = ""
        image["normal"] = normal
        image["tweens"] = obj.tweens
        image["border"] = obj.image_border_get()
        image["middle"] = obj.image_border_fill_get()

        #TODO: Bindings
        """
        fill = dict()
        self["fill"] = fill
        fill["smooth"] =
        fill["angle"] =

        origin = dict()
        fill["origin"] = origin
        origin["relative"] =
        origin["offset"] =

        size = dict()
        fill["size"] = object()
        size["relative"] =
        size["offset"] =
        """

    def apply_to(self, obj):
        if obj.part_get().type != edje.EDJE_PART_TYPE_IMAGE:
            return False

        State.apply_to(self, obj)

        image = self["image"]
        obj.image_set(image["normal"])
        for t in obj.tweens:
            obj.tween_del(t)
        for t in image["tweens"]:
            obj.tween_add(t)
        obj.image_border_set(*image["border"])
        obj.image_border_fill_set(image["middle"])

        return True

    def _source_get(self, indent=""):
        image = self["image"]
        ret = indent + '   image {\n'
        ret += indent + '      normal: "%s";\n' % image["normal"]

        for t in image["tweens"]:
            ret += indent + '      tween: "%s";\n' % t

        ret += indent + '      border: %d %d %d %d;\n' % image["border"]

        middle = ["DEFAULT", "NONE", "SOLID"]
        ret += indent + '      middle: %s;\n' % middle[image["middle"]]
        ret += indent + '   }\n'
        return ret


class StateExternal(State):
    def __init__(self, obj):
        State.__init__(self, obj)

        params = dict()
        self["params"] = params
        for param in obj.external_params_get():
            params[param.name] = param.value

    def apply_to(self, obj):
        if obj.part_get().type != edje.EDJE_PART_TYPE_EXTERNAL:
            return False

        State.apply_to(self, obj)

        params = self["params"]
        for param in obj.external_params_get():
            value = params.get(param.name)
            if value:
                obj.external_param_set(param.name, value)

        return True

    def _source_get(self, indent=""):
        ret = indent + '   params {\n'
        for name, value in self["params"].items():
            if isinstance(value, int):
                ret += indent + '      int: "%s" "%d";\n' % (name, value)
            elif isinstance(value, float):
                ret += indent + '      double: "%s" "%f";\n' % (name, value)
            elif isinstance(value, str):
                ret += indent + '      string: "%s" "%s";\n' % (name, value)
        ret += indent + '   }\n'
        return ret


class Program(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)

        self["signal"] = obj.signal_get() or ""
        self["source"] = obj.source_get() or ""
        self["action"] = obj.action_get()
        self["state"] = obj.state_get() or ""
        self["state2"] = obj.state2_get() or ""
        self["value"] = obj.value_get()
        self["transition"] = obj.transition_get()
        self["transition_time"] = obj.transition_time_get()
        self["in"] = (obj.in_from_get(), obj.in_range_get())
        self["api"] = obj.api

        targets = []
        self["targets"] = targets
        for target in obj.targets_get():
            targets.append(target)

        afters = []
        self["afters"] = afters
        for after in obj.afters_get():
            afters.append(after)

    def apply_to(self, obj):
        obj.signal_set(self["signal"])
        obj.source_set(self["source"])
        obj.action_set(self["action"])
        obj.state_set(self["state"])
        obj.state2_set(self["state2"])
        obj.value_set(self["value"])
        obj.transition_set(self["transition"])
        obj.transition_time_set(self["transition_time"])
        in_from, in_range = self["in"]
        obj.in_from_set(in_from)
        obj.in_range_set(in_range)
        obj.api= self["api"]

        obj.targets_clear()
        for target in self["targets"]:
            obj.target_add(target)

        obj.afters_clear()
        for after in self["afters"]:
            obj.after_add(after)

    def _source_get(self, indent=""):
        ret = indent + 'description { name: "%s";\n' % self.name
        ret += indent + '   signal: "%s";\n' % self["signal"]
        ret += indent + '   source: "%s";\n' % self["source"]

        action = self["action"]
        if action == edje.EDJE_ACTION_TYPE_ACTION_STOP:
            ret += indent + '   action: ACTION_STOP;\n'
        elif action == edje.EDJE_ACTION_TYPE_STATE_SET:
            ret += indent + '   action: STATE_SET "%s" %.2f;\n' % \
                (self["state"], self["value"])
        elif action == edje.EDJE_ACTION_TYPE_SIGNAL_EMIT:
            ret += indent + '   action: SIGNAL_EMIT "%s" "%s";\n' % \
                (self["state"], self["state2"])

        trans = ["NONE", "LINEAR", "SINUSOIDAL", "ACCELERATE", "DECELERATE"]
        ret += indent + '   transition: %s %.5f;\n' % \
            (trans[self["transition"]], self["transition_time"])
        ret += indent + '   in: %.5f %.5f;\n' % self["in"]

        for target in self["targets"]:
            ret += indent + '   target: "%s";\n' % target

        for after in self["afters"]:
            ret += indent + '   after: "%s";\n' % after
        ret += indent + '   }\n'
        return ret


class ColorClass(Object):
    pass


class Style(Object):
    pass


class Animation(Object):
    """Animations data serializer.

    Objects must be instantiated with an animation's stop program reference.

    """

    def __init__(self, obj):
        prog = re_anim_program_stop.match(obj.name)
        if not prog:
            raise TypeError("Not an animation's stop program")

        name = re_anim_program.match(obj.name).group(1)
        Object.__init__(self, name)

        edj = obj.edje_get()

        self["parts"] = None

        frames = {}
        self["frames"] = frames
        for target in obj.targets_get():
            program = edj.program_get(target)
            if not program:
                continue

            time = re_anim_program.match(program.name).group(2)
            if time == "end":
                continue
            time = float(time)

            frames[time] = AnimationFrame(program)
            if not self["parts"]:
                self["parts"] = frames[time]["targets"].keys()

    def apply_to(self, obj):
        anim = obj.animation
        # select it, so we to act on it by this ptr
        anim.name = self.name

        # in pristine order we were getting wrong state selected at the end
        for time, frame_data in reversed(self["frames"].items()):
            if time != 0.0:  # we have it, already
                r = anim.state_add(time)
                if not r:
                    obj.animation_del(self.name)
                    return False

            pname = "@%s@%.2f" % (self.name, time)
            prog = obj.edje.program_get(pname)
            frame_data.apply_to(prog)

        return True


class AnimationFrame(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)

        self["transition"] = obj.transition_get()

        edj = obj.edje_get()
        targets = {}
        self["targets"] = targets
        for target in obj.targets_get():
            part = edj.part_get(target)
            if not part:
                continue

            state = part.state_get(self.name)
            if not state:
                continue

            stateclass = state_class_from_part_type_get(part)
            targets[target] = stateclass(state)

    def apply_to(self, obj):
        state_name = obj.name
        edj = obj.edje_get()

        animation = re_anim_program.match(state_name).group(1)
        time = re_anim_program.match(state_name).group(2)

        obj.transition_set(self["transition"])

        obj.targets_clear()
        for target, data in self["targets"].items():
            obj.target_add(target)

            part = edj.part_get(target)
            if not part:
                continue

            if not part.state_add(state_name):
               continue

            state = part.state_get(state_name)
            if not state:
                continue

            data.apply_to(state)


def state_class_from_part_type_get(part):
    st_class = State
    if part.type == edje.EDJE_PART_TYPE_IMAGE:
        st_class = StateImage
    elif part.type == edje.EDJE_PART_TYPE_TEXT:
        st_class = StateText
    elif part.type == edje.EDJE_PART_TYPE_EXTERNAL:
        st_class = StateExternal
    return st_class
