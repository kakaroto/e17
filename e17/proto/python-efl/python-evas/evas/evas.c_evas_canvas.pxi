# This file is included verbatim by c_evas.pyx

cdef public class Canvas [object PyEvasCanvas, type PyEvasCanvas_Type]:
    def __new__(self):
        self.obj = NULL

    def __dealloc__(self):
        if self.obj:
            evas_free(self.obj)
            self.obj = NULL

    cdef int _set_obj(self, Evas *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        return 1

    # XXX: this should be C-only, but it would require ecore_evas
    # XXX: being able to use it.
    def _unset_obj(self):
        "Remove internally wrapped Evas* object."
        self.obj = NULL

    def _new_evas(self):
        if self.obj == NULL:
            self._set_obj(evas_new())

    def __str__(self):
        return "%s(0x%x, refcount=%d, Evas_Object=0x%x)" % \
               (self.__class__.__name__, <unsigned long>self,
                python.REFCOUNT(self), <unsigned long>self.obj)

    def output_method_set(self, method):
        "Set canvas render method, can be either a name or id."
        cdef int engine_id

        if isinstance(method, (int, long)):
            engine_id = method
        elif isinstance(method, basestring):
            engine_id = evas_render_method_lookup(method)
        else:
            raise TypeError("method must be integer or string")

        evas_output_method_set(self.obj, engine_id)

    def output_method_get(self):
        "Returns the id of the current output method, or 0 if error happened."
        return evas_output_method_get(self.obj)

    property output_method:
        def __set__(self, method):
            self.output_method_set(method)

        def __get__(self):
            return self.output_method_get()

    def size_set(self, int w, int h):
        evas_output_size_set(self.obj, w, h)

    def size_get(self):
        cdef int w, h
        evas_output_size_get(self.obj, &w, &h)
        return (w, h)

    property size:
        def __set__(self, spec):
            self.size_set(*spec)

        def __get__(self):
            return self.size_get()

    property rect:
        def __set__(self, spec):
            cdef Rect r
            r = Rect(spec)
            self.size_set(r.w, r.h)

        def __get__(self):
            cdef int w, h
            w, h = self.size_get()
            return Rect(0, 0, w, h)

    def viewport_set(self, int x, int y, int w, int h):
        evas_output_viewport_set(self.obj, x, y, w, h)

    def viewport_get(self):
        cdef int x, y, w, h
        evas_output_viewport_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    property viewport:
        def __set__(self, spec):
            self.viewport_set(*spec)

        def __get__(self):
            return self.viewport_get()

    def coord_screen_x_to_world(self, int x):
        return evas_coord_screen_x_to_world(self.obj, x)

    def coord_screen_y_to_world(self, int y):
        return evas_coord_screen_y_to_world(self.obj, y)

    def coord_world_x_to_screen(self, int x):
        return evas_coord_world_x_to_screen(self.obj, x)

    def coord_world_y_to_screen(self, int y):
        return evas_coord_world_y_to_screen(self.obj, y)

    def pointer_output_xy_get(self):
        cdef int x, y
        evas_pointer_output_xy_get(self.obj, &x, &y)
        return (x, y)

    property pointer_output_xy:
        def __get__(self):
            return self.pointer_output_xy_get()

    def pointer_canvas_xy_get(self):
        cdef int x, y
        evas_pointer_canvas_xy_get(self.obj, &x, &y)
        return (x, y)

    property pointer_canvas_xy:
        def __get__(self):
            return self.pointer_canvas_xy_get()

    def pointer_button_down_mask_get(self):
        return evas_pointer_button_down_mask_get(self.obj)

    property pointer_button_down_mask:
        def __get__(self):
            return self.pointer_button_down_mask_get()

    def pointer_inside(self):
        return bool(evas_pointer_inside_get(self.obj))

    def top_at_xy_get(self, int x, int y,
                      include_pass_events_objects=False,
                      include_hidden_objects=False):
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        o = evas_object_top_at_xy_get(self.obj, x, y, ip, ih)
        return Object_from_instance(o)

    def top_in_rectangle_get(self, int x, int y, int w, int h,
                             include_pass_events_objects=False,
                             include_hidden_objects=False):
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        o = evas_object_top_in_rectangle_get(self.obj, x, y, w, h, ip, ih)
        return Object_from_instance(o)

    def objects_at_xy_get(self, int x, int y,
                             include_pass_events_objects=False,
                             include_hidden_objects=False):
        cdef Evas_List *objs, *itr
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        objs = evas_objects_at_xy_get(self.obj, x, y, ip, ih)
        lst = []
        itr = objs
        while itr != NULL:
            o = <Evas_Object*>itr.data
            lst.append(Object_from_instance(o))
            itr = itr.next
        evas_list_free(objs)
        return lst

    def objects_in_rectangle_get(self, int x, int y, int w, int h,
                             include_pass_events_objects=False,
                             include_hidden_objects=False):
        cdef Evas_List *objs, *itr
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        objs = evas_objects_in_rectangle_get(self.obj, x, y, w, h, ip, ih)
        lst = []
        itr = objs
        while itr != NULL:
            o = <Evas_Object*>itr.data
            lst.append(Object_from_instance(o))
            itr = itr.next
        evas_list_free(objs)
        return lst

    def damage_rectangle_add(self, int x, int y, int w, int h):
        evas_damage_rectangle_add(self.obj, x, y, w, h)

    def obscured_rectangle_add(self, int x, int y, int w, int h):
        evas_obscured_rectangle_add(self.obj, x, y, w, h)

    def obscured_clear(self):
        evas_obscured_clear(self.obj)

    def render_updates(self):
        cdef Evas_List *lst

        lst = evas_render_updates(self.obj)
        evas_render_updates_free(lst)

    def render(self):
        evas_render(self.obj)

    def norender(self):
        evas_norender(self.obj)

    def top_get(self):
        cdef Evas_Object *other
        other = evas_object_top_get(self.obj)
        return Object_from_instance(other)

    property top:
        def __get__(self):
            return self.top_get()

    def bottom_get(self):
        cdef Evas_Object *other
        other = evas_object_bottom_get(self.obj)
        return Object_from_instance(other)

    property bottom:
        def __get__(self):
            return self.bottom_get()

    def focus_get(self):
        cdef Evas_Object *o
        o = evas_focus_get(self.obj)
        return Object_from_instance(o)

    property focus:
        def __get__(self):
            return self.focus_get()

    def object_name_find(self, char *name):
        cdef Evas_Object *other
        other = evas_object_name_find(self.obj, name)
        return Object_from_instance(other)

    def image_cache_flush(self):
        evas_image_cache_flush(self.obj)

    def image_cache_reload(self):
        evas_image_cache_reload(self.obj)

    def image_cache_set(self, int size):
        evas_image_cache_set(self.obj, size)

    def image_cache_get(self):
        evas_image_cache_get(self.obj)

    property image_cache:
        def __get__(self):
            return self.image_cache_get()

        def __set__(self, int value):
            self.image_cache_set(value)

    def font_cache_flush(self):
        evas_font_cache_flush(self.obj)

    def font_cache_get(self):
        return evas_font_cache_get(self.obj)

    def font_cache_set(self, int value):
        evas_font_cache_set(self.obj, value)

    property font_cache:
        def __get__(self):
            return self.font_cache_get()

        def __set__(self, int value):
            self.font_cache_set(value)

    def font_path_clear(self):
        evas_font_path_clear(self.obj)

    def font_path_append(self, char *path):
        evas_font_path_append(self.obj, path)

    def font_path_prepend(self, char *path):
        evas_font_path_prepend(self.obj, path)

    def font_path_list(self):
        cdef Evas_List *itr
        lst = []
        itr = evas_font_path_list(self.obj)
        while itr != NULL:
            lst.append(<char*>itr.data)
            itr = itr.next
        return lst

    def font_available_list(self):
        cdef void *p
        cdef Evas_List *itr, *head
        lst = []
        p = <void*>evas_font_available_list(self.obj) # avoid warning
        head = <Evas_List*>p
        itr = head
        while itr != NULL:
            lst.append(<char*>itr.data)
            itr = itr.next
        evas_font_available_list_free(self.obj, head)
        return lst

    def font_hinting_can_hint(self, int flags):
        return bool(evas_font_hinting_can_hint(self.obj,
                                               <Evas_Font_Hinting_Flags>flags))

    def font_hinting_set(self, int flags):
        evas_font_hinting_set(self.obj, <Evas_Font_Hinting_Flags>flags)

    def font_hinting_get(self):
        return <int>evas_font_hinting_get(self.obj)

    property font_hinting:
        def __get__(self):
            return self.font_hinting_get()

        def __set__(self, int value):
            self.font_hinting_set(value)

    def freeze(self):
        evas_event_freeze(self.obj)

    def thaw(self):
        evas_event_thaw(self.obj)

    def freeze_get(self):
        return evas_event_freeze_get(self.obj)

    # Factory
    def Rectangle(self, size=None, pos=None, geometry=None, color=None,
                  name=None):
        obj = Rectangle(self)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj

    def Line(self, start=None, end=None, size=None, pos=None,
             geometry=None, color=None, name=None):
        obj = Line(self)
        obj._new_obj()
        obj._set_common_params(start=start, end=end, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj


    def Image(self, file=None, size=None, pos=None, geometry=None,
              color=None, name=None):
        obj = Image(self)
        obj._new_obj()
        obj._set_common_params(file=file, size=size, pos=pos,
                               geometry=geometry, color=color, name=name)
        return obj

    def Gradient(self, size=None, pos=None, geometry=None, color=None,
                 name=None):
        obj = Gradient(self)
        obj._new_obj()
        obj._set_common_params(size=size, pos=pos, geometry=geometry,
                               color=color, name=name)
        return obj
