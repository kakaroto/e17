# Copyright (c) 2011 Fabiano Fidêncio
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

cdef public class FileselectorButton(Button) [object PyElementaryFileselectorButton, type PyElementaryFileselectorButton_Type]:

    """This is a button that, when clicked, creates an Elementary window (or
    inner window) with a L{Fileselector} "file selector widget" within.

    When a file is chosen, the (inner) window is closed and the button emits
    a signal having the selected file as it's C{event_info}.

    This widget encapsulates operations on its internal file selector on its
    own API. There is less control over its file selector than that one
    would have instantiating one directly.

    The following styles are available for this button:
        - C{"default"}
        - C{"anchor"}
        - C{"hoversel_vertical"}
        - C{"hoversel_vertical_entry"}

    This widget emits the following signals, besides the ones sent from
    L{Button}:
        - C{"file,chosen"} - the user has selected a path which comes as the
          C{event_info} data

    Default text parts of the fileselector_button widget that you can use for
    are:
        - "default" - Label of the fileselector_button

    Default content parts of the fileselector_button widget that you can use for
    are:
        - "icon" - Icon of the fileselector_button

    """

    cdef object _cbs

    def __init__(self, evasObject parent):
        """Add a new file selector button widget to the given parent
        Elementary (container) object

        @param parent: The parent object
        @type parent: L{Object}
        @return: a new file selector button widget handle or C{None}, on errors
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_button_add(parent.obj))
        self._cbs = {}

    def window_title_set(self, title):
        """Set the title for a given file selector button widget's window

        This will change the popup window's title, when the file selector pops
        out after a click on the button. Those windows have the default
        (unlocalized) value of C{"Select a file"} as titles.

        @note: It will only take effect if the file selector
            button widget is B{not} under "inwin mode".

        @see: L{window_title_get()}

        @param title: The title string
        @type title: string

        """
        elm_fileselector_button_window_title_set(self.obj, _cfruni(title))

    def window_title_get(self):
        """Get the title for a given file selector button widget's window

        @see: L{window_title_get()} for more details

        @return: Title of the file selector button's window
        @rtype: string

        """
        return _ctouni(elm_fileselector_button_window_title_get(self.obj))

    property window_title:
        """The title for a given file selector button widget's window

        This is the popup window's title, when the file selector pops
        out after a click on the button. Those windows have the default
        (unlocalized) value of C{"Select a file"} as titles.

        @note: Setting this will only take effect if the file selector
            button widget is B{not} under "inwin mode".

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_fileselector_button_window_title_get(self.obj))

        def __set__(self, title):
            elm_fileselector_button_window_title_set(self.obj, _cfruni(title))

    def window_size_set(self, width, height):
        """Set the size of a given file selector button widget's window,
        holding the file selector itself.

        @note: it will only take any effect if the file selector button
            widget is B{not} under "inwin mode". The default size for the
            window (when applicable) is 400x400 pixels.

        @see: L{window_size_get()}

        @param width: The window's width
        @type width: Evas_Coord (int)
        @param height: The window's height
        @type height: Evas_Coord (int)

        """
        elm_fileselector_button_window_size_set(self.obj, width, height)

    def window_size_get(self):
        """Get the size of a given file selector button widget's window,
        holding the file selector itself.

        @see: L{window_size_set()}, for more details

        @return: Width and height of the window
        @rtype: tuple of Evas_Coords (int)

        """
        cdef Evas_Coord w, h
        elm_fileselector_button_window_size_get(self.obj, &w, &h)
        return (w, h)

    property window_size:
        """The size of a given file selector button widget's window,
        holding the file selector itself.

        @note: Setting this will only take any effect if the file selector button
            widget is B{not} under "inwin mode". The default size for the
            window (when applicable) is 400x400 pixels.

        @type: tuple of Evas_Coords (int)

        """
        def __get__(self):
            cdef Evas_Coord w, h
            elm_fileselector_button_window_size_get(self.obj, &w, &h)
            return (w, h)

        def __set__(self, value):
            cdef Evas_Coord w, h
            w, h = value
            elm_fileselector_button_window_size_set(self.obj, w, h)

    def path_set(self, path):
        """Set the initial file system path for a given file selector
        button widget

        It must be a B{directory} path, which will have the contents
        displayed initially in the file selector's view. The default initial
        path is the C{"HOME"} environment variable's value.

        @see: L{path_get()}

        @param path: The path string
        @type path: string

        """
        elm_fileselector_button_path_set(self.obj, _cfruni(path))

    def path_get(self):
        """Get the initial file system path set for a given file selector
        button widget

        @see: L{path_set()} for more details

        @return: The path string
        @rtype: string

        """
        return _ctouni(elm_fileselector_button_path_get(self.obj))

    property path:
        """The initial file system path for a given file selector
        button widget

        It must be a B{directory} path, which will have the contents
        displayed initially in the file selector's view. The default initial
        path is the C{"HOME"} environment variable's value.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_fileselector_button_path_get(self.obj))

        def __set__(self, path):
            elm_fileselector_button_path_set(self.obj, _cfruni(path))

    def expandable_set(self, expand):
        """Enable/disable a tree view in the given file selector button
        widget's internal file selector

        This has the same effect as L{Fileselector.expandable_set()},
        but now applied to a file selector button's internal file
        selector.

        @note: There's no way to put a file selector button's internal
        file selector in "grid mode", as one may do with "pure" file
        selectors.

        @see: L{expandable_get()}

        @param value: C{True} to enable tree view, C{False} to disable
        @type value: bool

        """
        elm_fileselector_button_expandable_set(self.obj, expand)

    def expandable_get(self):
        """Get whether tree view is enabled for the given file selector
        button widget's internal file selector

        @see: L{expandable_set()} for more details

        @return: C{True} if widget's internal file selector
            is in tree view, C{False} otherwise (and on errors)
        @rtype: bool

        """
        return bool(elm_fileselector_button_expandable_get(self.obj))

    property expandable:
        """Enable/disable a tree view in the given file selector button
        widget's internal file selector

        This has the same effect as L{Fileselector.expandable},
        but now applied to a file selector button's internal file
        selector.

        @note: There's no way to put a file selector button's internal
        file selector in "grid mode", as one may do with "pure" file
        selectors.

        @type: bool

        """
        def __get__(self):
            return bool(elm_fileselector_button_expandable_get(self.obj))

        def __set__(self, expand):
            elm_fileselector_button_expandable_set(self.obj, expand)

    def folder_only_set(self, folder_only):
        """Set whether a given file selector button widget's internal file
        selector is to display folders only or the directory contents,
        as well.

        This has the same effect as L{Fileselector.folder_only_set()},
        but now applied to a file selector button's internal file
        selector.

        @see: L{folder_only_get()}

        @param value: C{True} to make widget's internal file
            selector only display directories, C{False} to make files
            to be displayed in it too
        @type value: bool

        """
        elm_fileselector_button_folder_only_set(self.obj, folder_only)

    def folder_only_get(self):
        """Get whether a given file selector button widget's internal file
        selector is displaying folders only or the directory contents,
        as well.

        @see: L{folder_only_set()} for more details

        @return: C{True} if widget's internal file
            selector is only displaying directories, C{False} if files
            are being displayed in it too (and on errors)
        @rtype: bool

        """
        return bool(elm_fileselector_button_folder_only_get(self.obj))

    property folder_only:
        """Whether a given file selector button widget's internal file
        selector is to display folders only or the directory contents,
        as well.

        This has the same effect as L{Fileselector.folder_only_set()},
        but now applied to a file selector button's internal file
        selector.

        @type: bool

        """
        def __get__(self):
            return bool(elm_fileselector_button_folder_only_get(self.obj))

        def __set__(self, folder_only):
            elm_fileselector_button_folder_only_set(self.obj, folder_only)

    def is_save_set(self, is_save):
        """Enable/disable the file name entry box where the user can type
        in a name for a file, in a given file selector button widget's
        internal file selector.

        This has the same effect as L{Fileselector.is_save_set()},
        but now applied to a file selector button's internal file
        selector.

        @see: L{is_save_get()}

        @param value: C{True} to make widget's internal
            file selector a "saving dialog", C{False} otherwise
        @type value: bool

        """
        elm_fileselector_button_is_save_set(self.obj, is_save)

    def is_save_get(self):
        """Get whether the given file selector button widget's internal
        file selector is in "saving dialog" mode

        @see: L{is_save_set()} for more details

        @return: C{True}, if widget's internal file selector is in
            "saving dialog" mode, C{False} otherwise (and on errors)
        @rtype: bool

        """
        return bool(elm_fileselector_button_is_save_get(self.obj))

    property is_save:
        """Enable/disable the file name entry box where the user can type
        in a name for a file, in a given file selector button widget's
        internal file selector.

        This has the same effect as L{Fileselector.is_save},
        but now applied to a file selector button's internal file
        selector.

        @type: bool

        """
        def __get__(self):
            return bool(elm_fileselector_button_is_save_get(self.obj))

        def __set__(self, is_save):
            elm_fileselector_button_is_save_set(self.obj, is_save)

    def inwin_mode_set(self, inwin_mode):
        """Set whether a given file selector button widget's internal file
        selector will raise an Elementary "inner window", instead of a
        dedicated Elementary window. By default, it won't.

        @see: L{InnerWindow} for more information on inner windows
        @see: L{inwin_mode_get()}

        @param value: C{True} to make it use an inner window, C{False} to make
            it use a dedicated window
        @type value: bool

        """
        elm_fileselector_button_inwin_mode_set(self.obj, inwin_mode)

    def inwin_mode_get(self):
        """Get whether a given file selector button widget's internal file
        selector will raise an Elementary "inner window", instead of a
        dedicated Elementary window.

        @see: L{inwin_mode_set()} for more details

        @return: C{True} if will use an inner window, C{False}
        if it will use a dedicated window

        """
        return bool(elm_fileselector_button_inwin_mode_get(self.obj))

    property inwin_mode:
        """Whether a given file selector button widget's internal file
        selector will raise an Elementary "inner window", instead of a
        dedicated Elementary window. By default, it won't.

        @see: L{InnerWindow} for more information on inner windows

        @type: bool

        """
        def __get__(self):
            return bool(elm_fileselector_button_inwin_mode_get(self.obj))

        def __set__(self, inwin_mode):
            elm_fileselector_button_inwin_mode_set(self.obj, inwin_mode)

    def callback_file_chosen_add(self, func, *args, **kwargs):
        """The user has selected a path which comes as the C{event_info} data."""
        self._callback_add_full("file,chosen", _cb_string_conv,
                                func, *args, **kwargs)

    def callback_file_chosen_del(self, func):
        self._callback_del_full("file,chosen", _cb_string_conv, func)

_elm_widget_type_register("fileselector_button", FileselectorButton)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryFileselectorButton_Type # hack to install metaclass
_install_metaclass(&PyElementaryFileselectorButton_Type, ElementaryObjectMeta)
