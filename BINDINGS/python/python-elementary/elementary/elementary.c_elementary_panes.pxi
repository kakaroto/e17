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

cdef public class Panes(LayoutClass) [object PyElementaryPanes, type PyElementaryPanes_Type]:

    """The panes widget adds a draggable bar between two contents. When
    dragged this bar will resize contents' size.

    Panes can be displayed vertically or horizontally, and contents size
    proportion can be customized (homogeneous by default).

    This widget emits the following signals, besides the ones sent from
    L{Layout}:
        - C{"press"} - The panes has been pressed (button wasn't released yet).
        - C{"unpressed"} - The panes was released after being pressed.
        - C{"clicked"} - The panes has been clicked.
        - C{"clicked,double"} - The panes has been double clicked.

    Available styles for it:
        - C{"default"}

    Default content parts of the panes widget that you can use are:
        - "left" - A leftside content of the panes
        - "right" - A rightside content of the panes

    If panes are displayed vertically, left content will be displayed on top.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_panes_add(parent.obj))

    property fixed:
        """Whether the left and right panes resize homogeneously or not.

        By default panes are resized homogeneously.

        @see: L{content_left_size}
        @see: L{content_right_size}

        @type: bool

        """
        def __set__(self, fixed):
            elm_panes_fixed_set(self.obj, fixed)
        def __get__(self):
            return bool(elm_panes_fixed_get(self.obj))

    property content_left_size:
        """The size proportion of panes widget's left side.

        By default it's homogeneous, i.e., both sides have the same size.

        If something different is required, it can be set with this property.
        For example, if the left content should be displayed over
        75% of the panes size, C{size} should be passed as C{0.75}.
        This way, right content will be resized to 25% of panes size.

        If displayed vertically, left content is displayed at top, and
        right content at bottom.

        @note: This proportion will change when user drags the panes bar.

        @type: float

        """
        def __get__(self):
            return elm_panes_content_left_size_get(self.obj)
        def __set__(self, size):
            elm_panes_content_left_size_set(self.obj, size)

    property content_right_size:
        """The size proportion of panes widget's right side.

        By default it's homogeneous, i.e., both sides have the same size.

        If something different is required, it can be set with this property.
        For example, if the right content should be displayed over
        75% of the panes size, C{size} should be passed as C{0.75.}
        This way, left content will be resized to 25% of panes size.

        If displayed vertically, left content is displayed at top, and
        right content at bottom.

        @note: This proportion will change when user drags the panes bar.

        @type: float

        """
        def __get__(self):
            return elm_panes_content_right_size_get(self.obj)
        def __set__(self, size):
            elm_panes_content_right_size_set(self.obj, size)

    property horizontal:
        """The orientation of a given panes widget.

        Use this property to change how your panes is to be disposed:
        vertically or horizontally.

        By default it's displayed horizontally.

        @type: bool

        """
        def __set__(self, horizontal):
            elm_panes_horizontal_set(self.obj, horizontal)
        def __get__(self):
            return bool(elm_panes_horizontal_get(self.obj))

    def callback_press_add(self, func, *args, **kwargs):
        """The panes has been pressed (button wasn't released yet)."""
        self._callback_add("press", func, *args, **kwargs)

    def callback_press_del(self, func):
        self._callback_del("press", func)

    def callback_unpress_add(self, func, *args, **kwargs):
        """The panes was released after being pressed."""
        self._callback_add("unpress", func, *args, **kwargs)

    def callback_unpress_del(self, func):
        self._callback_del("unpress", func)

    def callback_clicked_add(self, func, *args, **kwargs):
        """The panes has been clicked."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        """The panes has been double clicked."""
        self._callback_add("clicked,double", func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del("clicked,double", func)

_elm_widget_type_register("panes", Panes)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPanes_Type # hack to install metaclass
_install_metaclass(&PyElementaryPanes_Type, ElementaryObjectMeta)
