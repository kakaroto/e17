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

    name = property(_name_get)

    def apply_to(self, obj):
        return False

    def source_get(self, indent=""):
        return ""


class Group(Object):
    pass


class Part(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)

        self._type = obj.type
        self["mouse_events"] = obj.mouse_events
        self["repeat_events"] = obj.repeat_events
        self["clip_to"] = obj.clip_to
        self["source"] = obj.source
        self["effect"] = obj.effect

        dragable = dict()
        self["dragable"] = dragable
        x, y = obj.drag
        dragable["x"] = x
        dragable["y"] = y
        dragable["confine"] = obj.drag_confine
        dragable["events"] = obj.drag_event

        self._class = State
        if self._type == edje.EDJE_PART_TYPE_IMAGE:
            self._class = StateImage
        elif self._type == edje.EDJE_PART_TYPE_GRADIENT:
            self._class = StateGradient
        elif self._type == edje.EDJE_PART_TYPE_TEXT:
            self._class = StateText
        elif self._type == edje.EDJE_PART_TYPE_EXTERNAL:
            self._class = StateExternal

        states = dict()
        self["states"] = states
        for state_name in obj.states:
            state = obj.state_get(state_name)
            states[state_name] = self._class(state)

    def _type_get(self):
        return self._type

    type = property(_type_get)

    def apply_to(self, obj):
        # type is imutable
        obj.mouse_events = self["mouse_events"]
        obj.repeat_events = self["repeat_events"]
        obj.clip_to = self["clip_to"]
        obj.source = self["source"]
        obj.effect = self["effect"]

        #dragable = self["dragable"]
        #obj.drag = (dragable["x"], dragable["y"])
        #obj.drag_confine = dragable["confine"]
        #obj.drag_event = dragable["events"]

        for state in self["states"].values():
            name = state.name
            obj.state_add(name.split()[0]) #TODO: Remove this hack
            state_obj = obj.state_get(name)
            state.apply_to(obj.state_get(name))

        return True

    def source_get(self, indent=""):
        ret = indent + 'part { name: "%s";\n' % self.name
        types = ["NONE", "RECT", "TEXT", "IMAGE", "SWALLOW", "TEXTBLOCK",
                 "GRADIENT", "GROUP", "BOX", "TABLE", "EXTERNAL"]
        ret += indent + '   type: %s;\n' % types[self.type]
        ret += indent + '   mouse_events: %d;\n' % int(self["mouse_events"])
        ret += indent + '   repeat_events: %d;\n' % int(self["repeat_events"])
        ret += indent + '   clip_to: "%s";\n' % self["clip_to"]
        ret += indent + '   source: "%s";\n' % self["source"]
        effects = ["NONE", "PLAIN", "OUTLINE", "SOFT_OUTLINE", "SHADOW",
                   "SOFT_SHADOW", "OUTLINE_SHADOW", "OUTLINE_SOFT_SHADOW ",
                   "FAR_SHADOW ", "FAR_SOFT_SHADOW", "GLOW"]
        ret += indent + '   effect: %s;\n' % effects[self["effect"]]
        for state in self["states"].values():
            ret += state.source_get("   ")
        ret += indent + '}\n'
        return ret


class State(Object):
    def __init__(self, obj):
        Object.__init__(self, obj.name)

        self["visible"] = obj.visible
        self["align"] = obj.align
        self["min"] = obj.min
        self["max"] = obj.max
        self["aspect"] = (obj.aspect_min_get(),
                                 obj.aspect_max_get())
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
        ret = indent + 'description { state: "%s";\n' % self.name
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
        image["normal"] = obj.image_get()
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


class StateGradient(State):
    def __init__(self, obj):
        State.__init__(self, obj)

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

        gradient = dict()
        self["gradient"] = gradient
        type = obj.gradient_type_get()
        if type is None:
            type = ""
        gradient["type"] = type
        spec = obj.gradient_spectra_get()
        if spec is None:
            spec = ""
        gradient["spectrum"] = spec

    def apply_to(self, obj):
        if obj.part_get().type != edje.EDJE_PART_TYPE_GRADIENT:
            return False

        State.apply_to(self, obj)

        gradient = self["gradient"]
        obj.gradient_type_set(gradient["type"])
        obj.gradient_spectra_set(gradient["spectrum"])

        return True

    def _source_get(self, indent=""):
        gradient = self["gradient"]
        ret = indent + '   gradient {\n'
        ret += indent + '      type: "%s";\n' % gradient["type"]
        ret += indent + '      spectrun: "%s";\n' % gradient["spectrum"]
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

        self["signal"] = obj.signal_get()
        self["source"] = obj.source_get()
        self["action"] = obj.action_get()
        self["state"] = obj.state_get()
        self["state2"] = obj.state2_get()
        self["value"] = obj.value_get()
        self["transition"] = obj.transition_get()
        self["transition_time"] = obj.transition_time_get()
        self["in"] = (obj.in_from_get(), obj.in_range_get())


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
            ret += indent + '   action: STATE_SET "%s" %.2f;\n' % (self["state"], self["value"])
        elif action == edje.EDJE_ACTION_TYPE_SIGNAL_EMIT:
            ret += indent + '   action: SIGNAL_EMIT "%s" "%s";\n' % (self["state"], self["state2"])

        trans["NONE", "LINEAR", "SINUSOIDAL", "ACCELERATE", "DECELERATE"]
        ret += indent + '   transition: %s %.5f;\n' % (trans[self["transition"]], self["transition_time"])
        ret += indent + '   in: %.5f %.5f;\n' % self["in"]

        for target in self["targets"]:
            ret += indent + '   target: "%s";\n' % target

        for after in self["afters"]:
            ret += indent + '   after: "%s";\n' % after
        ret += indent + '   }\n'
        return ret


class ColorClass(Object):
    pass


class Spectra(Object):
    pass


class Style(Object):
    pass

class Animation(Object):
    def __init__(self, obj):
        prog = re_anim_program_stop.match(obj.name)
        if not prog:
           raise TypeError("not animation end program")
        name = prog.group(0)

        Object.__init__(self, name)

        edj = obj.edje_get()
        keyframes = {}
        self["keyframes"] = keyframes
        for target in obj.targets_get():
            program = edj.program_get(target)
            if not program:
                continue

            time = re_anim_program.match(program.name).group(2)
            if time == "end":
                continue
            time = float(time)

            keyframes[time] = AnimatioKeyFrame(program)

    def apply_to(self, obj):
        pass


class AnimatioKeyFrame(Object):
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
            state = part.state_get(self._name+" 0.00")
            if not state:
                continue

            type = part.type
            stateclass = State
            if type == edje.EDJE_PART_TYPE_IMAGE:
                stateclass = StateImage
            elif type == edje.EDJE_PART_TYPE_GRADIENT:
                stateclass = StateGradient
            elif type == edje.EDJE_PART_TYPE_TEXT:
                stateclass = StateText
            elif type == edje.EDJE_PART_TYPE_EXTERNAL:
                stateclass = StateExternal

            targets[target] = stateclass(state)

    def apply_to(self, obj):
        name = obj.name
        edj = obj.edje_get()
        animation, time = program_time.match(name)

        obj.transition_set(self["transition"])

        obj.targets_clear()
        for target, data in self["targets"].values():
            obj.target_add(target)

            part = edj.part_get(target)
            if not part:
                continue
            state_name = name + " 0.00"
            part.state_add(state_name)
            state = part.state_get(state_name)
            if not state:
                continue
            data.apply_to(state)
