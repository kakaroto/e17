# This file is included verbatim by c_evas.pyx

cdef class Image(Object):
    """Image from file or buffer.

    @note: if an image is resized it will B{tile} it's contents respecting
      geometry set by L{fill_set()}, so if you want the contents to be
      B{scaled} you need to call L{fill_set()} with C{x=0, y=0, w=new_width,
      h=new_height}, or you should use L{FilledImage} instead.

    @group Border settings: border_set, border_get, border,
       border_center_fill_set, border_center_fill_get, border_center_fill
    @group Load settings: load_size_set, load_size_get, load_size,
       load_dpi_set, load_dpi_get, load_dpi,
       load_scale_down_set, load_scale_down_get, load_scale_down
    @group Often unused: alpha_set, alpha_get, alpha, colorspace_set,
       colorspace_set, colorspace, pixels_dirty_set, pixels_dirty_get,
       pixels_dirty
    """
    def __init__(self, Canvas canvas not None, **kargs):
        Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(evas_object_image_add(self.evas.obj))
        self._set_common_params(**kargs)

    def _set_common_params(self, file=None, **kargs):
        if file:
            if isinstance(file, basestring):
                file = (file, None)
            self.file_set(*file)
        Object._set_common_params(self, **kargs)

    def file_set(self, char *filename, key=None):
        """Set the image to display a file.

        @parm: B{filename} file that contains the image.
        @parm: B{key} required for some file formats, like EET.
        @raise EvasLoadError: on load error.
        """
        cdef char *k
        cdef int err
        if key:
            k = key
        else:
            k = NULL
        evas_object_image_file_set(self.obj, filename, k)
        err = evas_object_image_load_error_get(self.obj)
        if err != EVAS_LOAD_ERROR_NONE:
            raise EvasLoadError(err, filename, key)

    def file_get(self):
        "@rtype: tuple of str"
        cdef char *f, *k
        evas_object_image_file_get(self.obj, &f, &k)
        if f == NULL:
            file = None
        else:
            file = f
        if k == NULL:
            key = None
        else:
            key = k
        return (file, key)

    property file:
        def __get__(self):
            return self.file_get()

        def __set__(self, value):
            if isinstance(value, basestring):
                value = (value, None)
            self.file_set(*value)

    def load_error_get(self):
        "@rtype: int"
        return evas_object_image_load_error_get(self.obj)

    property load_error:
        def __get__(self):
            return self.load_error_get()

    def border_get(self):
        "@rtype: tuple of int"
        cdef int left, right, top, bottom
        evas_object_image_border_get(self.obj, &left, &right, &top, &bottom)
        return (left, right, top, bottom)

    def border_set(self, int left, int right, int top, int bottom):
        """Sets how much of each border is not to be scaled.

        When rendering, the image may be scaled to fit the size of the
        image object.  This function sets what area around the border of
        the image is not to be scaled.  This sort of function is useful for
        widget theming, where, for example, buttons may be of varying
        sizes, but the border size must remain constant.

        @parm: B{left}
        @parm: B{right}
        @parm: B{top}
        @parm: B{bottom}
        """
        evas_object_image_border_set(self.obj, left, right, top, bottom)

    property border:
        def __get__(self):
            return self.border_get()

        def __set__(self, spec):
            self.border_set(*spec)

    def border_center_fill_get(self):
        "@rtype: bool"
        return bool(evas_object_image_border_center_fill_get(self.obj))

    def border_center_fill_set(self, int value):
        """Sets if the center part of an image (not the border) should be drawn

        @see: B{border_set()}

        When rendering, the image may be scaled to fit the size of the
        image object.  This function sets if the center part of the scaled
        image is to be drawn or left completely blank. Very useful for frames
        and decorations.
        """
        evas_object_image_border_center_fill_set(self.obj, value)

    property border_center_fill:
        def __get__(self):
            return self.border_center_fill_get()

        def __set__(self, int value):
            self.border_center_fill_set(value)

    def fill_get(self):
        "@rtype: tuple of int"
        cdef int x, y, w, h
        evas_object_image_fill_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def fill_set(self, int x, int y, int w, int h):
        """Sets the rectangle that the image will be drawn to.

        Note that the image will be B{tiled} around this one rectangle.
        To have only one copy of the image drawn, B{x} and B{y} must be
        0 and B{w} and B{h} need to be the width and height of the object
        respectively.

        The default values for the fill parameters is B{x} = 0, B{y} = 0,
        B{w} = 1 and B{h} = 1.

        @parm: B{x}
        @parm: B{y}
        @parm: B{w}
        @parm: B{h}
        """
        evas_object_image_fill_set(self.obj, x, y, w, h)

    property fill:
        def __get__(self):
            return self.fill_get()

        def __set__(self, spec):
            self.fill_set(*spec)

    def image_size_get(self):
        """Returns the original size of the image being displayed.

        @rtype: tuple of int
        """
        cdef int w, h
        evas_object_image_size_get(self.obj, &w, &h)
        return (w, h)

    def image_size_set(self, int w, int h):
        """Sets the size of the image to be displayed.

        This function will scale down or crop the image so that it is
        treated as if it were at the given size.  If the size given is
        smaller than the image, it will be cropped.  If the size given is
        larger, then the image will be treated as if it were in the upper
        left hand corner of a larger image that is otherwise transparent.

        @parm: B{w}
        @parm: B{h}
        """
        evas_object_image_size_set(self.obj, w, h)

    property image_size:
        def __get__(self):
            return self.image_size_get()

        def __set__(self, spec):
            self.image_size_set(*spec)

    def alpha_get(self):
        "@rtype: bool"
        return bool(evas_object_image_alpha_get(self.obj))

    def alpha_set(self, int value):
        "Enable or disable alpha channel."
        evas_object_image_alpha_set(self.obj, value)

    property alpha:
        def __get__(self):
            return self.alpha_get()

        def __set__(self, int value):
            self.alpha_set(value)

    def smooth_scale_get(self):
        "@rtype: bool"
        return bool(evas_object_image_smooth_scale_get(self.obj))

    def smooth_scale_set(self, int value):
        "Enable or disalbe smooth scaling."
        evas_object_image_smooth_scale_set(self.obj, value)

    property smooth_scale:
        def __get__(self):
            return self.smooth_scale_get()

        def __set__(self, int value):
            self.smooth_scale_set(value)

    def pixels_dirty_get(self):
        "@rtype: bool"
        return bool(evas_object_image_pixels_dirty_get(self.obj))

    def pixels_dirty_set(self, int value):
        "Mark or unmark pixels as dirty."
        evas_object_image_pixels_dirty_set(self.obj, value)

    property pixels_dirty:
        def __get__(self):
            return self.pixels_dirty_get()

        def __set__(self, int value):
            self.pixels_dirty_set(value)

    def load_dpi_get(self):
        "@rtype: float"
        return evas_object_image_load_dpi_get(self.obj)

    def load_dpi_set(self, double value):
        "Set dots-per-inch to be used at image load time."
        evas_object_image_load_dpi_set(self.obj, value)

    property load_dpi:
        def __get__(self):
            return self.load_dpi_get()

        def __set__(self, int value):
            self.load_dpi_set(value)

    def load_size_get(self):
        "@rtype: tuple of int"
        cdef int w, h
        evas_object_image_load_size_get(self.obj, &w, &h)
        return (w, h)

    def load_size_set(self, int w, int h):
        """Set size you want image loaded.

        Loads image to the desired size, saving memory when loading large
        files.

        @parm: B{w}
        @parm: B{h}
        """
        evas_object_image_load_size_set(self.obj, w, h)

    property load_size:
        def __get__(self):
            return self.load_size_get()

        def __set__(self, spec):
            self.load_size_set(*spec)

    def load_scale_down_get(self):
        "@rtype: int"
        return evas_object_image_load_scale_down_get(self.obj)

    def load_scale_down_set(self, int value):
        "Set scale down loaded image by the given amount."
        evas_object_image_load_scale_down_set(self.obj, value)

    property load_scale_down:
        def __get__(self):
            return self.load_scale_down_get()

        def __set__(self, int value):
            self.load_scale_down_set(value)

    def colorspace_get(self):
        "@rtype: int"
        return evas_object_image_colorspace_get(self.obj)

    def colorspace_set(self, int value):
        """Set the colorspace of image data (pixels).

        May be one of (subject to engine implementation):
         - B{EVAS_COLORSPACE_ARGB8888} ARGB 32 bits per pixel, high-byte is
           Alpha, accessed 1 32bit word at a time.
         - B{EVAS_COLORSPACE_YCBCR422P601_PL} YCbCr 4:2:2 Planar, ITU.BT-601
           specifications. The data poitned to is just an array of row
           pointer, pointing to the Y rows, then the Cb, then Cr rows.
         - B{EVAS_COLORSPACE_YCBCR422P709_PL} YCbCr 4:2:2 Planar, ITU.BT-709
           specifications. The data poitned to is just an array of row
           pointer, pointing to the Y rows, then the Cb, then Cr rows.
         - B{EVAS_COLORSPACE_RGB565_A5P} 16bit rgb565 + Alpha plane at end -
           5 bits of the 8 being used per alpha byte.
        """
        evas_object_image_colorspace_set(self.obj, <Evas_Colorspace>value)

    property colorspace:
        def __get__(self):
            return self.colorspace_get()

        def __set__(self, int value):
            self.colorspace_set(value)

    def reload(self):
        "Force reload of image data."
        evas_object_image_reload(self.obj)

    def save(self, char *filename, key=None, flags=None):
        """Save image to file.

        @parm: B{filename} where to save.
        @parm: B{key} some formats may require a key, EET for example.
        @parm: B{flags} string of extra flags (separated by space), like
          "quality=85 compress=9".
        """
        cdef char *k, *f
        if key:
            k = key
        else:
            k = NULL

        if flags:
            f = flags
        else:
            f = NULL
        evas_object_image_save(self.obj, filename, key, flags)



cdef void _cb_on_filled_image_resize(void *data, Evas *e,
                                     Evas_Object *obj,
                                     void *event_info) with GIL:
    cdef int w, h
    evas_object_geometry_get(obj, NULL, NULL, &w, &h)
    evas_object_image_fill_set(obj, 0, 0, w, h)

cdef class FilledImage(Image):
    """Image that automatically resize it's contents to fit object size.

    This L{Image} subclass already calls L{Image.fill_set()} on resize so
    it will match and so be scaled to fill the whole area.
    """
    def __init__(self, Canvas canvas not None, **kargs):
        Image.__init__(self, canvas, **kargs)
        evas_object_event_callback_add(self.obj, EVAS_CALLBACK_RESIZE,
                                       _cb_on_filled_image_resize, NULL)

    def fill_set(self, int x, int y, int w, int h):
        "Not available for this class."
        raise NotImplementedError("FilledImage doesn't support fill_set()")

