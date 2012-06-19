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

cdef class LayoutClass(Object):

    """Elementary, besides having the L{Layout} widget, exposes its
    foundation -- the Elementary Layout Class -- in order to create
    other widgets which are, basically, a certain layout with some more
    logic on top.

    The idea is to make the creation of that widgets as easy as possible,
    factorizing code on this common base. For example, a button is a
    layout (that looks like push button) that happens to react on
    clicks and keyboard events in a special manner, calling its user
    back on those events. That's no surprise, then, that the L{Button}
    implementation relies on LayoutClass, if you go to check it.

    Container parts, here, map directly to Edje parts from the layout's Edje
    group. Besides that, there's a whole infrastructure around Edje files:
        - interfacing by signals,
        - setting/retrieving text part values,
        - dealing with table and box parts directly,
        - etc.

    Finally, layout objects will do B{part aliasing} for you, if you set
    it up properly. For that, take a look at
    Elm_Layout_Part_Alias_Description, where it's explained in detail.

    """

    def file_set(self, filename, group):
        """Set the file that will be used as layout

        @param file: The path to file (edj) that will be used as layout
        @type file: string
        @param group: The group that the layout belongs in edje file
        @type group: string

        @return: (True = success, False = error)
        @rtype: bool

        """
        return bool(elm_layout_file_set(self.obj, _cfruni(filename), _cfruni(group)))

    property file:
        """Set the file path and group of the edje file that will be used as
        layout.

        @type: tuple of string

        """
        def __set__(self, value):
            filename, group = value
            # TODO: check return value
            elm_layout_file_set(self.obj, _cfruni(filename), _cfruni(group))

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
        return bool(elm_layout_theme_set(self.obj, _cfruni(clas), _cfruni(group), _cfruni(style)))

    property theme:
        """Set the edje group class, group name and style from the elementary
        theme that will be used as layout.

        Note that C{style} will be the new style too, as in an
        L{Object.style_set()} call.

        @type: tuple of string

        """
        def __set__(self, theme):
            clas, group, style = theme
            # TODO: check return value
            elm_layout_theme_set(self.obj, _cfruni(clas), _cfruni(group), _cfruni(style))

    def signal_emit(self, emission, source):
        """signal_emit(emission, source)

        Send a (Edje) signal to a given layout widget's underlying Edje object.

        This function sends a signal to the underlying Edje object. An Edje
        program on that Edje object's definition can respond to a signal by
        specifying matching 'signal' and 'source' fields.

        @param emission: The signal's name string
        @type emission: string
        @param source: The signal's source string
        @type source: string

        """
        elm_layout_signal_emit(self.obj, _cfruni(emission), _cfruni(source))

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
        #elm_layout_signal_callback_add(self.obj, _cfruni(emission), _cfruni(source), Edje_Signal_Cb func, voiddata)

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
        #elm_layout_signal_callback_del(self.obj, _cfruni(emission), _cfruni(source), Edje_Signal_Cb func)

    def box_append(self, part, evasObject child):
        """box_append(part, child)

        Append child to layout box part.

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
        return bool(elm_layout_box_append(self.obj, _cfruni(part), child.obj))

    def box_prepend(self, part, evasObject child):
        """box_prepend(part, child)

        Prepend child to layout box part.

        Once the object is prepended, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the
        child will be deleted automatically. One should use L{box_remove()}
        to make this layout forget about the object.

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
        return bool(elm_layout_box_prepend(self.obj, _cfruni(part), child.obj))

    def box_insert_before(self, part, evasObject child, evasObject reference):
        """box_insert_before(part, child, reference)

        Insert child to layout box part before a reference object.

        Once the object is inserted, it will become child of the layout. Its
        lifetime will be bound to the layout, whenever the layout dies the
        child will be deleted automatically. One should use L{box_remove()}
        to make this layout forget about the object.

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
        return bool(elm_layout_box_insert_before(self.obj, _cfruni(part), child.obj, reference.obj))

    def box_insert_at(self, part, evasObject child, pos):
        """box_insert_at(part, child, pos)

        Insert child to layout box part at a given position.

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
        return bool(elm_layout_box_insert_at(self.obj, _cfruni(part), child.obj, pos))

    def box_remove(self, part, evasObject child):
        """box_remove(part, child):

        Remove a child of the given part box.

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
        return Object_from_instance(elm_layout_box_remove(self.obj, _cfruni(part), child.obj))

    def box_remove_all(self, part, clear):
        """box_remove_all(part, clear)

        Remove all children of the given part box.

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
        return bool(elm_layout_box_remove_all(self.obj, _cfruni(part), clear))

    def table_pack(self, part, evasObject child_obj, col, row, colspan, rowspan):
        """table_pack(part, child_obj, col, row, colspan, rowspan)

        Insert child to layout table part.

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
        return bool(elm_layout_table_pack(self.obj, _cfruni(part), child_obj.obj, col, row, colspan, rowspan))

    def table_unpack(self, part, evasObject child_obj):
        """table_unpack(part, child_obj)

        Unpack (remove) a child of the given part table.

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
        return Object_from_instance(elm_layout_table_unpack(self.obj, _cfruni(part), child_obj.obj))

    def table_clear(self, part, clear):
        """table_clear(part, clear)

        Remove all the child objects of the given part table.

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
        return bool(elm_layout_table_clear(self.obj, _cfruni(part), clear))

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
        return Object_from_instance(elm_layout_edje_get(self.obj))

    property edje:
        """Get the edje layout

        This returns the edje object. It is not expected to be used to then
        swallow objects via edje_object_part_swallow() for example. Use
        L{part_content_set()} instead so child object handling and sizing is
        done properly.

        @note: This function should only be used if you really need to call
        some low level Edje function on this edje object. All the common
        stuff (setting text, emitting signals, hooking callbacks to signals,
        etc.) can be done with proper elementary functions.

        @see: L{signal_callback_add()}
        @see: L{signal_emit()}
        @see: L{part_text_set()}
        @see: L{part_content_set()}
        @see: L{box_append()}
        @see: L{table_pack()}
        @see: L{data_get()}

        @type: L{edje.Edje}

        """
        def __get__(self):
            return Object_from_instance(elm_layout_edje_get(self.obj))

    def data_get(self, key):
        """data_get(key)

        Get the edje data from the given layout

        This function fetches data specified inside the edje theme of this
        layout. This function returns None if data is not found.

        In EDC this comes from a data block within the group block that it
        was loaded from. E.g::

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
        return _ctouni(elm_layout_data_get(self.obj, _cfruni(key)))

    def sizing_eval(self):
        """sizing_eval()

        Eval sizing

        Manually forces a sizing re-evaluation. This is useful when the
        minimum size required by the edje theme of this layout has changed.
        The change on the minimum size required by the edje theme is not
        immediately reported to the elementary layout, so one needs to call
        this function in order to tell the widget (layout) that it needs to
        reevaluate its own size.

        The minimum size of the theme is calculated based on minimum size of
        parts, the size of elements inside containers like box and table,
        etc. All of this can change due to state changes, and that's when
        this function should be called.

        Also note that a standard signal of "size,eval" "elm" emitted from
        the edje object will cause this to happen too.

        """
        elm_layout_sizing_eval(self.obj)

    def part_cursor_set(self, part_name, cursor):
        """part_cursor_set(part_name, cursor)

        Sets a specific cursor for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @param cursor: cursor name to use, see Elementary_Cursor.h
        @type cursor: string

        @return: True on success or False on failure, that may be
            part not exists or it has "mouse_events: 0".
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_set(self.obj, _cfruni(part_name), _cfruni(cursor)))

    def part_cursor_get(self, part_name):
        """part_cursor_get(part_name)

        Get the cursor to be shown when mouse is over an edje part

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @return: the cursor name.
        @rtype: string

        """
        return _ctouni(elm_layout_part_cursor_get(self.obj, _cfruni(part_name)))

    def part_cursor_unset(self, part_name):
        """part_cursor_unset(part_name)

        Unsets a cursor previously set with L{part_cursor_set()}.

        @param part_name: a part from loaded edje group, that had a cursor set
            with L{part_cursor_set()}.
        @type part_name: string
        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_unset(self.obj, _cfruni(part_name)))

    def part_cursor_style_set(self, part_name, style):
        """part_cursor_style_set(part_name, style)

        Sets a specific cursor style for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string
        @param style: the theme style to use (default, transparent, ...)
        @type style: string

        @return: True on success or False on failure, that may be
            part not exists or it did not had a cursor set.
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_style_set(self.obj, _cfruni(part_name), _cfruni(style)))

    def part_cursor_style_get(self, part_name):
        """part_cursor_style_get(part_name)

        Gets a specific cursor style for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string

        @return: the theme style in use, defaults to "default". If the
            object does not have a cursor set, then None is returned.
        @rtype: string

        """
        return _ctouni(elm_layout_part_cursor_style_get(self.obj, _cfruni(part_name)))

    def part_cursor_engine_only_set(self, part_name, engine_only):
        """part_cursor_engine_only_set(part_name, engine_only)

        Sets if the cursor set should be searched on the theme or should use
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
        return bool(elm_layout_part_cursor_engine_only_set(self.obj, _cfruni(part_name), engine_only))

    def part_cursor_engine_only_get(self, part_name):
        """part_cursor_engine_only_get(part_name)

        Gets a specific cursor engine_only for an edje part.

        @param part_name: a part from loaded edje group.
        @type part_name: string

        @return: whenever the cursor is just provided by engine or also from theme.
        @rtype: bool

        """
        return bool(elm_layout_part_cursor_engine_only_get(self.obj, _cfruni(part_name)))

    def icon_set(self, evasObject icon):
        """Convenience macro to set the icon object in a layout that follows the
        Elementary naming convention for its parts.

        @param icon: The icon object
        @type icon: L{Object}

        """
        elm_layout_icon_set(self.obj, icon.obj if icon else NULL)

    def icon_get(self):
        """Convenience macro to get the icon object from a layout that follows the
        Elementary naming convention for its parts.

        @return: The icon object
        @rtype: L{Object}

        """
        return Object_from_instance(elm_layout_icon_get(self.obj))

    property icon:
        """The icon object in a layout that follows the Elementary naming
        convention for its parts.

        @type: L{Object}

        """
        def __get__(self):
            return Object_from_instance(elm_layout_icon_get(self.obj))

        def __set__(self, evasObject icon):
            elm_layout_icon_set(self.obj, icon.obj if icon else NULL)

    def end_set(self, evasObject end):
        """Convenience macro to set the end object in a layout that follows the
        Elementary naming convention for its parts.

        @param end: The end object
        @type end: L{Object}

        """
        elm_layout_end_set(self.obj, end.obj if end else NULL)

    def end_get(self):
        """Convenience macro to get the end object in a layout that follows the
        Elementary naming convention for its parts.

        @return: The end object
        @rtype: L{Object}

        """
        return Object_from_instance(elm_layout_end_get(self.obj))

    property end:
        """The end object in a layout that follows the Elementary naming
        convention for its parts.

        @type: L{Object}

        """
        def __get__(self):
            return Object_from_instance(elm_layout_end_get(self.obj))

        def __set__(self, evasObject end):
            elm_layout_end_set(self.obj, end.obj if end else NULL)

    def callback_theme_changed_add(self, func, *args, **kwargs):
        """The theme was changed."""
        self._callback_add("theme,changed", func, *args, **kwargs)

    def callback_theme_changed_del(self, func):
        self._callback_del("theme,changed", func)
