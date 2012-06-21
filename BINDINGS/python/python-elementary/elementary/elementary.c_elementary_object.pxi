# Copyright (c) 2008-2009 Simon Busch
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

from evas.c_evas cimport Object as evasObject
from evas.c_evas cimport EventKeyDown, EventKeyUp
from evas.c_evas cimport evas_object_data_get
from evas.c_evas cimport evas_object_smart_callback_add
from evas.c_evas cimport evas_object_smart_callback_del
from evas.c_evas cimport EVAS_CALLBACK_KEY_DOWN
from evas.c_evas cimport EVAS_CALLBACK_KEY_UP
from evas.c_evas cimport eina_list_append

from evas.c_evas import _extended_object_mapping_register

#API XXX: Callbacks!
cdef void _object_callback(void *data,
                           Evas_Object *o, void *event_info) with gil:
    cdef Object obj
    cdef object event, ei
    obj = <Object>evas_object_data_get(o, "python-evas")
    event = <object>data
    lst = tuple(obj._elmcallbacks[event])
    for event_conv, func, args, kargs in lst:
        try:
            if event_conv is None:
                func(obj, *args, **kargs)
            else:
                ei = event_conv(<long>event_info)
                func(obj, ei, *args, **kargs)
        except Exception, e:
            traceback.print_exc()

cdef Evas_Object *_tooltip_content_create(void *data, Evas_Object *o, Evas_Object *t) with gil:
    cdef Object ret, obj, tooltip

    obj = <Object>evas_object_data_get(o, "python-evas")
    tooltip = Object_from_instance(t)
    (func, args, kargs) = <object>data
    ret = func(obj, tooltip, *args, **kargs)
    if not ret:
        return NULL
    return ret.obj

cdef void _tooltip_data_del_cb(void *data, Evas_Object *o, void *event_info) with gil:
    Py_DECREF(<object>data)

cdef Eina_Bool _event_dispatcher(o, src, Evas_Callback_Type t, event_info):
    cdef Object obj = o
    cdef object ret
    for func, args, kargs in obj._elm_event_cbs:
        try:
            ret = func(obj, src, t, event_info, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
        else:
            if ret:
                return True
    return False

#TODO: More event types
cdef Eina_Bool _event_callback(void *data, Evas_Object *o, Evas_Object *src, Evas_Callback_Type t, void *event_info) with gil:
    cdef Object obj = <Object>Object_from_instance(o)
    cdef Object src_obj = <Object>Object_from_instance(src)
    cdef Eina_Bool ret = False
    cdef EventKeyDown down_event
    cdef EventKeyUp up_event
    if t == EVAS_CALLBACK_KEY_DOWN:
        down_event = EventKeyDown()
        down_event._set_obj(event_info)
        ret = _event_dispatcher(obj, src_obj, t, down_event)
        down_event._unset_obj()
    elif t == EVAS_CALLBACK_KEY_UP:
        up_event = EventKeyUp()
        up_event._set_obj(event_info)
        ret = _event_dispatcher(obj, src_obj, t, up_event)
        up_event._unset_obj()


cdef void _event_data_del_cb(void *data, Evas_Object *o, void *event_info) with gil:
    Py_DECREF(<object>data)

cdef _strings_to_python(const_Eina_List *lst):
    cdef const_char_ptr s
    ret = []
    while lst:
        s = <const_char_ptr>lst.data
        if s != NULL:
            ret.append(_ctouni(s))
        lst = lst.next
    return ret

cdef Eina_List * _strings_from_python(strings):
    cdef Eina_List *lst = NULL
    for s in strings:
        lst = eina_list_append(lst, _cfruni(s))
    return lst

def _cb_string_conv(long addr):
    cdef const_char_ptr s = <const_char_ptr>addr
    if s == NULL:
        return None
    else:
        return s

cdef _object_list_to_python(const_Eina_List *lst):
    cdef Evas_Object *o
    ret = []
    while lst:
        o = <Evas_Object *>lst.data
        obj = Object_from_instance(o)
        ret.append(obj)
        lst = lst.next
    return ret

cdef class Canvas(evas.c_evas.Canvas):
    def __init__(self):
        pass

cdef public class Object(evasObject) [object PyElementaryObject, type PyElementaryObject_Type]:

    """An abstract class to manage object and callback handling.

    All widgets are based on this class.

    @group General: part_text_set, text_set, part_text_get, text_get, text,
        part_content_*, content_*, access_info_set, name_find, signal_emit,
        elm_event_callback_*, domain_translatable_*, translatable_*
    @group Styles: style_*, disabled_*, disabled
    @group Widget Tree Navigation: widget_check, parent_widget_get,
        top_widget_get, widget_type_get
    @group Cursors: cursor_*
    @group Focus: focus_*
    @group Mirroring: mirrored_*
    @group Widget Scaling: scale_*
    @group Scrollhints: scroll_*
    @group Tooltips: tooltip_*

    @group Callbacks: callback_*

    """

    cdef object _elmcallbacks
    cdef object _elm_event_cbs

    def part_text_set(self, part, text):
        """part_text_set(part, text)

        Sets the text of a given part of this object.

        @see: L{text_set()} and L{part_text_get()}

        @param part: part name to set the text.
        @type part: string
        @param text: text to set.
        @type text: string

        """
        elm_object_part_text_set(self.obj, _cfruni(part) if part is not None else NULL, _cfruni(text))

    def text_set(self, text):
        """text_set(text)

        Sets the main text for this object.

        @see: L{text_get()} and L{part_text_set()}

        @param text: any text to set as the main textual part of this object.
        @type text: string

        """
        elm_object_text_set(self.obj, _cfruni(text))

    def part_text_get(self, part):
        """part_text_get(part)

        Gets the text of a given part of this object.

        @see: L{text_get()} and L{part_text_set()}

        @param part: part name to get the text.
        @type part: string
        @return: the text of a part or None if nothing was set.
        @rtype: string

        """
        return _ctouni(elm_object_part_text_get(self.obj, _cfruni(part) if part is not None else NULL))

    def text_get(self):
        """text_get()

        Gets the main text for this object.

        @see: L{text_set()} and L{part_text_get()}

        @return: the main text or None if nothing was set.
        @rtype: string

        """
        return _ctouni(elm_object_text_get(self.obj))

    property text:
        def __get__(self):
            return self.text_get()

        def __set__(self, value):
            self.text_set(value)

    def part_content_set(self, part, evasObject content):
        """part_content_set(part, content)

        Set a content of an object

        This sets a new object to a widget as a content object. If any
        object was already set as a content object in the same part,
        previous object will be deleted automatically.

        @note: Elementary objects may have many contents

        @param part: The content part name to set (None for the default content)
        @type part: string
        @param content: The new content of the object
        @type content: L{Object}

        """
        elm_object_part_content_set(self.obj, _cfruni(part) if part is not None else NULL, content.obj)

    def content_set(self, evasObject obj):
        elm_object_part_content_set(self.obj, NULL, obj.obj)

    def part_content_get(self, part):
        """part_content_get(part)

        Get a content of an object

        @note: Elementary objects may have many contents

        @param part: The content part name to get (None for the default content)
        @type part: string
        @return: content of the object or None for any error
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_part_content_get(self.obj, _cfruni(part) if part is not None else NULL))

    def content_get(self):
        return Object_from_instance(elm_object_content_get(self.obj))

    def part_content_unset(self, part):
        """part_content_unset(part)

        Unset a content of an object

        @note: Elementary objects may have many contents

        @param part: The content part name to unset (None for the default
            content)
        @type part: string

        """
        return Object_from_instance(elm_object_part_content_unset(self.obj, _cfruni(part) if part is not None else NULL))

    def content_unset(self):
        return Object_from_instance(elm_object_content_unset(self.obj))

    property content:
        def __get__(self):
            return self.content_get()
        def __set__(self, content):
            self.content_set(content)
        def __del__(self):
            self.content_unset()

    def access_info_set(self, txt):
        """access_info_set(txt)

        Set the text to read out when in accessibility mode

        @param txt: The text that describes the widget to people with poor or no vision
        @type txt: string

        """
        elm_object_access_info_set(self.obj, _cfruni(txt))

    def name_find(self, name not None, int recurse = 0):
        """name_find(name, recurse)

        Get a named object from the children

        This function searches the children (or recursively children of
        children and so on) of the given object looking for a child with the
        name of C{name}. If the child is found the object is returned, or
        None is returned. You can set the name of an object with
        L{name_set()}. If the name is not unique within the child objects
        (or the tree is C{recurse} is greater than 0) then it is undefined
        as to which child of that name is returned, so ensure the name is
        unique amongst children. If recurse is set to -1 it will recurse
        without limit.

        @param name: The name of the child to find
        @type name: string
        @param recurse: Set to the maximum number of levels to recurse (0 == none, 1 is only look at 1 level of children etc.)
        @type recurse: int
        @return: The found object of that name, or None if none is found
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_name_find(self.obj, _cfruni(name), recurse))

    def style_set(self, style):
        """style_set(style)

        Set the style to used by a given widget

        This sets the style (by name) that will define the appearance of a
        widget. Styles vary from widget to widget and may also be defined
        by other themes by means of extensions and overlays.

        @see: elm_theme_extension_add()
        @see: elm_theme_extension_del()
        @see: elm_theme_overlay_add()
        @see: elm_theme_overlay_del()

        @param style: The name of the style to use on it
        @type style: string
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        elm_object_style_set(self.obj, _cfruni(style))

    def style_get(self):
        """style_get()

        Get the style used by the widget

        This gets the style being used for that widget. Note that the string
        pointer is only valid as long as the object is valid and the style doesn't
        change.

        @see: L{style_set()}

        @return: The style name used
        @rtype: string

        """
        return _ctouni(elm_object_style_get(self.obj))

    property style:
        """The style to be used by the widget

        @type: string

        """
        def __get__(self):
            return self.style_get()
        def __set__(self, value):
            self.style_set(value)

    def disabled_set(self, disabled):
        """disabled_set(disabled)

        Set the disabled state of an Elementary object.

        Elementary objects can be B{disabled}, in which state they won't
        receive input and, in general, will be themed differently from
        their normal state, usually greyed out. Useful for contexts
        where you don't want your users to interact with some of the
        parts of you interface.

        This sets the state for the widget, either disabling it or
        enabling it back.

        @param disabled: The state to put in in: C{True} for
            disabled, C{False} for enabled
        @type disabled: bool

        """
        elm_object_disabled_set(self.obj, disabled)

    def disabled_get(self):
        """disabled_get()

        Get the disabled state of an Elementary object.

        This gets the state of the widget, which might be enabled or disabled.

        @return: C{True}, if the widget is disabled, C{False} if it's enabled
            (or on errors)
        @rtype: bool

        """
        return bool(elm_object_disabled_get(self.obj))

    property disabled:
        """The disabled state of an Elementary object.

        Elementary objects can be B{disabled}, in which state they won't
        receive input and, in general, will be themed differently from
        their normal state, usually greyed out. Useful for contexts
        where you don't want your users to interact with some of the
        parts of you interface.

        @type: bool

        """
        def __get__(self):
            return self.disabled_get()
        def __set__(self, disabled):
            self.disabled_set(disabled)

    def widget_check(self):
        """widget_check()

        Check if the given Evas Object is an Elementary widget.

        @return: C{True} if it is an elementary widget variant, C{False}
            otherwise
        @rtype: bool

        """
        return bool(elm_object_widget_check(self.obj))

    def parent_widget_get(self):
        """parent_widget_get()

        Get the first parent of the given object that is an Elementary widget.

        Use this to query for an object's parent widget.

        @note: Most of Elementary users wouldn't be mixing non-Elementary
            smart objects in the objects tree of an application, as this is
            an advanced usage of Elementary with Evas. So, except for the
            application's window, which is the root of that tree, all other
            objects would have valid Elementary widget parents.

        @return: the parent object that is an Elementary widget, or C{None},
            if it was not found.
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_parent_widget_get(self.obj))

    property parent_widget:
        """The first parent of the given object that is an Elementary
        widget. This is a readonly property.

        @type: L{Object}

        """
        def __get__(self):
            return self.parent_widget_get()

    def top_widget_get(self):
        """top_widget_get()

        Get the top level parent of an Elementary widget.

        @return: The top level Elementary widget, or C{None} if parent
            cannot be found.
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_top_widget_get(self.obj))

    property top_widget:
        """The top level parent of an Elementary widget.
        This is a readonly property.

        @type: L{Object}

        """
        def __get__(self):
            return self.top_widget_get()

    def widget_type_get(self):
        """widget_type_get()

        Get the string that represents this Elementary widget.

        @note: Elementary is weird and exposes itself as a single
            Evas_Object_Smart_Class of type "elm_widget", so
            evas_object_type_get() always return that, making debug and
            language bindings hard. This function tries to mitigate this
            problem, but the solution is to change Elementary to use
            proper inheritance.

        @return: Elementary widget name, or C{None} if not a valid widget.
        @rtype: string

        """
        return elm_object_widget_type_get(self.obj)

    property widget_type:
        """The string that represents this Elementary widget.
        This is a readonly property.

        @type: L{Object}

        """
        def __get__(self):
            return elm_object_widget_type_get(self.obj)

    def signal_emit(self, emission, source):
        """signal_emit(emission, source)

        Send a signal to the widget edje object.

        This function sends a signal to the edje object of the obj. An edje
        program can respond to a signal by specifying matching 'signal' and
        'source' fields.

        @param emission: The signal's name.
        @type emission: string
        @param source: The signal's source.
        @type source: string

        """
        elm_object_signal_emit(self.obj, _cfruni(emission), _cfruni(source))

    #def signal_callback_add(self, emission, source, func, data):
        #elm_object_signal_callback_add(self.obj, emission, source, func, data)

    #def signal_callback_del(self, emission, source, func):
        #elm_object_signal_callback_del(self.obj, emission, source, func)

    # XXX: Clashes badly with evas event_callback_*
    def elm_event_callback_add(self, func, *args, **kargs):
        """elm_event_callback_add(func, *args, **kargs)

        Add a callback for input events (key up, key down, mouse wheel)
        on a given Elementary widget

        Every widget in an Elementary interface set to receive focus, with
        elm_object_focus_allow_set(), will propagate B{all} of its key up,
        key down and mouse wheel input events up to its parent object, and
        so on. All of the focusable ones in this chain which had an event
        callback set, with this call, will be able to treat those events.
        There are two ways of making the propagation of these event upwards
        in the tree of widgets to B{cease}:
            - Just return C{True} on C{func}. C{False} will mean the event
                was B{not} processed, so the propagation will go on.
            - The C{event_info} pointer passed to C{func} will contain the
                event's structure and, if you OR its C{event_flags} inner
                value to C{EVAS_EVENT_FLAG_ON_HOLD}, you're telling
                Elementary one has already handled it, thus killing the
                event's propagation, too.

        @note: Your event callback will be issued on those events taking
            place only if no other child widget has consumed the event already.

        @note: Not to be confused with C{evas_object_event_callback_add()},
            which will add event callbacks per type on general Evas objects
            (no event propagation infrastructure taken in account).

        @note: Not to be confused with L{signal_callback_add()},
            which will add callbacks to B{signals} coming from a widget's
            theme, not input events.

        @note: Not to be confused with C{edje_object_signal_callback_add()},
            which does the same as L{signal_callback_add()}, but directly on
            an Edje object.

        @note: Not to be confused with C{evas_object_smart_callback_add()},
            which adds callbacks to smart objects' B{smart events}, and not
            input events.

        @see: L{event_callback_del()}

        @param func: The callback function to be executed when the event
            happens
        @type func: function
        @param args: Optional arguments containing data passed to C{func}
        @param kargs: Optional keyword arguments containing data passed to
            C{func}

        """
        if not callable(func):
            raise TypeError("func must be callable")

        if self._elm_event_cbs is None:
            self._elm_event_cbs = []

        if not self._elm_event_cbs:
            elm_object_event_callback_add(self.obj, _event_callback, NULL)

        data = (func, args, kargs)
        self._elm_event_cbs.append(data)

    def elm_event_callback_del(self, func, *args, **kargs):
        """elm_event_callback_del(func, *args, **kargs)

        Remove an event callback from a widget.

        This function removes a callback, previously attached to event emission.
        The parameters func and args, kwargs must match exactly those passed to
        a previous call to L{event_callback_add()}.

        @param func: The callback function to be executed when the event is
            emitted.
        @type func: function
        @param args: Optional arguments containing data passed to C{func}
        @param kargs: Optional keyword arguments containing data passed to
            C{func}

        """
        data = (func, args, kargs)
        self._elm_event_cbs.remove(data)

        if not self._elm_event_cbs:
            elm_object_event_callback_del(self.obj, _event_callback, NULL)

    # Cursors
    def cursor_set(self, cursor):
        """cursor_set(cursor)

        Set the cursor to be shown when mouse is over the object

        Set the cursor that will be displayed when mouse is over the object.
        The object can have only one cursor set to it, so if this function
        is called twice for an object, the previous set will be unset.

        """
        elm_object_cursor_set(self.obj, _cfruni(cursor))

    def cursor_get(self):
        return _ctouni(elm_object_cursor_get(self.obj))

    def cursor_unset(self):
        """cursor_unset()

        Unset cursor for object

        Unset cursor for object, and set the cursor to default if the mouse
        was over this object.

        """
        elm_object_cursor_unset(self.obj)

    property cursor:
        """The cursor to be shown when mouse is over the object

        @type: string

        """
        def __get__(self):
            return self.cursor_get()
        def __set__(self, value):
            self.cursor_set(value)
        def __del__(self):
            self.cursor_unset()

    def cursor_style_set(self, style=None):
        """cursor_style_set(style)

        Sets a different style for this object cursor.

        @note: before you set a style you should define a cursor with
            elm_object_cursor_set()

        """
        elm_object_cursor_style_set(self.obj, _cfruni(style) if style is not None else NULL)

    def cursor_style_get(self):
        """cursor_style_get()

        Get the style for this object cursor.

        """
        return _ctouni(elm_object_cursor_style_get(self.obj))

    property cursor_style:
        """The style for this object cursor."""
        def __get__(self):
            return self.cursor_style_get()
        def __set__(self, value):
            self.cursor_style_set(value)

    def cursor_theme_search_enabled_set(self, engine_only):
        """cursor_theme_search_enabled_set(engine_only)

        Sets cursor engine only usage for this object.

        @note: before you set engine only usage you should define a cursor with
            elm_object_cursor_set()

        """
        elm_object_cursor_theme_search_enabled_set(self.obj, bool(engine_only))

    def cursor_theme_search_enabled_get(self):
        """cursor_theme_search_enabled_get()

        Get the engine only usage for this object.

        """
        return elm_object_cursor_theme_search_enabled_get(self.obj)

    property cursor_theme_search_enabled:
        def __get__(self):
            return self.cursor_theme_search_enabled_get()
        def __set__(self, value):
            self.cursor_theme_search_enabled_set(value)

    # Focus
    def focus_get(self):
        """focus_get()

        Get the whether an Elementary object has the focus or not.

        @see: L{focus_set()}
        @return: C{True}, if the object is focused, C{False} if
            not (and on errors).
        @rtype: bool

        """
        return bool(elm_object_focus_get(self.obj))

    def focus_set(self, focus):
        """focus_set(focus)

        Set/unset focus to a given Elementary object.

        @note: When you set focus to this object, if it can handle focus, will
            take the focus away from the one who had it previously and will, for
            now on, be the one receiving input events. Unsetting focus will remove
            the focus from the object, passing it back to the previous element in the
            focus chain list.

        @see: L{focus_get()}
        @see: L{focus_custom_chain_get()}

        @param focus: C{True} set focus to a given object,
            C{False} unset focus to a given object.
        @type focus: bool

        """
        elm_object_focus_set(self.obj, focus)

    def focus_allow_set(self, allow):
        """focus_allow_set(allow)

        Set the ability for an Elementary object to be focused

        This sets whether the object is able to take focus or not.
        Unfocusable objects do nothing when programmatically focused, being
        the nearest focusable parent object the one really getting focus.
        Also, when they receive mouse input, they will get the event, but
        not take away the focus from where it was previously.

        @param allow: C{True} if the object can be focused,
            C{False} if not (and on errors)
        @type allow: bool

        """
        elm_object_focus_allow_set(self.obj, allow)

    def focus_allow_get(self):
        """focus_allow_get()

        Get whether an Elementary object is focusable or not

        @note: Objects which are meant to be interacted with by input
            events are created able to be focused, by default. All the
            others are not.

        @return: C{True} if the object is allowed to be focused,
            C{False} if not (and on errors)
        @rtype: bool

        """
        return elm_object_focus_allow_get(self.obj)

    property focus_allow:
        """The ability for the Elementary object to be focused.

        @type: bool

        """
        def __get__(self):
            return self.focus_allow_get()
        def __set__(self, value):
            self.focus_allow_set(value)

    def focus_custom_chain_set(self, objs):
        """focus_custom_chain_set(objs)

        Set custom focus chain.

        This function overwrites any previous custom focus chain within
        the list of objects. The previous list will be deleted and this list
        will be managed by elementary. After it is set, don't modify it.

        @note: On focus cycle, only will be evaluated children of this container.

        @param objs: Chain of objects to pass focus
        @type objs: list of L{Object}s

        """
        elm_object_focus_custom_chain_unset(self.obj)
        cdef Object obj
        for obj in objs:
            elm_object_focus_custom_chain_append(self.obj, obj.obj, NULL)

    def focus_custom_chain_unset(self):
        """focus_custom_chain_unset()

        Unset a custom focus chain on a given Elementary widget

        Any focus chain previously set on the object (for its child objects)
        is removed entirely after this call.

        """
        elm_object_focus_custom_chain_unset(self.obj)

    def focus_custom_chain_get(self):
        """focus_custom_chain_get()

        Get custom focus chain

        @return: Chain of objects
        @rtype: list of L{Object}s

        """
        return _object_list_to_python(elm_object_focus_custom_chain_get(self.obj))

    property focus_custom_chain:
        """The custom focus chain.

        @type: list of L{Object}s

        """
        def __get__(self):
            return self.focus_custom_chain_get()
        def __set__(self, value):
            self.focus_custom_chain_set(value)
        def __del__(self):
            self.focus_custom_chain_unset()

    def focus_custom_chain_append(self, Object child, Object relative_child=None):
        """focus_custom_chain_append(child, relative_child=None)

        Append object to custom focus chain.

        @note: If relative_child equal to None or not in custom chain, the
            object will be added in end.

        @note: On focus cycle, only will be evaluated children of this
            container.

        @param child: The child to be added in custom chain
        @type child: L{Object}
        @param relative_child: The relative object to position the child
        @type relative_child: L{Object}

        """
        cdef Evas_Object *rel = NULL
        if relative_child:
            rel = relative_child.obj
        elm_object_focus_custom_chain_append(self.obj, child.obj, rel)

    def focus_custom_chain_prepend(self, Object child, Object relative_child=None):
        """focus_custom_chain_prepend(child, relative_child=None)

        Prepend object to custom focus chain.

        @note: If relative_child equal to None or not in custom chain, the
            object will be added in begin.

        @note: On focus cycle, only will be evaluated children of this
            container.

        @param child: The child to be added in custom chain
        @type child: L{Object}
        @param relative_child: The relative object to position the child
        @type relative_child: L{Object}

        """
        cdef Evas_Object *rel = NULL
        if relative_child:
            rel = relative_child.obj
        elm_object_focus_custom_chain_prepend(self.obj, child.obj, rel)

    #def focus_next(self, direction):
        """focus_next(direction)

        Give focus to next object in object tree.

        Give focus to next object in focus chain of one object sub-tree. If
        the last object of chain already have focus, the focus will go to the
        first object of chain.

        @param dir: Direction to move the focus
        @type dir: Elm_Focus_Direction

        """
        #elm_object_focus_next(self.obj, direction)

    def tree_focus_allow_set(self, focusable):
        """tree_focus_allow_set(focusable)

        Make the elementary object and its children to be focusable
        (or unfocusable).

        This sets whether the object and its children objects are able to
        take focus or not. If the tree is set as unfocusable, newest focused
        object which is not in this tree will get focus. This API can be
        helpful for an object to be deleted. When an object will be deleted
        soon, it and its children may not want to get focus (by focus
        reverting or by other focus controls). Then, just use this API
        before deleting.

        @see: L{tree_focus_allow_get()}

        @param focusable: C{True} for focusable, C{False} for unfocusable.
        @type focusable: bool

        """
        elm_object_tree_focus_allow_set(self.obj, focusable)

    def tree_focus_allow_get(self):
        """tree_focus_allow_get()

        Get whether an Elementary object and its children are focusable
        or not.

        @see: L{tree_focus_allow_set()}

        @return: C{True}, if the tree is focusable, C{False} if not (and on
            errors).

        """
        return bool(elm_object_tree_focus_allow_get(self.obj))

    property tree_focus_allow:
        """Whether the Elementary object and its children are focusable
        or not.

        @type: bool

        """
        def __get__(self):
            return self.tree_focus_allow_get()
        def __set__(self, value):
            self.tree_focus_allow_set(value)

    # Mirroring
    def mirrored_get(self):
        """mirrored_get()

        Get the widget's mirrored mode.

        @return: True if mirrored is set, False otherwise
        @rtype: bool

        """
        return bool(elm_object_mirrored_get(self.obj))

    def mirrored_set(self, mirrored):
        """mirrored_set(mirrored)

        Set the widget's mirrored mode.

        @param mirrored: True to set mirrored mode, False to unset it.
        @type mirrored: bool

        """
        elm_object_mirrored_set(self.obj, mirrored)

    property mirrored:
        """The widget's mirrored mode.

        @type: bool

        """
        def __get__(self):
            return self.mirrored_get()
        def __set__(self, value):
            self.mirrored_set(value)

    def mirrored_automatic_get(self):
        """mirrored_automatic_get()

        Returns the widget's mirrored mode setting.

        @return: mode setting of the object.
        @rtype: bool

        """
        return bool(elm_object_mirrored_automatic_get(self.obj))

    def mirrored_automatic_set(self, automatic):
        """mirrored_automatic_set(automatic)

        Sets the widget's mirrored mode setting. When widget in automatic
        mode, it follows the system mirrored mode set by elm_mirrored_set().

        @param automatic: True for auto mirrored mode, False for manual.
        @type automatic: bool

        """
        elm_object_mirrored_automatic_set(self.obj, automatic)

    property mirrored_automatic:
        """The widget's mirrored mode setting.

        @type: bool

        """
        def __get__(self):
            return self.mirrored_automatic_get()
        def __set__(self, value):
            self.mirrored_automatic_set(value)

    # Scaling
    def scale_set(self, scale):
        """scale_set(scale)

        Set the scaling factor for a given Elementary object

        @param scale: Scale factor (from C{0.0} up, with C{1.0} meaning
            no scaling)
        @type scale: float

        """
        elm_object_scale_set(self.obj, scale)

    def scale_get(self):
        """scale_get()

        Get the scaling factor for a given Elementary object

        @return: The scaling factor set by L{scale_set()}
        @rtype: float

        """
        return elm_object_scale_get(self.obj)

    property scale:
        """The scaling factor for the Elementary object.

        @type: float

        """
        def __get__(self):
            return self.scale_get()
        def __set__(self, value):
            self.scale_set(value)

    # Scrollhints
    def scroll_hold_push(self):
        """scroll_hold_push()

        Push the scroll hold by 1

        This increments the scroll hold count by one. If it is more
        than 0 it will take effect on the parents of the indicated
        object.

        """
        elm_object_scroll_hold_push(self.obj)

    def scroll_hold_pop(self):
        """scroll_hold_pop()

        Pop the scroll hold by 1

        This decrements the scroll hold count by one. If it is more than 0
        it will take effect on the parents of the indicated object.

        """
        elm_object_scroll_hold_pop(self.obj)

    def scroll_freeze_push(self):
        """scroll_freeze_push()

        Push the scroll freeze by 1

        This increments the scroll freeze count by one. If it is more than 0
        it will take effect on the parents of the indicated object.

        """
        elm_object_scroll_freeze_push(self.obj)

    def scroll_freeze_pop(self):
        """scroll_freeze_pop()

        Pop the scroll freeze by 1

        This decrements the scroll freeze count by one. If it is more than 0
        it will take effect on the parents of the indicated object.

        """
        elm_object_scroll_freeze_pop(self.obj)

    def scroll_lock_x_set(self, lock):
        elm_object_scroll_lock_x_set(self.obj, lock)

    def scroll_lock_y_set(self, lock):
        elm_object_scroll_lock_y_set(self.obj, lock)

    def scroll_lock_x_get(self):
        return bool(elm_object_scroll_lock_x_get(self.obj))

    def scroll_lock_y_get(self):
        return bool(elm_object_scroll_lock_y_get(self.obj))

    property scroll_lock_x:
        def __get__(self):
            return self.scroll_lock_x_get()
        def __set__(self, value):
            self.scroll_lock_x_set(value)

    property scroll_lock_y:
        def __get__(self):
            return self.scroll_lock_y_get()
        def __set__(self, value):
            self.scroll_lock_y_set(value)

    # Theme
    property theme:
        """A theme to be used for this object and its children.

        This sets a specific theme that will be used for the given object and any
        child objects it has. If @p th is NULL then the theme to be used is
        cleared and the object will inherit its theme from its parent (which
        ultimately will use the default theme if no specific themes are set).

        Use special themes with great care as this will annoy users and make
        configuration difficult. Avoid any custom themes at all if it can be
        helped.

        @type: L{Theme}

        """
        def __set__(self, Theme th):
            elm_object_theme_set(self.obj, th.th)
        def __get__(self):
            cdef Theme th = Theme()
            th.th = elm_object_theme_get(self.obj)
            return th

    # Tooltips
    def tooltip_show(self):
        """tooltip_show()

        Force show tooltip of object

        Force show the tooltip and disable hide on mouse_out If another
        content is set as tooltip, the visible tooltip will hidden and
        showed again with new content.

        This can force show more than one tooltip at a time.

        """
        elm_object_tooltip_show(self.obj)

    def tooltip_hide(self):
        """tooltip_hide()

        Force hide tooltip of the object

        Force hide tooltip of object and (re)enable future mouse interations.

        """
        elm_object_tooltip_hide(self.obj)

    def tooltip_text_set(self, text):
        """tooltip_text_set(text)

        Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed. Internally, this
        method calls L{tooltip_content_cb_set}

        """
        elm_object_tooltip_text_set(self.obj, _cfruni(text))

    def tooltip_domain_translatable_text_set(self, domain, text):
        elm_object_tooltip_domain_translatable_text_set(self.obj, _cfruni(domain), _cfruni(text))

    def tooltip_translatable_text_set(self, text):
        elm_object_tooltip_translatable_text_set(self.obj, _cfruni(text))

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """tooltip_content_cb_set(func, *args, **kwargs)

        Set the content to be shown in the tooltip object

        Setup the tooltip to object. The object can have only one tooltip,
        so any previews tooltip data is removed. C{func(owner, tooltip,
        args, kargs)} will be called every time that need show the tooltip
        and it should return a valid Evas_Object. This object is then
        managed fully by tooltip system and is deleted when the tooltip is
        gone.

        @param func: Function to be create tooltip content, called when
            need show tooltip.

        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_object_tooltip_content_cb_set(self.obj, _tooltip_content_create,
                                          cbdata, _tooltip_data_del_cb)

    def tooltip_unset(self):
        """tooltip_unset()

        Unset tooltip from object

        Remove tooltip from object. If used the L{tooltip_text_set} the
        internal copy of label will be removed correctly. If used
        L{tooltip_content_cb_set}, the data will be unreferred but no freed.

        """
        elm_object_tooltip_unset(self.obj)

    def tooltip_style_set(self, style=None):
        """tooltip_style_set(style=None)

        Sets a different style for this object tooltip.

        @note: before you set a style you should define a tooltip with
            L{tooltip_content_cb_set()} or L{tooltip_text_set()}

        """
        elm_object_tooltip_style_set(self.obj, _cfruni(style) if style is not None else NULL)

    def tooltip_style_get(self):
        """tooltip_style_get()

        Get the style for this object tooltip.

        """
        return _ctouni(elm_object_tooltip_style_get(self.obj))

    property tooltip_style:
        """The style for this object tooltip.

        @type: string

        """
        def __get__(self):
            return self.tooltip_style_get()
        def __set__(self, value):
            self.tooltip_style_set(value)

    def tooltip_window_mode_set(self, disable):
        return bool(elm_object_tooltip_window_mode_set(self.obj, disable))

    def tooltip_window_mode_get(self):
        return bool(elm_object_tooltip_window_mode_get(self.obj))

    property tooltip_window_mode:
        def __get__(self):
            return self.tooltip_window_mode_get()
        def __set__(self, value):
            self.tooltip_window_mode_set(value)

    #Translatable text
    def domain_translatable_text_part_set(self, part, domain, text):
        """domain_translatable_text_part_set(part, domain, text)

        Set the text for an objects' part, marking it as translatable.

        The string to set as C{text} must be the original one. Do not pass the
        return of @C{gettext()} here. Elementary will translate the string
        internally and set it on the object using L{part_text_set()},
        also storing the original string so that it can be automatically
        translated when the language is changed with L{language_set()}.

        The C{domain} will be stored along to find the translation in the
        correct catalog. It can be None, in which case it will use whatever
        domain was set by the application with C{textdomain()}. This is useful
        in case you are building a library on top of Elementary that will have
        its own translatable strings, that should not be mixed with those of
        programs using the library.

        @param part: The name of the part to set
        @type part: string
        @param domain: The translation domain to use
        @type domain: string
        @param text: The original, non-translated text to set
        @type text: string

        """
        elm_object_domain_translatable_text_part_set(self.obj, _cfruni(part), _cfruni(domain), _cfruni(text))

    def domain_translatable_text_set(self, domain, text):
        elm_object_domain_translatable_text_set(self.obj, _cfruni(domain), _cfruni(text))

    def translatable_text_set(self, text):
        elm_object_translatable_text_set(self.obj, _cfruni(text))

    def translatable_text_part_get(self, part):
        """translatable_text_part_get(part)

        Gets the original string set as translatable for an object

        When setting translated strings, the function L{part_text_get()}
        will return the translation returned by C{gettext()}. To get the
        original string use this function.

        @param part: The name of the part that was set
        @type part: string

        @return: The original, untranslated string
        @rtype: string

        """
        return _ctouni(elm_object_translatable_text_part_get(self.obj, _cfruni(part)))

    def translatable_text_get(self):
        return _ctouni(elm_object_translatable_text_get(self.obj))

    property translatable_text:
        def __get__(self):
            return self.translatable_text_get()
        def __set__(self, value):
            self.translatable_text_set(value)

    # Callbacks
    def _callback_add_full(self, event, event_conv, func, *args, **kargs):
        """_callback_add_full(event, event_conv, func, *args, **kargs)

        Add a callback for the smart event specified by event.

        @param event: event name
        @type event: string
        @param event_conv: Conversion function to get the
            pointer (as a long) to the object to be given to the
            function as the second parameter. If None, then no
            parameter will be given to the callback.
        @type event_conv: function
        @param func: what to callback. Should have the signature:
            C{function(object, event_info, *args, **kargs)}
            C{function(object, *args, **kargs)} (if no event_conv is provided)
        @type func: function

        @raise TypeError: if B{func} is not callable.
        @raise TypeError: if B{event_conv} is not callable or None.

        """
        if not callable(func):
            raise TypeError("func must be callable")
        if event_conv is not None and not callable(event_conv):
            raise TypeError("event_conv must be None or callable")

        if self._elmcallbacks is None:
            self._elmcallbacks = {}

        e = intern(event)
        lst = self._elmcallbacks.setdefault(e, [])
        if not lst:
            evas_object_smart_callback_add(self.obj, _fruni(event),
                                                  _object_callback, <void *>e)
        lst.append((event_conv, func, args, kargs))

    def _callback_del_full(self, event, event_conv, func):
        """_callback_del_full(event, event_conv, func)

        Remove a smart callback.

        Removes a callback that was added by L{_callback_add_full()}.

        @param event: event name
        @type event: string
        @param event_conv: same as registered with L{_callback_add_full()}
        @type event_conv: function
        @param func: what to callback, should have be previously registered.
        @type func: function

        @precond: B{event}, B{event_conv} and B{func} must be used as
           parameter for L{_callback_add_full()}.

        @raise ValueError: if there was no B{func} connected with this event.

        """
        try:
            lst = self._elmcallbacks[event]
        except KeyError as e:
            raise ValueError("Unknown event %r" % event)

        i = -1
        ec = None
        f = None
        for i, (ec, f, a, k) in enumerate(lst):
            if event_conv == ec and func == f:
                break

        if f != func or ec != event_conv:
            raise ValueError("Callback %s was not registered with event %r" %
                             (func, event))

        lst.pop(i)
        if lst:
            return
        self._elmcallbacks.pop(event)
        evas_object_smart_callback_del(self.obj, _fruni(event), _object_callback)

    def _callback_add(self, event, func, *args, **kargs):
        """_callback_add(event, func, *args, **kargs)

        Add a callback for the smart event specified by event.

        @param event: event name
        @type event: string
        @param func: what to callback. Should have the signature:
            C{function(object, *args, **kargs)}
        @type func: function

        @raise TypeError: if B{func} is not callable.

        """
        return self._callback_add_full(event, None, func, *args, **kargs)

    def _callback_del(self, event, func):
        """_callback_del(event, func)

        Remove a smart callback.

        Removes a callback that was added by L{_callback_add()}.

        @param event: event name
        @type event: string
        @param func: what to callback, should have be previously registered.
        @type func: function

        @precond: B{event} and B{func} must be used as parameter for
            L{_callback_add_full()}.

        @raise ValueError: if there was no B{func} connected with this event.

        """
        return self._callback_del_full(event, None, func)

    def _get_obj_addr(self):
        """_get_obj_addr()

        Return the address of the internal save Evas_Object

        @return: Address of saved Evas_Object

        """
        return <long>self.obj


def __elm_widget_cls_resolver(unsigned long ptr):
    cdef Evas_Object *obj = <Evas_Object *>ptr
    cdef const_char_ptr t

    t = elm_object_widget_type_get(obj)
    assert t != NULL
    return _elm_widget_type_mapping.get(_ctouni(t), None)

_extended_object_mapping_register("elm_widget",
                                              __elm_widget_cls_resolver)

# NOTE: this is just transitional, need to be removed after all the widgets
#       in elm will be ported to the new hierarchical pattern.
_extended_object_mapping_register("elm_widget_compat",
                                              __elm_widget_cls_resolver)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryObject_Type # hack to install metaclass
_install_metaclass(&PyElementaryObject_Type, ElementaryObjectMeta)
