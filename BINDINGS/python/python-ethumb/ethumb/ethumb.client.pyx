# Copyright (C) 2009 by ProFUSION embedded systems
#
# This file is part of Python-Ethumb.
#
# Python-Ethumb is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ethumb is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ethumb.  If not, see <http://www.gnu.org/licenses/>.

cimport ethumb.python as python
import traceback

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def shutdown():
    ethumb_client_shutdown()

def init():
    return ethumb_client_init()

cdef void _connect_cb(Ethumb_Client *client, Eina_Bool success, void *data) with gil:
    obj = <object>data
    (self, func, args, kargs) = obj
    if success:
        s = True
    else:
        s = False
    try:
        func(self, s, *args, **kargs)
    except Exception, e:
        traceback.print_exc()

cdef void _free_connect_cb_data(void *data) with gil:
    obj = <object>data
    python.Py_DECREF(obj)

cdef void _generated_cb(int id, char *file, char *key, char *thumb_path, char *thumb_key, Eina_Bool success, void *data) with gil:
    obj = <object>data
    (self, func, args, kargs) = obj
    f = str_from_c(file)
    k = str_from_c(key)
    tp = str_from_c(thumb_path)
    tk = str_from_c(thumb_key)
    s = success != 0
    if success:
        s = True
    else:
        s = False
    try:
        func(id, f, k, tp, tk, s, *args, **kargs)
    except Exception, e:
        traceback.print_exc()

cdef void _generated_cb_free_data(void *data) with gil:
    obj = <object>data
    python.Py_DECREF(obj)

cdef char *str_to_c(object s):
    cdef char *mystr
    if s is None:
        mystr = NULL
    else:
        mystr = s
    return mystr

cdef object str_from_c(char *mystr):
    if mystr != NULL:
        return mystr

cdef class Client:
    """Client for Ethumbd server."""
    def __init__(self, func, *args, **kargs):
        """Ethumb Client constructor."""
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        if self.obj == NULL:
            targs = (self, func, args, kargs)
            python.Py_INCREF(targs)
            self.obj = ethumb_client_connect(
                _connect_cb, <void*>targs, _free_connect_cb_data)

    # FDO
    def fdo_set(self, int s):
        ethumb_client_fdo_set(self.obj, s)

    # Size
    def size_set(self, int w, int h):
        ethumb_client_size_set(self.obj, w, h)

    def size_get(self):
        cdef int w, h
        ethumb_client_size_get(self.obj, &w, &h)
        return (w, h)

    property size:
        def __set__(self, value):
            cdef int w, h
            w, h = value
            self.size_set(w, h)

        def __get__(self):
            return self.size_get()

    # Format
    def format_set(self, int f):
        ethumb_client_format_set(self.obj, f)

    def format_get(self):
        return ethumb_client_format_get(self.obj)

    property format:
        def __set__(self, value):
            self.format_set(value)

        def __get__(self):
            return self.format_get()

    # Aspect
    def aspect_set(self, int a):
        ethumb_client_aspect_set(self.obj, a)

    def aspect_get(self):
        return ethumb_client_aspect_get(self.obj)

    property aspect:
        def __set__(self, value):
            self.aspect_set(value)

        def __get__(self):
            return self.aspect_get()

    # Crop
    def crop_set(self, float x, float y):
        ethumb_client_crop_align_set(self.obj, x, y)

    def crop_get(self):
        cdef float x, y
        ethumb_client_crop_align_get(self.obj, &x, &y)
        return (x, y)

    property crop:
        def __set__(self, value):
            cdef float x, y
            x, y = value
            self.crop_set(x, y)

        def __get__(self):
            return self.crop_get()

    # Quality
    def quality_set(self, int quality):
        ethumb_client_quality_set(self.obj, quality)

    def quality_get(self):
        return ethumb_client_quality_get(self.obj)

    property quality:
        def __set__(self, value):
            self.quality_set(value)

        def __get__(self):
            return self.quality_get()

    # Compress
    def compress_set(self, int compress):
        ethumb_client_compress_set(self.obj, compress)

    def compress_get(self):
        return ethumb_client_compress_get(self.obj)

    property compress:
        def __set__(self, value):
            self.compress_set(value)

        def __get__(self):
            return self.compress_get()

    # Directory
    def directory_set(self, path):
        ethumb_client_dir_path_set(self.obj, str_to_c(path))

    def directory_get(self):
        return str_from_c(ethumb_client_dir_path_get(self.obj))

    property directory:
        def __set__(self, value):
            self.directory_set(value)

        def __get__(self):
            return self.directory_get()

    # Category
    def category_set(self, category):
        ethumb_client_category_set(self.obj, str_to_c(category))

    def category_get(self):
        return str_from_c(ethumb_client_category_get(self.obj))

    property category:
        def __set__(self, value):
            self.category_set(value)

        def __get__(self):
            return self.category_get()

    # frame
    def frame_set(self, file, group, swallow):
        cdef char *f, *g, *s
        f = str_to_c(file)
        g = str_to_c(group)
        s = str_to_c(swallow)
        return ethumb_client_frame_set(self.obj, f, g, s)

    # file
    def file_set(self, path, key=None):
        cdef char *p, *k
        p = str_to_c(path)
        k = str_to_c(key)
        ethumb_client_file_set(self.obj, p, k)

    def file_get(self):
        cdef char *p, *k
        ethumb_client_file_get(self.obj, &p, &k)
        return (str_from_c(p), str_from_c(k))

    property file:
        def __set__(self, value):
            p, k = value
            self.file_set(p, k)

        def __get__(self):
            return self.file_get()

    def file_free(self):
        ethumb_client_file_free(self.obj)

    # thumbnail
    def thumb_set(self, path, key=None):
        cdef char *p, *k
        p = str_to_c(path)
        k = str_to_c(key)
        ethumb_client_thumb_path_set(self.obj, p, k)

    def thumb_get(self):
        cdef char *p, *k
        ethumb_client_thumb_path_get(self.obj, &p, &k)
        return (str_from_c(p), str_from_c(k))

    property thumb_path:
        def __set__(self, value):
            p, k = value
            self.thumb_set(p, k)
        def __get__(self):
            return self.thumb_get()

    # video_time
    def video_time_set(self, float time):
        ethumb_client_video_time_set(self.obj, time)

    def video_start_set(self, float start):
        ethumb_client_video_start_set(self.obj, start)

    def video_interval_set(self, float interval):
        ethumb_client_video_interval_set(self.obj, interval)

    def video_ntimes_set(self, int ntimes):
        ethumb_client_video_ntimes_set(self.obj, ntimes)

    def video_fps_set(self, int fps):
        ethumb_client_video_fps_set(self.obj, fps)

    # document_page
    def document_page_set(self, int page):
        ethumb_client_document_page_set(self.obj, page)

    # thumbnail generation
    def exists(self):
        return ethumb_client_thumb_exists(self.obj)

    def generate(self, func, *args, **kargs):
        targs = (self, func, args, kargs)
        python.Py_INCREF(targs)
        r = ethumb_client_generate(self.obj, _generated_cb, <void*>targs,
                                   _generated_cb_free_data)
        return r

    def cancel(self, int id):
        ethumb_client_queue_remove(self.obj, id, NULL, NULL)

    def clear(self):
        ethumb_client_queue_clear(self.obj)

    def __dealloc__(self):
        ethumb_client_disconnect(self.obj)

init()
