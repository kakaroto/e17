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

from evas.c_evas cimport Image as evasImage

cdef public class Photocam(Object) [object PyElementaryPhotocam, type PyElementaryPhotocam_Type]:

    """This is a widget specifically for displaying high-resolution digital
    camera photos giving speedy feedback (fast load), low memory footprint
    and zooming and panning as well as fitting logic. It is entirely focused
    on jpeg images, and takes advantage of properties of the jpeg format (via
    evas loader features in the jpeg loader).

    Signals that you can add callbacks for are:
        - "clicked" - This is called when a user has clicked the photo without
            dragging around.
        - "press" - This is called when a user has pressed down on the photo.
        - "longpressed" - This is called when a user has pressed down on the
            photo for a long time without dragging around.
        - "clicked,double" - This is called when a user has double-clicked the
            photo.
        - "load" - Photo load begins.
        - "loaded" - This is called when the image file load is complete for
            the first view (low resolution blurry version).
        - "load,detail" - Photo detailed data load begins.
        - "loaded,detail" - This is called when the image file load is
            complete for the detailed image data (full resolution needed).
        - "zoom,start" - Zoom animation started.
        - "zoom,stop" - Zoom animation stopped.
        - "zoom,change" - Zoom changed when using an auto zoom mode.
        - "scroll" - the content has been scrolled (moved)
        - "scroll,anim,start" - scrolling animation has started
        - "scroll,anim,stop" - scrolling animation has stopped
        - "scroll,drag,start" - dragging the contents around has started
        - "scroll,drag,stop" - dragging the contents around has stopped

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_photocam_add(parent.obj))

    property file:
        """The photo file to be shown

        This sets (and shows) the specified file (with a relative or absolute
        path) and will return a load error (same error that
        evas_object_image_load_error_get() will return). The image will
        change and adjust its size at this point and begin a background load
        process for this photo that at some time in the future will be
        displayed at the full quality needed.

        @type: string

        """
        def __set__(self, file):
            elm_photocam_file_set(self.obj, _cfruni(file))
            #TODO: handle errors from return status
        def __get__(self):
            return _ctouni(elm_photocam_file_get(self.obj))

    property zoom:
        """The zoom level of the photo

        This sets the zoom level. 1 will be 1:1 pixel for pixel. 2 will be
        2:1 (that is 2x2 photo pixels will display as 1 on-screen pixel).
        4:1 will be 4x4 photo pixels as 1 screen pixel, and so on. The
        parameter must be greater than 0. It is suggested to stick to powers
        of 2. (1, 2, 4, 8, 16, 32, etc.).

        @type: float

        """
        def __set__(self, zoom):
            elm_photocam_zoom_set(self.obj, zoom)
        def __get__(self):
            return elm_photocam_zoom_get(self.obj)

    property zoom_mode:
        """Set the zoom mode

        This sets the zoom mode to manual or one of several automatic levels.
        Manual (ELM_PHOTOCAM_ZOOM_MODE_MANUAL) means that zoom is set
        manually by L{zoom} and will stay at that level until changed by
        code or until zoom mode is changed. This is the default mode. The
        Automatic modes will allow the photocam object to automatically
        adjust zoom mode based on properties.
        ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT) will adjust zoom so the photo fits
        EXACTLY inside the scroll frame with no pixels outside this region.
        ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL will be similar but ensure no
        pixels within the frame are left unfilled.

        @type: Elm_Photocam_Zoom_Mode

        """
        def __set__(self, mode):
            elm_photocam_zoom_mode_set(self.obj, mode)
        def __get__(self):
            return elm_photocam_zoom_mode_get(self.obj)

    property image_size:
        """Get the current image pixel width and height

        This gets the current photo pixel width and height (for the
        original). The size will be returned in the integers @C{w} and @C{h}
        that are pointed to.

        @type: tuple of ints

        """
        def __get__(self):
            cdef int w, h
            elm_photocam_image_size_get(self.obj, &w, &h)
            return (w, h)

    property image_region:
        """Get the region of the image that is currently shown

        @see: L{image_region_show()}
        @see: L{image_region_bring_in()}

        @type: tuple of ints

        """
        def __get__(self):
            cdef int x, y, w, h
            elm_photocam_image_region_get(self.obj, &x, &y, &w, &h)
            return (x, y, w, h)

    def image_region_show(self, x, y, w, h):
        """image_region_show(x, y, w, h)

        Set the viewed region of the image

        This shows the region of the image without using animation.

        @param x: X-coordinate of region in image original pixels
        @type x: int
        @param y: Y-coordinate of region in image original pixels
        @type y: int
        @param w: Width of region in image original pixels
        @type w: int
        @param h: Height of region in image original pixels
        @type h: int

        """
        elm_photocam_image_region_show(self.obj, x, y, w, h)

    def image_region_bring_in(self, x, y, w, h):
        """image_region_bring_in(x, y, w, h)

        Bring in the viewed portion of the image

        This shows the region of the image using animation.

        @param x: X-coordinate of region in image original pixels
        @type x: int
        @param y: Y-coordinate of region in image original pixels
        @type y: int
        @param w: Width of region in image original pixels
        @type w: int
        @param h: Height of region in image original pixels
        @type h: int

        """
        elm_photocam_image_region_bring_in(self.obj, x, y, w, h)

    property paused:
        """Set the paused state for photocam

        This sets the paused state to on (True) or off (False) for photocam.
        The default is off. This will stop zooming using animation on zoom
        level changes and change instantly. This will stop any existing
        animations that are running.

        @type: bool

        """
        def __set__(self, paused):
            elm_photocam_paused_set(self.obj, paused)
        def __get__(self):
            return bool(elm_photocam_paused_get(self.obj))

    property internal_image:
        """Get the internal low-res image used for photocam

        This gets the internal image object inside photocam. Do not modify
        it. It is for inspection only, and hooking callbacks to. Nothing
        else. It may be deleted at any time as well.

        @type: evasImage

        """
        def __get__(self):
            cdef evasImage img = evasImage()
            cdef Evas_Object *obj = elm_photocam_internal_image_get(self.obj)
            img.obj = obj
            return img

    property bounce:
        """Photocam scrolling bouncing.

        @type: tuple of bools

        """
        def __set__(self, value):
            h_bounce, v_bounce = value
            elm_scroller_bounce_set(self.obj, h_bounce, v_bounce)
        def __get__(self):
            cdef Eina_Bool h_bounce, v_bounce
            elm_scroller_bounce_get(self.obj, &h_bounce, &v_bounce)
            return (h_bounce, v_bounce)

    property gesture_enabled:
        """Set the gesture state for photocam.

        This sets the gesture state to on (True) or off (False) for
        photocam. The default is off. This will start multi touch zooming.

        @type: bool

        """
        def __set__(self, gesture):
            elm_photocam_gesture_enabled_set(self.obj, gesture)
        def __get__(self):
            return bool(elm_photocam_gesture_enabled_get(self.obj))

    def callback_clicked_add(self, func, *args, **kwargs):
        """This is called when a user has clicked the photo without dragging
        around."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_press_add(self, func, *args, **kwargs):
        """This is called when a user has pressed down on the photo."""
        self._callback_add("press", func, *args, **kwargs)

    def callback_press_del(self, func):
        self._callback_del("press", func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        """This is called when a user has pressed down on the photo for a
        long time without dragging around."""
        self._callback_add("longpressed", func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del("longpressed", func)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        """This is called when a user has double-clicked the photo."""
        self._callback_add("clicked,double", func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del("clicked,double", func)

    def callback_load_add(self, func, *args, **kwargs):
        """Photo load begins."""
        self._callback_add("load", func, *args, **kwargs)

    def callback_load_del(self, func):
        self._callback_del("load", func)

    def callback_loaded_add(self, func, *args, **kwargs):
        """This is called when the image file load is complete for the first
        view (low resolution blurry version)."""
        self._callback_add("loaded", func, *args, **kwargs)

    def callback_loaded_del(self, func):
        self._callback_del("loaded", func)

    def callback_load_detail_add(self, func, *args, **kwargs):
        """Photo detailed data load begins."""
        self._callback_add("load,detail", func, *args, **kwargs)

    def callback_load_detail_del(self, func):
        self._callback_del("load,detail", func)

    def callback_loaded_detail_add(self, func, *args, **kwargs):
        """This is called when the image file load is complete for the
        detailed image data (full resolution needed)."""
        self._callback_add("loaded,detail", func, *args, **kwargs)

    def callback_loaded_detail_del(self, func):
        self._callback_del("loaded,detail", func)

    def callback_zoom_start_add(self, func, *args, **kwargs):
        """Zoom animation started."""
        self._callback_add("zoom,start", func, *args, **kwargs)

    def callback_zoom_start_del(self, func):
        self._callback_del("zoom,start", func)

    def callback_zoom_stop_add(self, func, *args, **kwargs):
        """Zoom animation stopped."""
        self._callback_add("zoom,stop", func, *args, **kwargs)

    def callback_zoom_stop_del(self, func):
        self._callback_del("zoom,stop", func)

    def callback_zoom_change_add(self, func, *args, **kwargs):
        """Zoom changed when using an auto zoom mode."""
        self._callback_add("zoom,change", func, *args, **kwargs)

    def callback_zoom_change_del(self, func):
        self._callback_del("zoom,change", func)

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


_elm_widget_type_register("photocam", Photocam)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPhotocam_Type # hack to install metaclass
_install_metaclass(&PyElementaryPhotocam_Type, ElementaryObjectMeta)
