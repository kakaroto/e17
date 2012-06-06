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

cdef public class Layout(Object) [object PyElementaryLayout, type PyElementaryLayout_Type]:

    """This is a container widget that takes a standard Edje design file and
    wraps it very thinly in a widget.

    An Edje design (theme) file has a very wide range of possibilities to
    describe the behavior of elements added to the Layout. Check out the Edje
    documentation and the EDC reference to get more information about what can
    be done with Edje.

    Just like L{List}, L{Box}, and other container widgets, any
    object added to the Layout will become its child, meaning that it will be
    deleted if the Layout is deleted, move if the Layout is moved, and so on.

    The Layout widget can contain as many Contents, Boxes or Tables as
    described in its theme file. For instance, objects can be added to
    different Tables by specifying the respective Table part names. The same
    is valid for Content and Box.

    The objects added as child of the Layout will behave as described in the
    part description where they were added. There are 3 possible types of
    parts where a child can be added:

    Content (SWALLOW part)
    ======================

    Only one object can be added to the C{SWALLOW} part (but you still can
    have many C{SWALLOW} parts and one object on each of them). Use the
    C{Object.content_set/get/unset} functions to set, retrieve and unset
    objects as content of the C{SWALLOW}. After being set to this part, the
    object size, position, visibility, clipping and other description
    properties will be totally controlled by the description of the given part
    (inside the Edje theme file).

    One can use C{size_hint_*} functions on the child to have some
    kind of control over its behavior, but the resulting behavior will still
    depend heavily on the C{SWALLOW} part description.

    The Edje theme also can change the part description, based on signals or
    scripts running inside the theme. This change can also be animated. All of
    this will affect the child object set as content accordingly. The object
    size will be changed if the part size is changed, it will animate move if
    the part is moving, and so on.

    Box (BOX part)
    ==============

    An Edje C{BOX} part is very similar to the Elementary L{Box} widget. It
    allows one to add objects to the box and have them distributed along its
    area, accordingly to the specified C{layout} property (now by C{layout} we
    mean the chosen layouting design of the Box, not the Layout widget
    itself).

    A similar effect for having a box with its position, size and other things
    controlled by the Layout theme would be to create an Elementary L{Box}
    widget and add it as a Content in the C{SWALLOW} part.

    The main difference of using the Layout Box is that its behavior, the box
    properties like layouting format, padding, align, etc. will be all
    controlled by the theme. This means, for example, that a signal could be
    sent to the Layout theme (with L{Object.signal_emit()}) and the theme
    handled the signal by changing the box padding, or align, or both. Using
    the Elementary L{Box} widget is not necessarily harder or easier, it
    just depends on the circumstances and requirements.

    The Layout Box can be used through the C{box_*} set of
    functions.

    Table (TABLE part)
    ==================

    Just like the C{Box}, the Layout Table is very similar to the
    Elementary L{Table} widget. It allows one to add objects to the Table
    specifying the row and column where the object should be added, and any
    column or row span if necessary.

    Again, we could have this design by adding a L{Table} widget to the
    C{SWALLOW} part using L{Object.part_content_set()}. The same difference happens
    here when choosing to use the Layout Table (a C{TABLE} part) instead of
    the L{Table} plus C{SWALLOW} part. It's just a matter of convenience.

    The Layout Table can be used through the C{table_*} set of
    functions.

    Predefined Layouts
    ==================

    Another interesting thing about the Layout widget is that it offers some
    predefined themes that come with the default Elementary theme. These
    themes can be set by the call L{theme_set()}, and provide some
    basic functionality depending on the theme used.

    Most of them already send some signals, some already provide a toolbar or
    back and next buttons.

    These are available predefined theme layouts. All of them have class =
    C{layout}, group = C{application}, and style = one of the following options:

        - C{toolbar-content} - application with toolbar and main content area
        - C{toolbar-content-back} - application with toolbar and main content
            area with a back button and title area
        - C{toolbar-content-back-next} - application with toolbar and main
            content area with a back and next buttons and title area
        - C{content-back} - application with a main content area with a back
            button and title area
        - C{content-back-next} - application with a main content area with a
            back and next buttons and title area
        - C{toolbar-vbox} - application with toolbar and main content area as a
            vertical box
        - C{toolbar-table} - application with toolbar and main content area as a
            table

    Emitted signals
    ===============

    This widget emits the following signals:

        - "theme,changed": The theme was changed.

    """


    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_layout_add(parent.obj))

    def file_set(self, filename, group):
        """Set the file that will be used as layout

        @param file: The path to file (edj) that will be used as layout
        @type file: string
        @param group: The group that the layout belongs in edje file
        @type group: string

        @return: (True = success, False = error)
        @rtype: bool

        """
        return bool(elm_layout_file_set(self.obj, filename, group))

    def theme_set(self, clas, group, style):
        """Set the edje group from the elementary theme that will be used as layout

        Note that C{style} will be the new style too, as in an
        L{Object.style_set()} call.

        @param clas: the class of the group
        @type clas: string
        @param group: the group
        @type group: string
        @param style: the style to used
        @type style: string

        @return: (True = success, False = error)
        @rtype: bool

        """
        return bool(elm_layout_theme_set(self.obj, clas, group, style))

    def signal_emit(self, emission, source):
        """Send a (Edje) signal to a given layout widget's underlying Edje
        object.

        This function sends a signal to the underlying Edje object of @a
        obj. An Edje program on that Edje object's definition can respond
        to a signal by specifying matching 'signal' and 'source' fields.

        @param emission: The signal's name string
        @type emission: string
        @param source: The signal's source string
        @type source: string

        """
        elm_layout_signal_emit(self.obj, emission, source)

    #def signal_callback_add(self, emission, source, func, data):
        """Add a callback for a (Edje) signal emitted by a layout widget's
        underlying Edje object.

        This function connects a callback function to a signal emitted by
        the underlying Edje object. Globs are accepted in either
        the emission or source strings (see
        C{edje_object_signal_callback_add()}).

        @param emission: The signal's name string
        @type emission: string
        @param source: The signal's source string
        @type source: string
        @param func: The callback function to be executed when the signal is
            emitted.
        @type func: function

        """
        #elm_layout_signal_callback_add(self.obj, emission, source, Edje_Signal_Cb func, voiddata)

    #def signal_callback_del(self, emission, source, func):
        """Remove a signal-triggered callback from a given layout widget.

        This function removes the B{last} callback attached to a signal
        emitted by the undelying Edje object, with parameters
        C{emission}, C{source} and C{func} matching exactly those passed to a
        previous call to L{Object.signal_callback_add()}. The data pointer
        that was passed to this call will be returned.

        @param emission: The signal's name string
        @type emission: string
        @param source: The signal's source string
        @type source: string
        @param func: The callback function being executed when the signal
            was emitted.
        @type func: function

        """
        #elm_layout_signal_callback_del(self.obj, emission, source, Edje_Signal_Cb func)

    def box_append(self, part, c_evas.Object child):
        """Append child to layout box part.

        Once the object is appended, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the child
        will be deleted automatically. One should use L{box_remove()} to
        make this layout forget about the object.

        @see: L{box_prepend()}
        @see: L{box_insert_before()}
        @see: L{box_insert_at()}
        @see: L{box_remove()}

        @param part: the box part to which the object will be appended.
        @type part: string
        @param child: the child object to append to box.
        @type child: L{Object}
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_box_append(self.obj, part, child.obj))

    def box_prepend(self, part, c_evas.Object child):
        """Prepend child to layout box part.

        Once the object is prepended, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the child
        will be deleted automatically. One should use L{box_remove()} to
        make this layout forget about the object.

        @see: L{box_append()}
        @see: L{box_insert_before()}
        @see: L{box_insert_at()}
        @see: L{box_remove()}

        @param part: the box part to prepend.
        @type part: string
        @param child: the child object to prepend to box.
        @type child: L{Object}
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_box_prepend(self.obj, part, child.obj))

    def box_insert_before(self, part, c_evas.Object child, c_evas.Object reference):
        """Insert child to layout box part before a reference object.

        Once the object is inserted, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the child
        will be deleted automatically. One should use L{box_remove()} to
        make this layout forget about the object.

        @see: L{box_append()}
        @see: L{box_prepend()}
        @see: L{box_insert_at()}
        @see: L{box_remove()}

        @param part: the box part to insert.
        @type part: string
        @param child: the child object to insert into box.
        @type child: L{Object}
        @param reference: another reference object to insert before in box.
        @type reference: L{Object}
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_box_insert_before(self.obj, part, child.obj, reference.obj))

    def box_insert_at(self, part, c_evas.Object child, pos):
        """Insert child to layout box part at a given position.

        Once the object is inserted, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the child
        will be deleted automatically. One should use L{box_remove()} to
        make this layout forget about the object.

        @see: L{box_append()}
        @see: L{box_prepend()}
        @see: L{box_insert_before()}
        @see: L{box_remove()}

        @param part: the box part to insert.
        @type part: string
        @param child: the child object to insert into box.
        @type child: L{Object}
        @param pos: the numeric position >=0 to insert the child.
        @type pos: int
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_box_insert_at(self.obj, part, child.obj, pos))

    def box_remove(self, part, c_evas.Object child):
        """Remove a child of the given part box.

        The object will be removed from the box part and its lifetime will
        not be handled by the layout anymore. This is equivalent to
        L{Object.part_content_unset()} for box.

        @see: L{box_append()}
        @see: L{box_remove_all()}

        @param part: The box part name to remove child.
        @type part: string
        @param child: The object to remove from box.
        @type child: L{Object}
        @return: The object that was being used, or None if not found.
        @rtype: L{Object}

        """
        cdef c_evas.Evas_Object *obj = elm_layout_box_remove(self.obj, part, child.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def box_remove_all(self, part, clear):
        """Remove all children of the given part box.

        The objects will be removed from the box part and their lifetime will
        not be handled by the layout anymore. This is equivalent to
        L{box_remove()} for all box children.

        @see: L{box_append()}
        @see: L{box_remove()}

        @param part: The box part name to remove child.
        @type part: string
        @param clear: If True, then all objects will be deleted as
            well, otherwise they will just be removed and will be
            dangling on the canvas.
        @type clear: bool
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_box_remove_all(self.obj, part, clear))

    def table_pack(self, part, c_evas.Object child_obj, col, row, colspan, rowspan):
        """Insert child to layout table part.

        Once the object is inserted, it will become child of the table. Its
        lifetime will be bound to the layout, and whenever the layout dies the
        child will be deleted automatically. One should use
        L{table_remove()} to make this layout forget about the object.

        If C{colspan} or C{rowspan} are bigger than 1, that object will occupy
        more space than a single cell.

        @see: L{table_unpack()}
        @see: L{table_clear()}

        @param part: the box part to pack child.
        @type part: string
        @param child_obj: the child object to pack into table.
        @type child_obj: L{Object}
        @param col: the column to which the child should be added. (>= 0)
        @type col: int
        @param row: the row to which the child should be added. (>= 0)
        @type row: int
        @param colspan: how many columns should be used to store this object.
            (>= 1)
        @type colspan: int
        @param rowspan: how many rows should be used to store this object. (>= 1)
        @type rowspan: int
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_table_pack(self.obj, part, child_obj.obj, col, row, colspan, rowspan))

    def table_unpack(self, part, c_evas.Object child_obj):
        """Unpack (remove) a child of the given part table.

        The object will be unpacked from the table part and its lifetime
        will not be handled by the layout anymore. This is equivalent to
        L{Object.part_content_unset()} for table.

        @see: L{table_pack()}
        @see: L{table_clear()}

        @param part: The table part name to remove child.
        @type part: string
        @param child_obj: The object to remove from table.
        @type child_obj: L{Object}
        @return: The object that was being used, or None if not found.
        @rtype: L{Object}

        """
        cdef c_evas.Evas_Object *obj = elm_layout_table_unpack(self.obj, part, child_obj.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def table_clear(self, part, clear):
        """Remove all the child objects of the given part table.

        The objects will be removed from the table part and their lifetime will
        not be handled by the layout anymore. This is equivalent to
        L{table_unpack()} for all table children.

        @see: L{table_pack()}
        @see: L{table_unpack()}

        @param part: The table part name to remove child.
        @type part: string
        @param clear: If True, then all objects will be deleted as
            well, otherwise they will just be removed and will be
            dangling on the canvas.
        @type clear: bool
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_table_clear(self.obj, part, clear))

    def edje_get(self):
        """Get the edje layout

        This returns the edje object. It is not expected to be used to then
        swallow objects via edje_object_part_swallow() for example. Use
        L{part_content_set()} instead so child object handling and sizing is
        done properly.

        @note: This function should only be used if you really need to call some
        low level Edje function on this edje object. All the common stuff (setting
        text, emitting signals, hooking callbacks to signals, etc.) can be done
        with proper elementary functions.

        @see: L{signal_callback_add()}
        @see: L{signal_emit()}
        @see: L{part_text_set()}
        @see: L{part_content_set()}
        @see: L{box_append()}
        @see: L{table_pack()}
        @see: L{data_get()}

        @return: A Evas_Object with the edje layout settings loaded
        with function L{file_set}
        @rtype: Evas_Object

        """
        cdef c_evas.Evas_Object *obj = elm_layout_edje_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property edje:
        def __get__(self):
            return self.edje_get()

    def data_get(self, key):
        """Get the edje data from the given layout

        This function fetches data specified inside the edje theme of this layout.
        This function return None if data is not found.

        In EDC this comes from a data block within the group block that it was
        loaded from. E.g::

            collections {
                group {
                    name: "a_group";
                    data {
                       item: "key1" "value1";
                       item: "key2" "value2";
                    }
                }
            }

        @param key: The data key
        @type key: string

        @return: The edje data string
        @rtype: string

        """
        return elm_layout_data_get(self.obj, key)

    def sizing_eval(self):
        """Eval sizing

        Manually forces a sizing re-evaluation. This is useful when the minimum
        size required by the edje theme of this layout has changed. The change on
        the minimum size required by the edje theme is not immediately reported to
        the elementary layout, so one needs to call this function in order to tell
        the widget (layout) that it needs to reevaluate its own size.

        The minimum size of the theme is calculated based on minimum size of
        parts, the size of elements inside containers like box and table, etc. All
        of this can change due to state changes, and that's when this function
        should be called.

        Also note that a standard signal of "size,eval" "elm" emitted from the
        edje object will cause this to happen too.

        """
        elm_layout_sizing_eval(self.obj)

    def part_cursor_set(self, part_name, cursor):
        """Sets a specific cursor for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @param cursor: cursor name to use, see Elementary_Cursor.h
        @type cursor: string

        @return: True on success or False on failure, that may be
            part not exists or it has "mouse_events: 0".
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_set(self.obj, part_name, cursor))

    def part_cursor_get(self, part_name):
        """Get the cursor to be shown when mouse is over an edje part

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @return: the cursor name.
        @rtype: string

        """
        return elm_layout_part_cursor_get(self.obj, part_name)

    def part_cursor_unset(self, part_name):
        """Unsets a cursor previously set with L{part_cursor_set()}.

        @param part_name: a part from loaded edje group, that had a cursor set
            with L{part_cursor_set()}.
        @type part_name: string
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_unset(self.obj, part_name))

    def part_cursor_style_set(self, part_name, style):
        """Sets a specific cursor style for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @param style: the theme style to use (default, transparent, ...)
        @type style: string

        @return: True on success or False on failure, that may be
            part not exists or it did not had a cursor set.
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_style_set(self.obj, part_name, style))

    def part_cursor_style_get(self, part_name):
        """Gets a specific cursor style for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string

        @return: the theme style in use, defaults to "default". If the
            object does not have a cursor set, then None is returned.
        @rtype: string

        """
        return elm_layout_part_cursor_style_get(self.obj, part_name)

    def part_cursor_engine_only_set(self, part_name, engine_only):
        """Sets if the cursor set should be searched on the theme or should use
        the provided by the engine, only.

        @note: before you set if should look on theme you should define a
        cursor with L{part_cursor_set()}. By default it will only
        look for cursors provided by the engine.

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @param engine_only: if cursors should be just provided by the engine (True)
            or should also search on widget's theme as well (False)
        @type engine_only: bool

        @return: True on success or False on failure, that may be
            part not exists or it did not had a cursor set.
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_engine_only_set(self.obj, part_name, engine_only))

    def part_cursor_engine_only_get(self, part_name):
        """Gets a specific cursor engine_only for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string

        @return: whenever the cursor is just provided by engine or also from theme.
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_engine_only_get(self.obj, part_name))

    def content_set(self, swallow, c_evas.Object content):
        """Set the layout content.

        Once the content object is set, a previously set one will be deleted.
        If you want to keep that old content object, use the
        L{Object.part_content_unset()} function.

        @note: In an Edje theme, the part used as a content container is called
        C{SWALLOW}. This is why the parameter name is called C{swallow}, but it is
        expected to be a part name just like the second parameter of
        L{box_append()}.

        @see: L{box_append()}
        @see: L{Object.part_content_get()}
        @see: L{Object.part_content_unset()}

        @param swallow: The swallow part name in the edje file
        @type swallow: string
        @param content: The child that will be added in this layout object
        @type content: L{Object}
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_object_part_content_set(self.obj, swallow, o)

    def content_get(self, swallow):
        """Get the child object in the given content part.

        @param swallow: The SWALLOW part to get its content
        @type swallow: string

        @return: The swallowed object or None if none or an error occurred

        """
        cdef c_evas.const_Evas_Object *obj = elm_object_part_content_get(self.obj, swallow)
        return evas.c_evas._Object_from_instance(<long> obj)

    def content_unset(self, swallow):
        """Unset the layout content.

        Unparent and return the content object which was set for this part.

        @param swallow: The swallow part name in the edje file
        @type swallow: string
        @return: The content that was being used
        @rtype: L{Object}

        """
        cdef c_evas.Evas_Object *obj = elm_object_part_content_unset(self.obj, swallow)
        return evas.c_evas._Object_from_instance(<long> obj)

    def text_set(self, part, text):
        """Set the text of the given part

        @param part: The TEXT part where to set the text
        @type part: string
        @param text: The text to set
        @type text: string
        @return: C{True} on success, C{False} otherwise

        """
        elm_layout_text_set(self.obj, part, text)

    def text_get(self, part):
        """Get the text set in the given part

        @param part: The TEXT part to retrieve the text off
        @type part: string

        @return: The text set in C{part}
        @rtype: string

        """
        return elm_layout_text_get(self.obj, part)

    def icon_set(self, c_evas.Object icon):
        """Convenience macro to set the icon object in a layout that follows the
        Elementary naming convention for its parts.

        """
        elm_layout_icon_set(self.obj, icon.obj if icon else NULL)

    def icon_get(self):
        """Convenience macro to get the icon object from a layout that follows the
        Elementary naming convention for its parts.

        """
        cdef c_evas.const_Evas_Object *obj = elm_layout_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property icon:
        def __get__(self):
            return self.icon_get()
        def __set__(self, value):
            self.icon_set(value)

    def end_set(self, c_evas.Object end):
        """Convenience macro to set the end object in a layout that follows the
        Elementary naming convention for its parts.

        """
        elm_layout_end_set(self.obj, end.obj if end else NULL)

    def end_get(self):
        """Convenience macro to get the end object in a layout that follows the
        Elementary naming convention for its parts.

        """
        cdef c_evas.const_Evas_Object *obj = elm_layout_end_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property end:
        def __get__(self):
            return self.end_get()
        def __set__(self, value):
            self.end_set(value)

    def callback_theme_changed_add(self, func, *args, **kwargs):
        """The theme was changed."""
        self._callback_add("theme,changed", func, *args, **kwargs)

    def callback_theme_changed_del(self, func):
        self._callback_del("theme,changed", func)

_elm_widget_type_register("layout", Layout)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryLayout_Type # hack to install metaclass
_install_metaclass(&PyElementaryLayout_Type, ElementaryObjectMeta)
