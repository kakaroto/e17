cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Epsilon.h":
    cdef enum Epsilon_Return_Value:
        EPSILON_FAIL = 0
        EPSILON_OK = 1

    cdef struct _Epsilon:
        char *hash
        char *src
        char *thumb
        char *key
        int w
        int h
        int tw
        int th

    ctypedef void Epsilon_Exif_Info

    cdef struct _Epsilon_Info:
        char *uri
        unsigned long long int mtime
        int w
        int h
        char *mimetype
        Epsilon_Exif_Info *eei


    int epsilon_init()

    _Epsilon *epsilon_new(char *file)
    void epsilon_free(_Epsilon *e)
    void epsilon_key_set(_Epsilon *e, char *key)
    void epsilon_resolution_set(_Epsilon *e, int w, int h)

    char *epsilon_file_get(_Epsilon *e)
    char *epsilon_thumb_file_get(_Epsilon *e)
    int epsilon_exists(_Epsilon *e)
    int epsilon_generate(_Epsilon *e)
    void epsilon_thumb_size(_Epsilon *e, int size)

    _Epsilon_Info *epsilon_info_get (_Epsilon *e)

    void epsilon_info_free(_Epsilon_Info *ei)
    int epsilon_info_exif_props_as_int_get(_Epsilon_Info *ei, unsigned short lvl, long prop)
    char *epsilon_info_exif_props_as_string_get(_Epsilon_Info *ei, unsigned short lvl, long prop)

    void epsilon_info_exif_props_print(_Epsilon_Info *ei)
    int epsilon_info_exif_get(_Epsilon_Info *ei)


cdef class Epsilon:
    cdef _Epsilon *obj
