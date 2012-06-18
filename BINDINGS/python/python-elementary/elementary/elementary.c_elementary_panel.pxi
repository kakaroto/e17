# Copyright (c) 2010 Boris Faure
#               2010 Sebastian Krzyszkowiak
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

cdef public class Panel(Object) [object PyElementaryPanel, type PyElementaryPanel_Type]:

    """A panel is a type of animated container that contains subobjects.

    It can be expanded or contracted by clicking the button on it's edge.

    Orientations are as follows:
        - ELM_PANEL_ORIENT_TOP
        - ELM_PANEL_ORIENT_LEFT
        - ELM_PANEL_ORIENT_RIGHT

    Default content parts of the panel widget that you can use for are:
        - "default" - A content of the panel

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_panel_add(parent.obj))

    def orient_set(self, orient):
        """Sets the orientation of the panel

        Sets from where the panel will (dis)appear.

        @param orient: The panel orientation.
        @type orient: Elm_Panel_Orient

        """
        elm_panel_orient_set(self.obj, orient)

    def orient_get(self):
        """Get the orientation of the panel.

        @return: The Elm_Panel_Orient, or ELM_PANEL_ORIENT_LEFT on failure.
        @rtype: Elm_Panel_Orient

        """
        return elm_panel_orient_get(self.obj)

    property orient:
        """The orientation of the panel.

        Tells from where the panel will (dis)appear.

        This has value ELM_PANEL_ORIENT_LEFT on failure

        @type: Elm_Panel_Orient

        """
        def __set__(self, orient):
            elm_panel_orient_set(self.obj, orient)
        def __get__(self):
            return elm_panel_orient_get(self.obj)

    def hidden_set(self, hidden):
        """Set the hidden state of the panel.

        @param hidden: If True, the panel will run the animation to disappear.
        @type hidden: bool

        """
        elm_panel_orient_set(self.obj, hidden)

    def hidden_get(self):
        """Get the hidden state of the panel.

        @return: True if it is hidden state

        """
        return elm_panel_hidden_get(self.obj)

    property hidden:
        """The hidden state of the panel.

        @type: bool

        """
        def __set__(self, hidden):
            elm_panel_hidden_set(self.obj, hidden)
        def __get__(self):
            return elm_panel_hidden_get(self.obj)

    def toggle(self):
        """toggle()

        Toggle the hidden state of the panel from code.

        """
        elm_panel_toggle(self.obj)

_elm_widget_type_register("panel", Panel)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPanel_Type # hack to install metaclass
_install_metaclass(&PyElementaryPanel_Type, ElementaryObjectMeta)
