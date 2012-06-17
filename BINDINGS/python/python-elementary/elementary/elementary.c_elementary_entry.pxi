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

def Entry_markup_to_utf8(str):
    cdef const_char_ptr string
    string = elm_entry_markup_to_utf8(str)
    if string == NULL:
        return None
    return string

def Entry_utf8_to_markup(str):
    cdef const_char_ptr string
    string = elm_entry_utf8_to_markup(str)
    if string == NULL:
        return None
    return string

class EntryAnchorInfo:
    def __init__(self):
        self.name = None
        self.button = 0
        self.x = 0
        self.y = 0
        self.w = 0
        self.h = 0

class EntryAnchorHoverInfo:
    def __init__(self):
        self.anchor_info = None
        self.hover = None
        self.hover_parent = (0, 0, 0, 0)
        self.hover_left = False
        self.hover_right = False
        self.hover_top = False
        self.hover_bottom = False

def _entryanchor_conv(long addr):
    cdef Elm_Entry_Anchor_Info *ei = <Elm_Entry_Anchor_Info *>addr
    eai = EntryAnchorInfo()
    eai.name = ei.name
    eai.button = ei.button
    eai.x = ei.x
    eai.y = ei.y
    eai.w = ei.w
    eai.h = ei.h
    return eai

def _entryanchorhover_conv(long addr):
    cdef Elm_Entry_Anchor_Hover_Info *ehi = <Elm_Entry_Anchor_Hover_Info *>addr
    eahi = EntryAnchorHoverInfo()
    eahi.anchor_info = <object>ehi.anchor_info
    eahi.hover = Hover(None, <object>ehi.hover)
    eahi.hover_parent = (ehi.hover_parent.x, ehi.hover_parent.y,
                       ehi.hover_parent.w, ehi.hover_parent.h)
    eahi.hover_left = ehi.hover_left
    eahi.hover_right = ehi.hover_right
    eahi.hover_top = ehi.hover_top
    eahi.hover_bottom = ehi.hover_bottom
    return eahi

cdef public class Entry(Object) [object PyElementaryEntry, type PyElementaryEntry_Type]:

    """An entry is a convenience widget which shows a box that the user can
    enter text into.

    Entries by default don't scroll, so they grow to
    accommodate the entire text, resizing the parent window as needed. This
    can be changed with the L{scrollable_set()} function.

    They can also be single line or multi line (the default) and when set
    to multi line mode they support text wrapping in any of the modes
    indicated by C{Elm_Wrap_Type}.

    Other features include password mode, filtering of inserted text with
    L{markup_filter_append()} and related functions, inline "items" and
    formatted markup text.

    Formatted text
    ==============

    The markup tags supported by the Entry are defined by the theme, but
    even when writing new themes or extensions it's a good idea to stick to
    a sane default, to maintain coherency and avoid application breakages.
    Currently defined by the default theme are the following tags:
        - <br>: Inserts a line break.
        - <ps>: Inserts a paragraph separator. This is preferred over line
          breaks.
        - <tab>: Inserts a tab.
        - <em>...</em>: Emphasis. Sets the @em oblique style for the
          enclosed text.
        - <b>...</b>: Sets the @b bold style for the enclosed text.
        - <link>...</link>: Underlines the enclosed text.
        - <hilight>...</hilight>: Highlights the enclosed text.

    Special markups
    ===============

    Besides those used to format text, entries support two special markup
    tags used to insert click-able portions of text or items inlined within
    the text.

    Anchors
    -------

    Anchors are similar to HTML anchors. Text can be surrounded by <a> and
    </a> tags and an event will be generated when this text is clicked,
    like this:

    C{This text is outside <a href=anc-01>but this one is an anchor</a>}

    The C{href} attribute in the opening tag gives the name that will be
    used to identify the anchor and it can be any valid utf8 string.

    When an anchor is clicked, an C{"anchor,clicked"} signal is emitted with
    an C{Elm_Entry_Anchor_Info} in the C{event_info} parameter for the
    callback function. The same applies for "anchor,in" (mouse in), "anchor,out"
    (mouse out), "anchor,down" (mouse down), and "anchor,up" (mouse up) events on
    an anchor.

    Items
    -----

    Inlined in the text, any other L{Object} can be inserted by using
    <item> tags this way:

    C{<item size=16x16 vsize=full href=emoticon/haha></item>}

    Just like with anchors, the C{href} identifies each item, but these need,
    in addition, to indicate their size, which is done using any one of
    C{size}, C{absize} or C{relsize} attributes. These attributes take their
    value in the WxH format, where W is the width and H the height of the
    item.

        - absize: Absolute pixel size for the item. Whatever value is set will
          be the item's size regardless of any scale value the object may have
          been set to. The final line height will be adjusted to fit larger items.
        - size: Similar to C{absize}, but it's adjusted to the scale value set
          for the object.
        - relsize: Size is adjusted for the item to fit within the current
          line height.

    Besides their size, items are specified a C{vsize} value that affects
    how their final size and position are calculated. The possible values
    are:
        - ascent: Item will be placed within the line's baseline and its
          ascent. That is, the height between the line where all characters are
          positioned and the highest point in the line. For C{size} and C{absize}
          items, the descent value will be added to the total line height to make
          them fit. C{relsize} items will be adjusted to fit within this space.
        - full: Items will be placed between the descent and ascent, or the
          lowest point in the line and its highest.

    After the size for an item is calculated, the entry will request an
    object to place in its space. For this, the functions set with
    L{item_provider_append()} and related functions will be called
    in order until one of them returns a non-C{None} value. If no providers
    are available, or all of them return C{None}, then the entry falls back
    to one of the internal defaults, provided the name matches with one of
    them.

    All of the following are currently supported:
        - emoticon/angry
        - emoticon/angry-shout
        - emoticon/crazy-laugh
        - emoticon/evil-laugh
        - emoticon/evil
        - emoticon/goggle-smile
        - emoticon/grumpy
        - emoticon/grumpy-smile
        - emoticon/guilty
        - emoticon/guilty-smile
        - emoticon/haha
        - emoticon/half-smile
        - emoticon/happy-panting
        - emoticon/happy
        - emoticon/indifferent
        - emoticon/kiss
        - emoticon/knowing-grin
        - emoticon/laugh
        - emoticon/little-bit-sorry
        - emoticon/love-lots
        - emoticon/love
        - emoticon/minimal-smile
        - emoticon/not-happy
        - emoticon/not-impressed
        - emoticon/omg
        - emoticon/opensmile
        - emoticon/smile
        - emoticon/sorry
        - emoticon/squint-laugh
        - emoticon/surprised
        - emoticon/suspicious
        - emoticon/tongue-dangling
        - emoticon/tongue-poke
        - emoticon/uh
        - emoticon/unhappy
        - emoticon/very-sorry
        - emoticon/what
        - emoticon/wink
        - emoticon/worried
        - emoticon/wtf

    Alternatively, an item may reference an image by its path, using
    the URI form C{file:///path/to/an/image.png} and the entry will then
    use that image for the item.

    Setting entry's style
    =====================

    There are 2 major ways to change the entry's style:
        - Theme - set the "base" field to the desired style.
        - User style - Pushing overrides to the theme style to the textblock object
          by using evas_object_textblock_style_user_push().

    You should modify the theme when you would like to change the style for
    aesthetic reasons. While the user style should be changed when you would
    like to change the style to something specific defined at run-time, e.g,
    setting font or font size in a text editor.

    Loading and saving files
    ========================

    Entries have convenience functions to load text from a file and save
    changes back to it after a short delay. The automatic saving is enabled
    by default, but can be disabled with L{autosave_set()} and files
    can be loaded directly as plain text or have any markup in them
    recognized. See L{file_set()} for more details.

    Emitted signals
    ===============

    This widget emits the following signals:
        - "changed": The text within the entry was changed.
        - "changed,user": The text within the entry was changed because of user interaction.
        - "activated": The enter key was pressed on a single line entry.
        - "press": A mouse button has been pressed on the entry.
        - "longpressed": A mouse button has been pressed and held for a couple
          seconds.
        - "clicked": The entry has been clicked (mouse press and release).
        - "clicked,double": The entry has been double clicked.
        - "clicked,triple": The entry has been triple clicked.
        - "focused": The entry has received focus.
        - "unfocused": The entry has lost focus.
        - "selection,paste": A paste of the clipboard contents was requested.
        - "selection,copy": A copy of the selected text into the clipboard was
          requested.
        - "selection,cut": A cut of the selected text into the clipboard was
          requested.
        - "selection,start": A selection has begun and no previous selection
          existed.
        - "selection,changed": The current selection has changed.
        - "selection,cleared": The current selection has been cleared.
        - "cursor,changed": The cursor has changed position.
        - "anchor,clicked": An anchor has been clicked. The event_info
          parameter for the callback will be an C{Elm_Entry_Anchor_Info}.
        - "anchor,in": Mouse cursor has moved into an anchor. The event_info
          parameter for the callback will be an C{Elm_Entry_Anchor_Info}.
        - "anchor,out": Mouse cursor has moved out of an anchor. The event_info
          parameter for the callback will be an C{Elm_Entry_Anchor_Info}.
        - "anchor,up": Mouse button has been unpressed on an anchor. The event_info
          parameter for the callback will be an C{Elm_Entry_Anchor_Info}.
        - "anchor,down": Mouse button has been pressed on an anchor. The event_info
          parameter for the callback will be an C{Elm_Entry_Anchor_Info}.
        - "preedit,changed": The preedit string has changed.
        - "language,changed": Program language changed.

    Default content parts of the entry items that you can use for are:
        - "icon" - An icon in the entry
        - "end" - A content in the end of the entry

    Default text parts of the entry that you can use for are:
        - "default" - text of the entry

    """

    def __init__(self, evasObject parent):
        """This adds an entry to @p parent object.

        By default, entries are:
            - not scrolled
            - multi-line
            - word wrapped
            - autosave is enabled

        @param parent: The parent object
        @type parent: L{Object}
        @return: The new object or None if it cannot be created
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_entry_add(parent.obj))

    def single_line_set(self, single_line):
        """Sets the entry to single line mode.

        In single line mode, entries don't ever wrap when the text reaches the
        edge, and instead they keep growing horizontally. Pressing the C{Enter}
        key will generate an C{"activate"} event instead of adding a new line.

        When C{single_line} is C{False}, line wrapping takes effect again
        and pressing enter will break the text into a different line
        without generating any events.

        @param single_line: If true, the text in the entry will be on a single
            line.
        @type single_line: bool

        """
        elm_entry_single_line_set(self.obj, single_line)

    def single_line_get(self):
        """Gets whether the entry is set to be single line.

        @see: L{single_line_set()}

        @return: If true, the text in the entry is set to display on a single
            line.
        @rtype: bool

        """
        return bool(elm_entry_single_line_get(self.obj))

    property single_line:
        """Single line mode.

        In single line mode, entries don't ever wrap when the text reaches the
        edge, and instead they keep growing horizontally. Pressing the C{Enter}
        key will generate an C{"activate"} event instead of adding a new line.

        When C{single_line} is C{False}, line wrapping takes effect again
        and pressing enter will break the text into a different line
        without generating any events.

        @type: bool

        """
        def __get__(self):
            return bool(elm_entry_single_line_get(self.obj))

        def __set__(self, single_line):
            elm_entry_single_line_set(self.obj, single_line)

    def password_set(self, password):
        """Sets the entry to password mode.

        In password mode, entries are implicitly single line and the display of
        any text in them is replaced with asterisks (*).

        @param password: If true, password mode is enabled.
        @type password: bool

        """
        elm_entry_password_set(self.obj, password)

    def password_get(self):
        """Gets whether the entry is set to password mode.

        @see: L{password_set()}

        @return: If true, the entry is set to display all characters
            as asterisks (*).
        @rtype: bool

        """
        return bool(elm_entry_password_get(self.obj))

    property password:
        """Sets the entry to password mode.

        In password mode, entries are implicitly single line and the display of
        any text in them is replaced with asterisks (*).

        @type: bool

        """
        def __get__(self):
            return bool(elm_entry_password_get(self.obj))

        def __set__(self, password):
            elm_entry_password_set(self.obj, password)

    def entry_set(self, entry):
        """This sets the text displayed within the entry to C{entry}.

        @note: Using this function bypasses text filters

        @param entry: The text to be displayed
        @type entry: string

        """
        elm_entry_entry_set(self.obj, _cfruni(entry))

    def entry_get(self):
        """This returns the text currently shown in object C{entry}.

        @see: L{entry_set()}.

        @return: The currently displayed text or None on failure
        @rtype: string

        """
        return _ctouni(elm_entry_entry_get(self.obj))

    property entry:
        """The text displayed within the entry to C{entry}.

        @note: Setting this bypasses text filters

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_entry_entry_get(self.obj))

        def __set__(self, entry):
            elm_entry_entry_set(self.obj, _cfruni(entry))

    def entry_append(self, text):
        """Appends C{entry} to the text of the entry.

        Adds the text in C{entry} to the end of any text already present in the
        widget.

        The appended text is subject to any filters set for the widget.

        @see: L{markup_filter_append()}

        @param entry: The text to be displayed
        @type entry: string

        """
        elm_entry_entry_append(self.obj, _cfruni(text))

    def is_empty(self):
        """Gets whether the entry is empty.

        Empty means no text at all. If there are any markup tags, like an item
        tag for which no provider finds anything, and no text is displayed, this
        function still returns False.

        @return: True if the entry is empty, False otherwise.
        @rtype: bool

        """
        return elm_entry_is_empty(self.obj)

    def selection_get(self):
        """Gets any selected text within the entry.

        If there's any selected text in the entry, this function returns it as
        a string in markup format. None is returned if no selection exists or
        if an error occurred.

        The returned value points to an internal string and should not be freed
        or modified in any way. If the C{entry} object is deleted or its
        contents are changed, the returned pointer should be considered invalid.

        @return: The selected text within the entry or None on failure
        @rtype: string

        """
        return _ctouni(elm_entry_selection_get(self.obj))

    property selection:
        """Gets any selected text within the entry.

        If there's any selected text in the entry, this function returns it as
        a string in markup format. None is returned if no selection exists or
        if an error occurred.

        The returned value points to an internal string and should not be freed
        or modified in any way. If the C{entry} object is deleted or its
        contents are changed, the returned pointer should be considered invalid.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_entry_selection_get(self.obj))

    def textblock_get(self):
        """Returns the actual textblock object of the entry.

        This function exposes the internal textblock object that actually
        contains and draws the text. This should be used for low-level
        manipulations that are otherwise not possible.

        Changing the textblock directly from here will not notify edje/elm to
        recalculate the textblock size automatically, so any modifications
        done to the textblock returned by this function should be followed by
        a call to L{calc_force()}.

        The return value is marked as const as an additional warning.
        One should not use the returned object with any of the generic evas
        functions (geometry_get/resize/move and etc), but only with the textblock
        functions; The former will either not work at all, or break the correct
        functionality.

        IMPORTANT: Many functions may change (i.e delete and create a new one)
        the internal textblock object. Do NOT cache the returned object, and try
        not to mix calls on this object with regular elm_entry calls (which may
        change the internal textblock object). This applies to all cursors
        returned from textblock calls, and all the other derivative values.

        @return: The textblock object.
        @rtype: Evas_Object

        """
        cdef Evas_Object *o
        o = elm_entry_textblock_get(self.obj)
        return Object_from_instance(o)

    property textblock:
        """Returns the actual textblock object of the entry.

        This function exposes the internal textblock object that actually
        contains and draws the text. This should be used for low-level
        manipulations that are otherwise not possible.

        Changing the textblock directly from here will not notify edje/elm to
        recalculate the textblock size automatically, so any modifications
        done to the textblock returned by this function should be followed by
        a call to L{calc_force()}.

        The return value is marked as const as an additional warning.
        One should not use the returned object with any of the generic evas
        functions (geometry_get/resize/move and etc), but only with the textblock
        functions; The former will either not work at all, or break the correct
        functionality.

        IMPORTANT: Many functions may change (i.e delete and create a new one)
        the internal textblock object. Do NOT cache the returned object, and try
        not to mix calls on this object with regular elm_entry calls (which may
        change the internal textblock object). This applies to all cursors
        returned from textblock calls, and all the other derivative values.

        @type: Evas_Object

        """
        def __get__(self):
            cdef Evas_Object *o
            o = elm_entry_textblock_get(self.obj)
            return Object_from_instance(o)

    def calc_force(self):
        """Forces calculation of the entry size and text layouting.

        This should be used after modifying the textblock object directly.

        @see: L{textblock_get()}

        """
        elm_entry_calc_force(self.obj)

    def entry_insert(self, entry):
        """Inserts the given text into the entry at the current cursor position.

        This inserts text at the cursor position as if it was typed
        by the user (note that this also allows markup which a user
        can't just "type" as it would be converted to escaped text, so this
        call can be used to insert things like emoticon items or bold push/pop
        tags, other font and color change tags etc.)

        If any selection exists, it will be replaced by the inserted text.

        The inserted text is subject to any filters set for the widget.

        @see: L{markup_filter_append()}

        @param entry: The text to insert
        @type entry: string

        """
        elm_entry_entry_insert(self.obj, _cfruni(entry))

    def line_wrap_set(self, wrap):
        """Set the line wrap type to use on multi-line entries.

        Sets the wrap type used by the entry to any of the specified in
        Elm_Wrap_Type. This tells how the text will be implicitly cut into a new
        line (without inserting a line break or paragraph separator) when it
        reaches the far edge of the widget.

        Note that this only makes sense for multi-line entries. A widget set
        to be single line will never wrap.

        @param wrap: The wrap mode to use. See Elm_Wrap_Type for details on them
        @type wrap: Elm_Wrap_Type

        """
        elm_entry_line_wrap_set(self.obj, wrap)

    def line_wrap_get(self):
        """Gets the wrap mode the entry was set to use.

        @see: L{line_wrap_set()}

        @return: Wrap type
        @rtype: Elm_Wrap_Type

        """
        return elm_entry_line_wrap_get(self.obj)

    property line_wrap:
        """The line wrap type to use on multi-line entries.

        This tells how the text will be implicitly cut into a new
        line (without inserting a line break or paragraph separator) when it
        reaches the far edge of the widget.

        Note that this only makes sense for multi-line entries. A widget set
        to be single line will never wrap.

        @type: Elm_Wrap_Type

        """
        def __get__(self):
            return elm_entry_line_wrap_get(self.obj)

        def __set__(self, wrap):
            elm_entry_line_wrap_set(self.obj, wrap)

    def editable_set(self, editable):
        """Sets if the entry is to be editable or not.

        By default, entries are editable and when focused, any text input by the
        user will be inserted at the current cursor position. But calling this
        function with C{editable} as False will prevent the user from
        inputting text into the entry.

        The only way to change the text of a non-editable entry is to use
        L{Object.text_set()}, L{entry_insert()} and other related
        functions.

        @param editable: If True, user input will be inserted in the entry,
            if not, the entry is read-only and no user input is allowed.
        @type editable: bool

        """
        elm_entry_editable_set(self.obj, editable)

    def editable_get(self):
        """Gets whether the entry is editable or not.

        @see: L{editable_set()}

        @return: If true, the entry is editable by the user.
            If false, it is not editable by the user
        @rtype: bool

        """
        return bool(elm_entry_editable_get(self.obj))

    property editable:
        """If the entry is to be editable or not.

        By default, entries are editable and when focused, any text input by the
        user will be inserted at the current cursor position. Setting this as
        False will prevent the user from inputting text into the entry.

        The only way to change the text of a non-editable entry is to use
        L{Object.text_set()}, L{entry_insert()} and other related
        functions and properties.

        @type: bool

        """
        def __get__(self):
            return bool(elm_entry_editable_get(self.obj))

        def __set__(self, editable):
            elm_entry_editable_set(self.obj, editable)

    def select_none(self):
        """This drops any existing text selection within the entry."""
        elm_entry_select_none(self.obj)

    def select_all(self):
        """This selects all text within the entry."""
        elm_entry_select_all(self.obj)

    def cursor_next(self):
        """This moves the cursor one place to the right within the entry.

        @return: True upon success, False upon failure
        @rtype: bool

        """
        return bool(elm_entry_cursor_next(self.obj))

    def cursor_prev(self):
        """This moves the cursor one place to the left within the entry.

        @return: True upon success, False upon failure
        @rtype: bool

        """
        return bool(elm_entry_cursor_prev(self.obj))

    def cursor_up(self):
        """This moves the cursor one line up within the entry.

        @return: True upon success, False upon failure
        @rtype: bool

        """
        return bool(elm_entry_cursor_up(self.obj))

    def cursor_down(self):
        """This moves the cursor one line down within the entry.

        @return: True upon success, False upon failure
        @rtype: bool

        """
        return bool(elm_entry_cursor_down(self.obj))

    def cursor_begin_set(self):
        """This moves the cursor to the beginning of the entry."""
        elm_entry_cursor_begin_set(self.obj)

    def cursor_end_set(self):
        """This moves the cursor to the end of the entry."""
        elm_entry_cursor_end_set(self.obj)

    def cursor_line_begin_set(self):
        """This moves the cursor to the beginning of the current line."""
        elm_entry_cursor_line_begin_set(self.obj)

    def cursor_line_end_set(self):
        """This moves the cursor to the end of the current line."""
        elm_entry_cursor_line_end_set(self.obj)

    def cursor_selection_begin(self):
        """This begins a selection within the entry as though the user were
        holding down the mouse button to make a selection."""
        elm_entry_cursor_selection_begin(self.obj)

    def cursor_selection_end(self):
        """This ends a selection within the entry as though the user had
        just released the mouse button while making a selection."""
        elm_entry_cursor_selection_end(self.obj)

    def cursor_is_format_get(self):
        """Gets whether a format node exists at the current cursor position.

        A format node is anything that defines how the text is rendered. It can
        be a visible format node, such as a line break or a paragraph separator,
        or an invisible one, such as bold begin or end tag.
        This function returns whether any format node exists at the current
        cursor position.

        @see: L{cursor_is_visible_format_get()}

        @return: True if the current cursor position contains a format node,
            False otherwise.
        @rtype: bool

        """
        return bool(elm_entry_cursor_is_format_get(self.obj))

    def cursor_is_visible_format_get(self):
        """Gets if the current cursor position holds a visible format node.

        @see: L{cursor_is_format_get()}

        @return: True if the current cursor is a visible format, False
            if it's an invisible one or no format exists.
        @rtype: bool

        """
        return bool(elm_entry_cursor_is_visible_format_get(self.obj))

    def cursor_content_get(self):
        """Gets the character pointed by the cursor at its current position.

        This function returns a string with the utf8 character stored at the
        current cursor position.
        Only the text is returned, any format that may exist will not be part
        of the return value. You must free the string when done with free().

        @return: The text pointed by the cursors.
        @rtype: string

        """
        return elm_entry_cursor_content_get(self.obj)

    def cursor_geometry_get(self):
        """This function returns the geometry of the cursor.

        It's useful if you want to draw something on the cursor (or where it is),
        or for example in the case of scrolled entry where you want to show the
        cursor.

        @return: Geometry (x, y, w, h)
        @rtype: tuple of Evas_Coords (int)

        """
        cdef Evas_Coord x, y, w, h
        #TODO: Check return status for success
        elm_entry_cursor_geometry_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def cursor_pos_set(self, pos):
        """Sets the cursor position in the entry to the given value

        The value in C{pos} is the index of the character position within the
        contents of the string as returned by L{cursor_pos_get()}.

        @param pos: The position of the cursor
        @type pos: int

        """
        elm_entry_cursor_pos_set(self.obj, pos)

    def cursor_pos_get(self):
        """Retrieves the current position of the cursor in the entry

        @return: The cursor position
        @rtype: int

        """
        return elm_entry_cursor_pos_get(self.obj)

    property cursor_pos:
        """The cursor position in the entry

        The value is the index of the character position within the
        contents of the string.

        @type: int

        """
        def __get__(self):
            return elm_entry_cursor_pos_get(self.obj)

        def __set__(self, pos):
            elm_entry_cursor_pos_set(self.obj, pos)

    def selection_cut(self):
        """This executes a "cut" action on the selected text in the entry."""
        elm_entry_selection_cut(self.obj)

    def selection_copy(self):
        """This executes a "copy" action on the selected text in the entry."""
        elm_entry_selection_copy(self.obj)

    def selection_paste(self):
        """This executes a "paste" action in the entry."""
        elm_entry_selection_paste(self.obj)

    def context_menu_clear(self):
        """This clears and frees the items in a entry's contextual (longpress) menu.

        @see: L{context_menu_item_add()}

        """
        elm_entry_context_menu_clear(self.obj)

    # TODO XXX
    # def context_menu_item_add(self, label, icon_file, icon_type, cb, data):
        """This adds an item to the entry's contextual menu.

        A longpress on an entry will make the contextual menu show up, if this
        hasn't been disabled with L{context_menu_disabled_set()}.
        By default, this menu provides a few options like enabling selection mode,
        which is useful on embedded devices that need to be explicit about it,
        and when a selection exists it also shows the copy and cut actions.

        With this function, developers can add other options to this menu to
        perform any action they deem necessary.

        @param label: The item's text label
        @type label: string
        @param icon_file: The item's icon file
        @type icon_file: string
        @param icon_type: The item's icon type
        @type icon_type: string
        @param func: The callback to execute when the item is clicked
        @type func: function

        """
        #elm_entry_context_menu_item_add(self.obj, label, icon_file, icon_type, func, data);

    def context_menu_disabled_set(self, disabled):
        """This disables the entry's contextual (longpress) menu.

        @param disabled: If true, the menu is disabled
        @type disabled: bool

        """
        elm_entry_context_menu_disabled_set(self.obj, disabled)

    def context_menu_disabled_get(self):
        """This returns whether the entry's contextual (longpress) menu is
        disabled.

        @return: If true, the menu is disabled
        @rtype: bool

        """
        return elm_entry_context_menu_disabled_get(self.obj)

    property context_menu_disabled:
        """This disables the entry's contextual (longpress) menu.

        @type: bool

        """
        def __get__(self):
            return elm_entry_context_menu_disabled_get(self.obj)

        def __set__(self, disabled):
            elm_entry_context_menu_disabled_set(self.obj, disabled)


    # elm_entry_item_provider_append() # TODO XXX

    # elm_entry_item_provider_prepend() # TODO XXX

    # elm_entry_item_provider_remove() # TODO XXX

    # elm_entry_markup_filter_append() # TODO XXX

    # elm_entry_markup_filter_prepend() # TODO XXX

    # elm_entry_markup_filter_remove() # TODO XXX

    markup_to_utf8 = staticmethod(Entry_markup_to_utf8)

    utf8_to_markup = staticmethod(Entry_utf8_to_markup)

    def file_set(self, file, format):
        """This sets the file (and implicitly loads it) for the text to display and
        then edit.

        All changes are written back to the file after a short delay if
        the entry object is set to autosave (which is the default).

        If the entry had any other file set previously, any changes made to it
        will be saved if the autosave feature is enabled, otherwise, the file
        will be silently discarded and any non-saved changes will be lost.

        @param file: The path to the file to load and save
        @type file: string
        @param format: The file format
        @type format: Elm_Text_Format
        @return: C{True} on success, C{False} otherwise

        """
        return bool(elm_entry_file_set(self.obj, _cfruni(file), format))

    def file_get(self):
        """Gets the file being edited by the entry.

        This function can be used to retrieve any file set on the entry for
        edition, along with the format used to load and save it.

        @return: Tuple of file and format
        @rtype: tuple of strings

        """
        cdef const_char_ptr file
        cdef Elm_Text_Format format

        elm_entry_file_get(self.obj, &file, &format)
        return (_ctouni(file), format)

    property file:
        """The file for the text to display and then edit.

        All changes are written back to the file after a short delay if
        the entry object is set to autosave (which is the default).

        If the entry had any other file set previously, any changes made to it
        will be saved if the autosave feature is enabled, otherwise, the file
        will be silently discarded and any non-saved changes will be lost.

        @type: tuple of strings (file, format)

        """
        def __get__(self):
            cdef const_char_ptr file
            cdef Elm_Text_Format format
            elm_entry_file_get(self.obj, &file, &format)
            return (_ctouni(file), format)

        def __set__(self, value):
            file, format = value
            elm_entry_file_set(self.obj, _cfruni(file), format)

    def file_save(self):
        """This function writes any changes made to the file set with
        L{file_set()}.

        """
        elm_entry_file_save(self.obj)

    def autosave_set(self, autosave):
        """This sets the entry object to 'autosave' the loaded text file or not.

        @see: L{file_set()}

        @param autosave: Autosave the loaded file or not
        @type autosave: bool

        """
        elm_entry_autosave_set(self.obj, autosave)

    def autosave_get(self):
        """This gets the entry object's 'autosave' status.

        @see: L{file_set()}

        @return: Autosave the loaded file or not
        @rtype: bool

        """
        return elm_entry_autosave_get(self.obj)

    property autosave:
        """Whether the entry object 'autosaves' the loaded text file or not.

        @type: bool

        """
        def __get__(self):
            return elm_entry_autosave_get(self.obj)

        def __set__(self, autosave):
            elm_entry_autosave_set(self.obj, autosave)

    def scrollable_set(self, scrollable):
        """Enable or disable scrolling in entry

        Normally the entry is not scrollable unless you enable it with this call.

        @param scroll: True if it is to be scrollable, False otherwise
        @type scroll: bool

        """
        elm_entry_scrollable_set(self.obj, scrollable)

    def scrollable_get(self):
        """Get the scrollable state of the entry

        Normally the entry is not scrollable. This gets the scrollable state
        of the entry.

        @see: L{scrollable_set()} for more information.

        @return: The scrollable state
        @rtype: bool

        """
        return bool(elm_entry_scrollable_get(self.obj))

    property scrollable:
        """Enable or disable scrolling in entry

        Normally the entry is not scrollable.

        @type: bool

        """
        def __get__(self):
            return bool(elm_entry_scrollable_get(self.obj))

        def __set__(self, scrollable):
            elm_entry_scrollable_set(self.obj, scrollable)

    def icon_visible_set(self, visible):
        """Sets the visibility of the end widget of the entry, set by
        C{Object.part_content_set("end", content)}.

        @param setting: True if the object should be displayed, False if not.
        @type setting: bool

        """
        elm_entry_icon_visible_set(self.obj, visible)

    property icon_visible:
        """Sets the visibility of the end widget of the entry, set by
        C{Object.part_content_set("end", content)}.

        @type: bool

        """
        def __set__(self, visible):
            elm_entry_icon_visible_set(self.obj, visible)

    def scrollbar_policy_set(self, Elm_Scroller_Policy h, Elm_Scroller_Policy v):
        """This sets the entry's scrollbar policy (i.e. enabling/disabling
        them).

        Setting an entry to single-line mode with L{single_line_set()}
        will automatically disable the display of scrollbars when the entry
        moves inside its scroller.

        @param h: The horizontal scrollbar policy to apply
        @type h: Elm_Scroller_Policy
        @param v: The vertical scrollbar policy to apply
        @type v: Elm_Scroller_Policy

        """
        elm_entry_scrollbar_policy_set(self.obj, h, v)

    property scrollbar_policy:
        """This sets the entry's scrollbar policy (i.e. enabling/disabling
        them).

        Setting an entry to single-line mode with L{single_line_set()}
        will automatically disable the display of scrollbars when the entry
        moves inside its scroller.

        @type: tuple of Elm_Scroller_Policy (h, v)

        """
        def __set__(self, value):
            cdef Elm_Scroller_Policy h, v
            h, v = value
            elm_entry_scrollbar_policy_set(self.obj, h, v)

    def bounce_set(self, h_bounce, v_bounce):
        """This enables/disables bouncing within the entry.

        This function sets whether the entry will bounce when scrolling reaches
        the end of the contained entry.

        @param h_bounce: The horizontal bounce state
        @type h_bounce: bool
        @param v_bounce: The vertical bounce state
        @type v_bounce: bool

        """
        elm_entry_bounce_set(self.obj, h_bounce, v_bounce)

    def bounce_get(self):
        """Get the bounce mode

        @return: Tuple with bounce modes for horizontal and vertical
        @rtype: tuple of bools

        """
        cdef Eina_Bool h_bounce, v_bounce

        elm_entry_bounce_get(self.obj, &h_bounce, &v_bounce)
        return (h_bounce, v_bounce)

    property bounce:
        """Whether the entry will bounce when scrolling reaches
        the end of the contained entry.

        @type: tuple of bools (h_bounce, v_bounce)

        """
        def __get__(self):
            cdef Eina_Bool h_bounce, v_bounce
            elm_entry_bounce_get(self.obj, &h_bounce, &v_bounce)
            return (h_bounce, v_bounce)

        def __set__(self, value):
            cdef Eina_Bool h_bounce, v_bounce
            h_bounce, v_bounce = value
            elm_entry_bounce_set(self.obj, h_bounce, v_bounce)

    def input_panel_layout_set(self, layout):
        """Set the input panel layout of the entry

        @param layout: layout type
        @type layout: Elm_Input_Panel_Layout

        """
        elm_entry_input_panel_layout_set(self.obj, layout)

    def input_panel_layout_get(self):
        """Get the input panel layout of the entry

        @see: L{input_panel_layout_set()}

        @return: layout type
        @rtype: Elm_Input_Panel_Layout

        """
        return elm_entry_input_panel_layout_get(self.obj)

    property input_panel_layout:
        """The input panel layout of the entry

        @type: Elm_Input_Panel_Layout

        """
        def __get__(self):
            return elm_entry_input_panel_layout_get(self.obj)

        def __set__(self, layout):
            elm_entry_input_panel_layout_set(self.obj, layout)

    def input_panel_enabled_set(self, enabled):
        """Sets the attribute to show the input panel automatically.

        @param enabled: If true, the input panel is appeared when entry is
            clicked or has a focus
        @type enabled: bool

        """
        elm_entry_input_panel_enabled_set(self.obj, enabled)

    def input_panel_enabled_get(self):
        """Retrieve the attribute to show the input panel automatically.

        @return: True if input panel will be appeared when the entry is clicked
            or has a focus, False otherwise
        @rtype: bool

        """
        return bool(elm_entry_input_panel_enabled_get(self.obj))

    property input_panel_enabled:
        """Whether to show the input panel automatically or not.

        @type: bool

        """
        def __get__(self):
            return bool(elm_entry_input_panel_enabled_get(self.obj))

        def __set__(self, enabled):
            elm_entry_input_panel_enabled_set(self.obj, enabled)

    def input_panel_show(self):
        """Show the input panel (virtual keyboard) based on the input panel
        property of entry such as layout, autocapital types, and so on.

        Note that input panel is shown or hidden automatically according to the
        focus state of entry widget.
        This API can be used in the case of manually controlling by using
        C{Entry.input_panel_enabled = False}.

        """
        elm_entry_input_panel_show(self.obj)

    def input_panel_hide(self):
        """Hide the input panel (virtual keyboard).

        Note that input panel is shown or hidden automatically according to the
        focus state of entry widget.
        This API can be used in the case of manually controlling by using
        C{Entry.input_panel_enabled_set = False}.

        """
        elm_entry_input_panel_hide(self.obj)

    def input_panel_language_set(self, lang):
        """Set the language mode of the input panel.

        This API can be used if you want to show the alphabet keyboard mode.

        @param lang: language to be set to the input panel.
        @type lang: Elm_Input_Panel_Lang

        """
        elm_entry_input_panel_language_set(self.obj, lang)

    def input_panel_language_get(self):
        """Get the language mode of the input panel.

        @see: L{input_panel_language_set()} for more details.

        @return: input panel language type
        @rtype: Elm_Input_Panel_Lang

        """
        return elm_entry_input_panel_language_get(self.obj)

    property input_panel_language:
        """The language mode of the input panel.

        This API can be used if you want to show the alphabet keyboard mode.

        @type: Elm_Input_Panel_Lang

        """
        def __get__(self):
            return elm_entry_input_panel_language_get(self.obj)

        def __set__(self, lang):
            elm_entry_input_panel_language_set(self.obj, lang)

    # TODO XXX elm_entry_input_panel_imdata_set() ??

    # TODO XXX elm_entry_input_panel_imdata_get() ??

    def input_panel_return_key_type_set(self, return_key_type):
        """Set the "return" key type. This type is used to set string or icon on
        the "return" key of the input panel.

        An input panel displays the string or icon associated with this type

        @param return_key_type: The type of "return" key on the input panel
        @type return_key_type: Elm_Input_Panel_Return_Key_Type

        """
        elm_entry_input_panel_return_key_type_set(self.obj, return_key_type)

    def input_panel_return_key_type_get(self):
        """Get the "return" key type.

        @see: L{input_panel_return_key_type_set()} for more details

        @return: The type of "return" key on the input panel
        @rtype: Elm_Input_Panel_Return_Key_Type

        """
        return elm_entry_input_panel_return_key_type_get(self.obj)

    property input_panel_return_key_type:
        """The "return" key type. This type is used to set string or icon on
        the "return" key of the input panel.

        An input panel displays the string or icon associated with this type

        @type: Elm_Input_Panel_Return_Key_Type

        """
        def __get__(self):
            return elm_entry_input_panel_return_key_type_get(self.obj)

        def __set__(self, return_key_type):
            elm_entry_input_panel_return_key_type_set(self.obj, return_key_type)

    def input_panel_return_key_disabled_set(self, disabled):
        """Set the return key on the input panel to be disabled.

        @param disabled: The state to put in in: C{True} for
            disabled, C{False} for enabled
        @type disabled: bool

        """
        elm_entry_input_panel_return_key_disabled_set(self.obj, disabled)

    def input_panel_return_key_disabled_get(self):
        """Get whether the return key on the input panel should be disabled or not.

        @return: True if it should be disabled
        @rtype: bool

        """
        return elm_entry_input_panel_return_key_disabled_get(self.obj)

    property input_panel_return_key_disabled:
        """Whether the return key on the input panel is disabled or not.

        @type: bool

        """
        def __get__(self):
            return elm_entry_input_panel_return_key_disabled_get(self.obj)

        def __set__(self, disabled):
            elm_entry_input_panel_return_key_disabled_set(self.obj, disabled)

    def input_panel_return_key_autoenabled_set(self, enabled):
        """Set whether the return key on the input panel is disabled automatically when entry has no text.

        If C{enabled} is True, The return key on input panel is disabled when the entry has no text.
        The return key on the input panel is automatically enabled when the entry has text.
        The default value is False.

        @param enabled: If True, the return key is automatically disabled when
            the entry has no text.
        @type enabled: bool

        """
        elm_entry_input_panel_return_key_autoenabled_set(self.obj, enabled)

    property input_panel_return_key_autoenabled:
        """Set whether the return key on the input panel is disabled automatically when entry has no text.

        If C{enabled} is True, The return key on input panel is disabled when the entry has no text.
        The return key on the input panel is automatically enabled when the entry has text.
        The default value is False.

        @type: bool

        """
        def __set__(self, enabled):
            elm_entry_input_panel_return_key_autoenabled_set(self.obj, enabled)

    def imf_context_reset(self):
        """Reset the input method context of the entry if needed.

        This can be necessary in the case where modifying the buffer would confuse on-going input method behavior.
        This will typically cause the Input Method Context to clear the preedit state.

        """
        elm_entry_imf_context_reset(self.obj)

    def prediction_allow_set(self, allow):
        """Set whether the entry should allow to use the text prediction.

        @param prediction: Whether the entry should allow to use the text prediction.
        @type prediction: bool

        """
        elm_entry_prediction_allow_set(self.obj, allow)

    def prediction_allow_get(self):
        """Get whether the entry should allow to use the text prediction.

        @return: True if it allows to use the text prediction, otherwise False.
        @rtype: bool

        """
        return elm_entry_prediction_allow_get(self.obj)

    property prediction_allow:
        """Whether the entry should allow to use the text prediction.

        @type: bool

        """
        def __get__(self):
            return elm_entry_prediction_allow_get(self.obj)

        def __set__(self, allow):
            elm_entry_prediction_allow_set(self.obj, allow)

    # TODO XXX elm_entry_filter_accept_set()
    # TODO XXX elm_entry_imf_context_get() ??

    def cnp_mode_set(self, mode):
        """Control pasting of text and images for the widget.

        Normally the entry allows both text and images to be pasted.
        By setting cnp_mode to be C{ELM_CNP_MODE_NO_IMAGE}, this prevents images from being copy or past.
        By setting cnp_mode to be C{ELM_CNP_MODE_PLAINTEXT}, this remove all tags in text .

        @note: this only changes the behaviour of text.

        @param cnp_mode: One of: ELM_CNP_MODE_MARKUP, ELM_CNP_MODE_NO_IMAGE, ELM_CNP_MODE_PLAINTEXT.
        @type cnp_mode: Elm_Cnp_Mode

        """
        elm_entry_cnp_mode_set(self.obj, mode)

    def cnp_mode_get(self):
        """Getting elm_entry text paste/drop mode.

        Normally the entry allows both text and images to be pasted.
        This gets the copy & paste mode of the entry.

        @return: One of: ELM_CNP_MODE_MARKUP, ELM_CNP_MODE_NO_IMAGE, ELM_CNP_MODE_PLAINTEXT.
        @rtype: Elm_Cnp_Mode

        """
        return elm_entry_cnp_mode_get(self.obj)

    property cnp_mode:
        """Control pasting of text and images for the widget.

        Normally the entry allows both text and images to be pasted.
        By setting cnp_mode to be C{ELM_CNP_MODE_NO_IMAGE}, this prevents images from being copy or past.
        By setting cnp_mode to be C{ELM_CNP_MODE_PLAINTEXT}, this remove all tags in text .

        @note: this only changes the behaviour of text.

        @type: Elm_Cnp_Mode

        """
        def __get__(self):
            return elm_entry_cnp_mode_get(self.obj)

        def __set__(self, mode):
            elm_entry_cnp_mode_set(self.obj, mode)

    def anchor_hover_parent_set(self, evasObject anchor_hover_parent):
        """Set the parent of the hover popup

        Sets the parent object to use by the hover created by the entry
        when an anchor is clicked. See L{Hover} for more details on this.

        @param parent: The object to use as parent for the hover
        @type parent: L{Object}

        """
        elm_entry_anchor_hover_parent_set(self.obj, anchor_hover_parent.obj)

    def anchor_hover_parent_get(self):
        """Get the parent of the hover popup

        Get the object used as parent for the hover created by the entry
        widget. See L{Hover} for more details on this.
        If no parent is set, the same entry object will be used.

        @return: The object used as parent for the hover, None if none is set.
        @rtype: L{Object}

        """
        cdef Evas_Object *anchor_hover_parent
        anchor_hover_parent = elm_entry_anchor_hover_parent_get(self.obj)
        return Object_from_instance(anchor_hover_parent)

    property anchor_hover_parent:
        """Parent of the hover popup

        The parent object to use by the hover created by the entry
        when an anchor is clicked. See L{Hover} for more details on this.

        @type: L{Object}

        """
        def __get__(self):
            cdef Evas_Object *anchor_hover_parent
            anchor_hover_parent = elm_entry_anchor_hover_parent_get(self.obj)
            return Object_from_instance(anchor_hover_parent)

        def __set__(self, evasObject anchor_hover_parent):
            elm_entry_anchor_hover_parent_set(self.obj, anchor_hover_parent.obj)

    def anchor_hover_style_set(self, style):
        """Set the style that the hover should use

        When creating the popup hover, entry will request that it's
        themed according to C{style}.

        Setting style to C{None} means disabling automatic hover.

        @see: L{Object.style_set()}

        @param style: The style to use for the underlying hover
        @type style: string

        """
        elm_entry_anchor_hover_style_set(self.obj, _cfruni(style))

    def anchor_hover_style_get(self):
        """Get the style that the hover created by entry will use.

        @return: The style to use by the hover. C{None} means the default is used.

        @see: L{Object.style_set()}

        """
        return _ctouni(elm_entry_anchor_hover_style_get(self.obj))

    property anchor_hover_style:
        """The style that the hover should use

        When creating the popup hover, entry will request that it's
        themed according to C{style}.

        Setting style to C{None} means disabling automatic hover.

        @see: L{Object.style_set()}

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_entry_anchor_hover_style_get(self.obj))

        def __set__(self, style):
            elm_entry_anchor_hover_style_set(self.obj, _cfruni(style))

    def anchor_hover_end(self):
        """Ends the hover popup in the entry

        When an anchor is clicked, the entry widget will create a hover
        object to use as a popup with user provided content. This function
        terminates this popup, returning the entry to its normal state.

        """
        elm_entry_anchor_hover_end(self.obj)

    def callback_changed_add(self, func, *args, **kwargs):
        """The text within the entry was changed."""
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_changed_user_add(self, func, *args, **kwargs):
        """The text within the entry was changed because of user interaction."""
        self._callback_add("changed,user", func, *args, **kwargs)

    def callback_changed_user_del(self, func):
        self._callback_del("changed,user", func)

    def callback_activated_add(self, func, *args, **kwargs):
        """The enter key was pressed on a single line entry."""
        self._callback_add("activated", func, *args, **kwargs)

    def callback_activated_del(self, func):
        self._callback_del("activated", func)

    def callback_press_add(self, func, *args, **kwargs):
        """A mouse button has been pressed on the entry."""
        self._callback_add("press", func, *args, **kwargs)

    def callback_press_del(self, func):
        self._callback_del("press", func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        """A mouse button has been pressed and held for a couple seconds."""
        self._callback_add("longpressed", func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del("longpressed", func)

    def callback_clicked_add(self, func, *args, **kwargs):
        """The entry has been clicked (mouse press and release)."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_double_clicked_add(self, func, *args, **kwargs):
        """The entry has been double clicked."""
        self._callback_add("clicked,double", func, *args, **kwargs)

    def callback_double_clicked_del(self, func):
        self._callback_del("clicked,double", func)

    def callback_triple_clicked_add(self, func, *args, **kwargs):
        """The entry has been triple clicked."""
        self._callback_add("clicked,triple", func, *args, **kwargs)

    def callback_triple_clicked_del(self, func):
        self._callback_del("clicked,triple", func)

    def callback_focused_add(self, func, *args, **kwargs):
        """The entry has received focus."""
        self._callback_add("focused", func, *args, **kwargs)

    def callback_focused_del(self, func):
        self._callback_del("focused", func)

    def callback_unfocused_add(self, func, *args, **kwargs):
        """The entry has lost focus."""
        self._callback_add("unfocused", func, *args, **kwargs)

    def callback_unfocused_del(self, func):
        self._callback_del("unfocused", func)

    def callback_selection_paste_add(self, func, *args, **kwargs):
        """A paste of the clipboard contents was requested."""
        self._callback_add("selection,paste", func, *args, **kwargs)

    def callback_selection_paste_del(self, func):
        self._callback_del("selection,paste", func)

    def callback_selection_copy_add(self, func, *args, **kwargs):
        """A copy of the selected text into the clipboard was requested."""
        self._callback_add("selection,copy", func, *args, **kwargs)

    def callback_selection_copy_del(self, func):
        self._callback_del("selection,copy", func)

    def callback_selection_cut_add(self, func, *args, **kwargs):
        """A cut of the selected text into the clipboard was requested."""
        self._callback_add("selection,cut", func, *args, **kwargs)

    def callback_selection_cut_del(self, func):
        self._callback_del("selection,cut", func)

    def callback_selection_start_add(self, func, *args, **kwargs):
        """A selection has begun and no previous selection existed."""
        self._callback_add("selection,start", func, *args, **kwargs)

    def callback_selection_start_del(self, func):
        self._callback_del("selection,start", func)

    def callback_selection_changed_add(self, func, *args, **kwargs):
        """The current selection has changed."""
        self._callback_add("selection,changed", func, *args, **kwargs)

    def callback_selection_changed_del(self, func):
        self._callback_del("selection,changed", func)

    def callback_selection_cleared_add(self, func, *args, **kwargs):
        """The current selection has been cleared."""
        self._callback_add("selection,cleared", func, *args, **kwargs)

    def callback_selection_cleared_del(self, func):
        self._callback_del("selection,cleared", func)

    def callback_cursor_changed_add(self, func, *args, **kwargs):
        """The cursor has changed position."""
        self._callback_add("cursor,changed", func, *args, **kwargs)

    def callback_cursor_changed_del(self, func):
        self._callback_del("cursor,changed", func)

    def callback_anchor_clicked_add(self, func, *args, **kwargs):
        """An anchor has been clicked. The event_info
        parameter for the callback will be an C{Elm_Entry_Anchor_Info}.

        """
        self._callback_add_full("anchor,clicked", _entryanchor_conv,
                                func, *args, **kwargs)

    def callback_anchor_clicked_del(self, func):
        self._callback_del_full("anchor,clicked", _entryanchor_conv,
                                func)

    def callback_anchor_in_add(self, func, *args, **kwargs):
        """Mouse cursor has moved into an anchor. The event_info
        parameter for the callback will be an C{Elm_Entry_Anchor_Info}.

        """
        self._callback_add_full("anchor,in", _entryanchor_conv,
                                func, *args, **kwargs)

    def callback_anchor_in_del(self, func):
        self._callback_del_full("anchor,in", _entryanchor_conv,
                                func)

    def callback_anchor_out_add(self, func, *args, **kwargs):
        """Mouse cursor has moved out of an anchor. The event_info
        parameter for the callback will be an C{Elm_Entry_Anchor_Info}.

        """
        self._callback_add_full("anchor,out", _entryanchor_conv,
                                func, *args, **kwargs)

    def callback_anchor_out_del(self, func):
        self._callback_del_full("anchor,out", _entryanchor_conv,
                                func)

    def callback_anchor_up_add(self, func, *args, **kwargs):
        """Mouse button has been unpressed on an anchor. The event_info
        parameter for the callback will be an C{Elm_Entry_Anchor_Info}.

        """
        self._callback_add_full("anchor,up", _entryanchor_conv,
                                func, *args, **kwargs)

    def callback_anchor_up_del(self, func):
        self._callback_del_full("anchor,up", _entryanchor_conv,
                                func)

    def callback_anchor_down_add(self, func, *args, **kwargs):
        """Mouse button has been pressed on an anchor. The event_info
        parameter for the callback will be an C{Elm_Entry_Anchor_Info}.

        """
        self._callback_add_full("anchor,down", _entryanchor_conv,
                                func, *args, **kwargs)

    def callback_anchor_down_del(self, func):
        self._callback_del_full("anchor,down", _entryanchor_conv,
                                func)

    def callback_anchor_hover_opened_add(self, func, *args, **kwargs):
        self._callback_add_full("anchor,hover,opened", _entryanchorhover_conv,
                                func, *args, **kwargs)

    def callback_anchor_hover_opened_del(self, func):
        self._callback_del_full("anchor,hover,opened", _entryanchorhover_conv,
                                func)

    def callback_preedit_changed_add(self, func, *args, **kwargs):
        """The preedit string has changed."""
        self._callback_add("preedit,changed", func, *args, **kwargs)

    def callback_preedit_changed_del(self, func):
        self._callback_del("preedit,changed", func)

    def callback_language_changed_add(self, func, *args, **kwargs):
        """Program language changed."""
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed", func)

_elm_widget_type_register("entry", Entry)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryEntry_Type # hack to install metaclass
_install_metaclass(&PyElementaryEntry_Type, ElementaryObjectMeta)
