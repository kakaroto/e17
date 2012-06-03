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

cdef public class Window(Object) [object PyElementaryWindow, type PyElementaryWindow_Type]:

    """Elementary Window class.

    This class represents a basic window.

    """

    def __init__(self, name, type):
        """
        @param name: Name of the window
        @param type: Type of the Window
        """
        self._set_obj(elm_win_add(NULL, name, type))

        cdef c_evas.Evas *e
        e = c_evas.evas_object_evas_get(self.obj)
        canvas = evas.c_evas._Canvas_from_instance(<long>e)
        c_evas.Object.__init__(self, canvas)

    def resize_object_add(self, c_evas.Object subobj):
        """Add subobj as a resize object of window obj.

        @param subobj: Widget object
        """
        elm_win_resize_object_add(self.obj, subobj.obj)

    def resize_object_del(self, c_evas.Object subobj):
        """Delete subobj as a resize object of window obj.

        @param subobj: Widget object
        """
        elm_win_resize_object_del(self.obj, subobj.obj)

    def title_set(self, title):
        """Set the title of the window.

        @param title: Title for the window
        """
        elm_win_title_set(self.obj, title)

    def title_get(self):
        """Get the title of the window."""
        return elm_win_title_get(self.obj)

    property title:
        def __get__(self):
            return self.title_get()
        def __set__(self, title):
            self.title_set(title)

    def icon_name_set(self, icon_name):
        """Set the icon name of the window.

        @param icon_name: Icon name for the window
        """
        elm_win_icon_name_set(self.obj, icon_name)

    def icon_name_get(self):
        """Get the icon name of the window."""
        return elm_win_icon_name_get(self.obj)

    property icon_name:
        def __get__(self):
            return self.icon_name_get()
        def __set__(self, icon_name):
            self.icon_name_set(icon_name)

    def role_set(self, role):
        """Set the role of the window.

        @param role: Role for the window
        """
        elm_win_role_set(self.obj, role)

    def role_get(self):
        """Get the role of the window."""
        return elm_win_role_get(self.obj)

    property role:
        def __get__(self):
            return self.role_get()
        def __set__(self, role):
            self.role_set(role)

    def icon_object_set(self, c_evas.Object icon):
        """Set the object to represent the window icon.

        @param icon: Icon object for the window
        """
        elm_win_icon_object_set(self.obj, icon.obj)

    def icon_object_get(self):
        """Get the icon object used for the window."""
        cdef c_evas.const_Evas_Object *obj = elm_win_icon_object_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property icon_object:
        def __get__(self):
            return self.icon_object_get()
        def __set__(self, icon):
            self.icon_object_set(icon)

    def autodel_set(self, autodel):
        """Set the window's autodel state.

        @param audodel: Auto deletion property
        """
        elm_win_autodel_set(self.obj, autodel)

    def autodel_get(self):
        """Get the window's autodel state."""
        return elm_win_autodel_get(self.obj)

    property autodel:
        def __get__(self):
            return self.autodel_get()
        def __set__(self, autodel):
            self.autodel_set(autodel)

    def activate(self):
        """Activates the window."""
        elm_win_activate(self.obj)

    def lower(self):
        """Lowers the window."""
        elm_win_lower(self.obj)

    def _raise(self):
        """Raises the window."""
        elm_win_raise(self.obj)

    def center(self, h, v):
        """Centers the window."""
        elm_win_center(self.obj, h, v)

    def borderless_set(self, borderless):
        """Set the borderless state of the window.

        @param borderless: Borderless state
        """
        elm_win_borderless_set(self.obj, borderless)

    def borderless_get(self):
        """Get the borderless state of the window."""
        return bool(elm_win_borderless_get(self.obj))

    property borderless:
        def __get__(self):
            return self.borderless_get()
        def __set__(self, borderless):
            self.borderless_set(borderless)

    def shaped_set(self,shaped):
        """Set the shaped state of the window.

        @param shaped: Shaped property
        """
        elm_win_shaped_set(self.obj, shaped)

    def shaped_get(self):
        """Get the shaped state of the window."""
        return bool(elm_win_shaped_get(self.obj))

    property shaped:
        def __get__(self):
            return self.shaped_get()
        def __set__(self, shaped):
            self.shaped_set(shaped)

    def alpha_set(self,alpha):
        """Set the alpha channel state of the window.

        @param alpha: Alpha channel state
        """
        elm_win_alpha_set(self.obj, alpha)

    def alpha_get(self):
        """Get the alpha channel state of the window."""
        return bool(elm_win_alpha_get(self.obj))

    property alpha:
        def __get__(self):
            return self.alpha_get()
        def __set__(self, alpha):
            self.alpha_set(alpha)

    def override_set(self, override):
        """Set the override property

        @param override: Override property
        """
        elm_win_override_set(self.obj, override)

    def override_get(self):
        """Get the override property"""
        return bool(elm_win_override_get(self.obj))

    property override:
        def __get__(self):
            return self.override_get()
        def __set__(self, override):
            self.override_set(override)

    def fullscreen_set(self, fullscreen):
        """Set the window to fullscreen mode

        @param fullscreen: Fullscreen mode
        """
        elm_win_fullscreen_set(self.obj, fullscreen)

    def fullscreen_get(self):
        """Get the window fullscreen mode.

        True means the window is in fullscreen mode while False means in
        windowed mode.
        """
        return bool(elm_win_fullscreen_get(self.obj))

    property fullscreen:
        def __get__(self):
            return self.fullscreen_get()
        def __set__(self, fullscreen):
            self.fullscreen_set(fullscreen)

    def maximized_set(self, maximized):
        """Set the maximized state of a window.

        @param maximized: Maximized state for the window
        """
        elm_win_maximized_set(self.obj, maximized)

    def maximized_get(self):
        """Get the maximized state of a window."""
        return bool(elm_win_maximized_get(self.obj))

    property maximized:
        def __get__(self):
            return self.maximized_get()
        def __set__(self, maximized):
            self.maximized_set(maximized)

    def iconified_set(self, iconified):
        """Set the iconified state of the window.

        @param iconified: iconified state
        """
        elm_win_iconified_set(self.obj, iconified)

    def iconified_get(self):
        """Get the Iconify state of the window."""
        return bool(elm_win_iconified_get(self.obj))

    property iconified:
        def __get__(self):
            return self.iconified_get()
        def __set__(self, iconified):
            self.iconified_set(iconified)

    def withdrawn_set(self, withdrawn):
        """Set the withdrawn state of the window.

        @param withdrawn: withdrawn state
        """
        elm_win_withdrawn_set(self.obj, withdrawn)

    def withdrawn_get(self):
        """Get the withdrawn state of the window."""
        return bool(elm_win_withdrawn_get(self.obj))

    property withdrawn:
        def __get__(self):
            return self.withdrawn_get()
        def __set__(self, withdrawn):
            self.withdrawn_set(withdrawn)

    def urgent_set(self, urgent):
        """Set the urgent state of the window.

        @param urgent: urgent state
        """
        elm_win_urgent_set(self.obj, urgent)

    def urgent_get(self):
        """Get the urgent state of the window."""
        return bool(elm_win_urgent_get(self.obj))

    property urgent:
        def __get__(self):
            return self.urgent_get()
        def __set__(self, urgent):
            self.urgent_set(urgent)

    def demand_attention_set(self, demand_attention):
        """Set the demand attention state of the window.

        @param demand_attention: demand attention state
        """
        elm_win_demand_attention_set(self.obj, demand_attention)

    def demand_attention_get(self):
        """Get the demand attention state of the window."""
        return bool(elm_win_demand_attention_get(self.obj))

    property demand_attention:
        def __get__(self):
            return self.demand_attention_get()
        def __set__(self, demand_attention):
            self.demand_attention_set(demand_attention)

    def modal_set(self, modal):
        """Set the Modal state of the window.

        @param modal: Modal state
        """
        elm_win_modal_set(self.obj, modal)

    def modal_get(self):
        """Get the Modal state of the window."""
        return bool(elm_win_modal_get(self.obj))

    property modal:
        def __get__(self):
            return self.modal_get()
        def __set__(self, modal):
            self.modal_set(modal)

    def aspect_set(self, aspect):
        """Set the aspect ratio of a window.

        @param aspect: If 0, the window has no aspect limits, otherwise it is width divided by height
        """
        elm_win_aspect_set(self.obj, aspect)

    def aspect_get(self):
        """Get the aspect ratio of a window."""
        return elm_win_aspect_get(self.obj)

    property aspect:
        def __get__(self):
            return self.aspect_get()
        def __set__(self, aspect):
            self.aspect_set(aspect)

    def layer_set(self, layer):
        """Set the layer of the window.

        @param layer: Layer
        """
        elm_win_layer_set(self.obj, layer)

    def layer_get(self):
        """Get the layer of the window."""
        return elm_win_layer_get(self.obj)

    property layer:
        def __get__(self):
            return self.layer_get()
        def __set__(self, layer):
            self.layer_set(layer)

    def rotation_set(self, rotation):
        """Set the rotation of the window.

        @param rotation: Rotation
        """
        elm_win_rotation_set(self.obj, rotation)

    def rotation_get(self):
        """Get the rotation of the window."""
        return elm_win_rotation_get(self.obj)

    property rotation:
        def __get__(self):
            return self.rotation_get()
        def __set__(self, rotation):
            self.rotation_set(rotation)

    def rotation_with_resize_set(self, rotation):
        """Rotates the window and resizes it.

        @param rotation: The rotation of the window in degrees (0-360), counter-clockwise.
        """
        elm_win_rotation_set(self.obj, rotation)

    def sticky_set(self, sticky):
        """Set the Sticky state of the window.

        @param sticky: Sticky state
        """
        elm_win_sticky_set(self.obj, sticky)

    def sticky_get(self):
        """Get the Sticky state of the window."""
        return bool(elm_win_sticky_get(self.obj))

    property sticky:
        def __get__(self):
            return self.sticky_get()
        def __set__(self, sticky):
            self.sticky_set(sticky)

    def conformant_set(self, conformant):
        """Set if this window is an illume conformant window.

        @param conformant: Conformant state
        """
        elm_win_conformant_set(self.obj, conformant)

    def conformant_get(self):
        """Get if this window is an illume conformant window."""
        return bool(elm_win_conformant_get(self.obj))

    property conformant:
        def __get__(self):
            return self.conformant_get()
        def __set__(self, conformant):
            self.conformant_set(conformant)

    def quickpanel_set(self, quickpanel):
        """Set a window to be an illume quickpanel window.

        @param quickpanel:
        """
        elm_win_quickpanel_set(self.obj, quickpanel)

    def quickpanel_get(self):
        """Get if this window is a quickpanel or not."""
        return bool(elm_win_quickpanel_get(self.obj))

    property quickpanel:
        def __get__(self):
            return self.quickpanel_get()
        def __set__(self, quickpanel):
            self.quickpanel_set(quickpanel)

    def quickpanel_priority_major_set(self, priority):
        """Set the major priority of a quickpanel window.

        @param priority:
        """
        elm_win_quickpanel_priority_major_set(self.obj, priority)

    def quickpanel_priority_major_get(self):
        """Get the major priority of a quickpanel window."""
        return elm_win_quickpanel_priority_major_get(self.obj)

    property quickpanel_priority_major:
        def __get__(self):
            return self.quickpanel_priority_major_get()
        def __set__(self, priority):
            self.quickpanel_priority_major_set(priority)

    def quickpanel_priority_minor_set(self, priority):
        """Set the major priority of a quickpanel window.

        @param quickpanel_priority_minor:
        """
        elm_win_quickpanel_priority_minor_set(self.obj, priority)

    def quickpanel_priority_minor_get(self):
        """Get the major priority of a quickpanel window."""
        return elm_win_quickpanel_priority_minor_get(self.obj)

    property quickpanel_priority_minor:
        def __get__(self):
            return self.quickpanel_priority_minor_get()
        def __set__(self, priority):
            self.quickpanel_priority_minor_set(priority)

    def quickpanel_zone_set(self, zone):
        """Set which zone this quickpanel should appear in.

        @param quickpanel_zone:
        """
        elm_win_quickpanel_zone_set(self.obj, zone)

    def quickpanel_zone_get(self):
        """Get which zone this quickpanel should appear in."""
        return elm_win_quickpanel_zone_get(self.obj)

    property quickpanel_zone:
        def __get__(self):
            return self.quickpanel_zone_get()
        def __set__(self, zone):
            self.quickpanel_zone_set(zone)

    def prop_focus_skip_set(self, skip):
        """Set the window to be skipped by keyboard focus

        @param skip: The skip flag state, a boolean.
        """
        elm_win_prop_focus_skip_set(self.obj, skip)

    def illume_command_send(self, command, *params):
        """Send a command to the windowing environment

        @param command: The command to send
        @param params: Optional parameters for the command.
        """
        elm_win_illume_command_send(self.obj, command, params)

    def inlined_image_object_get(self):
        """Get the inlined image object"""
        cdef Object o
        o = <Object>elm_win_inlined_image_object_get(self.obj)
        return o

    def focus_get(self):
        """Determine whether a window has focus"""
        return bool(elm_win_focus_get(self.obj))

    def screen_constrain_set(self, constrain):
        """Constrain the maximum width and height of a window to the width and height of its screen.

        @param constrain:
        """
        elm_win_screen_constrain_set(self.obj, constrain)

    def screen_constrain_get(self):
        """Retrieve the constraints on the maximum width and height of a window relative to the width and height of its screen."""
        return bool(elm_win_screen_constrain_get(self.obj))

    property screen_constrain:
        def __get__(self):
            return self.screen_constrain_get()
        def __set__(self, constrain):
            self.screen_constrain_set(constrain)

    def screen_size_get(self):
        cdef int x, y, w, h
        elm_win_screen_size_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def focus_highlight_enabled_set(self, enabled):
        """Enable focus highlight."""
        elm_win_focus_highlight_enabled_set(self.obj, enabled)

    def focus_highlight_enabled_get(self):
        """Get if focus highlight is enabled."""
        return bool(elm_win_focus_highlight_enabled_get(self.obj))

    property focus_highlight_enabled:
        def __get__(self):
            return self.focus_highlight_enabled_get()
        def __set__(self, enabled):
            self.focus_highlight_enabled_set(enabled)

    def focus_highlight_style_set(self, style):
        """Set the style for the focus highlight on this window."""
        elm_win_focus_highlight_style_set(self.obj, style)

    def focus_highlight_style_get(self):
        """Get the style set for the focus highlight object."""
        return elm_win_focus_highlight_style_get(self.obj)

    property focus_highlight_style:
        def __get__(self):
            return self.focus_highlight_style_get()
        def __set__(self, style):
            self.focus_highlight_style_set(style)

    def keyboard_mode_set(self, mode):
        """Sets the keyboard mode of the window.

        @param mode: The mode to set, one of Elm_Win_Keyboard_Mode
        """
        elm_win_keyboard_mode_set(self.obj, mode)

    def keyboard_mode_get(self):
        """Gets the keyboard mode of the window."""
        return elm_win_keyboard_mode_get(self.obj)

    property keyboard_mode:
        def __get__(self):
            return self.keyboard_mode_get()
        def __set__(self, mode):
            self.keyboard_mode_set(mode)

    def keyboard_win_set(self, is_keyboard):
        """Sets whether the window is a keyboard.

        @param is_keyboard: If true, the window is a virtual keyboard
        """
        elm_win_keyboard_win_set(self.obj, is_keyboard)

    def keyboard_win_get(self):
        """Gets whether the window is a keyboard."""
        return bool(elm_win_keyboard_win_get(self.obj))

    property keyboard_win:
        def __get__(self):
            return self.keyboard_win_get()
        def __set__(self, is_keyboard):
            self.keyboard_win_set(is_keyboard)

    def indicator_mode_set(self, mode):
        """Sets the indicator mode of the window."""
        elm_win_indicator_mode_set(self.obj, mode)

    def indicator_mode_get(self):
        """Gets the indicator mode of the window."""
        return elm_win_indicator_mode_get(self.obj)

    property indicator_mode:
        def __get__(self):
            return self.indicator_mode_get()
        def __set__(self, mode):
            self.indicator_mode_set(mode)

    def indicator_opacity_set(self, mode):
        """Sets the indicator opacity mode of the window."""
        elm_win_indicator_opacity_set(self.obj, mode)

    def indicator_opacity_get(self):
        """Gets the indicator opacity mode of the window."""
        return elm_win_indicator_opacity_get(self.obj)

    property indicator_opacity:
        def __get__(self):
            return self.indicator_opacity_get()
        def __set__(self, mode):
            self.indicator_opacity_set(mode)

    def screen_position_get(self):
        """Get the screen position of a window."""
        cdef int x, y
        elm_win_screen_position_get(self.obj, &x, &y)
        return (x, y)

    def socket_listen(self, svcname, svcnum, svcsys):
        """Create a socket to provide the service for Plug widget

        @param svcname: The name of the service to be advertised. ensure that it is unique.
        @param svcnum: A number (any value, 0 being the common default) to differentiate multiple instances of services with the same name.
        @param svcsys: A boolean that if true, specifies to create a system-wide service all users can connect to, otherwise the service is private to the user id that created the service.
        """
        return bool(elm_win_socket_listen(self.obj, svcname, svcnum, svcsys))

    def xwindow_xid_get(self):
        """Returns the X Window id.

        X Window id is a value of type long int which can be used in combination
        with some functions/objects in the ecore.x module.

        For example you can hide the mouse cursor with:
        import ecore.x
        xid = your_elm_win.xwindow_xid_get()
        xwin = ecore.x.Window_from_xid(xid)
        xwin.cursor_hide()

        Note: This is not portable at all. Works only under the X window system.

        """
        cdef Ecore_X_Window xwin
        xwin = elm_win_xwindow_get(self.obj)
        return xwin

    def callback_delete_request_add(self, func, *args, **kwargs):
        self._callback_add("delete,request", func, *args, **kwargs)

    def callback_delete_request_del(self, func):
        self._callback_del("delete,request", func)

    def callback_focus_in_add(self, func, *args, **kwargs):
        self._callback_add("focus,in", func, *args, **kwargs)

    def callback_focus_in_del(self, func):
        self._callback_del("focus,in", func)

    def callback_focus_out_add(self, func, *args, **kwargs):
        self._callback_add("focus,out", func, *args, **kwargs)

    def callback_focus_out_del(self, func):
        self._callback_del("focus,out")

    def callback_moved_add(self, func, *args, **kwargs):
        self._callback_add("moved", func, *args, **kwargs)

    def callback_moved_del(self, func):
        self._callback_del("moved")

    def callback_withdrawn_add(self, func, *args, **kwargs):
        self._callback_add("withdrawn", func, *args, **kwargs)

    def callback_withdrawn_del(self, func):
        self._callback_del("withdrawn")

    def callback_iconified_add(self, func, *args, **kwargs):
        self._callback_add("iconified", func, *args, **kwargs)

    def callback_iconified_del(self, func):
        self._callback_del("iconified")

    def callback_normal_add(self, func, *args, **kwargs):
        self._callback_add("normal", func, *args, **kwargs)

    def callback_normal_del(self, func):
        self._callback_del("normal")

    def callback_stick_add(self, func, *args, **kwargs):
        self._callback_add("stick", func, *args, **kwargs)

    def callback_stick_del(self, func):
        self._callback_del("stick")

    def callback_unstick_add(self, func, *args, **kwargs):
        self._callback_add("unstick", func, *args, **kwargs)

    def callback_unstick_del(self, func):
        self._callback_del("unstick")

    def callback_fullscreen_add(self, func, *args, **kwargs):
        self._callback_add("fullscreen", func, *args, **kwargs)

    def callback_fullscreen_del(self, func):
        self._callback_del("fullscreen")

    def callback_unfullscreen_add(self, func, *args, **kwargs):
        self._callback_add("unfullscreen", func, *args, **kwargs)

    def callback_unfullscreen_del(self, func):
        self._callback_del("unfullscreen")

    def callback_maximized_add(self, func, *args, **kwargs):
        self._callback_add("maximized", func, *args, **kwargs)

    def callback_maximized_del(self, func):
        self._callback_del("maximized")

    def callback_unmaximized_add(self, func, *args, **kwargs):
        self._callback_add("unmaximized", func, *args, **kwargs)

    def callback_unmaximized_del(self, func):
        self._callback_del("unmaximized")

_elm_widget_type_register("win", Window)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryWindow_Type # hack to install metaclass
_install_metaclass(&PyElementaryWindow_Type, ElementaryObjectMeta)
