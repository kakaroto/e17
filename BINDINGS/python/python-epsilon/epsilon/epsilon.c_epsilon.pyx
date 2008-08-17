# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Epsilon.
#
# Python-Epsilon is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Epsilon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Epsilon.  If not, see <http://www.gnu.org/licenses/>.


__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def init():
    return epsilon_init()

EPSILON_THUMB_NORMAL = 0
EPSILON_THUMB_LARGE = 1

cdef class Epsilon:
    """Epsilon thumbnail generator.

    Epsilon produces freedesktop.org-compatible thumbnails, it can
    use Imlib2, EPEG and external plugins. Imlib2 is the general
    purpose image loader, EPEG is a super-fast JPEG, external plugins
    can provide any thumbnail support, like xine to use videos.
    """
    def __init__(self, char *path, thumb_size=None, key=None, resolution=None):
        """Epsilon constructor.

        @parm path: full path of image to process.
        @parm thumb_size: one of EPSILON_THUMB_NORMAL or EPSILON_THUMB_LARGE
        @parm key: just used by edje to specify the part to process.
        @parm resolution: just used by edje to specify render size.
        """
        if self.obj == NULL:
            self.obj = epsilon_new(path)
            if thumb_size is not None:
                self.thumb_size_set(thumb_size)
            if key is not None:
                self.key_set(key)
            if resolution is not None:
                self.resolution_set(resolution)

    def __str__(self):
        return ("%s(path=%r, key=%s, thumb_file=%r, thumb_size=%r, "
                "resolution=%r)") % \
               (self.__class__.__name__, self.file, self.key, self.thumb_file,
                self.thumb_size, self.resolution)

    def __repr__(self):
        return ("%s(%#x, path=%r, key=%s, thumb_file=%r, thumb_size=%r, "
                "resolution=%r, Epsilon=%#x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self.file, self.key, self.thumb_file,
                self.thumb_size, self.resolution,
                <unsigned long>self.obj, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.obj != NULL:
            epsilon_free(self.obj)
            self.obj = NULL

    def exists(self):
        """Check if thumbnail exists.

           @rtype: bool
        """
        return bool(epsilon_exists(self.obj) == EPSILON_OK)

    def generate(self):
        """Force thumbnail generation.

           @note: this will overwrite existing thumbnail.
        """
        return bool(epsilon_generate(self.obj) == EPSILON_OK)

    def key_set(self, char *key):
        """Specify key inside file to use.

           So far, this is just used by Edje to choose which group to use.
        """
        epsilon_key_set(self.obj, key)

    def key_get(self):
        "@rtype: str or None"
        if self.obj and self.obj.key:
            return self.obj.key

    property key:
        def __set__(self, char *key):
            self.key_set(key)

        def __get__(self):
            return self.key_get()

    def resolution_set(self, int w, int h):
        """Specify resolution to render thumbnail before scaling.

           So far, this is just used to render Edje objects.
        """
        epsilon_resolution_set(self.obj, w, h)

    def resolution_get(self):
        "@rtype: tuple of int"
        if self.obj:
            return (self.obj.w, self.obj.h)
        else:
            return (0, 0)

    property resolution:
        def __set__(self, value):
            cdef int w, h
            w, h = value
            self.resolution_set(w, h)

        def __get__(self):
            return self.resolution_get()

    def thumb_size_set(self, int value):
        """Specify thumbnail size, either EPSILON_THUMB_NORMAL (128x128) or
           EPSILON_THUMB_LARGE (256x256).
        """
        if value != EPSILON_THUMB_NORMAL and value != EPSILON_THUMB_LARGE:
            raise ValueError("value must be either EPSILON_THUMB_NORMAL or "
                             "EPSILON_THUMB_LARGE")
        epsilon_thumb_size(self.obj, value)

    def thumb_size_get(self):
        "@rtype: int"
        if self.obj:
            if self.obj.tw == EPSILON_THUMB_NORMAL:
                return EPSILON_THUMB_NORMAL
            else:
                return EPSILON_THUMB_LARGE

    property thumb_size:
        def __set__(self, int value):
            self.thumb_size_set(value)

        def __get__(self):
            return self.thumb_size_get()

    def file_get(self):
        "@rtype: str"
        cdef char *s
        s = epsilon_file_get(self.obj)
        if s != NULL:
            return s

    property file:
        def __get__(self):
            return self.file_get()

    def thumb_file_get(self):
        "@rtype: str or None"
        cdef char *s
        s = epsilon_thumb_file_get(self.obj)
        if s != NULL:
            return s

    property thumb_file:
        def __get__(self):
            return self.thumb_file_get()




init()
