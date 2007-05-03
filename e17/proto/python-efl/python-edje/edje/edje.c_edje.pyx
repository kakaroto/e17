cimport python

def init():
    return edje_init()


def shutdown():
    return edje_shutdown()


def frametime_set(double t):
    edje_frametime_set(t)


def frametime_get():
    return edje_frametime_get()


def freeze():
    edje_freeze()


def thaw():
    edje_thaw()


def fontset_append_set(char *fonts):
    edje_fontset_append_set(fonts)


def fontset_append_get():
    return edje_fontset_append_get()


def file_collection_list(char *file):
    cdef evas.c_evas.Evas_List *lst, *itr
    ret = []
    lst = edje_file_collection_list(file)
    itr = lst
    while itr:
        ret.append(<char*>itr.data)
        itr = itr.next
    edje_file_collection_list_free(lst)
    return ret


def file_group_exists(char *file, char *group):
    return bool(edje_file_group_exists(file, group))


def file_data_get(char *file, char *key):
    return edje_file_data_get(file, key)


def file_cache_set(int count):
    edje_file_cache_set(count)


def file_cache_get():
    return edje_file_cache_get()


def file_cache_flush():
    edje_file_cache_flush()


def collection_cache_set(int count):
    edje_collection_cache_set(count)


def collection_cache_get():
    return edje_collection_cache_get()


def collection_cache_flush():
    edje_collection_cache_flush()


def color_class_set(char *color_class,
                    int r, int g, int b, int a,
                    int r2, int g2, int b2, int a2,
                    int r3, int g3, int b3, int a3):
    edje_color_class_set(color_class,
                         r, g, b, a,
                         r2, g2, b2, a2,
                         r3, g3, b3, a3)


def color_class_del(char *color_class):
    edje_color_class_del(color_class)


def color_class_list():
    cdef evas.c_evas.Evas_List *lst, *itr
    ret = []
    lst = edje_color_class_list()
    itr = lst
    while itr:
        ret.append(<char*>itr.data)
        python.free(itr.data)
        itr = itr.next
    evas.c_evas.evas_list_free(lst)
    return ret


def text_class_set(char *text_class, char *font, int size):
    edje_text_class_set(text_class, font, size)


def text_class_del(char *text_class):
    edje_text_class_del(text_class)


def text_class_list():
    cdef evas.c_evas.Evas_List *lst, *itr
    ret = []
    lst = edje_text_class_list()
    itr = lst
    while itr:
        ret.append(<char*>itr.data)
        python.free(itr.data)
        itr = itr.next
    evas.c_evas.evas_list_free(lst)
    return ret


def message_signal_process():
    edje_message_signal_process()

include "edje.c_edje_message.pxi"
include "edje.c_edje_object.pxi"
