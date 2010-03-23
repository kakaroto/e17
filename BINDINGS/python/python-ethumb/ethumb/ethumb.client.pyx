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

from python_ref cimport PyObject, Py_INCREF, Py_DECREF
import traceback

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def shutdown():
    ethumb_client_shutdown()

def init():
    return ethumb_client_init()

cdef void _connect_cb(void *data, Ethumb_Client *client, Eina_Bool success) with gil:
    cdef Client self = <Client>data
    s = bool(<unsigned int>success)
    try:
        func, args, kargs = self._on_connect_callback
        func(self, s, *args, **kargs)
    except Exception, e:
        traceback.print_exc()

    if not s and self.obj != NULL:
        ethumb_client_disconnect(self.obj)
        self.obj = NULL
    self._on_connect_callback = None


cdef void _on_server_die_cb(void *data, Ethumb_Client *client) with gil:
    cdef Client self = <Client>data
    if self._on_server_die_callback is not None:
        try:
            func, args, kargs = self._on_server_die_callback
            func(self, *args, **kargs)
        except Exception, e:
            traceback.print_exc()

    if self.obj != NULL:
        ethumb_client_disconnect(self.obj)
        self.obj = NULL
    self._on_server_die_callback = None


cdef void _generated_cb(void *data, Ethumb_Client *client, int id, char *file, char *key, char *thumb_path, char *thumb_key, Eina_Bool success) with gil:
    obj = <object>data
    (self, func, args, kargs) = obj
    f = str_from_c(file)
    k = str_from_c(key)
    tp = str_from_c(thumb_path)
    tk = str_from_c(thumb_key)
    s = bool(success != 0)
    try:
        func(self, id, f, k, tp, tk, s, *args, **kargs)
    except Exception, e:
        traceback.print_exc()

cdef void _generated_cb_free_data(void *data) with gil:
    obj = <object>data
    Py_DECREF(obj)

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
    """Client for Ethumbd server.

    This client is the recommended way to generate thumbnails with
    Ethumb. All you have to do is create a client instance, wait it to
    be connected to server, configure thumbnail parameters and then
    start feed it with file_set(), exists() generate(). Basic steps are:

     - instantiate Client, wait for func to be called with success.
     - set various parameters, like format and size.
     - loop on original files:
       - C{c.file_set(file)}
       - C{if not c.exists(): c.generate(generated_cb)}

    When the last reference to client is released, server is
    automatically disconnected. Since callback may contain references
    to server itself, it is recommended explicit call to
    L{disconnect()} function.
    """

    def __init__(self, func, *args, **kargs):
        """Ethumb Client constructor.

        Server is ready to receive requests just after B{func} is
        called back with C{status == True}.

        @parm: B{func} function to call when connection with server is
           established. Function signature is:
              C{func(client, status, *args, **kargs)}
           with status being True for successful connection or False
           on error.

        @raise TypeError: if B{func} is not callable.
        @raise SystemError: if it was not possible to connect to
            server, allocate memory or use DBus.
        """
        if not callable(func):
            raise TypeError("Parameter 'func' must be callable")
        if self.obj == NULL:
            self._on_connect_callback = (func, args, kargs)
            self._on_server_die_callback = None
            self.obj = ethumb_client_connect(_connect_cb, <void*>self, NULL)
            if self.obj == NULL:
                raise SystemError("Error connecting to server.")
            else:
                ethumb_client_on_server_die_callback_set(
                    self.obj, _on_server_die_cb, <void*>self, NULL)

    def __dealloc__(self):
        if self.obj != NULL:
            ethumb_client_disconnect(self.obj)

    def disconnect(self):
        """Explicitly request server disconnection.

        After this call object becomes shallow, that is operations
        will be void.
        """
        if self.obj != NULL:
            ethumb_client_disconnect(self.obj)
            self.obj = NULL
        self._on_connect_callback = None
        self._on_server_die_callback = None

    def __str__(self):
        f, k = self.file
        tf, tk = self.thumb_path
        w, h = self.size

        format = ("FDO", "JPEG", "EET")[self.format]
        aspect = ("KEEP", "IGNORE", "CROP")[self.aspect]
        if self.aspect == 2:
            aspect = "CROP[%f, %f]" % self.crop
        return ("%s(file=(%r, %r), thumb=(%r, %r), exists=%s, size=%dx%d, "
                "format=%s, aspect=%s, quality=%d, compress=%d, "
                "directory=%r, category=%r)") % \
                (self.__class__.__name__, f, k, tf, tk, self.exists(),
                 w, h, format, aspect, self.quality, self.compress,
                 self.directory, self.category)

    def __repr__(self):
        f, k = self.file
        tf, tk = self.thumb_path
        return "%s(obj=%#x, file=(%r, %r), thumb=(%r, %r), exists=%s)" % \
            (self.__class__.__name__, f, k, tf, tk, self.exists())

    def on_server_die_callback_set(self, func, *args, **kargs):
        """Function to call when server dies.

        When server is dead there is nothing to do with this client
        anymore, just create a new one and start over, hope that
        server could be started and you could generate more
        thumbnails.

        @parm: B{func} function to call when server dies. Signature:
           C{func(client, *args, **kargs)

        @raise TypeError: if B{func} is not callable or None.
        """
        if func is None:
            self._on_server_die_callback = None
        elif callable(func):
            self._on_server_die_callback = (func, args, kargs)
        else:
            raise TypeError("Parameter 'func' must be callable or None")

    def fdo_set(self, int s):
        """Configure future requests to use FreeDesktop.Org preset.

        This is a preset to provide freedesktop.org (fdo) standard
        compliant thumbnails. That is, files are stored as JPEG under
        ~/.thumbnails/SIZE, with size being either normal (128x128) or
        large (256x256).

        @parm: B{s} size identifier, either ETHUMB_THUMB_NORMAL (0) or
           ETHUMB_THUMB_LARGE.

        @see: L{size_set()}, L{format_set()}, L{aspect_set()}, L{crop_set()},
           L{category_set()}, L{directory_set()}.
        """
        ethumb_client_fdo_set(self.obj, s)

    def size_set(self, int w, int h):
        """Configure future request to use custom size.

        @parm: B{w} width, default is 128.
        @parm: B{h} height, default is 128.
        """
        ethumb_client_size_set(self.obj, w, h)

    def size_get(self):
        """Get current size being used by requests.

        @rtype: tuple of int.
        """
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

    def format_set(self, int f):
        """Configure format to use for future requests.

        @parm: B{f} format identifier to use, either ETHUMB_THUMB_FDO (0),
           ETHUMB_THUMB_JPEG (1) or ETHUMB_THUMB_EET (2). Default is FDO.
        """
        ethumb_client_format_set(self.obj, f)

    def format_get(self):
        """Get current format in use for requests.

        @rtype: int
        """
        return ethumb_client_format_get(self.obj)

    property format:
        def __set__(self, value):
            self.format_set(value)

        def __get__(self):
            return self.format_get()

    def aspect_set(self, int a):
        """Configure aspect mode to use.

        If aspect is kept (ETHUMB_THUMB_KEEP_ASPECT), then image will
        be rescaled so the largest dimension is not bigger than it's
        specified size (see L{size_get()}) and the other dimension is
        resized in the same proportion. Example: size is 256x256,
        image is 1000x500, resulting thumbnail is 256x128.

        If aspect is ignored (ETHUMB_THUMB_IGNORE_ASPECT), then image
        will be distorted to match required thumbnail size. Example:
        size is 256x256, image is 1000x500, resulting thumbnail is
        256x256.

        If crop is required (ETHUMB_THUMB_CROP), then image will be
        cropped so the smallest dimension is not bigger than its
        specified size (see L{size_get()}) and the other dimension
        will overflow, not being visible in the final image. How it
        will overflow is speficied by L{crop_set()}
        alignment. Example: size is 256x256, image is 1000x500, crop
        alignment is 0.5, 0.5, resulting thumbnail is 256x256 with 250
        pixels from left and 250 pixels from right being lost, that is
        just the 500x500 central pixels of image will be considered
        for scaling.

        @parm: B{a} aspect mode identifier, either ETHUMB_THUMB_KEEP_ASPECT (0),
           ETHUMB_THUMB_IGNORE_ASPECT (1) or ETHUMB_THUMB_CROP (2).

        """
        ethumb_client_aspect_set(self.obj, a)

    def aspect_get(self):
        """Get current aspect in use for requests.

        @rtype: int
        """
        return ethumb_client_aspect_get(self.obj)

    property aspect:
        def __set__(self, value):
            self.aspect_set(value)

        def __get__(self):
            return self.aspect_get()

    def crop_set(self, float x, float y):
        """Configure crop alignment in use for future requests.

        @parm: B{x} horizontal alignment. 0.0 means left side will be
           visible or right side is being lost. 1.0 means right
           side will be visible or left side is being lost. 0.5
           means just center is visible, both sides will be lost.
           Default is 0.5.
        @parm: B{y} vertical alignment. 0.0 is top visible, 1.0 is
           bottom visible, 0.5 is center visible. Default is 0.5
        """
        ethumb_client_crop_align_set(self.obj, x, y)

    def crop_get(self):
        """Get current crop alignment in use for requests.

        @rtype: tuple of float
        """
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

    def quality_set(self, int quality):
        """Configure quality to be used in thumbnails.

        @parm: B{quality} value from 0 to 100, default is 80. The
           effect depends on the format being used, PNG will not
           use it.
        """
        ethumb_client_quality_set(self.obj, quality)

    def quality_get(self):
        """Get current quality in use for requests.

        @rtype: int
        """
        return ethumb_client_quality_get(self.obj)

    property quality:
        def __set__(self, value):
            self.quality_set(value)

        def __get__(self):
            return self.quality_get()

    def compress_set(self, int compress):
        """Configure compression level used in requests.

        @parm: B{compress} value from 0 to 9, default is 9. The effect
           depends on the format being used, JPEG will not use it.
        """
        ethumb_client_compress_set(self.obj, compress)

    def compress_get(self):
        """Get current compression level in use for requests.

        @rtype: int
        """
        return ethumb_client_compress_get(self.obj)

    property compress:
        def __set__(self, value):
            self.compress_set(value)

        def __get__(self):
            return self.compress_get()

    def directory_set(self, path):
        """Configure where to store thumbnails in future requests.

        Note that this is the base, a category is added to this path
        as a sub directory.

        @parm: B{path} base directory where to store
           thumbnails. Default is ~/.thumbnails
        """
        ethumb_client_dir_path_set(self.obj, str_to_c(path))

    def directory_get(self):
        """Get current base directory to store thumbnails.

        @rtype: str or None
        """
        return str_from_c(ethumb_client_dir_path_get(self.obj))

    property directory:
        def __set__(self, value):
            self.directory_set(value)

        def __get__(self):
            return self.directory_get()

    def category_set(self, category):
        """Category directory to store thumbnails.

        @parm: B{category} category sub directory to store
           thumbnail. Default is either "normal" or "large" for FDO
           compliant thumbnails or
           WIDTHxHEIGHT-ASPECT[-FRAMED]-FORMAT. It can be a string or
           None to use auto generated names.
        """
        ethumb_client_category_set(self.obj, str_to_c(category))

    def category_get(self):
        """Get current category sub directory to store thumbnails.

        @rtype: str or None
        """
        return str_from_c(ethumb_client_category_get(self.obj))

    property category:
        def __set__(self, value):
            self.category_set(value)

        def __get__(self):
            return self.category_get()

    def frame_set(self, file, group, swallow):
        """Set frame to apply to future thumbnails.

        This will create an edje object that will have image swallowed
        in. This can be used to simulate Polaroid or wood frames in
        the generated image. Remeber it is bad to modify the original
        contents of thumbnails, but sometimes it's useful to have it
        composited and avoid runtime overhead.

        @parm: B{file} file path to edje.
        @parm: B{group} group inside edje to use.
        @parm: B{swallow} name of swallow part.
        """
        cdef char *f, *g, *s
        f = str_to_c(file)
        g = str_to_c(group)
        s = str_to_c(swallow)
        return ethumb_client_frame_set(self.obj, f, g, s)

    def file_set(self, path, key=None):
        """Set file to thumbnail.

        Calling this function will zero L{thumb_set()}
        specifications. This is done to avoid one using the last thumb
        path for new images.

        @parm: B{path} path to thumbnail subject.
        @parm: B{key} path to key inside B{path}, this is used to
           generate thumbnail of edje groups or images inside EET.
        """
        cdef char *p, *k
        p = str_to_c(path)
        k = str_to_c(key)
        ethumb_client_file_set(self.obj, p, k)

    def file_get(self):
        """Get current file to thumbnail.

        @rtype: tuple of str
        """
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
        """Zero/Reset file parameters.

        This call will reset file and thumb specifications.

        @see: L{file_set()} and L{thumb_set()}
        """
        ethumb_client_file_free(self.obj)

    def thumb_set(self, path, key=None):
        """Set thumbnail path and key.

        Note that these parameters are forgotten (reset) after
        L{file_set()}.

        @parm: B{path} path to generated thumbnail to use, this is an
           absolute path to file, overriding directory and category.
        @parm: B{key} path to key inside B{path}, this is used to
           generate thumbnail inside EET files.
        """
        cdef char *p, *k
        p = str_to_c(path)
        k = str_to_c(key)
        ethumb_client_thumb_path_set(self.obj, p, k)

    def thumb_get(self):
        """Get current path and key of thumbnail.

        Note that if no explicit L{thumb_set()} was called, it will
        auto generate path based on existing parameters such as
        directory, category and others.

        @rtype: tuple of str
        """
        cdef char *p, *k
        ethumb_client_thumb_path_get(self.obj, &p, &k)
        return (str_from_c(p), str_from_c(k))

    property thumb_path:
        def __set__(self, value):
            p, k = value
            self.thumb_set(p, k)
        def __get__(self):
            return self.thumb_get()

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

    def exists(self):
        """Checks if thumbnail already exists.

        If you want to avoid regenerating thumbnails, check if they
        already exist with this function.
        """
        return bool(ethumb_client_thumb_exists(self.obj))

    def generate(self, func, *args, **kargs):
        """Ask EThumb server to generate the specified thumbnail.

        Thumbnail generation is asynchronous and depend on ecore main
        loop running. Given function will be called back with
        generation status if True is returned by this call. If False
        is returned, given function will not be called.

        Existing thumbnails will be overwritten with this call. Check
        if they already exist with L{exists()} before calling.

        @parm: B{func} function to call on generation completion, even
           if failed or succeeded. Signature is:
              C{func(self, id, file, key, thumb_path, thumb_key, status, *args, **kargs)}
           with status being True for successful generation or
           False on failure.

        @return: request identifier. Request can be canceled calling
           L{cancel()} with given id. If an identifier is returned (>=
           0), then func is guaranteed to be called unless it is
           explicitly canceled.

        @raise TypeError: if B{func} is not callable.
        @raise SystemError: if could not generate thumbnail, probably
           no L{file_set()}.

        @see: L{cancel()}, L{clear()}, L{exists()}
        """
        if not callable(func):
            raise TypeError("func must be callable")

        targs = (self, func, args, kargs)
        r = ethumb_client_generate(self.obj, _generated_cb, <void*>targs,
                                   _generated_cb_free_data)
        if r >= 0:
            Py_INCREF(targs)
            return r
        else:
            raise SystemError("could not generate thumbnail. "
                              "Did you set the file?")

    def generate_cancel(self, int id):
        """Cancel thumbnail request given its id.

        Calling this function aborts thumbnail generation and B{func}
        given to L{generate()} will not be called!

        @parm: B{id} identifier returned by L{generate()}
        """
        ethumb_client_generate_cancel(self.obj, id, NULL, NULL, NULL)

    def generate_cancel_all(self):
        """Clear request queue, canceling all generation requests.

        This will abort all existing requests, no B{func} given to
        L{generate()} will be called.

        Same as calling L{cancel()} in all exising requests.
        """
        ethumb_client_generate_cancel_all(self.obj)

init()
