cimport python

def init():
    return evas_init()


def shutdown():
    return evas_shutdown()


def render_method_lookup(char *name):
    "Lookup render method and return its id (> 0 if found)."
    return evas_render_method_lookup(name)


def render_method_list():
    "Returns a list of render method names."
    cdef Evas_List *lst

    ret = []
    lst = evas_render_method_list()
    while lst != NULL:
        ret.append(<char*> lst.data)
        lst = lst.next

    evas_render_method_list_free(lst)
    return ret


cdef Canvas Canvas_from_instance(Evas *evas):
    cdef void *data
    cdef Canvas c

    if evas == NULL:
        return None

    c = Canvas()
    c._set_obj(evas)

    return c


# XXX: this should be C-only, but it would require ecore_evas
# XXX: being able to use it.
def _Canvas_from_instance(long ptr):
    return Canvas_from_instance(<Evas *>ptr)


cdef Object Object_from_instance(Evas_Object *obj):
    cdef void *data
    cdef Object o

    if obj == NULL:
        return None

    data = evas_object_data_get(obj, "python-evas")
    if data != NULL:
        o = <Object>data
    else:
        o = Object(Canvas_from_instance(evas_object_evas_get(obj)))
        o._set_obj(obj)

    return o


# XXX: this should be C-only, but it would require edje
# XXX: being able to use it.
def _Object_from_instance(long ptr):
    return Object_from_instance(<Evas_Object *>ptr)


def color_parse(desc, is_premul=None):
    """Converts a color description to (r, g, b, a) in pre-multiply form.

    is_premul default value will depend on desc type:
       * desc is string: is_premul=False
       * desc is integer: is_premul=False
       * desc is tuple: is_premul=True

    @param desc: can be either a string, an integer or a tuple.
    @param is_premul: specifies if the color is in pre-multiply form. This
                      is the format expected by evas.
    @return: (r, g, b, a) in pre-multiply form.
    """
    cdef unsigned long c, desc_len
    cdef int r, g, b, a

    r = 0
    g = 0
    b = 0
    a = 0

    if isinstance(desc, basestring):
        if not desc or desc[0] != "#":
            raise ValueError("Invalid color description")
        desc_len = len(desc)
        c = int(desc[1:], 16)
        r = (c >> 16) & 0xff
        g = (c >> 8) & 0xff
        b = c & 0xff

        if is_premul is None:
            is_premul = False

        if desc_len == 9:
            a = (c >> 24) & 0xff
        elif desc_len == 7:
            a = 255
        else:
            raise ValueError("Invalid color description")

    elif isinstance(desc, (int, long)):
        c = desc
        a = (c >> 24) & 0xff
        r = (c >> 16) & 0xff
        g = (c >> 8) & 0xff
        b = c & 0xff

        if is_premul is None:
            is_premul = False

    elif isinstance(desc, (list, tuple)):
        if is_premul is None:
            is_premul = True

        if len(desc) == 3:
            a = 255
            r, g, b = desc
        else:
            r, g, b, a = desc
    else:
        raise TypeError("Unsupported type %s for color description." %
                        type(desc))

    if is_premul is False:
        evas_color_argb_premul(a, &r, &g, &b)

    return (r, g, b, a)


def color_argb_premul(int r, int g, int b, int a):
    evas_color_argb_premul(a, &r, &g, &b)
    return (r, g, b, a)

def color_argb_unpremul(int r, int g, int b, int a):
    evas_color_argb_unpremul(a, &r, &g, &b)
    return (r, g, b, a)

def color_hsv_to_rgb(float h, float s, float v):
    cdef int r, g, b
    evas_color_hsv_to_rgb(h, s, v, &r, &g, &b)
    return (r, g, b)

def color_rgb_to_hsv(int r, int g, int b):
    cdef float h, s, v
    evas_color_rgb_to_hsv(r, g, b, &h, &s, &v)
    return (h, s, v)


include "evas.c_evas_rect.pxi"
include "evas.c_evas_canvas.pxi"
include "evas.c_evas_object_events.pxi"
include "evas.c_evas_object_callbacks.pxi"
include "evas.c_evas_object.pxi"
include "evas.c_evas_object_smart.pxi"
include "evas.c_evas_object_rectangle.pxi"
include "evas.c_evas_object_line.pxi"
include "evas.c_evas_object_image.pxi"
