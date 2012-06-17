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

cdef public class Bubble(LayoutClass) [object PyElementaryBubble, type PyElementaryBubble_Type]:

    """The Bubble is a widget to show text similar to how speech is
    represented in comics.

    The bubble widget contains 5 important visual elements:
        - The frame is a rectangle with rounded edjes and an "arrow".
        - The C{icon} is an image to which the frame's arrow points to.
        - The C{label} is a text which appears to the right of the icon if the
            corner is B{top_left} or B{bottom_left} and is right aligned to
            the frame otherwise.
        - The C{info} is a text which appears to the right of the label. Info's
            font is of a lighter color than label.
        - The C{content} is an evas object that is shown inside the frame.

    The position of the arrow, icon, label and info depends on which corner is
    selected. The four available corners are:
        - B{top_left} - Default
        - B{top_right}
        - B{bottom_left}
        - B{bottom_right}

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{clicked} - This is called when a user has clicked the bubble.

    Default content parts of the bubble that you can use for are:
        - B{default} - A content of the bubble
        - B{icon} - An icon of the bubble

    Default text parts of the button widget that you can use for are:
        - B{default} - Label of the bubble
        - B{info} - info of the bubble

    """

    def __init__(self, evasObject parent):
        """Add a new bubble to the parent

        This function adds a text bubble to the given parent evas object.

        @param parent: The parent object
        @type parent: L{Object}
        @return: The new object or None if it cannot be created
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_bubble_add(parent.obj))

    def pos_set(self, pos):
        """Set the corner of the bubble

        This function sets the corner of the bubble. The corner will be used
        to determine where the arrow in the frame points to and where label,
        icon and info are shown.

        @param pos: The given corner for the bubble.
        @type pos: Elm_Bubble_Pos

        """
        elm_bubble_pos_set(self.obj, pos)

    def pos_get(self):
        """Get the corner of the bubble

        This function gets the selected corner of the bubble.

        @return: The given corner for the bubble.
        @rtype: Elm_Bubble_Pos

        """
        return elm_bubble_pos_get(self.obj)

    property pos:
        """The corner of the bubble

        This property reflects the corner of the bubble. The corner will be
        used to determine where the arrow in the frame points to and where
        label, icon and info are shown.

        @type: Elm_Bubble_Pos

        """
        def __get__(self):
            return elm_bubble_pos_get(self.obj)

        def __set__(self, value):
            elm_bubble_pos_set(self.obj, value)

    def callback_clicked_add(self, func, *args, **kwargs):
        """This is called when a user has clicked the bubble."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

_elm_widget_type_register("bubble", Bubble)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryBubble_Type # hack to install metaclass
_install_metaclass(&PyElementaryBubble_Type, ElementaryObjectMeta)
