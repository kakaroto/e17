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

cdef class Theme(object):

    """Elementary uses Edje to theme its widgets, naturally. But for the most
    part this is hidden behind a simpler interface that lets the user set
    extensions and choose the style of widgets in a much easier way.

    Instead of thinking in terms of paths to Edje files and their groups
    each time you want to change the appearance of a widget, Elementary
    works so you can add any theme file with extensions or replace the
    main theme at one point in the application, and then just set the style
    of widgets with elm_object_style_set() and related functions. Elementary
    will then look in its list of themes for a matching group and apply it,
    and when the theme changes midway through the application, all widgets
    will be updated accordingly.

    There are three concepts you need to know to understand how Elementary
    theming works: default theme, extensions and overlays.

    Default theme, obviously enough, is the one that provides the default
    look of all widgets. End users can change the theme used by Elementary
    by setting the C{ELM_THEME} environment variable before running an
    application, or globally for all programs using the C{elementary_config}
    utility. Applications can change the default theme using elm_theme_set(),
    but this can go against the user wishes, so it's not an advised practice.

    Ideally, applications should find everything they need in the already
    provided theme, but there may be occasions when that's not enough and
    custom styles are required to correctly express the idea. For this
    cases, Elementary has extensions.

    Extensions allow the application developer to write styles of its own
    to apply to some widgets. This requires knowledge of how each widget
    is themed, as extensions will always replace the entire group used by
    the widget, so important signals and parts need to be there for the
    object to behave properly (see documentation of Edje for details).
    Once the theme for the extension is done, the application needs to add
    it to the list of themes Elementary will look into, using
    elm_theme_extension_add(), and set the style of the desired widgets as
    he would normally with elm_object_style_set().

    Overlays, on the other hand, can replace the look of all widgets by
    overriding the default style. Like extensions, it's up to the application
    developer to write the theme for the widgets it wants, the difference
    being that when looking for the theme, Elementary will check first the
    list of overlays, then the set theme and lastly the list of extensions,
    so with overlays it's possible to replace the default view and every
    widget will be affected. This is very much alike to setting the whole
    theme for the application and will probably clash with the end user
    options, not to mention the risk of ending up with not matching styles
    across the program. Unless there's a very special reason to use them,
    overlays should be avoided for the reasons exposed before.

    All these theme lists are handled by ::Elm_Theme instances. Elementary
    keeps one default internally and every function that receives one of
    these can be called with NULL to refer to this default (except for
    elm_theme_free()). It's possible to create a new instance of a
    ::Elm_Theme to set other theme for a specific widget (and all of its
    children), but this is as discouraged, if not even more so, than using
    overlays. Don't use this unless you really know what you are doing.

    """

    cdef Elm_Theme *th

    def __cinit__(self):
        self.th = NULL

    def __init__(self, default=False):
        """Create a new specific theme

        This creates an empty specific theme that only uses the default
        theme. A specific theme has its own private set of extensions and
        overlays too (which are empty by default). Specific themes do not
        fall back to themes of parent objects. They are not intended for
        this use. Use styles, overlays and extensions when needed, but avoid
        specific themes unless there is no other way (example: you want to
        have a preview of a new theme you are selecting in a "theme
        selector" window. The preview is inside a scroller and should
        display what the theme you selected will look like, but not actually
        apply it yet. The child of the scroller will have a specific theme
        set to show this preview before the user decides to apply it to all
        applications).

        """
        cdef Elm_Theme *th
        if default:
            th = elm_theme_default_get()
        else:
            th = elm_theme_new()

        if th != NULL:
            self.th = th
        else:
            Py_DECREF(self)

    def __dealloc__(self):
        """Free a specific theme

        This frees a theme created with elm_theme_new().

        """
        if self.th != NULL:
            elm_theme_free(self.th)
            self.th = NULL

    def copy(self, Theme thdst):
        """copy(thdst)

        Copy the theme from the source to the destination theme

        This makes a one-time static copy of all the theme config, extensions
        and overlays from C{th} to C{thdst}. If C{th} references a theme, then
        C{thdst} is also set to reference it, with all the theme settings,
        overlays and extensions that C{th} had.

        @param thdst: The destination theme to copy data to
        @type thdst: L{Theme}

        """
        elm_theme_copy(self.th, thdst.th)

    property reference:
        """Tell the source theme to reference the ref theme

        This clears C{th} to be empty and then sets it to refer to C{thref}
        so C{th} acts as an override to C{thref}, but where its overrides
        don't apply, it will fall through to C{thref} for configuration.

        @type: L{Theme}

        """
        def __set__(self, Theme thref):
            elm_theme_ref_set(self.th, thref.th)

        def __get__(self):
            cdef Theme thref = Theme()
            thref.th = elm_theme_ref_get(self.th)
            return thref

    def overlay_add(self, item):
        """overlay_add(item)

        Prepends a theme overlay to the list of overlays

        Use this if your application needs to provide some custom overlay
        theme (An Edje file that replaces some default styles of widgets)
        where adding new styles, or changing system theme configuration is
        not possible. Do NOT use this instead of a proper system theme
        configuration. Use proper configuration files, profiles, environment
        variables etc. to set a theme so that the theme can be altered by
        simple configuration by a user. Using this call to achieve that
        effect is abusing the API and will create lots of trouble.

        @see: L{extension_add()}

        @param item: The Edje file path to be used
        @type item: string

        """
        elm_theme_overlay_add(self.th, _cfruni(item))

    def overlay_del(self, item):
        """overlay_del(item)

        Delete a theme overlay from the list of overlays

        @see: L{overlay_add()}

        @param item: The name of the theme overlay
        @type item: string

        """
        elm_theme_overlay_del(self.th, _cfruni(item))

    property overlay_list:
        """Get the list of registered overlays for the given theme

        @see: L{overlay_add()}

        @type: tuple of strings

        """
        def __get__(self):
            return tuple(_strings_to_python(elm_theme_overlay_list_get(self.th)))

    def extension_add(self, item):
        """extension_add(item)

        Appends a theme extension to the list of extensions.

        This is intended when an application needs more styles of widgets or
        new widget themes that the default does not provide (or may not
        provide). The application has "extended" usage by coming up with new
        custom style names for widgets for specific uses, but as these are
        not "standard", they are not guaranteed to be provided by a default
        theme. This means the application is required to provide these extra
        elements itself in specific Edje files. This call adds one of those
        Edje files to the theme search path to be search after the default
        theme. The use of this call is encouraged when default styles do not
        meet the needs of the application. Use this call instead of
        elm_theme_overlay_add() for almost all cases.

        @see: L{Object.style}

        @param item: The Edje file path to be used
        @type item: string

        """
        elm_theme_extension_add(self.th, _cfruni(item))

    def extension_del(self, item):
        """extension_del(item)

        Deletes a theme extension from the list of extensions.

        @see: L{extension_add()}

        @param item: The name of the theme extension
        @type item: string

        """
        elm_theme_extension_del(self.th, _cfruni(item))

    property extension_list:
        """Get the list of registered extensions for the given theme

        @see: L{extension_add()}

        @type: tuple of strings

        """
        def __get__(self):
            return tuple(_strings_to_python(elm_theme_extension_list_get(self.th)))

    property order:
        """Set the theme search order for the given theme

        This sets the search string for the theme in path-notation from first
        theme to search, to last, delimited by the : character. Example:

        "shiny:/path/to/file.edj:default"

        See the ELM_THEME environment variable for more information.

        @see: L{list_get()}

        @type: string

        """
        def __set__(self, theme):
            elm_theme_set(self.th, _cfruni(theme))

        def __get__(self):
            return _ctouni(elm_theme_get(self.th))

    property elements:
        """Return a list of theme elements to be used in a theme.

        This returns the internal list of theme elements (will only be valid as
        long as the theme is not modified by elm_theme_set() or theme is not
        freed by elm_theme_free(). This is a list of strings which must not be
        altered as they are also internal. If C{th} is NULL, then the default
        theme element list is returned.

        A theme element can consist of a full or relative path to a .edj file,
        or a name, without extension, for a theme to be searched in the known
        theme paths for Elementary.

        @see: L{order}

        @type: tuple of strings

        """
        def __get__(self):
            return tuple(_strings_to_python(elm_theme_list_get(self.th)))

    def flush(self):
        """flush()

        Flush the current theme.

        This flushes caches that let elementary know where to find theme elements
        in the given theme. If C{th} is NULL, then the default theme is flushed.
        Call this function if source theme data has changed in such a way as to
        make any caches Elementary kept invalid.

        """
        elm_theme_flush(self.th)

    def data_get(self, key):
        """data_get(key)

        Get a data item from a theme

        This function is used to return data items from edc in C{th}, an
        overlay, or an extension. It works the same way as
        edje_file_data_get() except that the return is stringshared.

        @param key: The data key to search with
        @type key: string

        @return: The data value, or NULL on failure
        @rtype: string

        """
        return _ctouni(elm_theme_data_get(self.th, _cfruni(key)))

def theme_list_item_path_get(f, in_search_path):
    """theme_list_item_path_get(f, in_search_path)

    Return the full path for a theme element

    This returns a string you should free with free() on success, NULL on
    failure. This will search for the given theme element, and if it is a
    full or relative path element or a simple search-able name. The returned
    path is the full path to the file, if searched, and the file exists, or it
    is simply the full path given in the element or a resolved path if
    relative to home. The C{in_search_path} boolean pointed to is set to
    EINA_TRUE if the file was a search-able file and is in the search path,
    and EINA_FALSE otherwise.

    @param f: The theme element name
    @type f: string
    @param in_search_path: Pointer to a boolean to indicate if item is in the search path or not
    @type in_search_path: bool

    @return: The full path to the file found.
    @rtype: string

    """
    cdef Eina_Bool path = in_search_path
    return _ctouni(elm_theme_list_item_path_get(_cfruni(f), &path))

def theme_full_flush():
    """theme_full_flush()

    This flushes all themes (default and specific ones).

    This will flush all themes in the current application context, by calling
    elm_theme_flush() on each of them.

    """
    elm_theme_full_flush()

def theme_name_available_list():
    """theme_name_available_list()

    Return a list of theme elements in the theme search path

    This lists all available theme files in the standard Elementary search path
    for theme elements, and returns them in alphabetical order as theme
    element names in a list of strings. Free this with
    elm_theme_name_available_list_free() when you are done with the list.

    @return: A list of strings that are the theme element names.
    @rtype: tuple of strings

    """
    cdef Eina_List *lst = elm_theme_name_available_list_new()
    elements = tuple(_strings_to_python(lst))
    elm_theme_name_available_list_free(lst)
    return elements

# for compatibility
def theme_overlay_add(item):
    elm_theme_overlay_add(NULL, item)

def theme_extension_add(item):
    elm_theme_extension_add(NULL, item)
