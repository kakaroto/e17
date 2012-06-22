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

cdef public class Image(Object) [object PyElementaryImage, type PyElementaryImage_Type]:

    """An Elementary image object allows one to load and display an image
    file on it, be it from a disk file or from a memory region.

    Exceptionally, one may also load an Edje group as the contents of the
    image. In this case, though, most of the functions of the image API will
    act as a no-op.

    One can tune various properties of the image, like:
        - pre-scaling,
        - smooth scaling,
        - orientation,
        - aspect ratio during resizes, etc.

    An image object may also be made valid source and destination for drag
    and drop actions, through the L{editable_set()} call.

    Signals that you can add callbacks for are:

        - C{"drop"} - This is called when a user has dropped an image
            typed object onto the object in question -- the
            event info argument is the path to that image file
        - C{"clicked"} - This is called when a user has clicked the image

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_image_add(parent.obj))

    #def memfile_set(self, img, size, format, key):
        #return bool(elm_image_memfile_set(self.obj, img, size, _cfruni(format), _cfruni(key)))

    def file_set(self, filename, group = None):
        """Set the file that will be used as the image's source.

        @see: L{file_get()}

        @note: This function will trigger the Edje file case based on the
            extension of the C{file} string (expects C{".edj"}, for this
            case). If one wants to force this type of file independently of
            the extension, L{file_edje_set()} must be used, instead.

        @param file: The path to file that will be used as image source
        @type file: string
        @param group: The group that the image belongs to, in case it's an
            EET (including Edje case) file
        @type group: string
        @return: (C{True} = success, C{False} = error)
        @rtype: bool

        """
        if group == None:
            elm_image_file_set(self.obj, _cfruni(filename), NULL)
        else:
            elm_image_file_set(self.obj, _cfruni(filename), _cfruni(group))

    def file_get(self):
        """Get the file that will be used as image.

        @see: L{file_set()}

        @return: File path and group
        @rtype: tuple of strings

        """
        cdef const_char_ptr filename, group
        elm_image_file_get(self.obj, &filename, &group)
        return (_ctouni(filename), _ctouni(group))

    property file:
        """The file (and edje group) that will be used as the image's source.

        @note: Setting this will trigger the Edje file case based on the
            extension of the C{file} string (expects C{".edj"}, for this
            case). If one wants to force this type of file independently of
            the extension, L{file_edje_set()} must be used, instead.

        @type: string or tuple of strings

        """
        def __set__(self, value):
            if isinstance(value, tuple):
                filename, group = value
            else:
                filename = value
                group = None
            # TODO: check return value
            elm_image_file_set(self.obj, _cfruni(filename), _cfruni(group))

        def __get__(self):
            cdef const_char_ptr filename, group
            elm_image_file_get(self.obj, &filename, &group)
            return (_ctouni(filename), _ctouni(group))

    def smooth_set(self, smooth):
        """Set the smooth effect for an image.

        Set the scaling algorithm to be used when scaling the image. Smooth
        scaling provides a better resulting image, but is slower.

        The smooth scaling should be disabled when making animations that change
        the image size, since it will be faster. Animations that don't require
        resizing of the image can keep the smooth scaling enabled (even if the
        image is already scaled, since the scaled image will be cached).

        @see: L{smooth_get()}

        @param smooth: C{True} if smooth scaling should be used, C{False}
            otherwise. Default is C{True}.
        @type smooth: bool

        """
        elm_image_smooth_set(self.obj, smooth)

    def smooth_get(self):
        """Get the smooth effect for an image.

        @see: L{smooth_get()}

        @return: C{True} if smooth scaling is enabled, C{False} otherwise.
        @rtype: bool

        """
        return bool(elm_image_smooth_get(self.obj))

    property smooth:
        """The smooth effect for an image.

        The scaling algorithm to be used when scaling the image. Smooth
        scaling provides a better resulting image, but is slower.

        The smooth scaling should be disabled when making animations that change
        the image size, since it will be faster. Animations that don't require
        resizing of the image can keep the smooth scaling enabled (even if the
        image is already scaled, since the scaled image will be cached).

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_smooth_get(self.obj))

        def __set__(self, smooth):
            elm_image_smooth_set(self.obj, smooth)

    def object_size_get(self):
        """Gets the current size of the image.

        This is the real size of the image, not the size of the object.

        @return: Width and height of the image
        @rtype: tuple of ints

        """
        cdef int width, height
        elm_image_object_size_get(self.obj, &width, &height)
        return (width, height)

    property object_size:
        """The current size of the image.

        This is the real size of the image, not the size of the object.

        @type: tuple of ints

        """
        def __get__(self):
            cdef int width, height
            elm_image_object_size_get(self.obj, &width, &height)
            return (width, height)

    def no_scale_set(self, no_scale):
        """Disable scaling of this object.

        This function disables scaling of the elm_image widget through the
        function L{Object.scale_set()}. However, this does not affect the widget
        size/resize in any way. For that effect, take a look at
        L{resizable_set()}.

        @see: L{no_scale_get()}
        @see: L{resizable_set()}
        @see: L{Object.scale_set()}

        @param no_scale: C{True} if the object is not scalable, C{False}
            otherwise. Default is C{False}.
        @type no_scale: bool

        """
        elm_image_no_scale_set(self.obj, no_scale)

    def no_scale_get(self):
        """Get whether scaling is disabled on the object.

        @see: L{no_scale_set()}

        @return: C{True} if scaling is disabled, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_image_no_scale_get(self.obj))

    property no_scale:
        """Whether to disable scaling of this object.

        This disables scaling of the elm_image widget through the
        function L{Object.scale_set()}. However, this does not affect the widget
        size/resize in any way. For that effect, take a look at
        L{resizable_set()}.

        @see: L{resizable_set()}
        @see: L{Object.scale_set()}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_no_scale_get(self.obj))
        def __set__(self, no_scale):
            elm_image_no_scale_set(self.obj, no_scale)

    def resizable_set(self, size_up, size_down):
        """Set if the object is (up/down) resizable.

        This function limits the image resize ability. If C{size_up} is set to
        C{False}, the object can't have its height or width resized to a value
        higher than the original image size. Same is valid for C{size_down}.

        @see: L{resizable_get()}

        @param size_up: A bool to set if the object is resizable up. Default is
            C{True}.
        @type size_up: bool
        @param size_down: A bool to set if the object is resizable down. Default
            is C{True}.
        @type size_down: bool

        """
        elm_image_resizable_set(self.obj, size_up, size_down)

    def resizable_get(self):
        """Get if the object is (up/down) resizable.

        @return: The values of resizable up and down
        @rtype: tuple of bools

        """
        cdef Eina_Bool size_up, size_down
        elm_image_resizable_get(self.obj, &size_up, &size_down)
        return (size_up, size_down)

    property resizable:
        """Whether the object is (up/down) resizable.

        This limits the image resize ability. If  set to C{False}, the
        object can't have its height or width resized to a value higher than
        the original image size. Same is valid for C{size_down}.

        @type: (bool size_up, bool size_down)

        """
        def __get__(self):
            cdef Eina_Bool size_up, size_down
            elm_image_resizable_get(self.obj, &size_up, &size_down)
            return (size_up, size_down)

        def __set__(self, value):
            size_up, size_down = value
            elm_image_resizable_set(self.obj, size_up, size_down)

    def fill_outside_set(self, fill_outside):
        """Set if the image fills the entire object area, when keeping the aspect ratio.

        When the image should keep its aspect ratio even if resized to another
        aspect ratio, there are two possibilities to resize it: keep the entire
        image inside the limits of height and width of the object (C{fill_outside}
        is C{False}) or let the extra width or height go outside of the object,
        and the image will fill the entire object (C{fill_outside} is C{True}).

        @note: This option will have no effect if
            L{aspect_fixed_set()} is set to C{False}.

        @see: L{fill_outside_get()}
        @see: L{aspect_fixed_set()}

        @param fill_outside: C{True} if the object is filled outside,
            C{False} otherwise. Default is C{False}.
        @type fill_outside: bool

        """
        elm_image_fill_outside_set(self.obj, fill_outside)

    def fill_outside_get(self):
        """Get if the object is filled outside

        @see: L{fill_outside_set()}

        @return: C{True} if the object is filled outside, C{False} otherwise.
        @rtype: bool

        """
        return bool(elm_image_fill_outside_get(self.obj))

    property fill_outside:
        """Whether the image fills the entire object area, when keeping the aspect ratio.

        When the image should keep its aspect ratio even if resized to another
        aspect ratio, there are two possibilities to resize it: keep the entire
        image inside the limits of height and width of the object (C{fill_outside}
        is C{False}) or let the extra width or height go outside of the object,
        and the image will fill the entire object (C{fill_outside} is C{True}).

        @note: This option will have no effect if
            L{aspect_fixed} is set to C{False}.

        @see: L{aspect_fixed}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_fill_outside_get(self.obj))

        def __set__(self, fill_outside):
            elm_image_fill_outside_set(self.obj, fill_outside)

    def preload_disabled_set(self, disabled):
        """Enable or disable preloading of the image

        @param disabled: If True, preloading will be disabled
        @type disabled: bool

        """
        elm_image_preload_disabled_set(self.obj, disabled)

    property preload_disabled:
        """Enable or disable preloading of the image

        @type: bool

        """
        def __set__(self, disabled):
            elm_image_preload_disabled_set(self.obj, disabled)

    def prescale_set(self, size):
        """Set the prescale size for the image

        This function sets a new size for pixmap representation of the given
        image. It allows the image to be loaded already in the specified size,
        reducing the memory usage and load time when loading a big image with load
        size set to a smaller size.

        It's equivalent to the L{Background.load_size_set()} function for bg.

        @note: this is just a hint, the real size of the pixmap may differ
        depending on the type of image being loaded, being bigger than requested.

        @see: L{prescale_get()}
        @see: L{Background.load_size_set()}

        @param size: The prescale size. This value is used for both width and
            height.
        @type size: int

        """
        elm_image_prescale_set(self.obj, size)

    def prescale_get(self):
        """Get the prescale size for the image

        @see: L{prescale_set()}

        @return: The prescale size
        @rtype: int

        """
        return elm_image_prescale_get(self.obj)

    property prescale:
        """The prescale size for the image

        This is the size for pixmap representation of the given
        image. It allows the image to be loaded already in the specified size,
        reducing the memory usage and load time when loading a big image with load
        size set to a smaller size.

        It's equivalent to the L{Background.load_size} property for bg.

        @note: this is just a hint, the real size of the pixmap may differ
        depending on the type of image being loaded, being bigger than requested.

        @see: L{Background.load_size}

        @type: int

        """
        def __get__(self):
            return elm_image_prescale_get(self.obj)
        def __set__(self, size):
            elm_image_prescale_set(self.obj, size)

    def orient_set(self, orientation):
        """Set the image orientation.

        This function allows to rotate or flip the given image.

        @see: L{orient_get()}

        @param orient: The image orientation. Default is ELM_IMAGE_ORIENT_NONE.
        @type orient: Elm_Image_Orient

        """
        elm_image_orient_set(self.obj, orientation)

    def orient_get(self):
        """Get the image orientation.

        @see: L{orient_set()}

        @return: The image orientation
        @rtype: Elm_Image_Orient

        """
        return elm_image_orient_get(self.obj)

    property orient:
        """The image orientation.

        Setting this allows to rotate or flip the given image.

        @type: Elm_Image_Orient

        """
        def __get__(self):
            return elm_image_orient_get(self.obj)
        def __set__(self, orientation):
            elm_image_orient_set(self.obj, orientation)

    def editable_set(self, editable):
        """Make the image 'editable'.

        This means the image is a valid drag target for drag and drop, and can be
        cut or pasted too.

        @param set: Turn on or off editability. Default is C{False}.
        @type set: bool

        """
        elm_image_editable_set(self.obj, editable)

    def editable_get(self):
        """Check if the image is 'editable'.

        A return value of True means the image is a valid drag target
        for drag and drop, and can be cut or pasted too.

        @return: Editability.
        @rtype: bool

        """
        return bool(elm_image_editable_get(self.obj))

    property editable:
        """Whether the image is 'editable'.

        This means the image is a valid drag target for drag and drop, and can be
        cut or pasted too. Default is C{False}.

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_editable_get(self.obj))
        def __set__(self, editable):
            elm_image_editable_set(self.obj, editable)

    def object_get(self):
        """Get the inlined image object of the image widget.

        This function allows one to get the underlying C{Evas_Object} of type
        Image from this elementary widget. It can be useful to do things like get
        the pixel data, save the image to a file, etc.

        @note: Be careful to not manipulate it, as it is under control of
        elementary.

        @return: The inlined image object, or None if none exists
        @rtype: evas.Image

        """
        return Object_from_instance(elm_image_object_get(self.obj))

    property object:
        """Get the inlined image object of the image widget.

        This function allows one to get the underlying C{Evas_Object} of type
        Image from this elementary widget. It can be useful to do things like get
        the pixel data, save the image to a file, etc.

        @note: Be careful to not manipulate it, as it is under control of
        elementary.

        @type: evas.Image

        """
        def __get__(self):
            return Object_from_instance(elm_image_object_get(self.obj))

    def aspect_fixed_set(self, fixed):
        """Set whether the original aspect ratio of the image should be kept on resize.

        The original aspect ratio (width / height) of the image is usually
        distorted to match the object's size. Enabling this option will retain
        this original aspect, and the way that the image is fit into the object's
        area depends on the option set by L{fill_outside_set()}.

        @see: L{aspect_fixed_get()}
        @see: L{fill_outside_set()}

        @param fixed: C{True} if the image should retain the aspect,
            C{False} otherwise.
        @type fixed: bool

        """
        elm_image_aspect_fixed_set(self.obj, fixed)

    def aspect_fixed_get(self):
        """Get if the object retains the original aspect ratio.

        @return: C{True} if the object keeps the original aspect, C{False}
            otherwise.
        @rtype: bool

        """
        return bool(elm_image_aspect_fixed_get(self.obj))

    property aspect_fixed:
        """Whether the original aspect ratio of the image should be kept on resize.

        The original aspect ratio (width / height) of the image is usually
        distorted to match the object's size. Enabling this option will retain
        this original aspect, and the way that the image is fit into the object's
        area depends on the option set by L{fill_outside}.

        @see: L{fill_outside}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_aspect_fixed_get(self.obj))
        def __set__(self, fixed):
            elm_image_aspect_fixed_set(self.obj, fixed)

    def animated_available_get(self):
        """Get whether an image object supports animation or not.

        This function returns if this Elementary image object's internal
        image can be animated. Currently Evas only supports GIF
        animation. If the return value is B{False}, other
        C{animated_xxx} API calls won't work.

        @see: L{animated_set()}

        @return: C{True} if the image supports animation,
            C{False} otherwise.
        @rtype: bool

        """
        return bool(elm_image_animated_available_get(self.obj))

    property animated_available:
        """Whether an image object supports animation or not.

        This returns if this Elementary image object's internal
        image can be animated. Currently Evas only supports GIF
        animation. If the return value is B{False}, other
        C{animated_xxx} API calls won't work.

        @see: L{animated}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_animated_available_get(self.obj))

    def animated_set(self, animated):
        """Set whether an image object (which supports animation) is to
        animate itself or not.

        An image object, even if it supports animation, will be displayed
        by default without animation. Call this function with C{animated}
        set to C{True} to enable its animation. To start or stop the
        animation, actually, use L{animated_play_set()}.

        @see: L{animated_get()}
        @see: L{animated_available_get()}
        @see: L{animated_play_set()}

        @param animated: C{True} if the object is to animate itself,
            C{False} otherwise. Default is C{False}.
        @type animated: bool

        """
        elm_image_animated_set(self.obj, animated)

    def animated_get(self):
        """Get whether an image object has animation enabled or not.

        @see: L{animated_set()}

        @return: C{True} if the image has animation enabled,
            C{False} otherwise.
        @rtype: bool

        """
        return bool(elm_image_animated_get(self.obj))

    property animated:
        """Whether an image object (which supports animation) is to
        animate itself or not.

        An image object, even if it supports animation, will be displayed
        by default without animation. Set this to C{True} to enable its
        animation. To start or stop the
        animation, actually, use L{animated_play}.

        @see: L{animated_available}
        @see: L{animated_play}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_animated_get(self.obj))
        def __set__(self, animated):
            elm_image_animated_set(self.obj, animated)

    def animated_play_set(self, play):
        """Start or stop an image object's animation.

        To actually start playing any image object's animation, if it
        supports it, one must do something like::

            if img.animated_available_get():
                img.animated_set(True)
                img.animated_play_set(True)

        L{animated_set()} will enable animation on the image, B{but
        not start it yet}. This is the function one uses to start and
        stop animations on image objects.

        @see: L{animated_available_get()}
        @see: L{animated_set()}
        @see: L{animated_play_get()}

        @param play: C{True} to start the animation, C{False}
            otherwise. Default is C{False}.
        @type play: bool

        """
        elm_image_animated_play_set(self.obj, play)

    def animated_play_get(self):
        """Get whether an image object is under animation or not.

        @see: L{animated_play_set()}

        @return: C{True}, if the image is being animated, C{False}
            otherwise.
        @rtype: bool

        """
        return bool(elm_image_animated_play_get(self.obj))

    property animated_play:
        """Start or stop an image object's animation.

        To actually start playing any image object's animation, if it
        supports it, one must do something like::

            if img.animated_available:
                img.animated = True
                img.animated_play = True

        L{animated} will enable animation on the image, B{but not start it yet}.
        This is the property one uses to start and stop animation on
        an image object or get whether it is animating or not.

        @see: L{animated_available}
        @see: L{animated}

        @type: bool

        """
        def __get__(self):
            return bool(elm_image_animated_play_get(self.obj))
        def __set__(self, play):
            elm_image_animated_play_set(self.obj, play)

    def callback_clicked_add(self, func, *args, **kwargs):
        """This is called when a user has clicked the image."""
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_drop_add(self, func, *args, **kwargs):
        """This is called when a user has dropped an image typed object onto
        the object in question -- the event info argument is the path to that
        image file."""
        self._callback_add_full("drop", _cb_string_conv, func, *args, **kwargs)

    def callback_drop_del(self, func):
        self._callback_del_full("drop", _cb_string_conv, func)

_elm_widget_type_register("image", Image)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryImage_Type # hack to install metaclass
_install_metaclass(&PyElementaryImage_Type, ElementaryObjectMeta)
