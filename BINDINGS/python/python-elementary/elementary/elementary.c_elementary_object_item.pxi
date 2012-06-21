# Copyright (c) 2010 ProFUSION embedded systems
#
#This file is part of python-elementary.
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

cdef Evas_Object *_tooltip_item_content_create(void *data, Evas_Object *o, Evas_Object *t, void *it) with gil:
   cdef Object ret, obj, tooltip

   obj = <Object>evas_object_data_get(o, "python-evas")
   tooltip = Object_from_instance(t)
   (func, item, args, kargs) = <object>data
   ret = func(obj, item, *args, **kargs)
   if not ret:
       return NULL
   return ret.obj

cdef void _tooltip_item_data_del_cb(void *data, Evas_Object *o, void *event_info) with gil:
   Py_DECREF(<object>data)

cdef class ObjectItem

def _cb_object_item_conv(long addr):
    cdef Elm_Object_Item *it = <Elm_Object_Item *>addr
    return _object_item_to_python(it)

cdef Elm_Object_Item * _object_item_from_python(ObjectItem item) except NULL:
    if item is None or item.item is NULL:
        raise TypeError("Invalid item!")
    return item.item

cdef _object_item_to_python(Elm_Object_Item *it):
    cdef void *data
    cdef object item

    if it == NULL:
        return None

    data = elm_object_item_data_get(it)
    if data == NULL:
        return None

    item = <object>data
    return item

cdef _object_item_list_to_python(const_Eina_List *lst):
    cdef Elm_Object_Item *it
    ret = []
    ret_append = ret.append
    while lst:
        it = <Elm_Object_Item *>lst.data
        lst = lst.next
        o = _object_item_to_python(it)
        if o is not None:
            ret_append(o)
    return ret

cdef void _object_item_del_cb(void *data, Evas_Object *o, void *event_info) with gil:
    cdef ObjectItem d = <object>data
    d.item = NULL
    Py_DECREF(d)

cdef void _object_item_callback(void *data, Evas_Object *obj, void *event_info) with gil:
    cdef ObjectItem item = <object>data
    (callback, a, ka) = item.params
    try:
        o = Object_from_instance(obj)
        callback(o, item, *a, **ka)
    except Exception as e:
        traceback.print_exc()

cdef class ObjectItem(object):

    """A generic item for the widgets.

    @group General: widget_get, part_content_set, content_set, part_content_get,
        content_get, part_content_unset, content_unset, part_text_set, text_set,
        part_text_get, text_get, text, access_info_set, data_get, signal_emit,
        tooltip_text_set, tooltip_window_mode_set, tooltip_window_mode_get,
        tooltip_content_cb_set, tooltip_unset, tooltip_style_set,
        tooltip_style_get, cursor_set, cursor_get, cursor_unset,
        cursor_style_set, cursor_style_get, cursor_engine_only_set,
        cursor_engine_only_get

    @group Styles: disabled_set, disabled_get, disabled

    """

    # Notes to bindings' developers:
    # ==============================
    #
    # After calling _set_obj, Elm_Object_Item's "data" contains the python item
    # instance pointer, and the attribute "item", that you see below, contains
    # a pointer to Elm_Object_Item.
    #
    # The variable params holds callback data, usually the tuple
    # (callback, args, kwargs). Note that some of the generic object item
    # functions expect this tuple. Use custom functions if you assign the
    # params differently.
    #
    # Gen type widgets MUST set the params BEFORE adding the item as the
    # items need their data immediately when adding them.

    cdef Elm_Object_Item *item
    cdef object params

    def __dealloc__(self):
        if self.item != NULL:
            elm_object_item_del_cb_set(self.item, NULL)
            elm_object_item_del(self.item)
            self.item = NULL

    cdef int _set_obj(self, Elm_Object_Item *item) except 0:
        assert self.item == NULL, "Object must be clean"
        self.item = item
        elm_object_item_data_set(item, <void*>self)
        elm_object_item_del_cb_set(item, _object_item_del_cb)
        Py_INCREF(self)
        return 1

    def widget_get(self):
        """widget_get()

        Get the widget object's handle which contains a given item

        @note: This returns the widget object itself that an item belongs to.
        @note: Every elm_object_item supports this API

        @return: The widget object
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_item_widget_get(self.item))

    def part_content_set(self, part, Object content not None):
        """part_content_set(part, content)

        Set a content of an object item

        This sets a new object to an item as a content object. If any object
        was already set as a content object in the same part, previous
        object will be deleted automatically.

        @note: Elementary object items may have many contents

        @param part: The content part name to set (None for the default
            content)
        @param content: The new content of the object item

        """
        elm_object_item_part_content_set(self.item, _cfruni(part) if part is not None else NULL, content.obj)

    def content_set(self, Object content not None):
        elm_object_item_content_set(self.item, content.obj)

    def part_content_get(self, part):
        """part_content_get(part)

        Get a content of an object item

        @note: Elementary object items may have many contents

        @param part: The content part name to unset (None for the default
            content)
        @type part: string
        @return: content of the object item or None for any error
        @rtype: L{Object}

        """
        return Object_from_instance(elm_object_item_part_content_get(self.item, _cfruni(part) if part is not None else NULL))

    def content_get(self):
        return Object_from_instance(elm_object_item_content_get(self.item))

    def part_content_unset(self, part):
        """part_content_unset(part)

        Unset a content of an object item

        @note: Elementary object items may have many contents

        @param part: The content part name to unset (None for the default
            content)
        @type part: string

        """
        return Object_from_instance(elm_object_item_part_content_unset(self.item, _cfruni(part) if part is not None else NULL))

    def content_unset(self):
        return Object_from_instance(elm_object_item_content_unset(self.item))

    def part_text_set(self, part, text):
        """part_text_set(part, text)

        Sets the text of a given part of this object.

        @see: L{text_set()} and L{part_text_get()}

        @param part: part name to set the text.
        @type part: string
        @param text: text to set.
        @type text: string

        """
        elm_object_item_part_text_set(self.item, _cfruni(part) if part is not None else NULL, _cfruni(text))

    def text_set(self, text):
        """text_set(text)

        Sets the main text for this object.

        @see: L{text_get()} and L{part_text_set()}

        @param text: any text to set as the main textual part of this object.
        @type text: string

        """
        elm_object_item_text_set(self.item, _cfruni(text))

    def part_text_get(self, part):
        """part_text_get(part)

        Gets the text of a given part of this object.

        @see: L{text_get()} and L{part_text_set()}

        @param part: part name to get the text.
        @type part: string
        @return: the text of a part or None if nothing was set.
        @rtype: string

        """
        return _ctouni(elm_object_item_part_text_get(self.item, _cfruni(part) if part is not None else NULL))

    def text_get(self):
        """text_get()

        Gets the main text for this object.

        @see: L{text_set()} and L{part_text_get()}

        @return: the main text or None if nothing was set.
        @rtype: string

        """
        return _ctouni(elm_object_item_text_get(self.item))

    property text:
        """The main text for this object.

        @type: string

        """
        def __get__(self):
            return self.text_get()

        def __set__(self, value):
            self.text_set(value)

    def access_info_set(self, txt):
        """access_info_set(txt)

        Set the text to read out when in accessibility mode

        @param txt: The text that describes the widget to people with poor
            or no vision
        @type txt: string

        """
        elm_object_item_access_info_set(self.item, _cfruni(txt))

    def data_get(self):
        """data_get()

        Returns the callback data given at creation time.

        @rtype: tuple of (args, kargs), args is tuple, kargs is dict.

        """
        (callback, a, ka) = self.params
        return (a, ka)

    def data_set(self, *args, **kwargs):
        """data_set(*args, **kwargs)

        Set the callback data.

        """
        (callback, a, ka) = self.params
        self.params = tuple(callback, *args, **kwargs)

    property data:
        def __get__(self):
            return self.data_get()
        #def __set__(self, data):
            #self.data_set(data)

    def signal_emit(self, emission, source):
        """signal_emit(emission, source)

        Send a signal to the edje object of the widget item.

        This function sends a signal to the edje object of the obj item. An
        edje program can respond to a signal by specifying matching
        'signal' and 'source' fields.

        @param emission: The signal's name.
        @type emission: string
        @param source: The signal's source.
        @type source: string

        """
        elm_object_item_signal_emit(self.item, _cfruni(emission), _cfruni(source))

    def disabled_set(self, disabled):
        """disabled_set(disabled)

        Set the disabled state of an widget item.

        Elementary object item can be B{disabled}, in which state they won't
        receive input and, in general, will be themed differently from
        their normal state, usually greyed out. Useful for contexts
        where you don't want your users to interact with some of the
        parts of you interface.

        This sets the state for the widget item, either disabling it or
        enabling it back.

        @param disabled: The state to put in in: C{True} for
            disabled, C{False} for enabled
        @type disabled: bool

        """
        elm_object_item_disabled_set(self.item, disabled)

    def disabled_get(self):
        """disabled_get()

        Get the disabled state of an widget item.

        This gets the state of the widget, which might be enabled or disabled.

        @return: C{True}, if the widget item is disabled, C{False}
            if it's enabled (or on errors)
        @rtype: bool

        """
        return bool(elm_object_item_disabled_get(self.item))

    property disabled:
        """The disabled state of an widget item.

        Elementary object item can be B{disabled}, in which state they won't
        receive input and, in general, will be themed differently from their
        normal state, usually greyed out. Useful for contexts where you
        don't want your users to interact with some of the parts of you
        interface.

        @type: bool

        """
        def __get__(self):
            return self.disabled_get()
        def __set__(self, disabled):
            self.disabled_set(disabled)

    #def delete_cb_set(self, del_cb):
        #elm_object_item_del_cb_set(self.item, del_cb)

    def delete(self):
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_object_item_del(self.item)
        Py_DECREF(self)

    def tooltip_text_set(self, char *text):
        """tooltip_text_set(text)

        Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed. Internally, this
        method calls L{tooltip_content_cb_set}

        """
        elm_object_item_tooltip_text_set(self.item, _cfruni(text))

    def tooltip_window_mode_set(self, disable):
        return bool(elm_object_item_tooltip_window_mode_set(self.item, disable))

    def tooltip_window_mode_get(self):
        return bool(elm_object_item_tooltip_window_mode_get(self.item))

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """tooltip_content_cb_set(func, *args, **kargs)

        Set the content to be shown in the tooltip object

        Setup the tooltip to object. The object can have only one tooltip,
        so any previews tooltip data is removed. C{func(owner, tooltip,
        args, kargs)} will be called every time that need show the tooltip
        and it should return a valid Evas_Object. This object is then
        managed fully by tooltip system and is deleted when the tooltip is
        gone.

        @param func: Function to be create tooltip content, called when
            need show tooltip.
        @type func: function

        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_object_item_tooltip_content_cb_set(self.item, _tooltip_item_content_create,
                                          cbdata, _tooltip_item_data_del_cb)

    def tooltip_unset(self):
        """tooltip_unset()

        Unset tooltip from object

        Remove tooltip from object. If used the L{tooltip_text_set} the
        internal copy of label will be removed correctly. If used
        L{tooltip_content_cb_set}, the data will be unreferred but no freed.

        """
        elm_object_item_tooltip_unset(self.item)

    def tooltip_style_set(self, style=None):
        """tooltip_style_set(style=None)

        Sets a different style for this object tooltip.

        @note: before you set a style you should define a tooltip with
            L{tooltip_content_cb_set()} or L{tooltip_text_set()}

        """
        elm_object_item_tooltip_style_set(self.item, _cfruni(style) if style is not None else NULL)

    def tooltip_style_get(self):
        """tooltip_style_get()

        Get the style for this object tooltip.

        """
        return _ctouni(elm_object_item_tooltip_style_get(self.item))

    def cursor_set(self, char *cursor):
        """cursor_set(cursor)

        Set the cursor to be shown when mouse is over the object

        Set the cursor that will be displayed when mouse is over the object.
        The object can have only one cursor set to it, so if this function
        is called twice for an object, the previous set will be unset.

        """
        elm_object_item_cursor_set(self.item, _cfruni(cursor))

    def cursor_get(self):
        return _ctouni(elm_object_item_cursor_get(self.item))

    def cursor_unset(self):
        """cursor_unset()

        Unset cursor for object

        Unset cursor for object, and set the cursor to default if the mouse
        was over this object.

        """
        elm_object_item_cursor_unset(self.item)

    def cursor_style_set(self, style=None):
        """cusor_style_set(style=None)

        Sets a different style for this object cursor.

        @note: before you set a style you should define a cursor with
            L{cursor_set()}

        """
        elm_object_item_cursor_style_set(self.item, _cfruni(style) if style is not None else NULL)

    def cursor_style_get(self):
        """cursor_style_get()

        Get the style for this object cursor.

        """
        return _ctouni(elm_object_item_cursor_style_get(self.item))

    def cursor_engine_only_set(self, engine_only):
        """cursor_engine_only_set(engine_only)

        Sets cursor engine only usage for this object.

        @note: before you set engine only usage you should define a cursor with
            L{cursor_set()}

        """
        elm_object_item_cursor_engine_only_set(self.item, bool(engine_only))

    def cursor_engine_only_get(self):
        """cursor_engine_only_get()

        Get the engine only usage for this object.

        """
        return elm_object_item_cursor_engine_only_get(self.item)

_elm_widget_type_register("object_item", ObjectItem)
