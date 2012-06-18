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
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cdef class Configuration(object):

    """Elementary configuration is formed by a set options bounded to a
    given profile, like theme, "finger size", etc.

    These are functions with which one synchronizes changes made to those
    values to the configuration storing files, de facto. You most probably
    don't want to use the functions in this group unless you're writing an
    elementary configuration manager.

    """

    #{ Elementary Configuration
    def save(self):
        """Save back Elementary's configuration, so that it will persist on
        future sessions.

        This function will take effect -- thus, do I/O -- immediately. Use
        it when you want to save all configuration changes at once. The
        current configuration set will get saved onto the current profile
        configuration file.

        @return: C{True}, when successful. C{False}, otherwise.
        @rtype: bool

        """
        return bool(elm_config_save())

    def reload(self):
        """Reload Elementary's configuration, bounded to current selected
        profile.

        Useful when you want to force reloading of configuration values for
        a profile. If one removes user custom configuration directories,
        for example, it will force a reload with system values instead.

        @return: C{True}, when successful. C{False}, otherwise.
        @rtype: bool

        """
        elm_config_reload()

    def all_flush(self):
        """Flush all config settings then apply those settings to all
        applications using elementary on the current display."""
        elm_config_all_flush()

    #}

    #{ Elementary Profile
    """Profiles are pre-set options that affect the whole look-and-feel of
    Elementary-based applications. There are, for example, profiles
    aimed at desktop computer applications and others aimed at mobile,
    touchscreen-based ones. You most probably don't want to use the
    functions in this group unless you're writing an elementary
    configuration manager.

    """

    property profile:
        """Elementary's profile in use.

        The global profile that is applied to all Elementary applications.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_config_profile_get())
        def __set__(self, profile):
            elm_config_profile_set(_cfruni(profile))

    def profile_dir_get(self, profile, is_user):
        """Get an Elementary's profile directory path in the filesystem. One
        may want to fetch a system profile's dir or a user one (fetched
        inside $HOME).

        @param profile:  The profile's name
        @type profile: string
        @param is_user:  Whether to lookup for a user profile (C{True})
            or a system one (C{False})
        @type is_user: bool
        @return: The profile's directory path.
        @rtype: string

        """
        return _ctouni(elm_config_profile_dir_get(_cfruni(profile), is_user))

    property profile_list:
        """Get Elementary's list of available profiles.

        @type: tuple of strings

        """
        def __get__(self):
            cdef Eina_List *lst = elm_config_profile_list_get()
            return tuple(_strings_to_python(lst))

    #}

    #{ Elementary Scrolling
    """These set how scrollable views in Elementary widgets should behave on
    user interaction."""

    property scroll_bounce_enabled:
        """Whether scrollers should bounce when they reach their
        viewport's edge during a scroll.

        @type: bool

        """
        def __get__(self):
            return bool(elm_config_scroll_bounce_enabled_get())
        def __set__(self, enabled):
            elm_config_scroll_bounce_enabled_set(enabled)

    property scroll_bounce_friction:
        """The amount of inertia a scroller will impose at bounce animations.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_bounce_friction_get()
        def __set__(self, friction):
            elm_config_scroll_bounce_friction_set(friction)

    property scroll_page_scroll_friction:
        """The amount of inertia a B{paged} scroller will impose at page
        fitting animations.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_page_scroll_friction_get()
        def __set__(self, friction):
            elm_config_scroll_page_scroll_friction_set(friction)

    property scroll_bring_in_scroll_friction:
        """The amount of inertia a scroller will impose at region bring
        animations.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_bring_in_scroll_friction_get()
        def __set__(self, friction):
            elm_config_scroll_bring_in_scroll_friction_set(friction)

    property scroll_zoom_friction:
        """The amount of inertia scrollers will impose at animations
        triggered by Elementary widgets' zooming API.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_zoom_friction_get()
        def __set__(self, friction):
            elm_config_scroll_zoom_friction_set(friction)

    property scroll_thumbscroll_enabled:
        """Whether scrollers should be draggable from any point in their views.

        @note: This is the default behavior for touch screens, in general.
        @note: All other functions namespaced with "thumbscroll" will only
          have effect if this mode is enabled.

        @type: bool

        """
        def __get__(self):
            return bool(elm_config_scroll_thumbscroll_enabled_get())
        def __set__(self, enabled):
            elm_config_scroll_thumbscroll_enabled_set(enabled)

    property scroll_thumbscroll_threshold:
        """The number of pixels one should travel while dragging a
        scroller's view to actually trigger scrolling.

        One would use higher values for touch screens, in general, because
        of their inherent imprecision.

        @type: int

        """
        def __get__(self):
            return elm_config_scroll_thumbscroll_threshold_get()
        def __set__(self, threshold):
            elm_config_scroll_thumbscroll_threshold_set(threshold)

    property scroll_thumbscroll_momentum_threshold:
        """The minimum speed of mouse cursor movement which will trigger list
        self scrolling animation after a mouse up event (pixels/second).

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_thumbscroll_momentum_threshold_get()
        def __set__(self, threshold):
            elm_config_scroll_thumbscroll_momentum_threshold_set(threshold)

    property scroll_thumbscroll_friction:
        """The amount of inertia a scroller will impose at self scrolling
        animations.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_thumbscroll_friction_get()
        def __set__(self, friction):
            elm_config_scroll_thumbscroll_friction_set(friction)

    property scroll_thumbscroll_border_friction:
        """The amount of lag between your actual mouse cursor dragging
        movement and a scroller's view movement itself, while pushing it
        into bounce state manually.

        @note: parameter value will get bound to 0.0 - 1.0 interval, always

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_thumbscroll_border_friction_get()
        def __set__(self, friction):
            elm_config_scroll_thumbscroll_border_friction_set(friction)

    property scroll_thumbscroll_sensitivity_friction:
        """The sensitivity amount which is be multiplied by the length of
        mouse dragging.

        C{0}.1 for minimum sensitivity, C{1}.0 for maximum sensitivity. 0.25 is
        proper.

        @type: float

        """
        def __get__(self):
            return elm_config_scroll_thumbscroll_sensitivity_friction_get()
        def __set__(self, friction):
            elm_config_scroll_thumbscroll_sensitivity_friction_set(friction)

    #}

    property longpress_timeout:
        """The duration for occurring long press event.

        @type: float

        """
        def __get__(self):
            return elm_config_longpress_timeout_get()
        def __set__(self, longpress_timeout):
            elm_config_longpress_timeout_set(longpress_timeout)

    property tooltip_delay:
        """The duration after which tooltip will be shown.

        @type: float

        """
        def __get__(self):
            return elm_config_tooltip_delay_get()
        def __set__(self, delay):
            elm_config_tooltip_delay_set(delay)

    property cursor_engine_only:
        """The globally configured exclusive usage of engine cursors.

        If True only engine cursors will be enabled, if False will look for
        them on theme before.

        @type: bool

        """
        def __get__(self):
            return elm_config_cursor_engine_only_get()
        def __set__(self, engine_only):
            elm_config_cursor_engine_only_set(engine_only)

    property scale:
        """The global scaling factor

        This gets the globally configured scaling factor that is applied to
        all objects.

        @type: float

        """
        def __get__(self):
            return elm_config_scale_get()
        def __set__(self, scale):
            elm_config_scale_set(scale)

    #{ Password show last
    """Show last feature of password mode enables user to view the last input
    entered for few seconds before masking it. These functions allow to set
    this feature in password mode of entry widget and also allow to
    manipulate the duration for which the input has to be visible."""

    property password_show_last:
        """The "show last" setting of password mode.

        @type: bool

        """
        def __get__(self):
            return elm_config_password_show_last_get()
        def __set__(self, password_show_last):
            elm_config_password_show_last_set(password_show_last)

    property password_show_last_timeout:
        """The timeout value for which the last input entered in password
        mode will be visible.

        @type: float

        """
        def __get__(self):
            return elm_config_password_show_last_timeout_get()
        def __set__(self, password_show_last_timeout):
            elm_config_password_show_last_timeout_set(password_show_last_timeout)

    #}

    #{ Elementary Engine
    """These are functions setting and querying which rendering engine
    Elementary will use for drawing its windows' pixels.

    The following are the available engines:
        - "software_x11"
        - "fb"
        - "directfb"
        - "software_16_x11"
        - "software_8_x11"
        - "xrender_x11"
        - "opengl_x11"
        - "software_gdi"
        - "software_16_wince_gdi"
        - "sdl"
        - "software_16_sdl"
        - "opengl_sdl"
        - "buffer"
        - "ews"
        - "opengl_cocoa"
        - "psl1ght"

    """

    property engine:
        """Elementary's rendering engine in use.

        This gets the global rendering engine that is applied to all
        Elementary applications.

        Note that it will take effect only to Elementary windows created
        after this is set.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_config_engine_get())
        def __set__(self, engine):
            elm_config_engine_set(_cfruni(engine))

    property preferred_engine:
        """Get Elementary's preferred engine to use.

        This gets the global rendering engine that is applied to all
        Elementary applications and is PREFERRED by the application.

        Note that it will take effect only to Elementary windows created
        after this is called. This overrides the engine set by configuration
        at application startup. Note that it is a hint and may not be honored.

        @type: string

        """
        def __get__(self):
            return _ctouni(elm_config_preferred_engine_get())
        def __set__(self, engine):
            elm_config_preferred_engine_set(_cfruni(engine))

    property text_classes_list:
        """Get Elementary's list of supported text classes.

        @type: Elm_Text_Class

        """
        def __get__(self):
            cdef Eina_List *lst
            cdef Elm_Text_Class *data
            cdef const_char_ptr name, desc
            ret = []
            lst = elm_config_text_classes_list_get()
            while lst:
                data = <Elm_Text_Class *>lst.data
                if data != NULL:
                    name = data.name
                    desc = data.desc
                    ret.append((_ctouni(name), _ctouni(desc)))
                lst = lst.next
            return ret

    property font_overlay_list:
        """Get Elementary's list of font overlays, set with
        L{font_overlay_set()}.

        For each text class, one can set a B{font overlay} for it, overriding
        the default font properties for that class coming from the theme in
        use. There is no need to free this list.

        @type: Elm_Font_Overlay

        """
        def __get__(self):
            cdef const_Eina_List *lst
            cdef Elm_Font_Overlay *data
            cdef const_char_ptr text_class, font
            cdef Evas_Font_Size size
            ret = []
            lst = elm_config_font_overlay_list_get()
            while lst:
                data = <Elm_Font_Overlay *>lst.data
                if data != NULL:
                    text_class = data.text_class
                    font = data.font
                    size = data.size
                    ret.append((_ctouni(text_class), _ctouni(font), size))
                lst = lst.next
            return ret

    def font_overlay_set(self, text_class, font, size):
        """Set a font overlay for a given Elementary text class.

        C{font} has to be in the format returned by font_fontconfig_name_get().

        @see: L{font_overlay_list}
        @see: L{font_overlay_unset()}
        @see: C{edje_object_text_class_set()}

        @param text_class: Text class name
        @type text_class: string
        @param font:  Font name and style string
        @type font: string
        @param size:  Font size
        @type size: Evas_Font_Size

        """
        elm_config_font_overlay_set(_cfruni(text_class), _cfruni(font), size)

    def font_overlay_unset(self, text_class):
        """Unset a font overlay for a given Elementary text class.

        This will bring back text elements belonging to text class
        C{text_class} back to their default font settings.

        @param text_class: Text class name
        @type text_class: string

        """
        elm_config_font_overlay_unset(_cfruni(text_class))

    def font_overlay_apply(self):
        """Apply the changes made with L{font_overlay_set()} and
        L{font_overlay_unset()} on the current Elementary window.

        This applies all font overlays set to all objects in the UI.

        """
        elm_config_font_overlay_apply()

    property finger_size:
        """The configured "finger size"

        This gets the globally configured finger size, B{in pixels}

        @type: Evas_Coord (int)

        """
        def __get__(self):
            return elm_config_finger_size_get()
        def __set__(self, size):
            elm_config_finger_size_set(size)

    property cache_flush_interval:
        """The globally configured cache flush interval time, in ticks

        @see: C{cache_all_flush()}

        @note: The C{size} must be greater than 0. if not, the cache flush
            will be ignored.

        @type: int

        """
        def __get__(self):
            return elm_config_cache_flush_interval_get()
        def __set__(self, size):
            elm_config_cache_flush_interval_set(size)

    property cache_flush_enabled:
        """The configured cache flush enabled state

        This property reflects the globally configured cache flush state -
        if it is enabled or not. When cache flushing is enabled, elementary
        will regularly (see L{cache_flush_interval}) flush
        caches and dump data out of memory and allow usage to re-seed caches
        and data in memory where it can do so. An idle application will thus
        minimize its memory usage as data will be freed from memory and not
        be re-loaded as it is idle and not rendering or doing anything
        graphically right now.

        @see: C{cache_all_flush()}

        @type: bool

        """
        def __get__(self):
            return bool(elm_config_cache_flush_enabled_get())
        def __set__(self, enabled):
            elm_config_cache_flush_enabled_set(enabled)

    property cache_font_cache_size:
        """The globally configured font cache size, in bytes.

        @type: int

        """
        def __get__(self):
            return elm_config_cache_font_cache_size_get()
        def __set__(self, size):
            elm_config_cache_font_cache_size_set(size)

    property cache_image_cache_size:
        """The globally configured image cache size, in bytes

        @type: int

        """
        def __get__(self):
            return elm_config_cache_image_cache_size_get()
        def __set__(self, size):
            elm_config_cache_image_cache_size_set(size)


    property cache_edje_file_cache_size:
        """The globally configured edje file cache size, in number of files.

        @type: int

        """
        def __get__(self):
            return elm_config_cache_edje_file_cache_size_get()
        def __set__(self, size):
            elm_config_cache_edje_file_cache_size_set(size)

    property cache_edje_collection_cache_size:
        """The globally configured edje collections cache size, in number of
        collections.

        @type: int

        """
        def __get__(self):
            return elm_config_cache_edje_collection_cache_size_get()
        def __set__(self, size):
            elm_config_cache_edje_collection_cache_size_set(size)

    property focus_highlight_enabled:
        """Whether the highlight on focused objects is enabled or not

        Note that it will take effect only to Elementary windows created after
        this is set.

        @see: L{focus_highlight_enabled}

        @type: bool

        """
        def __get__(self):
            return bool(elm_config_focus_highlight_enabled_get())
        def __set__(self, enable):
            elm_config_focus_highlight_enabled_set(enable)

    property focus_highlight_animate:
        """Whether the focus highlight, if enabled, will animate its switch
        from one object to the next

        Note that it will take effect only to Elementary windows created after
        this is set.

        @see: L{Window}

        @type: bool

        """
        def __get__(self):
            return bool(elm_config_focus_highlight_animate_get())
        def __set__(self, animate):
            elm_config_focus_highlight_animate_set(animate)

    property mirrored:
        """Get the system mirrored mode. This determines the default
        mirrored mode of widgets.

        type: bool

        """
        def __get__(self):
            return bool(elm_config_mirrored_get())
        def __set__(self, mirrored):
            elm_config_mirrored_set(mirrored)

    #}

#For compatibility
def config_finger_size_get():
    return elm_config_finger_size_get()

def config_finger_size_set(size):
    elm_config_finger_size_set(size)

def config_tooltip_delay_get():
    return elm_config_tooltip_delay_get()

def config_tooltip_delay_set(delay):
    elm_config_tooltip_delay_set(delay)

def focus_highlight_enabled_get():
    return elm_config_focus_highlight_enabled_get()

def focus_highlight_enabled_set(enabled):
    elm_config_focus_highlight_enabled_set(enabled)

def focus_highlight_animate_get():
    return elm_config_focus_highlight_animate_get()

def focus_highlight_animate_set(animate):
    elm_config_focus_highlight_animate_set(animate)

def preferred_engine_get():
    return _ctouni(elm_config_preferred_engine_get())

def preferred_engine_set(engine):
    elm_config_preferred_engine_set(engine)

def engine_get():
    return _ctouni(elm_config_engine_get())

def engine_set(engine):
    elm_config_engine_set(_cfruni(engine))

def scale_get():
    return elm_config_scale_get()

def scale_set(scale):
    elm_config_scale_set(scale)

def cursor_engine_only_get():
    return elm_config_cursor_engine_only_get()

def cursor_engine_only_set(engine_only):
    elm_config_cursor_engine_only_set(engine_only)

