# This file is included verbatim by c_ecore_evas.pyx

cdef class Buffer(EcoreEvas):
    def __init__(self, int w=320, int h=240):
        cdef Ecore_Evas *obj

        if self.obj == NULL:
            obj = ecore_evas_buffer_new(w, h)
            self._set_obj(obj)

    cdef int _get_buf_size(self):
        cdef int w, h
        ecore_evas_geometry_get(self.obj, NULL, NULL, &w, &h);
        return w * h * sizeof(int)

    def __getreadbuffer__(self, int segment, void **ptrptr):
        cdef void *buf
        if segment != 0:
            return -1
        if ptrptr != NULL:
            buf = <void*>ecore_evas_buffer_pixels_get(self.obj);
            ptrptr[0] = buf
        return self._get_buf_size()

    def __getsegcount__(self, int *lenp):
        if lenp != NULL:
            lenp[0] = self._get_buf_size()
        return 1

    def __getitem__(self, int i):
        cdef char *buf
        cdef int ln
        ln = self._get_buf_size()

        if i >= ln or i < -ln:
            raise IndexError("list index out of range")

        if i < 0:
            i = i % ln
            i = i + ln

        if ln == 0:
            return None
        buf = <char *>ecore_evas_buffer_pixels_get(self.obj)
        return buf[i]

    def __len__(self):
        return self._get_buf_size()

    def __getslice__(self, int i, int j):
        cdef int ln
        ln = self._get_buf_size()
        if ln == 0:
            return []

        if i < 0:
            i = i % ln
            i = i + ln

        if j < 0:
            j = j % ln
            j = j + ln

        if j > ln:
            j = ln

        if i >= j:
            return []

        return python.PyBuffer_FromObject(self, i, j - i)
