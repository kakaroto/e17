# This file is included verbatim by c_evas.pyx

cdef class Text(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_text_add(self._evas.obj))

    def _set_common_params(self, text=None, font=None, font_source=None,
                           style=None, shadow_color=None, glow_color=None,
                           glow2_color=None, outline_color=None, size=None,
                           pos=None, geometry=None, color=None, name=None):
        Object._set_common_params(self, size=size, pos=pos, geometry=geometry,
                                  color=color, name=name)
        if text is not None:
            self.text_set(text)
        if font:
            if not isinstance(font, (tuple, list)):
                font = (font,)
            self.font_set(*font)
        if font_source:
            self.font_source_set(font_source)
        if style is not None:
            self.style_set(style)
        if shadow_color is not None:
            self.shadow_color_set(*color_parse(shadow_color))
        if glow_color is not None:
            self.glow_color_set(*color_parse(glow_color))
        if glow2_color is not None:
            self.glow2_color_set(*color_parse(glow2_color))
        if outline_color is not None:
            self.outline_color_set(*color_parse(outline_color))

    def font_source_get(self):
        cdef char *s
        s = evas_object_text_font_source_get(self.obj)
        if s != NULL:
            return s

    def font_source_set(self, char *value):
        evas_object_text_font_source_set(self.obj, value)

    property font_source:
        def __get__(self):
            return self.font_source_get()

        def __set__(self, char *value):
            self.font_source_set(value)

    def font_get(self):
        cdef char *f
        cdef int size
        evas_object_text_font_get(self.obj, &f, &size)
        if f == NULL:
            font = None
        else:
            font = f
        return (font, size)

    def font_set(self, char *font, int size=10):
        evas_object_text_font_set(self.obj, font, size)

    property font:
        def __get__(self):
            return self.font_get()

        def __set__(self, spec):
            if not isinstance(spec, (tuple, list)):
                spec = (spec,)
            self.font_set(*spec)

    def text_get(self):
        cdef char *s
        s = evas_object_text_text_get(self.obj)
        if s != NULL:
            return s

    def text_set(self, char *value):
        evas_object_text_text_set(self.obj, value)

    property text:
        def __get__(self):
            return self.text_get()

        def __set__(self, char *value):
            self.text_set(value)

    def ascent_get(self):
        return evas_object_text_ascent_get(self.obj)

    property ascent:
        def __get__(self):
            return self.ascent_get()

    def descent_get(self):
        return evas_object_text_descent_get(self.obj)

    property descent:
        def __get__(self):
            return self.descent_get()

    def max_ascent_get(self):
        return evas_object_text_max_ascent_get(self.obj)

    property max_ascent:
        def __get__(self):
            return self.max_ascent_get()

    def max_descent_get(self):
        return evas_object_text_max_descent_get(self.obj)

    property max_descent:
        def __get__(self):
            return self.max_descent_get()

    def horiz_advance_get(self):
        return evas_object_text_horiz_advance_get(self.obj)

    property horiz_advance:
        def __get__(self):
            return self.horiz_advance_get()

    def vert_advance_get(self):
        return evas_object_text_vert_advance_get(self.obj)

    property vert_advance:
        def __get__(self):
            return self.vert_advance_get()

    def inset_get(self):
        return evas_object_text_inset_get(self.obj)

    property inset:
        def __get__(self):
            return self.inset_get()

    def char_pos_get(self, int char_index):
        cdef int x, y, w, h, r
        r = evas_object_text_char_pos_get(self.obj, char_index, &x, &y, &w, &h)
        if r == 0:
            return None
        else:
            return (x, y, w, h)


    def char_coords_get(self, int x, int y):
        cdef int cx, cy, cw, ch, c
        c = evas_object_text_char_coords_get(self.obj, x, y,
                                             &cx, &cy, &cw, &ch)
        if c < 0:
            return None
        else:
            return ("%c" % c, cx, cy, cw, ch)

    def style_get(self):
        return evas_object_text_style_get(self.obj)

    def style_set(self, int value):
        evas_object_text_style_set(self.obj, <Evas_Text_Style_Type>value)

    property style:
        def __get__(self):
            return self.style_get()

        def __set__(self, int value):
            self.style_set(value)

    def shadow_color_get(self):
        cdef int r, g, b, a
        evas_object_text_shadow_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    def shadow_color_set(self, int r, int g, int b, int a):
        evas_object_text_shadow_color_set(self.obj, r, g, b, a)

    property shadow_color:
        def __get__(self):
            return self.shadow_color_get()

        def __set__(self, spec):
            self.shadow_color_set(*spec)

    def glow_color_get(self):
        cdef int r, g, b, a
        evas_object_text_glow_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    def glow_color_set(self, int r, int g, int b, int a):
        evas_object_text_glow_color_set(self.obj, r, g, b, a)

    property glow_color:
        def __get__(self):
            return self.glow_color_get()

        def __set__(self, spec):
            self.glow_color_set(*spec)

    def glow2_color_get(self):
        cdef int r, g, b, a
        evas_object_text_glow2_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    def glow2_color_set(self, int r, int g, int b, int a):
        evas_object_text_glow2_color_set(self.obj, r, g, b, a)

    property glow2_color:
        def __get__(self):
            return self.glow2_color_get()

        def __set__(self, spec):
            self.glow2_color_set(*spec)

    def outline_color_get(self):
        cdef int r, g, b, a
        evas_object_text_outline_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    def outline_color_set(self, int r, int g, int b, int a):
        evas_object_text_outline_color_set(self.obj, r, g, b, a)

    property outline_color:
        def __get__(self):
            return self.outline_color_get()

        def __set__(self, spec):
            self.outline_color_set(*spec)

    def style_pad_get(self):
        cdef int l, r, t, b
        evas_object_text_style_pad_get(self.obj, &l, &r, &t, &b)
        return (l, r, t, b)

    property style_pad:
        def __get__(self):
            return self.style_pad_get()

