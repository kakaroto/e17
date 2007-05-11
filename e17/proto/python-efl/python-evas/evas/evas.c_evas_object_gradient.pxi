# This file is included verbatim by c_evas.pyx

cdef class Gradient(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_gradient_add(self._evas.obj))

    def color_stop_add(self, int r, int g, int b, int a, int delta):
        """Adds a color stop to the given evas gradient object.

        The delta parameter determines the proportion of the gradient
        object that is to be set to the color.  For instance, if red is
        added with delta set to 2, and green is added with delta set to
        1, two-thirds will be red or reddish and one-third will be green
        or greenish.

        Colors are added from the top downwards.
        """
        evas_object_gradient_color_stop_add(self.obj, r, g, b, a, delta)

    def alpha_stop_add(self, int a, int delta):
        """Adds an alpha stop to the given evas gradient object.

        The delta parameter determines the proportion of the gradient
        object that is to be set to the alpha value.

        Alphas are added from the top downwards.
        """
        evas_object_gradient_alpha_stop_add(self.obj, a, delta)

    def clear(self):
        evas_object_gradient_clear(self.obj)

    def gradient_type_get(self):
        cdef char *st, *si
        evas_object_gradient_type_get(self.obj, &st, &si)
        if st == NULL:
            type = None
        else:
            type = st
        if si == NULL:
            instance_params = None
        else:
            instance_params = si
        return (type, instance_params)

    def gradient_type_set(self, char *type, instance_params=None):
        """Set a gradient's geometric type.

        Examples are "linear", "linear.diag", "linear.codiag", "radial",
        "rectangular", "angular", "sinusoidal", ...

        Some types may accept additional parameters to further specify
        the look.
        """
        cdef char *si
        if instance_params is None:
            si = NULL
        elif isinstance(instance_params, basestring):
            si = instance_params
        else:
            raise ValueError("instance_params must be either string or None")
        evas_object_gradient_type_set(self.obj, type, si)

    property gradient_type:
        def __get__(self):
            return self.gradient_type_get()

        def __set__(self, spec):
            self.gradient_type_set(*spec)

    def fill_get(self):
        cdef int x, y, w, h
        evas_object_gradient_fill_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def fill_set(self, int x, int y, int w, int h):
        """Sets the rectangle on the gradient object that the gradient
        will be drawn to.

        Note that the gradient may be tiled around this one rectangle,
        according to its spread value - restrict, repeat, or reflect.
        To have only one 'cycle' of the gradient drawn, the spread value
        must be set to restrict, or 'x' and 'y' must be 0 and 'w' and 'h'
        need to be the width and height of the gradient object respectively.

        The default values for the fill parameters is x = 0, y = 0,
        w = 32 and h = 32.
        """
        evas_object_gradient_fill_set(self.obj, x, y, w, h)

    property fill:
        def __get__(self):
            return self.fill_get()

        def __set__(self, spec):
            self.fill_set(*spec)

    def fill_angle_get(self):
        return evas_object_gradient_fill_angle_get(self.obj)

    def fill_angle_set(self, int value):
        """Sets the angle at which the given evas gradient object's fill
        sits clockwise from vertical.
        """
        evas_object_gradient_fill_angle_set(self.obj, value)

    property fill_angle:
        def __get__(self):
            return self.fill_angle_get()

        def __set__(self, int value):
            self.fill_angle_set(value)

    def fill_spread_get(self):
        """Retrieves the spread (tiling mode) for the given gradient object's
        fill.
        """
        return evas_object_gradient_fill_spread_get(self.obj)

    def fill_spread_set(self, int value):
        """Sets the tiling mode for the given evas gradient object's fill.

        value can be:
         * EVAS_TEXTURE_REFLECT: tiling reflects.
         * EVAS_TEXTURE_REPEAT: tiling repeats.
         * EVAS_TEXTURE_RESTRICT: tiling clamps - range offset ignored.
         * EVAS_TEXTURE_RESTRICT_REFLECT: tiling clamps and any range offset
           reflects.
         * EVAS_TEXTURE_RESTRICT_REPEAT: tiling clamps and any range offset
           repeats.
         * EVAS_TEXTURE_PAD: tiling extends with end values.
         """
        evas_object_gradient_fill_spread_set(self.obj, value)

    property fill_spread:
        def __get__(self):
            return self.fill_spread_get()

        def __set__(self, int value):
            self.fill_spread_set(value)

    def angle_get(self):
        """Retrieves the angle at which the given evas gradient object sits
        rel to its intrinsic orientation.
        """
        return evas_object_gradient_angle_get(self.obj)

    def angle_set(self, int value):
        """Sets the angle at which the given evas gradient sits,
        relative to whatever intrinsic orientation of the grad type.

        Used mostly by 'linear' kinds of gradients.
        """
        evas_object_gradient_angle_set(self.obj, value)

    property angle:
        def __get__(self):
            return self.angle_get()

        def __set__(self, int value):
            self.angle_set(value)

    def direction_get(self):
        return evas_object_gradient_direction_get(self.obj)

    def direction_set(self, int value):
        """Sets the direction of the given evas gradient object's spectrum.

        value can be either 1 (default) or -1.
        """
        evas_object_gradient_direction_set(self.obj, value)

    property direction:
        def __get__(self):
            return self.direction_get()

        def __set__(self, int value):
            self.direction_set(value)

    def offset_get(self):
        return evas_object_gradient_offset_get(self.obj)

    def offset_set(self, float value):
        """Sets the offset of the given evas gradient object's spectrum.

        value can be negative.
        """
        evas_object_gradient_offset_set(self.obj, value)

    property offset:
        def __get__(self):
            return self.offset_get()

        def __set__(self, float value):
            self.offset_set(value)
