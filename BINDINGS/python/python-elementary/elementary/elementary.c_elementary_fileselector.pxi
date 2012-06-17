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

cdef public class Fileselector(LayoutClass) [object PyElementaryFileselector, type PyElementaryFileselector_Type]:

    """
    A file selector is a widget that allows a user to navigate
    through a file system, reporting file selections back via its
    API.

    It contains shortcut buttons for home directory (C{~}) and to
    jump one directory upwards (..), as well as cancel/ok buttons to
    confirm/cancel a given selection. After either one of those two
    former actions, the file selector will issue its C{"done"} smart
    callback.

    There's a text entry on it, too, showing the name of the current
    selection. There's the possibility of making it editable, so it
    is useful on file saving dialogs on applications, where one
    gives a file name to save contents to, in a given directory in
    the system. This custom file name will be reported on the C{"done"}
    smart callback (explained in sequence).

    Finally, it has a view to display file system items into in two
    possible forms:
        - list
        - grid

    If Elementary is built with support of the Ethumb thumbnailing
    library, the second form of view will display preview thumbnails
    of files which it supports.

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"selected"} - the user has clicked on a file (when not in
          folders-only mode) or directory (when in folders-only mode)
        - C{"directory,open"} - the list has been populated with new
          content (C{event_info} is the directory's path)
        - C{"done"} - the user has clicked on the "ok" or "cancel"
          buttons (C{event_info} is the selection's path)

    """

    cdef object _cbs

    def __init__(self, evasObject parent):
        """Add a new file selector widget to the given parent Elementary
        (container) object

        This function inserts a new file selector widget on the canvas.

        @param parent: The parent object
        @type parent: L{Object}
        @return: a new file selector widget handle or C{None}, on errors
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_add(parent.obj))
        self._cbs = {}

    def is_save_set(self, is_save):
        """Enable/disable the file name entry box where the user can type
        in a name for a file, in a given file selector widget

        Having the entry editable is useful on file saving dialogs on
        applications, where one gives a file name to save contents to,
        in a given directory in the system. This custom file name will
        be reported on the C{"done"} smart callback.

        @see: L{is_save_get()}

        @param is_save: C{True} to make the file selector a "saving
            dialog", C{False} otherwise
        @type is_save: bool

        """
        elm_fileselector_is_save_set(self.obj, is_save)

    def is_save_get(self):
        """Get whether the given file selector is in "saving dialog" mode

        @see: L{is_save_set()} for more details

        @return: C{True}, if the file selector is in "saving dialog"
            mode, C{False} otherwise (and on errors)
        @rtype: bool

        """
        return elm_fileselector_is_save_get(self.obj)

    property is_save:
        """Enable/disable the file name entry box where the user can type
        in a name for a file, in a given file selector widget

        Having the entry editable is useful on file saving dialogs on
        applications, where one gives a file name to save contents to,
        in a given directory in the system. This custom file name will
        be reported on the C{"done"} smart callback.

        @type: bool

        """
        def __get__(self):
            return elm_fileselector_is_save_get(self.obj)

        def __set__(self, is_save):
            elm_fileselector_is_save_set(self.obj, is_save)

    def folder_only_set(self, folder_only):
        """Enable/disable folder-only view for a given file selector widget

        If enabled, the widget's view will only display folder items,
        naturally.

        @see: L{folder_only_get()}

        @param only: C{True} to make C{obj} only display directories, C{False}
            to make files to be displayed in it too
        @type only: bool

        """
        elm_fileselector_folder_only_set(self.obj, folder_only)

    def folder_only_get(self):
        """Get whether folder-only view is set for a given file selector
        widget

        @see: elm_fileselector_folder_only_get()

        @return: C{True} if the widget is only displaying directories, C{False}
            if files are being displayed in it too (and on errors)
        @rtype: bool

        """
        return elm_fileselector_folder_only_get(self.obj)

    property folder_only:
        """Enable/disable folder-only view for a given file selector widget

        If enabled, the widget's view will only display folder items,
        naturally.

        @type: bool

        """
        def __get__(self):
            return elm_fileselector_folder_only_get(self.obj)

        def __set__(self, folder_only):
            elm_fileselector_folder_only_set(self.obj, folder_only)

    def buttons_ok_cancel_set(self, buttons):
        """Enable/disable the "ok" and "cancel" buttons on a given file
        selector widget

        @note: A file selector without those buttons will never emit the
            C{"done"} smart event, and is only usable if one is just hooking
            to the other two events.

        @see: L{buttons_ok_cancel_get()}

        @param buttons: C{True} to show buttons, C{False} to hide.
        @type buttons: bool

        """
        elm_fileselector_buttons_ok_cancel_set(self.obj, buttons)

    def buttons_ok_cancel_get(self):
        """Get whether the "ok" and "cancel" buttons on a given file
        selector widget are being shown.

        @see: L{buttons_ok_cancel_set()} for more details

        @return: C{True} if they are being shown, C{False} otherwise
            (and on errors)
        @rtype: bool

        """
        return elm_fileselector_buttons_ok_cancel_get(self.obj)

    property buttons_ok_cancel:
        """Enable/disable the "ok" and "cancel" buttons on a given file
        selector widget

        @note: A file selector without those buttons will never emit the
            C{"done"} smart event, and is only usable if one is just hooking
            to the other two events.

        @type: bool

        """
        def __get__(self):
            return elm_fileselector_buttons_ok_cancel_get(self.obj)

        def __set__(self, buttons):
            elm_fileselector_buttons_ok_cancel_set(self.obj, buttons)

    def expandable_set(self, expand):
        """Enable/disable a tree view in the given file selector widget,
        B{if it's in C{ELM_FILESELECTOR_LIST} mode}

        In a tree view, arrows are created on the sides of directories,
        allowing them to expand in place.

        @note: If it's in other mode, the changes made by this function
            will only be visible when one switches back to "list" mode.

        @see: L{expandable_get()}

        @param expand: C{True} to enable tree view, C{False} to disable
        @type expand: bool

        """
        elm_fileselector_expandable_set(self.obj, expand)

    def expandable_get(self):
        """Get whether tree view is enabled for the given file selector
        widget

        @see: L{expandable_set()} for more details

        @return: C{True} if in tree view, C{False} otherwise (and or errors)

        """
        return elm_fileselector_expandable_get(self.obj)

    property expandable:
        """Enable/disable a tree view in the given file selector widget,
        B{if it's in C{ELM_FILESELECTOR_LIST} mode}

        In a tree view, arrows are created on the sides of directories,
        allowing them to expand in place.

        @note: If it's in other mode, the changes made by this function
            will only be visible when one switches back to "list" mode.

        @type: bool

        """
        def __get__(self):
            return elm_fileselector_expandable_get(self.obj)

        def __set__(self, expand):
            elm_fileselector_expandable_set(self.obj, expand)

    def path_set(self, path):
        """Set, programmatically, the B{directory} that a given file
        selector widget will display contents from

        This will change the B{directory} displayed. It
        will also clear the text entry area on the object, which
        displays select files' names.

        @see: L{path_get()}

        @param path: The path to display
        @type path: string

        """
        elm_fileselector_path_set(self.obj, _cfruni(path))

    def path_get(self):
        """Get the parent directory's path that a given file selector
        widget is displaying

        @see: L{path_set()}

        @return: The (full) path of the directory the file selector is
            displaying
        @rtype: string

        """
        return _ctouni(elm_fileselector_path_get(self.obj))

    property path:
        """The B{directory} that a given file selector widget will display
        contents from

        Setting this will change the B{directory} displayed. It
        will also clear the text entry area on the object, which
        displays select files' names.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_fileselector_path_get(self.obj))

        def __set__(self, path):
            elm_fileselector_path_set(self.obj, _cfruni(path))

    def selected_set(self, path):
        """Set, programmatically, the currently selected file/directory in
        the given file selector widget

        @see: L{selected_get()}

        @param path: The (full) path to a file or directory
        @type path: string
        @return: C{True} on success, C{False} on failure. The latter case occurs
            if the directory or file pointed to do not exist.
        @rtype: bool

        """
        return elm_fileselector_selected_set(self.obj, _cfruni(path))

    def selected_get(self):
        """Get the currently selected item's (full) path, in the given file
        selector widget

        @note: Custom editions on object's text entry, if made,
            will appear on the return string of this function, naturally.

        @see: L{selected_set()} for more details

        @return: The absolute path of the selected item
        @rtype: string

        """
        return _ctouni(elm_fileselector_selected_get(self.obj))

    property selected:
        """The currently selected file/directory in the given file selector
        widget

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_fileselector_selected_get(self.obj))

        def __set__(self, path):
            #TODO: Check return value for success
            elm_fileselector_selected_set(self.obj, _cfruni(path))

    def mode_set(self, mode):
        """Set the mode in which a given file selector widget will display
        (layout) file system entries in its view

        @note: By using L{expandable_set()}, the user may
            trigger a tree view for that list.

        @note: If Elementary is built with support of the Ethumb
            thumbnailing library, the second form of view will display
            preview thumbnails of files which it supports. You must have
            elm_need_ethumb() called in your Elementary for thumbnailing to
            work, though.

        @see: L{expandable_set()}
        @see: L{mode_get()}

        @param mode: The mode of the fileselector, being it one of
            ELM_FILESELECTOR_LIST (default) or ELM_FILESELECTOR_GRID. The first
            one, naturally, will display the files in a list. The latter will
            make the widget to display its entries in a grid form.
        @type mode: Elm_Fileselector_Mode

        """
        elm_fileselector_mode_set(self.obj, mode)

    def mode_get(self):
        """Get the mode in which a given file selector widget is displaying
        (layouting) file system entries in its view

        @see: L{mode_set()} for more details

        @return: The mode in which the fileselector is at
        @rtype: Elm_Fileselector_Mode

        """
        return elm_fileselector_mode_get(self.obj)

    property mode:
        """The mode in which a given file selector widget will display
        (layout) file system entries in its view

        @note: By using L{expandable_set()}, the user may
            trigger a tree view for that list.

        @note: If Elementary is built with support of the Ethumb
            thumbnailing library, the second form of view will display
            preview thumbnails of files which it supports. You must have
            elm_need_ethumb() called in your Elementary for thumbnailing to
            work, though.

        @see: L{expandable}

        @type: Elm_Fileselector_Mode

        """
        def __get__(self):
            return elm_fileselector_mode_get(self.obj)

        def __set__(self, mode):
            elm_fileselector_mode_set(self.obj, mode)

    def callback_selected_add(self, func, *args, **kwargs):
        """The user has clicked on a file (when not in folders-only mode) or
        directory (when in folders-only mode). Parameter C{event_info}
        contains the selected file or directory."""
        self._callback_add_full("selected", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _cb_string_conv, func)

    def callback_directory_open_add(self, func, *args, **kwargs):
        """The list has been populated with new content (C{event_info} is
        the directory's path)."""
        self._callback_add_full("directory,open", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_directory_open_del(self, func):
        self._callback_del_full("directory,open", _cb_string_conv, func)

    def callback_done_add(self, func, *args, **kwargs):
        """The user has clicked on the "ok" or "cancel" buttons
        (C{event_info} is a pointer to the selection's path)."""
        self._callback_add_full("done", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_done_del(self, func):
        self._callback_del_full("done", _cb_string_conv, func)

_elm_widget_type_register("fileselector", Fileselector)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryFileselector_Type # hack to install metaclass
_install_metaclass(&PyElementaryFileselector_Type, ElementaryObjectMeta)
