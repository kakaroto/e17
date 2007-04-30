# This file is included verbatim by c_ecore_evas.pyx

cdef class FB(EcoreEvas):
    def __init__(self, char *display=NULL, int rotation=0, int w=320,
                 int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_fb_new(display, rotation, w, h)
            self._set_obj(obj)
