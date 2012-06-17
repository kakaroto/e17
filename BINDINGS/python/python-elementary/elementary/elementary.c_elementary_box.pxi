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

ctypedef enum Elm_Box_CLayout:
    ELM_BOX_LAYOUT_HORIZONTAL
    ELM_BOX_LAYOUT_VERTICAL
    ELM_BOX_LAYOUT_HOMOGENEOUS_VERTICAL
    ELM_BOX_LAYOUT_HOMOGENEOUS_HORIZONTAL
    ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_HORIZONTAL
    ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_VERTICAL
    ELM_BOX_LAYOUT_FLOW_HORIZONTAL
    ELM_BOX_LAYOUT_FLOW_VERTICAL
    ELM_BOX_LAYOUT_STACK

cdef Evas_Object_Box_Layout _py_elm_box_layout_resolv(int layout) with gil:
    if layout == ELM_BOX_LAYOUT_HORIZONTAL:
        return evas_object_box_layout_horizontal
    elif layout == ELM_BOX_LAYOUT_VERTICAL:
        return evas_object_box_layout_vertical
    elif layout == ELM_BOX_LAYOUT_HOMOGENEOUS_VERTICAL:
        return evas_object_box_layout_homogeneous_vertical
    elif layout == ELM_BOX_LAYOUT_HOMOGENEOUS_HORIZONTAL:
        return evas_object_box_layout_homogeneous_horizontal
    elif layout == ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_HORIZONTAL:
        return evas_object_box_layout_homogeneous_max_size_horizontal
    elif layout == ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_VERTICAL:
        return evas_object_box_layout_homogeneous_max_size_vertical
    elif layout == ELM_BOX_LAYOUT_FLOW_HORIZONTAL:
        return evas_object_box_layout_flow_horizontal
    elif layout == ELM_BOX_LAYOUT_FLOW_VERTICAL:
        return evas_object_box_layout_flow_vertical
    elif layout == ELM_BOX_LAYOUT_STACK:
        return evas_object_box_layout_stack
    return evas_object_box_layout_vertical


cdef public class Box(Object) [object PyElementaryBox, type PyElementaryBox_Type]:

    """A box arranges objects in a linear fashion, governed by a layout function
    that defines the details of this arrangement.

    By default, the box will use an internal function to set the layout to
    a single row, either vertical or horizontal. This layout is affected
    by a number of parameters, such as the homogeneous flag set by
    L{homogeneous_set()}, the values given by L{padding_set()} and
    L{align_set()} and the hints set to each object in the box.

    For this default layout, it's possible to change the orientation with
    L{horizontal_set()}. The box will start in the vertical orientation,
    placing its elements ordered from top to bottom. When horizontal is set,
    the order will go from left to right. If the box is set to be
    homogeneous, every object in it will be assigned the same space, that
    of the largest object. Padding can be used to set some spacing between
    the cell given to each object. The alignment of the box, set with
    L{align_set()}, determines how the bounding box of all the elements
    will be placed within the space given to the box widget itself.

    The size hints of each object also affect how they are placed and sized
    within the box. L{size_hint_min_set()} will give the minimum
    size the object can have, and the box will use it as the basis for all
    latter calculations. Elementary widgets set their own minimum size as
    needed, so there's rarely any need to use it manually.

    C{size_hint_weight_set()}, when not in homogeneous mode, is
    used to tell whether the object will be allocated the minimum size it
    needs or if the space given to it should be expanded. It's important
    to realize that expanding the size given to the object is not the same
    thing as resizing the object. It could very well end being a small
    widget floating in a much larger empty space. If not set, the weight
    for objects will normally be 0.0 for both axis, meaning the widget will
    not be expanded. To take as much space possible, set the weight to
    C{EVAS_HINT_EXPAND} (defined to 1.0) for the desired axis to expand.

    Besides how much space each object is allocated, it's possible to control
    how the widget will be placed within that space using
    L{size_hint_align_set()}. By default, this value will be 0.5
    for both axis, meaning the object will be centered, but any value from
    0.0 (left or top, for the C{x} and C{y} axis, respectively) to 1.0
    (right or bottom) can be used. The special value C{EVAS_HINT_FILL}, which
    is -1.0, means the object will be resized to fill the entire space it
    was allocated.

    In addition, customized functions to define the layout can be set, which
    allow the application developer to organize the objects within the box
    in any number of ways.

    The special L{layout_transition()} function can be used
    to switch from one layout to another, animating the motion of the
    children of the box.

    @note: Objects should not be added to box objects using _add() calls.

    """

    def __init__(self, evasObject parent):
        """Add a new box to the parent

        By default, the box will be in vertical mode and non-homogeneous.

        @param parent: The parent object
        @type parent: L{Object}
        @return: The new object or None if it cannot be created
        @rtype: L{Object}

        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_box_add(parent.obj))

    def horizontal_set(self, horizontal):
        """Set the horizontal orientation.

        By default, box object arranges their contents vertically from top to
        bottom.
        By calling this function with C{horizontal} as C{True}, the box will
        become horizontal, arranging contents from left to right.

        @note: This flag is ignored if a custom layout function is set.

        @param horizontal: The horizontal flag (C{True} = horizontal,
            C{False} = vertical)
        @type horizontal: bool

        """
        elm_box_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        """Get the horizontal orientation.

        @return: C{True} if the box is set to horizontal mode, C{False} otherwise.
        @rtype: bool

        """
        return elm_box_horizontal_get(self.obj)

    property horizontal:
        """The horizontal orientation.

        By default, box object arranges their contents vertically from top to
        bottom. By setting this property as C{True}, the box will become
        horizontal, arranging contents from left to right.

        @note: This flag is ignored if a custom layout function is set.

        @type: bool

        """
        def __get__(self):
            return elm_box_horizontal_get(self.obj)
        def __set__(self, value):
            elm_box_horizontal_set(self.obj, value)

    def homogeneous_set(self, homogeneous):
        """Set the box to arrange its children homogeneously

        If enabled, homogeneous layout makes all items the same size, according
        to the size of the largest of its children.

        @note: This flag is ignored if a custom layout function is set.

        @param homogeneous: The homogeneous flag
        @type homogeneous: bool

        """
        elm_box_homogeneous_set(self.obj, homogeneous)

    def homogeneous_get(self):
        """Get whether the box is using homogeneous mode or not

        @return: C{True} if it's homogeneous, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_box_homogeneous_get(self.obj))

    property homogeneous:
        """Whether the box is using homogeneous mode or not

        If enabled, homogeneous layout makes all items the same size, according
        to the size of the largest of its children.

        @note: This flag is ignored if a custom layout function is set.

        @type: bool

        """
        def __get__(self):
            return bool(elm_box_homogeneous_get(self.obj))

        def __set__(self, value):
            elm_box_homogeneous_set(self.obj, value)

    def pack_start(self, evasObject obj):
        """Add an object to the beginning of the pack list.

        Pack C{subobj} into the box, placing it first in the list of
        children objects. The actual position the object will get on screen
        depends on the layout used. If no custom layout is set, it will be at
        the top or left, depending if the box is vertical or horizontal,
        respectively.

        @param subobj: The object to add to the box
        @type subobj: L{Object}

        @see: L{pack_end()}
        @see: L{pack_before()}
        @see: L{pack_after()}
        @see: L{unpack()}
        @see: L{unpack_all()}
        @see: L{clear()}

        """
        elm_box_pack_start(self.obj, obj.obj)

    def pack_end(self, evasObject obj):
        """Add an object at the end of the pack list.

        Pack C{subobj} into the box, placing it last in the list of
        children objects. The actual position the object will get on screen
        depends on the layout used. If no custom layout is set, it will be at
        the bottom or right, depending if the box is vertical or horizontal,
        respectively.

        @param subobj: The object to add to the box
        @type subobj: L{Object}

        @see: L{pack_start()}
        @see: L{pack_before()}
        @see: L{pack_after()}
        @see: L{unpack()}
        @see: L{unpack_all()}
        @see: L{clear()}

        """
        elm_box_pack_end(self.obj, obj.obj)

    def pack_before(self, evasObject obj, evasObject before):
        """Adds an object to the box before the indicated object.

        This will add the C{subobj} to the box indicated before the object
        indicated with C{before}. If C{before} is not already in the box, results
        are undefined. Before means either to the left of the indicated object or
        above it depending on orientation.

        @param subobj: The object to add to the box
        @type subobj: L{Object}
        @param before: The object before which to add it
        @type before: L{Object}

        @see: L{pack_start()}
        @see: L{pack_end()}
        @see: L{pack_after()}
        @see: L{unpack()}
        @see: L{unpack_all()}
        @see: L{clear()}

        """
        elm_box_pack_before(self.obj, obj.obj, before.obj)

    def pack_after(self, evasObject obj, evasObject after):
        """Adds an object to the box after the indicated object.

        This will add the C{subobj} to the box indicated after the object
        indicated with C{after}. If C{after} is not already in the box, results
        are undefined. After means either to the right of the indicated object or
        below it depending on orientation.

        @param subobj: The object to add to the box
        @type subobj: L{Object}
        @param after: The object after which to add it
        @type after: L{Object}

        @see: L{pack_start()}
        @see: L{pack_end()}
        @see: L{pack_before()}
        @see: L{unpack()}
        @see: L{unpack_all()}
        @see: L{clear()}

        """
        elm_box_pack_after(self.obj, obj.obj, after.obj)

    def clear(self):
        """Clear the box of all children

        Remove all the elements contained by the box, deleting the respective
        objects.

        @see: L{unpack()}
        @see: L{unpack_all()}

        """
        elm_box_clear(self.obj)

    def unpack(self, evasObject obj):
        """Unpack a box item.

        Remove the object given by C{subobj} from the box without
        deleting it.

        @param subobj: The object to unpack
        @type subobj: L{Object}

        @see: L{unpack_all()}
        @see: L{clear()}

        """
        elm_box_unpack(self.obj, obj.obj)

    def unpack_all(self):
        """Remove all items from the box, without deleting them.

        Clear the box from all children, but don't delete the respective objects.
        If no other references of the box children exist, the objects will never
        be deleted, and thus the application will leak the memory. Make sure
        when using this function that you hold a reference to all the objects
        in the box.

        @see: L{clear()}
        @see: L{unpack()}

        """
        elm_box_unpack_all(self.obj)

    def children_get(self):
        """Retrieve a list of the objects packed into the box

        Returns a C{tuple} with the child L{Object}s.
        The order of the list corresponds to the packing order the box uses.

        @return: The child objects
        @rtype: tuple of L{Object}s

        """
        cdef Evas_Object *o
        cdef Object obj
        cdef const_Eina_List *lst

        ret = []
        lst = elm_box_children_get(self.obj)
        while lst:
            o = <Evas_Object *> lst.data
            obj = <Object>evas_object_data_get(o, "python-evas")
            ret.append(obj)
            lst = lst.next
        return ret

    property children:
        """Retrieve a list of the objects packed into the box

        Returns a C{tuple} with the child L{Object}s.
        The order of the list corresponds to the packing order the box uses.

        @type: tuple of L{Object}s

        """
        def __get__(self):
            cdef Evas_Object *o
            cdef Object obj
            cdef const_Eina_List *lst

            ret = []
            lst = elm_box_children_get(self.obj)
            while lst:
                o = <Evas_Object *> lst.data
                obj = <Object>evas_object_data_get(o, "python-evas")
                ret.append(obj)
                lst = lst.next
            return ret
        #def __set__(self, value):
            #TODO: unpack_all() and then get the objects from value and pack_end() them.
        def __del__(self):
            elm_box_clear(self.obj)

    def padding_set(self, horizontal, vertical):
        """Set the space (padding) between the box's elements.

        Extra space in pixels that will be added between a box child and its
        neighbors after its containing cell has been calculated. This padding
        is set for all elements in the box, besides any possible padding that
        individual elements may have through their size hints.

        @param horizontal: The horizontal space between elements
        @type horizontal: Evas_Coord (int)
        @param vertical: The vertical space between elements
        @type vertical: Evas_Coord (int)

        """
        elm_box_padding_set(self.obj, horizontal, vertical)

    def padding_get(self):
        """Get the space (padding) between the box's elements.

        @see: L{padding_set()}

        @return: The horizontal and vertical space between elements
        @rtype: tuple of Evas_Coords (int)

        """
        cdef int horizontal, vertical

        elm_box_padding_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property padding:
        """The space (padding) between the box's elements.

        Extra space in pixels that will be added between a box child and its
        neighbors after its containing cell has been calculated. This padding
        is set for all elements in the box, besides any possible padding that
        individual elements may have through their size hints.

        @type: tuple of Evas_Coords (int)

        """
        def __get__(self):
            cdef int horizontal, vertical
            elm_box_padding_get(self.obj, &horizontal, &vertical)
            return (horizontal, vertical)

        def __set__(self, value):
            cdef int horizontal, vertical
            horizontal, vertical = value
            elm_box_padding_set(self.obj, horizontal, vertical)

    def align_set(self, horizontal, vertical):
        """Set the alignment of the whole bounding box of contents.

        Sets how the bounding box containing all the elements of the box, after
        their sizes and position has been calculated, will be aligned within
        the space given for the whole box widget.

        @param horizontal: The horizontal alignment of elements
        @type horizontal: float
        @param vertical: The vertical alignment of elements
        @type vertical: float

        """
        elm_box_align_set(self.obj, horizontal, vertical)

    def align_get(self):
        """Get the alignment of the whole bounding box of contents.

        @return: The horizontal and vertical alignment of elements
        @rtype: tuple of floats

        @see: L{align_set()}

        """
        cdef double horizontal, vertical

        elm_box_align_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property align:
        """Set the alignment of the whole bounding box of contents.

        Sets how the bounding box containing all the elements of the box, after
        their sizes and position has been calculated, will be aligned within
        the space given for the whole box widget.

        @rtype: tuple of floats

        """
        def __get__(self):
            cdef double horizontal, vertical
            elm_box_align_get(self.obj, &horizontal, &vertical)
            return (horizontal, vertical)

        def __set__(self, value):
            cdef double horizontal, vertical
            horizontal, vertical = value
            elm_box_align_set(self.obj, horizontal, vertical)

    def recalculate(self):
        """Force the box to recalculate its children packing.

        If any children was added or removed, box will not calculate the
        values immediately rather leaving it to the next main loop
        iteration. While this is great as it would save lots of
        recalculation, whenever you need to get the position of a just
        added item you must force recalculate before doing so.

        """
        elm_box_recalculate(self.obj)

    def layout_set(self, layout):
        """Set the layout function for the box.

        A box layout function affects how a box object displays child
        elements within its area. The list of pre-defined box layouts
        available in Evas is:
            - elementary.ELM_BOX_LAYOUT_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_VERTICAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_VERTICAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_VERTICAL
            - elementary.ELM_BOX_LAYOUT_FLOW_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_FLOW_VERTICAL
            - elementary.ELM_BOX_LAYOUT_STACK

        Note that you cannot set a custom layout function.

        @param layout: the new layout to set
        @type layout: Evas_Object_Box_Layout

        """
        cdef Evas_Object_Box_Layout ly

        ly = _py_elm_box_layout_resolv(layout)
        elm_box_layout_set(self.obj, ly, NULL, NULL)

    property layout:
        """Set the layout function for the box.

        A box layout function affects how a box object displays child
        elements within its area. The list of pre-defined box layouts
        available in Evas is:
            - elementary.ELM_BOX_LAYOUT_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_VERTICAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_VERTICAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_HOMOGENEOUS_MAX_SIZE_VERTICAL
            - elementary.ELM_BOX_LAYOUT_FLOW_HORIZONTAL
            - elementary.ELM_BOX_LAYOUT_FLOW_VERTICAL
            - elementary.ELM_BOX_LAYOUT_STACK

        Note that you cannot set a custom layout function.

        @type: Evas_Object_Box_Layout

        """
        def __set__(self, layout):
            cdef Evas_Object_Box_Layout ly
            ly = _py_elm_box_layout_resolv(layout)
            elm_box_layout_set(self.obj, ly, NULL, NULL)

    def layout_transition(self, duration, from_layout, to_layout):
        """Perform an animatation between two given different layout.

        If you want to animate the change from one layout to another, you
        just need to call this function with the starting layout and
        the final one.

        @see: L{layout_set()} for the list of available layouts.

        @param duration: the animation duration in seconds
        @type duration: double
        @param from_layout: one of elementary.ELM_BOX_LAYOUT_*
        @type from_layout: Evas_Object_Box_Layout
        @param to_layout: one of elementary.ELM_BOX_LAYOUT_*
        @type to_layout: Evas_Object_Box_Layout

        """
        cdef Elm_Box_Transition *t
        cdef Evas_Object_Box_Layout ly_from, ly_to

        ly_from = _py_elm_box_layout_resolv(from_layout)
        ly_to = _py_elm_box_layout_resolv(to_layout)
        t = elm_box_transition_new(duration,
                              ly_from, NULL, NULL,
                              ly_to, NULL, NULL,
                              NULL, NULL)
        elm_box_layout_set(self.obj, elm_box_layout_transition, t,
                           elm_box_transition_free)


_elm_widget_type_register("box", Box)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryBox_Type # hack to install metaclass
_install_metaclass(&PyElementaryBox_Type, ElementaryObjectMeta)
