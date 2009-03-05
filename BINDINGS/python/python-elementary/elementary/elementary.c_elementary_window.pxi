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

cdef class Window(Object):
    """
    Elementary Wind ow class
    
    This class represents a basic window.
    """

    def __init__(self, name, type):
        """
        @parm: B{name} Name of the window
        @parm: B{type} Type of the Window 
        """
        self._set_obj(elm_win_add(NULL, name, type))
    
    property canvas:
        def __get__(self):
            """
            Returns the canvas used by the window

            @rtype Canvas
            """
            cdef c_evas.Evas *e
            e = c_evas.evas_object_evas_get(self.obj)
            return evas.c_evas._Canvas_from_instance(<long>e)

    property destroy:
        def __set__(self, value):
            """
            Set the callback-function for the destroy-event

            @parm: B{value} Function which would be called, if the event occur
            """
            self._callback_add("delete-request",value)
        
    def resize_object_add(self,c_evas.Object obj):
        """
        Sets an object which sould be resized, when the window changes his size

        @parm: B{obj} Widget object
        """
        elm_win_resize_object_add(self.obj, obj.obj)

    def resize_object_del(self,c_evas.Object obj):
        """
        Removes a object from the resizing list

        @parm: B{obj} Widget object
        """
        elm_win_resize_object_del(self.obj, obj.obj)
    
    def title_set(self,title):
        """
        Sets the title of the window

        @parm: B{title} Title for the window
        """
        elm_win_title_set(self.obj, title)

    def activate(self):
        """Activates the window"""
        elm_win_activate(self.obj)
    
    def autodel_set(self,autodel):
        """
        Set the auto deletion property of the window

        @parm: B{audodel} Auto deletion property
        """
        elm_win_autodel_set(self.obj, autodel)

    def borderless_set(self, borderless):
        """
        Set the borderless property of the window

        @parm: B{borderless} Borderless property
        """
        elm_win_borderless_set(self.obj, borderless)

    def shaped_set(self,shaped):
        """Set the shaped property of the window

        @parm: B{shaped} Shaped property
        """
        elm_win_shaped_set(self.obj, shaped)

    def alpha_set(self,alpha):
        """
        Set the alpha value of the window

        @parm: b{alpha} Alpha value
        """
        elm_win_alpha_set(self.obj, alpha)
        
    def override_set(self, override):
        """
        Set the override property

        @parm: B{override} Override property
        """
        elm_win_override_set(self.obj, override)

    def keyboard_mode_set(self, mode):
        """
        Set the keyboard mode of the window

        @parm: B{mode} Keyboard mode
        """
        elm_win_keyboard_mode_set(self.obj, mode)

    def keyboard_win_set(self, is_keyboard):
        """
        Display the keyboard

        @parm: B{is_keyboard} Keyboard is visible?
        """
        elm_win_keyboard_win_set(self.obj, is_keyboard)

    # TODO
    """
    def xwindow_get(self):
        cdef Ecore_X_Window xwin
        xwin = elm_win_xwindow_get(self.obj)
        

        return Null
    """


