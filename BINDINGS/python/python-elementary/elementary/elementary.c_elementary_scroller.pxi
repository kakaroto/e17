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

cdef public class Scroller(Object) [object PyElementaryScroller, type PyElementaryScroller_Type]:

    """A scroller holds a single object and "scrolls it around".

    This means that it allows the user to use a scrollbar (or a finger) to
    drag the viewable region around, allowing to move through a much larger
    object that is contained in the scroller. The scroller will always have
    a small minimum size by default as it won't be limited by the contents
    of the scroller.

    Signals that you can add callbacks for are:
        - "edge,left" - the left edge of the content has been reached
        - "edge,right" - the right edge of the content has been reached
        - "edge,top" - the top edge of the content has been reached
        - "edge,bottom" - the bottom edge of the content has been reached
        - "scroll" - the content has been scrolled (moved)
        - "scroll,anim,start" - scrolling animation has started
        - "scroll,anim,stop" - scrolling animation has stopped
        - "scroll,drag,start" - dragging the contents around has started
        - "scroll,drag,stop" - dragging the contents around has stopped

    Default content parts of the scroller widget that you can use for are:
        - "default" - A content of the scroller

    @note: The "scroll,anim,*" and "scroll,drag,*" signals are only emitted by
        user intervention.

    @note: When Elementary is in embedded mode the scrollbars will not be
        draggable, they appear merely as indicators of how much has been
        scrolled.

    @note: When Elementary is in desktop mode the thumbscroll(a.k.a.
        fingerscroll) won't work.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_scroller_add(parent.obj))

    def custom_widget_base_theme_set(self, widget, base):
        """custom_widget_base_theme_set(widget, base)

        Set custom theme elements for the scroller

        @param widget: The widget name to use (default is "scroller")
        @type widget: string
        @param base: The base name to use (default is "base")
        @type base: string

        """
        elm_scroller_custom_widget_base_theme_set(self.obj, _cfruni(widget), _cfruni(base))

    def content_min_limit(self, w, h):
        """content_min_limit(w, h)

        Make the scroller minimum size limited to the minimum size of the
        content

        By default the scroller will be as small as its design allows,
        irrespective of its content. This will make the scroller minimum
        size the right size horizontally and/or vertically to perfectly fit
        its content in that direction.

        @param w: Enable limiting minimum size horizontally
        @type w: bool
        @param h: Enable limiting minimum size vertically
        @type h: bool

        """
        elm_scroller_content_min_limit(self.obj, w, h)

    def region_show(self, x, y, w, h):
        """region_show(x, y, w, h)

        Show a specific virtual region within the scroller content object

        This will ensure all (or part if it does not fit) of the designated
        region in the virtual content object (0, 0 starting at the top-left
        of the virtual content object) is shown within the scroller.

        @param x: X coordinate of the region
        @type x: Evas_Coord (int)
        @param y: Y coordinate of the region
        @type y: Evas_Coord (int)
        @param w: Width of the region
        @type w: Evas_Coord (int)
        @param h: Height of the region
        @type h: Evas_Coord (int)

        """
        elm_scroller_region_show(self.obj, x, y, w, h)

    def policy_set(self, policy_h, policy_v):
        """Set the scrollbar visibility policy

        This sets the scrollbar visibility policy for the given scroller.
        ELM_SCROLLER_POLICY_AUTO means the scrollbar is made visible if it is
        needed, and otherwise kept hidden. ELM_SCROLLER_POLICY_ON turns it
        on all the time, and ELM_SCROLLER_POLICY_OFF always keeps it off.
        This applies respectively for the horizontal and vertical scrollbars.

        @param policy_h: Horizontal scrollbar policy
        @type policy_h: Elm_Scroller_Policy
        @param policy_v: Vertical scrollbar policy
        @type policy_v: Elm_Scroller_Policy

        """
        elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def policy_get(self):
        """Gets scrollbar visibility policy

        @see: elm_scroller_policy_set()

        @return: A tuple with horizontal and vertical policies.
        @rtype: tuple of Elm_Scroller_Policy

        """
        cdef Elm_Scroller_Policy policy_h, policy_v
        elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
        return (policy_h, policy_v)

    property policy:
        """The scrollbar visibility policy

        This property reflects the scrollbar visibility policy for the given
        scroller. ELM_SCROLLER_POLICY_AUTO means the scrollbar is made
        visible if it is needed, and otherwise kept hidden.
        ELM_SCROLLER_POLICY_ON turns it on all the time, and
        ELM_SCROLLER_POLICY_OFF always keeps it off. This applies
        respectively for the horizontal and vertical scrollbars.

        @type: tuple of Elm_Scroller_Policy

        """
        def __get__(self):
            cdef Elm_Scroller_Policy policy_h, policy_v
            elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
            return (policy_h, policy_v)

        def __set__(self, value):
            cdef Elm_Scroller_Policy policy_h, policy_v
            policy_h, policy_v = value
            elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def region_get(self):
        """Get the currently visible content region

        This gets the current region in the content object that is visible
        through the scroller. The region co-ordinates are returned in the
        C{x}, C{y}, C{w}, C{h} values pointed to.

        @note: All coordinates are relative to the content.

        @see: L{region_show()}

        @return: The X and Y coordinates and the width and height of the region
        @rtype: tuple of Evas_Coord (int)

        """
        cdef Evas_Coord x, y, w, h
        elm_scroller_region_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    property region:
        """Get the currently visible content region

        This gets the current region in the content object that is visible
        through the scroller. The region co-ordinates are returned in the
        C{x}, C{y}, C{w}, C{h} values pointed to.

        @note: All coordinates are relative to the content.

        @see: L{region_show()}

        @type: tuple of Evas_Coord (int)

        """
        def __get__(self):
            cdef Evas_Coord x, y, w, h
            elm_scroller_region_get(self.obj, &x, &y, &w, &h)
            return (x, y, w, h)

    def child_size_get(self):
        """Get the size of the content object

        This gets the size of the content object of the scroller.

        @return: The width and height of the content object
        @rtype: tuple of Evas_Coord (int)

        """
        cdef Evas_Coord w, h
        elm_scroller_child_size_get(self.obj, &w, &h)
        return (w, h)

    property child_size:
        """Get the size of the content object

        This gets the size of the content object of the scroller.

        @type: tuple of Evas_Coord (int)

        """
        def __get__(self):
            cdef Evas_Coord w, h
            elm_scroller_child_size_get(self.obj, &w, &h)
            return (w, h)

    def bounce_set(self, h, v):
        """Set bouncing behavior

        When scrolling, the scroller may "bounce" when reaching an edge of
        the content object. This is a visual way to indicate the end has
        been reached. This is enabled by default for both axis. This API
        will set if it is enabled for the given axis with the boolean
        parameters for each axis.

        @param h_bounce: Allow bounce horizontally
        @type h_bounce: bool
        @param v_bounce: Allow bounce vertically
        @type v_bounce: bool

        """
        elm_scroller_bounce_set(self.obj, h, v)

    def bounce_get(self):
        """Get the bounce behaviour

        @see: L{bounce_set()}

        @param h_bounce: Will the scroller bounce horizontally or not
        @type h_bounce: bool
        @param v_bounce: Will the scroller bounce vertically or not
        @type v_bounce: bool

        @return: Bounce behaviour for horizontal and vertical
        @rtype: (bool, bool)

        """
        cdef Eina_Bool h, v
        elm_scroller_bounce_get(self.obj, &h, &v)
        return (h, v)

    property bounce:
        """The bouncing behavior

        When scrolling, the scroller may "bounce" when reaching an edge of
        the content object. This is a visual way to indicate the end has
        been reached. This is enabled by default for both axis. This API
        will set if it is enabled for the given axis with the boolean
        parameters for each axis.

        @type: (bool, bool)

        """
        def __get__(self):
            cdef Eina_Bool h, v
            elm_scroller_bounce_get(self.obj, &h, &v)
            return (h, v)

        def __set__(self, value):
            cdef Eina_Bool h, v
            h, v = value
            elm_scroller_bounce_set(self.obj, h, v)

    def page_relative_set(self, h_pagerel, v_pagerel):
        """Set scroll page size relative to viewport size.

        The scroller is capable of limiting scrolling by the user to
        "pages". That is to jump by and only show a "whole page" at a time
        as if the continuous area of the scroller content is split into page
        sized pieces. This sets the size of a page relative to the viewport
        of the scroller. 1.0 is "1 viewport" is size (horizontally or
        vertically). 0.0 turns it off in that axis. This is mutually
        exclusive with page size (see L{page_size_set()} for more
        information). Likewise 0.5 is "half a viewport". Sane usable values
        are normally between 0.0 and 1.0 including 1.0. If you only want a
        single axis to be page "limited", use 0.0 for the other axis.

        @param h_pagerel: The horizontal page relative size
        @type h_pagerel: float
        @param v_pagerel: The vertical page relative size
        @type v_pagerel: float

        """
        elm_scroller_page_relative_set(self.obj, h_pagerel, v_pagerel)

    def page_relative_get(self):
        """Get scroll page size relative to viewport size.
        @see: L{page_relative_set()}

        @return: A tuple with horizontal and vertical page size relative to
        viewport size.
        @rtype: (float, float)

        """
        cdef double h_pagerel, v_pagerel
        elm_scroller_page_relative_get(self.obj, &h_pagerel, &v_pagerel)
        return (h_pagerel, v_pagerel)

    property page_relative:
        """Set scroll page size relative to viewport size.

        The scroller is capable of limiting scrolling by the user to
        "pages". That is to jump by and only show a "whole page" at a time
        as if the continuous area of the scroller content is split into page
        sized pieces. This sets the size of a page relative to the viewport
        of the scroller. 1.0 is "1 viewport" is size (horizontally or
        vertically). 0.0 turns it off in that axis. This is mutually
        exclusive with page size (see L{page_size_set()} for more
        information). Likewise 0.5 is "half a viewport". Sane usable values
        are normally between 0.0 and 1.0 including 1.0. If you only want a
        single axis to be page "limited", use 0.0 for the other axis.

        @type: tuple of floats

        """
        def __set__(self, value):
            h_pagerel, v_pagerel = value
            elm_scroller_page_relative_set(self.obj, h_pagerel, v_pagerel)

        def __get__(self):
            cdef double h_pagerel, v_pagerel
            elm_scroller_page_relative_get(self.obj, &h_pagerel, &v_pagerel)
            return (h_pagerel, v_pagerel)

    def page_size_set(self, h_pagesize, v_pagesize):
        """Set scroll page size.

        This sets the page size to an absolute fixed value, with 0 turning it off
        for that axis.

        @see: L{page_relative_set()}

        @param h_pagesize: The horizontal page size
        @type h_pagesize: Evas_Coord (int)
        @param v_pagesize: The vertical page size
        @type v_pagesize: Evas_Coord (int)

        """
        elm_scroller_page_size_set(self.obj, h_pagesize, v_pagesize)

    property page_size:
        """Set scroll page size.

        This sets the page size to an absolute fixed value, with 0 turning it off
        for that axis.

        @see: L{page_relative_set()}

        @type: tuple of Evas_Coords (int)

        """
        def __set__(self, value):
            h_pagesize, v_pagesize = value
            elm_scroller_page_size_set(self.obj, h_pagesize, v_pagesize)

    def current_page_get(self):
        """Get scroll current page number.

        The page number starts from 0. 0 is the first page.
        Current page means the page which meets the top-left of the viewport.
        If there are two or more pages in the viewport, it returns the number of the page
        which meets the top-left of the viewport.

        @see: L{last_page_get()}
        @see: L{page_show()}
        @see: L{page_bring_in()}

        @return: horizontal and vertical page number
        @rtype: tuple of ints

        """
        cdef int h_pagenumber, v_pagenumber
        elm_scroller_current_page_get(self.obj, &h_pagenumber, &v_pagenumber)
        return (h_pagenumber, v_pagenumber)

    property current_page:
        """Get scroll current page number.

        The page number starts from 0. 0 is the first page.
        Current page means the page which meets the top-left of the viewport.
        If there are two or more pages in the viewport, it returns the number of the page
        which meets the top-left of the viewport.

        @see: L{last_page_get()}
        @see: L{page_show()}
        @see: L{page_bring_in()}

        @type: tuple of ints

        """
        def __get__(self):
            cdef int h_pagenumber, v_pagenumber
            elm_scroller_current_page_get(self.obj, &h_pagenumber, &v_pagenumber)
            return (h_pagenumber, v_pagenumber)

    def last_page_get(self):
        """Get scroll last page number.

        The page number starts from 0. 0 is the first page.
        This returns the last page number among the pages.

        @see: L{current_page_get()}
        @see: L{page_show()}
        @see: L{page_bring_in()}

        @return: horizontal and vertical page number
        @rtype: tuple of ints

        """
        cdef int h_pagenumber, v_pagenumber
        elm_scroller_last_page_get(self.obj, &h_pagenumber, &v_pagenumber)
        return (h_pagenumber, v_pagenumber)

    property last_page:
        """Get scroll last page number.

        The page number starts from 0. 0 is the first page.
        This returns the last page number among the pages.

        @see: L{current_page_get()}
        @see: L{page_show()}
        @see: L{page_bring_in()}

        @type: tuple of ints

        """
        def __get__(self):
            cdef int h_pagenumber, v_pagenumber
            elm_scroller_last_page_get(self.obj, &h_pagenumber, &v_pagenumber)
            return (h_pagenumber, v_pagenumber)

    def page_show(self, h_pagenumber, v_pagenumber):
        """page_show(h_pagenumber, v_pagenumber)

        Show a specific virtual region within the scroller content object by page number.

        0, 0 of the indicated page is located at the top-left of the viewport.
        This will jump to the page directly without animation.

        Example of usage::
            sc = Scroller(win)
            sc.content_set(content)
            sc.page_relative_set(1, 0)
            h_page, v_page = sc.current_page_get()
            sc.page_show(h_page + 1, v_page)

        @see: L{page_bring_in()}

        @param h_pagenumber: The horizontal page number
        @type h_pagenumber: int
        @param v_pagenumber: The vertical page number
        @type v_pagenumber: int

        """
        elm_scroller_page_show(self.obj, h_pagenumber, v_pagenumber)

    def page_bring_in(self, h_pagenumber, v_pagenumber):
        """page_bring_in(h_pagenumber, v_pagenumber)

        Show a specific virtual region within the scroller content object by page number.

        0, 0 of the indicated page is located at the top-left of the viewport.
        This will slide to the page with animation.

        Example of usage::
            sc = Scroller(win)
            sc.content_set(content)
            sc.page_relative_set(1, 0)
            h_page, v_page = sc.last_page_get()
            sc.page_bring_in(h_page, v_page)

        @see: L{page_show()}

        @param h_pagenumber: The horizontal page number
        @type h_pagenumber: int
        @param v_pagenumber: The vertical page number
        @type v_pagenumber: int

        """
        elm_scroller_page_bring_in(self.obj, h_pagenumber, v_pagenumber)

    def region_bring_in(self, x, y, w, h):
        """region_bring_in(x, y, w, h)

        Show a specific virtual region within the scroller content object.

        This will ensure all (or part if it does not fit) of the designated
        region in the virtual content object (0, 0 starting at the top-left of the
        virtual content object) is shown within the scroller. Unlike
        elm_scroller_region_show(), this allow the scroller to "smoothly slide"
        to this location (if configuration in general calls for transitions). It
        may not jump immediately to the new location and make take a while and
        show other content along the way.

        @see: L{region_show()}

        @param x: X coordinate of the region
        @type x: Evas_Coord (int)
        @param y: Y coordinate of the region
        @type y: Evas_Coord (int)
        @param w: Width of the region
        @type w: Evas_Coord (int)
        @param h: Height of the region
        @type h: Evas_Coord (int)

        """
        elm_scroller_region_bring_in(self.obj, x, y, w, h)

    def propagate_events_set(self, propagation):
        """Set event propagation on a scroller

        This enables or disabled event propagation from the scroller content to
        the scroller and its parent. By default event propagation is disabled.

        @param propagation: If propagation is enabled or not
        @type propagation: bool

        """
        elm_scroller_propagate_events_set(self.obj, propagation)

    def propagate_events_get(self):
        """Get event propagation for a scroller

        This gets the event propagation for a scroller.

        @see: L{propagate_events_set()}

        @return: The propagation state
        @rtype: bool

        """
        return bool(elm_scroller_propagate_events_get(self.obj))

    property propagate_events:
        """Event propagation for a scroller

        This enables or disables event propagation from the scroller content to
        the scroller and its parent. By default event propagation is disabled.

        @type: bool

        """
        def __get__(self):
            return bool(elm_scroller_propagate_events_get(self.obj))

        def __set__(self, propagation):
            elm_scroller_propagate_events_set(self.obj, propagation)

    def gravity_set(self, x, y):
        """Set scrolling gravity on a scroller

        The gravity, defines how the scroller will adjust its view
        when the size of the scroller contents increase.

        The scroller will adjust the view to glue itself as follows.

        x=0.0, for showing the left most region of the content.
        x=1.0, for showing the right most region of the content.
        y=0.0, for showing the bottom most region of the content.
        y=1.0, for showing the top most region of the content.

        Default values for x and y are 0.0

        @param x: The scrolling horizontal gravity
        @type x: float
        @param y: The scrolling vertical gravity
        @type y: float

        """
        elm_scroller_gravity_set(self.obj, x, y)

    def gravity_get(self):
        """Get scrolling gravity values for a scroller

        This gets gravity values for a scroller.

        @see: L{gravity_set()}

        @return: The scrolling horizontal and vertical gravities
        @rtype: tuple of floats

        """
        cdef double x, y
        elm_scroller_gravity_get(self.obj, &x, &y)
        return (x, y)

    property gravity:
        """Scrolling gravity on a scroller

        The gravity, defines how the scroller will adjust its view
        when the size of the scroller contents increase.

        The scroller will adjust the view to glue itself as follows.

        x=0.0, for showing the left most region of the content.
        x=1.0, for showing the right most region of the content.
        y=0.0, for showing the bottom most region of the content.
        y=1.0, for showing the top most region of the content.

        Default values for x and y are 0.0

        @type: tuple of floats

        """
        def __get__(self):
            cdef double x, y
            elm_scroller_gravity_get(self.obj, &x, &y)
            return (x, y)

        def __set__(self, value):
            x, y = value
            elm_scroller_gravity_set(self.obj, x, y)

    def callback_edge_left_add(self, func, *args, **kwargs):
        """The left edge of the content has been reached."""
        self._callback_add("edge,left", func, *args, **kwargs)

    def callback_edge_left_del(self, func):
        self._callback_del("edge,left", func)

    def callback_edge_right_add(self, func, *args, **kwargs):
        """The right edge of the content has been reached."""
        self._callback_add("edge,right", func, *args, **kwargs)

    def callback_edge_right_del(self, func):
        self._callback_del("edge,right", func)

    def callback_edge_top_add(self, func, *args, **kwargs):
        """The top edge of the content has been reached."""
        self._callback_add("edge,top", func, *args, **kwargs)

    def callback_edge_top_del(self, func):
        self._callback_del("edge,top", func)

    def callback_edge_bottom_add(self, func, *args, **kwargs):
        """The bottom edge of the content has been reached."""
        self._callback_add("edge,bottom", func, *args, **kwargs)

    def callback_edge_bottom_del(self, func):
        self._callback_del("edge,bottom", func)

    def callback_scroll_add(self, func, *args, **kwargs):
        """The content has been scrolled (moved)."""
        self._callback_add("scroll", func, *args, **kwargs)

    def callback_scroll_del(self, func):
        self._callback_del("scroll", func)

    def callback_scroll_anim_start_add(self, func, *args, **kwargs):
        """Scrolling animation has started."""
        self._callback_add("scroll,anim,start", func, *args, **kwargs)

    def callback_scroll_anim_start_del(self, func):
        self._callback_del("scroll,anim,start", func)

    def callback_scroll_anim_stop_add(self, func, *args, **kwargs):
        """Scrolling animation has stopped."""
        self._callback_add("scroll,anim,stop", func, *args, **kwargs)

    def callback_scroll_anim_stop_del(self, func):
        self._callback_del("scroll,anim,stop", func)

    def callback_scroll_drag_start_add(self, func, *args, **kwargs):
        """Dragging the contents around has started."""
        self._callback_add("scroll,drag,start", func, *args, **kwargs)

    def callback_scroll_drag_start_del(self, func):
        self._callback_del("scroll,drag,start", func)

    def callback_scroll_drag_stop_add(self, func, *args, **kwargs):
        """Dragging the contents around has stopped."""
        self._callback_add("scroll,drag,stop", func, *args, **kwargs)

    def callback_scroll_drag_stop_del(self, func):
        self._callback_del("scroll,drag,stop", func)

_elm_widget_type_register("scroller", Scroller)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryScroller_Type # hack to install metaclass
_install_metaclass(&PyElementaryScroller_Type, ElementaryObjectMeta)
