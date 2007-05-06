# This file is included verbatim by c_evas.pyx

cdef class Image(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_image_add(self._evas.obj))

    def _set_common_params(self, file=None, size=None, pos=None, geometry=None,
                           color=None, name=None):
        if file:
            if isinstance(file, basestring):
                file = (file, None)
            self.file_set(*file)
        Object._set_common_params(self, size=size, pos=pos, geometry=geometry,
                                  color=color, name=name)

    def file_set(self, char *filename, key=None):
        cdef char *k
        cdef int err
        if key:
            k = key
        else:
            k = NULL
        evas_object_image_file_set(self.obj, filename, k)
        err = evas_object_image_load_error_get(self.obj)
        if err != EVAS_LOAD_ERROR_NONE:
            raise EvasLoadError(err)

    def file_get(self):
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
        return evas_object_image_load_error_get(self.obj)

    property load_error:
        def __get__(self):
            return self.load_error_get()

    def border_get(self):
        cdef int left, right, top, bottom
        evas_object_image_border_get(self.obj, &left, &right, &top, &bottom)
        return (left, right, top, bottom)

    def border_set(self, int left, int right, int top, int bottom):
        evas_object_image_border_set(self.obj, left, right, top, bottom)

    property border:
        def __get__(self):
            return self.border_get()

        def __set__(self, spec):
            self.border_set(*spec)

    def border_center_fill_get(self):
        return bool(evas_object_image_border_center_fill_get(self.obj))

    def border_center_fill_set(self, int value):
        evas_object_image_border_center_fill_set(self.obj, value)

    property border_center_fill:
        def __get__(self):
            return self.border_center_fill_get()

        def __set__(self, int value):
            self.border_center_fill_set(value)

    def fill_get(self):
        cdef int x, y, w, h
        evas_object_image_fill_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def fill_set(self, int x, int y, int w, int h):
        evas_object_image_fill_set(self.obj, x, y, w, h)

    property fill:
        def __get__(self):
            return self.fill_get()

        def __set__(self, spec):
            self.fill_set(*spec)

    def image_size_get(self):
        cdef int w, h
        evas_object_image_size_get(self.obj, &w, &h)
        return (w, h)

    def image_size_set(self, int w, int h):
        evas_object_image_size_set(self.obj, w, h)

    property image_size:
        def __get__(self):
            return self.image_size_get()

        def __set__(self, spec):
            self.image_size_set(*spec)

    def alpha_get(self):
        return bool(evas_object_image_alpha_get(self.obj))

    def alpha_set(self, int value):
        evas_object_image_alpha_set(self.obj, value)

    property alpha:
        def __get__(self):
            return self.alpha_get()

        def __set__(self, int value):
            self.alpha_set(value)

    def smooth_scale_get(self):
        return bool(evas_object_image_smooth_scale_get(self.obj))

    def smooth_scale_set(self, int value):
        evas_object_image_smooth_scale_set(self.obj, value)

    property smooth_scale:
        def __get__(self):
            return self.smooth_scale_get()

        def __set__(self, int value):
            self.smooth_scale_set(value)

    def pixels_dirty_get(self):
        return bool(evas_object_image_pixels_dirty_get(self.obj))

    def pixels_dirty_set(self, int value):
        evas_object_image_pixels_dirty_set(self.obj, value)

    property pixels_dirty:
        def __get__(self):
            return self.pixels_dirty_get()

        def __set__(self, int value):
            self.pixels_dirty_set(value)

    def load_dpi_get(self):
        return evas_object_image_load_dpi_get(self.obj)

    def load_dpi_set(self, double value):
        evas_object_image_load_dpi_set(self.obj, value)

    property load_dpi:
        def __get__(self):
            return self.load_dpi_get()

        def __set__(self, int value):
            self.load_dpi_set(value)

    def load_size_get(self):
        cdef int w, h
        evas_object_image_load_size_get(self.obj, &w, &h)
        return (w, h)

    def load_size_set(self, int w, int h):
        evas_object_image_load_size_set(self.obj, w, h)

    property load_size:
        def __get__(self):
            return self.load_size_get()

        def __set__(self, spec):
            self.load_size_set(*spec)

    def load_scale_down_get(self):
        return evas_object_image_load_scale_down_get(self.obj)

    def load_scale_down_set(self, int value):
        evas_object_image_load_scale_down_set(self.obj, value)

    property load_scale_down:
        def __get__(self):
            return self.load_scale_down_get()

        def __set__(self, int value):
            self.load_scale_down_set(value)

    def colorspace_get(self):
        return evas_object_image_colorspace_get(self.obj)

    def colorspace_set(self, int value):
        evas_object_image_colorspace_set(self.obj, <Evas_Colorspace>value)

    property colorspace:
        def __get__(self):
            return self.colorspace_get()

        def __set__(self, int value):
            self.colorspace_set(value)

    def reload(self):
        evas_object_image_reload(self.obj)

    def save(self, char *filename, key=None, flags=None):
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
