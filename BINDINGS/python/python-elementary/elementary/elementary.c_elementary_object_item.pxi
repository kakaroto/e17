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

cdef c_evas.Evas_Object *_tooltip_item_content_create(void *data, c_evas.Evas_Object *o, evas.c_evas.Evas_Object *t, void *it) with gil:
   cdef Object ret, obj, tooltip

   obj = <Object>c_evas.evas_object_data_get(o, "python-evas")
   tooltip = evas.c_evas._Object_from_instance(<long> t)
   (func, item, args, kargs) = <object>data
   ret = func(obj, item, *args, **kargs)
   if not ret:
       return NULL
   return ret.obj

cdef void _tooltip_item_data_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
   Py_DECREF(<object>data)

cdef class ObjectItem:
    """
    An generic item for the widgets
    """
    cdef void *base
    cdef Elm_Object_Item *obj

    def widget_get(self):
        cdef c_evas.const_Evas_Object *obj = elm_object_item_widget_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def part_content_set(self, char *part, Object obj):
        elm_object_item_part_content_set(self.obj, part, obj.obj)

    def content_set(self, Object obj):
        elm_object_item_part_content_set(self.obj, NULL, obj.obj)

    def part_content_get(self, char *part):
        cdef c_evas.Evas_Object *obj = elm_object_item_part_content_get(self.obj, part)
        return evas.c_evas._Object_from_instance(<long> obj)

    def content_get(self):
        cdef c_evas.Evas_Object *obj = elm_object_item_content_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def part_content_unset(self, char *part):
        cdef c_evas.Evas_Object *obj = elm_object_item_part_content_unset(self.obj, part)
        return evas.c_evas._Object_from_instance(<long> obj)

    def content_unset(self):
        cdef c_evas.Evas_Object *obj = elm_object_item_content_unset(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)


    def part_text_set(self, part, text):
        """Sets the text of a given part of this object.

        @parm: B{part} part name to set the text.
        @parm: B{text} text to set.
        @see: L{text_set()} and L{text_part_get()}
        """
        elm_object_item_part_text_set(self.obj, part, text)

    def text_set(self, text):
        """Sets the main text for this object.

        @parm: B{text} any text to set as the main textual part of this object.
        @see: L{text_get()} and L{text_part_set()}
        """
        elm_object_item_text_set(self.obj, text)

    def part_text_get(self, part):
        """Gets the text of a given part of this object.

        @parm: B{part} part name to get the text.
        @return: the text of a part or None if nothing was set.
        @see: L{text_get()} and L{text_part_set()}
        """
        cdef const_char_ptr l
        l = elm_object_item_part_text_get(self.obj, part)
        if l == NULL:
            return None
        return l

    def text_get(self):
        """Gets the main text for this object.

        @return: the main text or None if nothing was set.
        @see: L{text_set()} and L{text_part_get()}
        """
        cdef const_char_ptr l
        l = elm_object_item_text_get(self.obj)
        if l == NULL:
            return None
        return l

    property text:
        def __get__(self):
            return self.text_get()

        def __set__(self, value):
            self.text_set(value)

    def access_info_set(self, txt):
        elm_object_item_access_info_set(self.obj, txt)

    def data_get(self):
        """Returns the callback data given at creation time.

        @rtype: tuple of (args, kargs), args is tuple, kargs is dict.
        """
        cdef void* data
        data = elm_object_item_data_get(self.obj)
        if data == NULL:
            return None
        else:
            (obj, callback, it, a, ka) = <object>data

            return (a, ka)

    #def data_set(self, data):
        #elm_object_item_data_set(self.obj, <void*>data)

    property data:
        def __get__(self):
            return self.data_get()
        #def __set__(self, data):
            #self.data_set(data)

    def signal_emit(self, emission, source):
        elm_object_item_signal_emit(self.obj, emission, source)

    def disabled_set(self, disabled):
        elm_object_item_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return bool(elm_object_item_disabled_get(self.obj))

    property disabled:
        def __get__(self):
            return self.disabled_get()
        def __set__(self, disabled):
            self.disabled_set(disabled)

    #def delete_cb_set(self, del_cb):
        #elm_object_item_del_cb_set(self.obj, del_cb)

    def delete(self):
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        elm_object_item_del(self.obj)

    def tooltip_text_set(self, char *text):
        """ Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed.
        Internaly, this method call @tooltip_content_cb_set
        """
        elm_object_item_tooltip_text_set(self.obj, text)

    def tooltip_window_mode_set(self, disable):
        return bool(elm_object_item_tooltip_window_mode_set(self.obj, disable))

    def tooltip_window_mode_get(self):
        return bool(elm_object_item_tooltip_window_mode_get(self.obj))

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """ Set the content to be shown in the tooltip object

        @param: B{func} Function to be create tooltip content, called when
                need show tooltip.

        Setup the tooltip to object. The object can have only one
        tooltip, so any previews tooltip data is removed. func(owner,
        tooltip, args, kargs) will be called every time that need
        show the tooltip and it should return a valid
        Evas_Object. This object is then managed fully by tooltip
        system and is deleted when the tooltip is gone.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_object_item_tooltip_content_cb_set(self.obj, _tooltip_item_content_create,
                                          cbdata, _tooltip_item_data_del_cb)

    def tooltip_unset(self):
        """ Unset tooltip from object

        Remove tooltip from object. If used the @tool_text_set the internal
        copy of label will be removed correctly. If used
        @tooltip_content_cb_set, the data will be unreferred but no freed.
        """
        elm_object_item_tooltip_unset(self.obj)

    def tooltip_style_set(self, style=None):
        """ Sets a different style for this object tooltip.

        @note before you set a style you should define a tooltip with
        elm_object_item_tooltip_content_cb_set() or
        elm_object_item_tooltip_text_set()
        """
        if style:
            elm_object_item_tooltip_style_set(self.obj, style)
        else:
            elm_object_item_tooltip_style_set(self.obj, NULL)

    def tooltip_style_get(self):
        """ Get the style for this object tooltip.
        """
        cdef const_char_ptr style
        style = elm_object_item_tooltip_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_set(self, char *cursor):
        """ Set the cursor to be shown when mouse is over the object

        Set the cursor that will be displayed when mouse is over the
        object. The object can have only one cursor set to it, so if
        this function is called twice for an object, the previous set
        will be unset.
        """
        elm_object_item_cursor_set(self.obj, cursor)

    def cursor_get(self):
        return elm_object_item_cursor_get(self.obj)

    def cursor_unset(self):
        """ Unset cursor for object

        Unset cursor for object, and set the cursor to default if the mouse
        was over this object.
        """
        elm_object_item_cursor_unset(self.obj)

    def cursor_style_set(self, style=None):
        """ Sets a different style for this object cursor.

        @note before you set a style you should define a cursor with
        elm_object_item_cursor_set()
        """
        if style:
            elm_object_item_cursor_style_set(self.obj, style)
        else:
            elm_object_item_cursor_style_set(self.obj, NULL)

    def cursor_style_get(self):
        """ Get the style for this object cursor.
        """
        cdef const_char_ptr style
        style = elm_object_item_cursor_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_engine_only_set(self, engine_only):
        """ Sets cursor engine only usage for this object.

        @note before you set engine only usage you should define a cursor with
        elm_object_item_cursor_set()
        """
        elm_object_item_cursor_engine_only_set(self.obj, bool(engine_only))

    def cursor_engine_only_get(self):
        """ Get the engine only usage for this object.
        """
        return elm_object_item_cursor_engine_only_get(self.obj)

_elm_widget_type_register("object_item", ObjectItem)
