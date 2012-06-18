# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cdef class Transit(object):

    """Transit is designed to apply various animated transition effects to
    C{Evas_Object}, such like translation, rotation, etc. For using these
    effects, create a L{Transit} and add the desired transition effects.

    Once the effects are added into transit, they will be automatically managed
    (their callback will be called for the set duration and they will be deleted
    upon completion).

    Example::
        t = elementary.Transit()
        t.object_add(obj)
        t.effect_translation_add(0, 0, 280, 280)
        t.duration = 1
        t.auto_reverse = True
        t.tween_mode = elementary.ELM_TRANSIT_TWEEN_MODE_DECELERATE
        t.repeat_times = 3

    Some transition effects are used to change the properties of objects. They
    are:
        - L{effect_translation_add}
        - L{effect_color_add}
        - L{effect_rotation_add}
        - L{effect_wipe_add}
        - L{effect_zoom_add}
        - L{effect_resizing_add}

    Other transition effects are used to make one object disappear and another
    object appear on its place. These effects are:
        - L{effect_flip_add}
        - L{effect_resizable_flip_add}
        - L{effect_fade_add}
        - L{effect_blend_add}

    It's also possible to make a transition chain with L{chain_transit_add}.

    @warning: We strongly recommend to use elm_transit just when edje can
    not do the trick. Edje is better at handling transitions than
    Elm_Transit. Edje has more flexibility and animations can be manipulated
    inside the theme.

    """

    cdef Elm_Transit* obj

    def __cinit__(self, *a, **ka):
        self.obj = NULL

    def __init__(self):
        """Create new transit.

        @note: It is not necessary to delete the transit object, it will be
            deleted at the end of its operation.
        @note: The transit will start playing when the program enters the
            main loop.

        @return: The transit object.

        """
        self.obj = elm_transit_add()

    def delete(self):
        """delete()

        Stops the animation and delete the C{transit} object.

        Call this function if you want to stop the animation before the
        transit time. Make sure the C{transit} object is still alive with
        L{del_cb_set()} function. All added effects will be deleted, calling
        its respective data_free_cb functions. The function set by
        L{del_cb_set()} will be called.

        L{del_cb_set()}

        """
        elm_transit_del(self.obj)

    #def effect_add(self, Elm_Transit_Effect_Transition_Cb transition_cb, effect, Elm_Transit_Effect_End_Cb end_cb):
        """effect_ass(transition_cb, effect, end_cb)

        Add a new effect to the transit.

        Example::
            t = Transit()
            t.effect_add(   elm_transit_effect_blend_op,
                            elm_transit_effect_blend_context_new(),
                            elm_transit_effect_blend_context_free)

        @warning: The transit will free the context data at the and of the
            transition with the data_free_cb function. Do not share the
            context data in between different transit objects.

        @note: The cb function and the data are the key to the effect. If
            you try to add an existing effect, nothing is done.
        @note: After the first addition of an effect to C{transit}, if its
            effect list become empty again, the C{transit} will be killed by
            elm_transit_del(transit) function.

        @param transition_cb: The operation function. It is called when the
            animation begins, it is the function that actually performs the
            animation. It is called with the C{data}, C{transit} and the time
            progression of the animation (a double value between 0.0 and
            1.0).
        @param effect: The context data of the effect.
        @param end_cb: The function to free the context data, it will be
            called at the end of the effect, it must finalize the animation
            and free the C{data}.

        """
        #elm_transit_effect_add(self.obj, transition_cb, effect, end_cb)

    #def effect_del(self, Elm_Transit_Effect_Transition_Cb transition_cb, effect):
        """effect_del(transition_cb, effect)

        Delete an added effect.

        This function will remove the effect from the C{transit}, calling the
        data_free_cb to free the C{data}.

        L{effect_add()}

        @note: If the effect is not found, nothing is done.
        @note: If the effect list become empty, this function will call
            elm_transit_del(transit), i.e., it will kill the C{transit}.

        @param transition_cb: The operation function.
        @param effect: The context data of the effect.

        """
        #elm_transit_effect_del(self.obj, transition_cb, effect)

    def object_add(self, evasObject obj):
        """object_add(obj)

        Add new object to apply the effects.

        @note: After the first addition of an object to C{transit}, if its
            object list become empty again, the C{transit} will be killed by
            elm_transit_del(transit) function.
        @note: If the C{obj} belongs to another transit, the C{obj} will be
            removed from it and it will only belong to the other C{transit}.
            If the old transit stays without objects, it will die.
        @note: When you add an object into the C{transit}, its state from
            evas_object_pass_events_get(obj) is saved, and it is applied
            when the transit ends, if you change this state with
            evas_object_pass_events_set() after add the object, this state
            will change again when C{transit} stops.

        @param obj: Object to be animated.

        @warning: It is not allowed to add a new object after transit begins.

        """
        elm_transit_object_add(self.obj, obj.obj)

    def object_remove(self, evasObject obj):
        """object_remove(obj)

        Removes an added object from the transit.

        @note: If the C{obj} is not in the C{transit}, nothing is done.
        @note: If the list become empty, this function will call
            transit.delete(), i.e., it will kill the C{transit}.

        @param obj: Object to be removed from C{transit}.

        @warning: It is not allowed to remove objects after transit begins.

        """
        elm_transit_object_remove(self.obj, obj.obj)

    property objects:
        """The objects of the transit.

        type: list

        """
        def __get__(self):
            cdef Eina_List *lst
            cdef Evas_Object *data
            ret = []
            lst = <Eina_List *>elm_transit_objects_get(self.obj)
            while lst:
                data = <Evas_Object *>lst.data
                if data != NULL:
                    o = Object_from_instance(data)
                    ret.append(o)
                lst = lst.next
            return ret

    property objects_final_state_keep:
        """Enable/disable keeping up the objects states.

        If it is not kept, the objects states will be reset when transition
        ends.

        @note: One state includes geometry, color, map data.

        @type: bool

        """
        def __set__(self, state_keep):
            elm_transit_objects_final_state_keep_set(self.obj, state_keep)

        def __get__(self):
            return bool(elm_transit_objects_final_state_keep_get(self.obj))

    property event_enabled:
        """Set the event enabled when transit is operating.

        If C{enabled} is True, the objects of the transit will receive
        events from mouse and keyboard during the animation.

        @note: When you add an object with L{object_add()}, its
        state from evas_object_freeze_events_get(obj) is saved, and it is
        applied when the transit ends. If you change this state with
        evas_object_freeze_events_set() after adding the object, this state
        will change again when C{transit} stops to run.

        @type: bool

        """
        def __set__(self, enabled):
            elm_transit_event_enabled_set(self.obj, enabled)
        def __get__(self):
            return bool(elm_transit_event_enabled_get(self.obj))

    def del_cb_set(self, cb, *args, **kwargs):
        """Set the user-callback function when the transit is deleted.

        @note: Using this function twice will overwrite the first function set.
        @note: the C{transit} object will be deleted after call C{cb} function.

        @param cb: Callback function pointer. This function will be called before
        the deletion of the transit.
        @param data: Callback function user data. It is the C{op} parameter.

        """
        pass
        #elm_transit_del_cb_set(self.obj, cb, data)

    property auto_reverse:
        """If auto reverse is set, after running the effects with the
        progress parameter from 0 to 1, it will call the effects again with
        the progress from 1 to 0.

        The transit will last for a time equal to (2 * duration * repeat),
        where the duration was set with the function elm_transit_add and
        the repeat with the function L{repeat_times_set()}.

        @type: bool

        """
        def __set__(self, reverse):
            elm_transit_auto_reverse_set(self.obj, reverse)
        def __get__(self):
            return bool(elm_transit_auto_reverse_get(self.obj))

    property repeat_times:
        """The transit repeat count. Effect will be repeated by repeat count.

        This property reflects the number of repetition the transit will run
        after the first one, i.e., if C{repeat} is 1, the transit will run 2
        times. If the C{repeat} is a negative number, it will repeat
        infinite times.

        @note: If this function is called during the transit execution, the
            transit will run C{repeat} times, ignoring the times it already
            performed.

        @type: int

        """
        def __set__(self, repeat):
            elm_transit_repeat_times_set(self.obj, repeat)
        def __get__(self):
            return elm_transit_repeat_times_get(self.obj)

    property tween_mode:
        """The transit animation acceleration type.

        This property reflects the tween mode of the transit that can be:
            - ELM_TRANSIT_TWEEN_MODE_LINEAR - The default mode.
            - ELM_TRANSIT_TWEEN_MODE_SINUSOIDAL - Starts in accelerate mode
                and ends decelerating.
            - ELM_TRANSIT_TWEEN_MODE_DECELERATE - The animation will be
                slowed over time.
            - ELM_TRANSIT_TWEEN_MODE_ACCELERATE - The animation will
                accelerate over time.

        @type: Elm_Transit_Tween_Mode

        """
        def __set__(self, tween_mode):
            elm_transit_tween_mode_set(self.obj, tween_mode)
        def __get__(self):
            return elm_transit_tween_mode_get(self.obj)

    property duration:
        """Set the transit animation time

        @type: float

        """
        def __set__(self, duration):
            elm_transit_duration_set(self.obj, duration)
        def __get__(self):
            return elm_transit_duration_get(self.obj)

    def go(self):
        """go()

        Starts the transition. Once this API is called, the transit
        begins to measure the time.

        """
        elm_transit_go(self.obj)

    property paused:
        """Pause/Resume the transition.

        If you call elm_transit_go again, the transit will be started from the
        beginning, and will be played.

        @type: bool

        """
        def __set__(self, paused):
            elm_transit_paused_set(self.obj, paused)
        def __get__(self):
            return bool(elm_transit_paused_get(self.obj))

    property progress_value:
        """Get the time progression of the animation (a double value between
        0.0 and 1.0).

        The value returned is a fraction (current time / total time). It
        represents the progression position relative to the total.

        @type: float

        """
        def __get__(self):
            return elm_transit_progress_value_get(self.obj)

    def chain_transit_add(self, Transit chain_transit):
        """chain_transit_add(chain_transit)

        Makes the chain relationship between two transits.

        This function adds C{chain_transit} transition to a chain after the
        C{transit}, and will be started as soon as C{transit} ends.

        @note: C{chain_transit} can not be None. Chain transits could be
            chained to the only one transit.

        @param chain_transit: The chain transit object. This transit will be
            operated after transit is done.

        """
        elm_transit_chain_transit_add(self.obj, chain_transit.obj)

    def chain_transit_del(self, Transit chain_transit):
        """chain_transit_del(chain_transit)

        Cut off the chain relationship between two transits.

        This function removes the C{chain_transit} transition from the
        C{transit}.

        @note: C{chain_transit} can not be None. Chain transits should be
            chained to the C{transit}.

        @param chain_transit: The chain transit object.

        """
        elm_transit_chain_transit_del(self.obj, chain_transit.obj)

    property chain_transits:
        """Get the current chain transit list.

        @type: list

        """
        def __get__(self):
            cdef Eina_List *lst
            cdef Evas_Object *data
            ret = []
            lst = elm_transit_chain_transits_get(self.obj)
            while lst:
                data = <Evas_Object *>lst.data
                if data != NULL:
                    o = Object_from_instance(data)
                    ret.append(o)
                lst = lst.next
            return ret

    def effect_resizing_add(self, Evas_Coord from_w, Evas_Coord from_h, Evas_Coord to_w, Evas_Coord to_h):
        """effect_resizing_add(from_w, from_h, to_w, to_h)

        Add the Resizing Effect to Elm_Transit.

        @note: This API is one of the facades. It creates resizing effect
            context and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param from_w: Object width size when effect begins.
        @type from_w: Evas_Coord (int)
        @param from_h: Object height size when effect begins.
        @type from_h: Evas_Coord (int)
        @param to_w: Object width size when effect ends.
        @type to_w: Evas_Coord (int)
        @param to_h: Object height size when effect ends.
        @type to_h: Evas_Coord (int)
        @return: Resizing effect context data.
        @rtype: Elm_Transit_Effect

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_resizing_add(self.obj, from_w, from_h, to_w, to_h)

    def effect_translation_add(self, Evas_Coord from_dx, Evas_Coord from_dy, Evas_Coord to_dx, Evas_Coord to_dy):
        """effect_translation_add(from_dx, from_dy, to_dx, to_dy)

        Add the Translation Effect to Elm_Transit.

        @note: This API is one of the facades. It creates translation effect
            context and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param from_dx: X Position variation when effect begins.
        @param from_dy: Y Position variation when effect begins.
        @param to_dx: X Position variation when effect ends.
        @param to_dy: Y Position variation when effect ends.
        @return: Translation effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_translation_add(self.obj, from_dx, from_dy, to_dx, to_dy)

    def effect_zoom_add(self, float from_rate, float to_rate):
        """effect_zoom_add(from_rate, to_rate)

        Add the Zoom Effect to Elm_Transit.

        @note: This API is one of the facades. It creates zoom effect context
            and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param from_rate: Scale rate when effect begins (1 is current rate).
        @param to_rate: Scale rate when effect ends.
        @return: Zoom effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_zoom_add(self.obj, from_rate, to_rate)

    def effect_flip_add(self, Elm_Transit_Effect_Flip_Axis axis, Eina_Bool cw):
        """effect_flip_add(axis, cw)

        Add the Flip Effect to Elm_Transit.

        @note: This API is one of the facades. It creates flip effect context
            and add it's required APIs to elm_transit_effect_add.
        @note: This effect is applied to each pair of objects in the order
            they are listed in the transit list of objects. The first object
            in the pair will be the "front" object and the second will be the
            "back" object.

        @see: L{effect_add()}

        @param axis: Flipping Axis(X or Y).
        @param cw: Flipping Direction. True is clock-wise.
        @return: Flip effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_flip_add(self.obj, axis, cw)

    def effect_resizable_flip_add(self, Elm_Transit_Effect_Flip_Axis axis, Eina_Bool cw):
        """effect_resizable_flip_add(axis, cw)

        Add the Resizeable Flip Effect to Elm_Transit.

        @note: This API is one of the facades. It creates resizable flip
            effect context and add it's required APIs to
            elm_transit_effect_add.
        @note: This effect is applied to each pair of objects in the order
            they are listed in the transit list of objects. The first object
            in the pair will be the "front" object and the second will be the
            "back" object.

        @see: L{effect_add()}

        @param axis: Flipping Axis(X or Y).
        @param cw: Flipping Direction. True is clock-wise.
        @return: Resizeable flip effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_resizable_flip_add(self.obj, axis, cw)

    def effect_wipe_add(self, Elm_Transit_Effect_Wipe_Type type, Elm_Transit_Effect_Wipe_Dir dir):
        """effect_wipe_add(type, dir)

        Add the Wipe Effect to Elm_Transit.

        @note: This API is one of the facades. It creates wipe effect context
            and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param type: Wipe type. Hide or show.
        @param dir: Wipe Direction.
        @return: Wipe effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_wipe_add(self.obj, type, dir)

    def effect_color_add(self, unsigned int from_r, unsigned int from_g, unsigned int from_b, unsigned int from_a, unsigned int to_r, unsigned int to_g, unsigned int to_b, unsigned int to_a):
        """effect_color_add(from_r, from_g, from_b, from_a, to_r, to_g, to_b, to_a)

        Add the Color Effect to Elm_Transit.

        @note: This API is one of the facades. It creates color effect
            context and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param  from_r:        RGB R when effect begins.
        @param  from_g:        RGB G when effect begins.
        @param  from_b:        RGB B when effect begins.
        @param  from_a:        RGB A when effect begins.
        @param  to_r:          RGB R when effect ends.
        @param  to_g:          RGB G when effect ends.
        @param  to_b:          RGB B when effect ends.
        @param  to_a:          RGB A when effect ends.
        @return:               Color effect context data.
        @rtype: Elm_Transit_Effect

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_color_add(self.obj, from_r, from_g, from_b, from_a, to_r, to_g, to_b, to_a)

    def effect_fade_add(self):
        """effect_face_add()

        Add the Fade Effect to Elm_Transit.

        @note: This API is one of the facades. It creates fade effect context
            and add it's required APIs to elm_transit_effect_add.
        @note: This effect is applied to each pair of objects in the order
            they are listed in the transit list of objects. The first object
            in the pair will be the "before" object and the second will be
            the "after" object.

        @see: L{effect_add()}

        @return: Fade effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            color information about the objects, and if the window was not
            created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_fade_add(self.obj)

    def effect_blend_add(self):
        """effect_blend_add()

        Add the Blend Effect to Elm_Transit.

        @note: This API is one of the facades. It creates blend effect
            context and add it's required APIs to elm_transit_effect_add.
        @note: This effect is applied to each pair of objects in the order
            they are listed in the transit list of objects. The first object
            in the pair will be the "before" object and the second will be
            the "after" object.

        @see: L{effect_add()}

        @return: Blend effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            color information about the objects, and if the window was not
            created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_blend_add(self.obj)

    def effect_rotation_add(self, float from_degree, float to_degree):
        """effect_rotation_add(from_degree, to_degree)

        Add the Rotation Effect to Elm_Transit.

        @note: This API is one of the facades. It creates rotation effect
            context and add it's required APIs to elm_transit_effect_add.

        @see: L{effect_add()}

        @param from_degree: Degree when effect begins.
        @type from_degree: float
        @param to_degree: Degree when effect is ends.
        @type to_degree: float
        @return: Rotation effect context data.
        @rtype: Elm_Transit_Effect

        @warning: It is highly recommended just create a transit with this
            effect when the window that the objects of the transit belongs
            has already been created. This is because this effect needs the
            geometry information about the objects, and if the window was
            not created yet, it can get a wrong information.

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_rotation_add(self.obj, from_degree, to_degree)

    def effect_image_animation_add(self, images):
        """effect_image_animation_add(images)

        Add the ImageAnimation Effect to Elm_Transit.

        Example::
            t = Transit()
            images = []
            images.append("%s/images/icon_11.png" % PACKAGE_DATA_DIR)
            images.append("%s/images/logo_small.png" % PACKAGE_DATA_DIR)
            t.effect_image_animation_add(images)

        @note: This API is one of the facades. It creates image animation
            effect context and add it's required APIs to
            elm_transit_effect_add. The C{images} parameter is a list images
            paths. This list and its contents will be deleted at the end of
            the effect by elm_transit_effect_image_animation_context_free()
            function.

        @see: L{effect_add()}

        @param images: A list of images file paths. This list and its
            contents will be deleted at the end of the effect by
            elm_transit_effect_image_animation_context_free() function.
        @type images: list
        @return: Image Animation effect context data.
        @rtype: Elm_Transit_Effect

        """
        #TODO: can the return value Elm_Transit_Effect be used somehow?
        elm_transit_effect_image_animation_add(self.obj, _strings_from_python(images))
