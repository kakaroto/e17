# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_evas.pyx

cdef public class Rectangle(Object) [object PyEvasRectangle,
                                     type PyEvasRectangle_Type]:

    """A rectangle.

    There is only one function to deal with rectangle objects, this may make
    this function seem useless given there are no functions to manipulate
    the created rectangle, however the rectangle is actually very useful and
    should be manipulated using the generic L{Object} functions.

    The evas rectangle serves a number of key functions when working on evas
    programs:
        - Background
        - Debugging
        - Clipper

    Background
    ==========

    One extremely common requirement of evas programs is to have a solid
    color background, this can be accomplished with the following very
    simple code::

        bg = Rectangle(evas_canvas)
        # Here we set the rectangles red, green, blue and opacity levels
        bg.color_set(255, 255, 255, 255) # opaque white background
        bg.resize(WIDTH, HEIGHT) # covers full canvas
        bg.show()

    This however will have issues if the C{evas_canvas} is resized, however
    most windows are created using ecore evas and that has a solution to
    using the rectangle as a background::

        ee = EcoreEvas()
        bg = Rectangle(ee.evas_get())
        # Here we set the rectangles red, green, blue and opacity levels
        bg.color_set(255, 255, 255, 255) # opaque white background
        bg.resize(WIDTH, HEIGHT) # covers full canvas
        bg.show()
        ee.associate(bg, ECORE_EVAS_OBJECT_ASSOCIATE_BASE)

    So this gives us a white background to our window that will be resized
    together with it.

    Debugging
    =========

    Debugging is a major part of any programmers task and when debugging
    visual issues with evas programs the rectangle is an extremely useful
    tool. The rectangle's simplicity means that it's easier to pinpoint
    issues with it than with more complex objects. Therefore a common
    technique to use when writing an evas program and not getting the
    desired visual result is to replace the misbehaving object for a solid
    color rectangle and seeing how it interacts with the other elements,
    this often allows us to notice clipping, parenting or positioning
    issues. Once the issues have been identified and corrected the rectangle
    can be replaced for the original part and in all likelihood any
    remaining issues will be specific to that object's type.

    Clipping
    ========

    Clipping serves two main functions:
        - Limiting visibility(i.e. hiding portions of an object).
        - Applying a layer of color to an object.

    Limiting visibility
    -------------------

    It is often necessary to show only parts of an object, while it may be
    possible to create an object that corresponds only to the part that must
    be shown(and it isn't always possible) it's usually easier to use a a
    clipper. A clipper is a rectangle that defines what's visible and what
    is not. The way to do this is to create a solid white rectangle(which is
    the default, no need to call evas_object_color_set()) and give it a
    position and size of what should be visible. The following code
    exemplifies showing the center half of C{my_evas_object}::

        clipper = Rectangle(evas_canvas)
        clipper.move(my_evas_object_x / 4, my_evas_object_y / 4)
        clipper.resize(my_evas_object_width / 2, my_evas_object_height / 2)
        my_evas_object.clip_set(clipper)
        clipper.show()


    Layer of color
    --------------

    In the L{clipping} section we used a solid white clipper, which produced
    no change in the color of the clipped object, it just hid what was
    outside the clippers area. It is however sometimes desirable to change
    the of color an object, this can be accomplished using a clipper that
    has a non-white color. Clippers with color work by multiplying the
    colors of clipped object. The following code will show how to remove all
    the red from an object::

        clipper = Rectangle(evas)
        clipper.move(my_evas_object_x, my_evas_object_y)
        clipper.resize(my_evas_object_width, my_evas_object_height)
        clipper.color_set(0, 255, 255, 255)
        obj.clip_set(clipper)
        clipper.show()


    @warning: We don't guarantee any proper results if you create a Rectangle
    object without setting the evas engine.

    @param canvas: Evas canvas for this object
    @type canvas: Canvas
    @keyword size: Width and height
    @type size: tuple of ints
    @keyword pos: X and Y
    @type pos: tuple of ints
    @keyword geometry: X, Y, width, height
    @type geometry: tuple of ints
    @keyword color: R, G, B, A
    @type color: tuple of ints
    @keyword name: Object name
    @type name: string

    """

    def __init__(self, Canvas canvas not None, **kargs):
        Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(evas_object_rectangle_add(self.evas.obj))
        self._set_common_params(**kargs)


cdef extern from "Evas.h": # hack to force type to be known
    cdef PyTypeObject PyEvasRectangle_Type # hack to install metaclass
_install_metaclass(&PyEvasRectangle_Type, EvasObjectMeta)
