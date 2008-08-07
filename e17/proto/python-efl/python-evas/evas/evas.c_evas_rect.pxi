# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_evas.pyx

cdef public class Rect [object PyEvasRect, type PyEvasRect_Type]:
    """Type to store and manipulate rectangular coordinates.

    This class provides the description of a rectangle and means to
    access and modify its properties in an easy way.

    Useful properties are:
     - left (or "x")
     - right
     - top (or "y")
     - bottom
     - center_x
     - center_y
     - width (or "w")
     - height (or "h")
     - top_left
     - top_right
     - bottom_left
     - bottom_right
     - center
     - pos
     - size
     - area

    Useful methods are:
     - normalize
     - contains
     - intercepts
     - clip
     - union
     - clamp
     - move_by
     - inflate

    Usage example:

     >>> r1 = Rect(10, 20, 30, 40)
     >>> r2 = Rect((0, 0), (100, 100))
     >>> r1
     Rect(x=10, y=20, w=30, h=40)
     >>> r2
     Rect(x=0, y=0, w=100, h=100)
     >>> r1.contains(r2)
     False
     >>> r2.contains(r1)
     True
     >>> r1 in r2 # same as r2.contains(r1)
     True
     >>> r1.intercepts(r2)
     True

    @attention: this is not a graphical object! Do not confuse with
       L{Rectangle}.
    """

    def __init__(self, *args, **kargs):
        cdef Rect other
        self.x0 = 0
        self.y0 = 0
        self._w = 0
        self._h = 0
        if args:
            if len(args) == 1:
                o = args[0]
                if isinstance(o, Rect):
                    other = o
                    self.x0 = other.x0
                    self.y0 = other.y0
                    self._w = other._w
                    self._h = other._h
                elif isinstance(o, (tuple, list)):
                    self.x0, self.y0, self._w, self._h = o
            elif len(args) == 2:
                self.x0, self.y0 = args[0]
                self._w, self._h = args[1]
            elif len(args) > 1:
                self.x0, self.y0, self._w, self._h = args

        if kargs:
            if "rect" in kargs:
                o = kargs["rect"]
                self.x0 = o.x
                self.y0 = o.y
                self._w = o.w
                self._h = o.h
            elif "geometry" in kargs:
                self.x0, self.y0, self._w, self._h = kargs["geometry"]
            else:
                if "size" in kargs:
                    self._w, self._h = kargs["size"]

                if "pos" in kargs:
                    self.x0, self.y0 = kargs["pos"]

                if "w" in kargs:
                    self._w = kargs["w"]
                elif "width" in kargs:
                    self._w = kargs["width"]

                if "h" in kargs:
                    self._h = kargs["h"]
                elif "height" in kargs:
                    self._h = kargs["height"]

                if "x" in kargs:
                    self.x0 = kargs["x"]
                elif "left" in kargs:
                    self.x0 = kargs["left"]

                if "right" in kargs:
                    self.x1 = kargs["right"]
                    if "x" in kargs or "left" in kargs or "pos" in kargs:
                        self._w = self.x1 - self.x0
                    elif "w" in kargs or "width" in kargs or "size" in kargs:
                        self.x0 = self.x1 - self._w

                if "y" in kargs:
                    self.y0 = kargs["y"]
                elif "top" in kargs:
                    self.y0 = kargs["top"]

                if "bottom" in kargs:
                    self.y1 = kargs["bottom"]
                    if "y" in kargs or "top" in kargs or "pos" in kargs:
                        self._h = self.y1 - self.y0
                    elif "h" in kargs or "height" in kargs or "size" in kargs:
                        self.y0 = self.y1 - self._h

        self.x1 = self.x0 + self._w
        self.cx = self.x0 + self._w/2

        self.y1 = self.y0 + self._h
        self.cy = self.y0 + self._h/2

    def __str__(self):
        return "%s(x=%d, y=%d, w=%d, h=%d)" % \
               (self.__class__.__name__, self.x0, self.y0, self._w, self._h)

    def __repr__(self):
        return "%s(x=%d, y=%d, w=%d, h=%d)" % \
               (self.__class__.__name__, self.x0, self.y0, self._w, self._h)

    property x:
        def __get__(self):
            return self.x0

        def __set__(self, int x):
            self.x0 = x
            self.x1 = x + self._w
            self.cx = x + self._w/2

    property left: # same as "x"
        def __get__(self):
            return self.x0

        def __set__(self, int x):
            self.x0 = x
            self.x1 = x + self._w
            self.cx = x + self._w/2

    property right:
        def __get__(self):
            return self.x1

        def __set__(self, int x):
            self.x0 = x - self._w
            self.x1 = x
            self.cx = self.x0 + self._w/2

    property center_x:
        def __get__(self):
            return self.cx

        def __set__(self, int cx):
            self.x0 = cx - self._w/2
            self.x1 = self.x0 + self._w
            self.cx = cx

    property y:
        def __get__(self):
            return self.y0

        def __set__(self, int y):
            self.y0 = y
            self.y1 = y + self._h
            self.cy = y + self._h/2

    property top: # same as "y"
        def __get__(self):
            return self.y0

        def __set__(self, int y):
            self.y0 = y
            self.y1 = y + self._h
            self.cy = y + self._h/2

    property bottom:
        def __get__(self):
            return self.y1

        def __set__(self, int y):
            self.y0 = y - self._h
            self.y1 = y
            self.cy = self.y0 + self._h/2

    property center_y:
        def __get__(self):
            return self.cy

        def __set__(self, int cy):
            self.y0 = cy - self._h/2
            self.y1 = self.y0 + self._h
            self.cy = cy

    property w:
        def __get__(self):
            return self._w

        def __set__(self, int w):
            self._w = w
            self.x1 = self.x0 + w
            self.cx = self.x0 + w/2

    property width:
        def __get__(self):
            return self._w

        def __set__(self, int w):
            self._w = w
            self.x1 = self.x0 + w
            self.cx = self.x0 + w/2

    property h:
        def __get__(self):
            return self._h

        def __set__(self, int h):
            self._h = h
            self.y1 = self.y0 + h
            self.cy = self.y0 + h/2

    property height:
        def __get__(self):
            return self._h

        def __set__(self, int h):
            self._h = h
            self.y1 = self.y0 + h
            self.cy = self.y0 + h/2

    property center:
        def __get__(self):
            return (self.cx, self.cy)

        def __set__(self, spec):
            cdef int cx, cy

            cx, cy = spec

            self.x0 = cx - self._w/2
            self.x1 = self.x0 + self._w
            self.cx = cx

            self.y0 = cy - self._h/2
            self.y1 = self.y0 + self._h
            self.cy = cy

    property top_left:
        def __get__(self):
            return (self.x0, self.y0)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec

            self.x0 = x
            self.x1 = x + self._w
            self.cx = x + self._w/2

            self.y0 = y
            self.y1 = y + self._h
            self.cy = y + self._h/2

    property top_right:
        def __get__(self):
            return (self.x1, self.y0)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec

            self.x0 = x - self._w
            self.x1 = x
            self.cx = self.x0 + self._w/2

            self.y0 = y
            self.y1 = y + self._h
            self.cy = y + self._h/2

    property bottom_left:
        def __get__(self):
            return (self.x0, self.y1)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec

            self.x0 = x
            self.x1 = x + self._w
            self.cx = x + self._w/2

            self.y0 = y - self._h
            self.y1 = y
            self.cy = self.y0 + self._h/2

    property bottom_right:
        def __get__(self):
            return (self.x1, self.y1)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec

            self.x0 = x - self._w
            self.x1 = x
            self.cx = self.x0 + self._w/2

            self.y0 = y - self._h
            self.y1 = y
            self.cy = self.y0 + self._h/2

    property pos:
        def __get__(self):
            return (self.x0, self.y0)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec

            self.x0 = x
            self.x1 = x + self._w
            self.cx = x + self._w/2

            self.y0 = y
            self.y1 = y + self._h
            self.cy = y + self._h/2

    property size:
        def __get__(self):
            return (self._w, self._h)

        def __set__(self, spec):
            cdef int w, h
            w, h = spec

            self._w = w
            self.x1 = self.x0 + w
            self.cx = self.x0 + w/2

            self._h = h
            self.y1 = self.y0 + h
            self.cy = self.y0 + h/2

    property area:
        def __get__(self):
            return self._w * self._h

    def normalize(self):
        "Normalize coordinates so both width and height are positive."
        cdef int tmp
        if self._w < 0:
            tmp = self.x0
            self.x0 = self.x1
            self.x1 = tmp
            self._w = -self._w

        if self._h < 0:
            tmp = self.y0
            self.y0 = self.y1
            self.y1 = tmp
            self._h = -self._h

    def __richcmp__(a, b, int op):
        "Compares two rectangles for (in)equality"
        cdef Rect o1, o2
        cdef int res
        if isinstance(a, Rect):
            o1 = a
        else:
            o1 = Rect(a)

        if isinstance(b, Rect):
            o2 = b
        else:
            o2 = Rect(b)

        if op == 2 or op == 3:
            res = o1.x == o2.x and \
                  o1.y == o2.y and \
                  o1.w == o2.w and \
                  o1.h == o2.h
            if op == 3:
                res = not res

            return res
        else:
            raise TypeError("unsupported comparison operation")

    def __nonzero__(self):
        "Checks if all coordinates are not zero."
        return bool(self.x0 != 0 and self._w != 0 and \
                    self.y0 != 0 and self._h != 0)

    def __contains__(self, obj):
        "Checks if contains given rectangle."
        cdef Rect o
        if isinstance(obj, Rect):
            o = obj
        elif isinstance(obj, (tuple, list)) and len(obj) == 2:
            o = Rect(pos=obj)
        else:
            o = Rect(obj)

        return bool(self.x0 <= o.left and o.right <= self.x1 and \
                    self.y0 <= o.top and o.bottom <= self.y1)

    def contains(self, obj):
        "Checks if contains given rectangle."
        return obj in self

    def contains_point(self, x, y):
        "Checks if contains the given point."
        return bool(self.x0 <= x <= self.x1 and \
                    self.y0 <= y <= self.y1)

    def intercepts(self, obj):
        "Checks if intercepts given rectangle."
        cdef Rect o
        cdef int left, right, top, bottom, a, b, c, d
        if isinstance(obj, Rect):
            o = obj
        elif isinstance(obj, (tuple, list)) and len(obj) == 2:
            o = Rect(pos=obj)
        else:
            o = Rect(obj)

        left = o.left
        right = o.right
        top = o.top
        bottom = o.bottom

        a = left <= self.x0 and self.x0 <= right
        b = left <= self.x1 and self.x1 <= right
        c = top <= self.y0 and self.y0 <= bottom
        d = top <= self.y1 and self.y1 <= bottom
        if (a or b) and (c or d):
            return True

        a = self.x0 <= left and left <= self.x1
        b = self.x0 <= right and right <= self.x1
        c = self.y0 <= top and top <= self.y1
        d = self.y0 <= bottom and bottom <= self.y1
        return bool((a or b) and (c or d))

    def clip(self, obj):
        "Returns a new Rect that represents current cropped inside parameter."
        cdef Rect o
        cdef int left, right, top, bottom, width, height
        if isinstance(obj, Rect):
            o = obj
        else:
            o = Rect(obj)

        left = o.left
        right = o.right
        top = o.top
        bottom = o.bottom

        if left < self.x0:
            left = self.x0
        if right > self.x1:
            right = self.x1
        if top < self.y0:
            top = self.y0
        if bottom > self.y1:
            bottom = self.y1

        width = right - left
        height = bottom - top
        if width > 0 and height > 0:
            return Rect(left, top, width, height)
        else:
            return Rect(0, 0, 0, 0)

    def union(self, obj):
        "Returns a new Rect that covers both rectangles."
        cdef Rect o
        cdef int left, right, top, bottom
        if isinstance(obj, Rect):
            o = obj
        else:
            o = Rect(obj)

        left = o.left
        right = o.right
        top = o.top
        bottom = o.bottom

        if left > self.x0:
            left = self.x0
        if right < self.x1:
            right = self.x1
        if top > self.y0:
            top = self.y0
        if bottom < self.y1:
            bottom = self.y1

        return Rect(left, top, right - left, bottom - top)

    def clamp(self, obj):
        """Returns a new Rect that represents current moved inside given
           parameter.

           If given rectangle is smaller, it'll be centered.
        """
        cdef Rect o, ret
        cdef int left, right, top, bottom, width, height

        ret = Rect(self)
        if isinstance(obj, Rect):
            o = obj
        else:
            o = Rect(obj)

        width = o.width
        height = o.height

        if self._w > width:
            ret.center_x = o.center_x
        else:
            left = o.left
            right = o.right
            if ret.x0 < left:
                ret.left = left
            elif ret.x1 > right:
                ret.right = right

        if self._h > height:
            ret.center_y = o.center_y
        else:
            top = o.top
            bottom = o.bottom
            if ret.y0 < top:
                ret.top = top
            elif ret.y1 > bottom:
                ret.bottom = bottom

        return ret

    def move_by(self, int offset_x, int offset_y):
        "Returns a new Rect that represents current moved by given offsets."
        return Rect(self.x0 + offset_x, self.y0 + offset_y, self._w, self._h)

    def inflate(self, int amount_w, int amount_h):
        "Returns a new Rect that represents current inflated by given amount."
        return Rect(self.x0, self.y0, self._w + amount_w, self._h + amount_h)
