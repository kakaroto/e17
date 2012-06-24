# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

from evas.c_evas cimport Image as evasImage

cdef public class Plug(Object) [object PyElementaryPlug, type PyElementaryPlug_Type]:

    """An object that allows one to show an image which other process created.
    It can be used anywhere like any other elementary widget.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_plug_add(parent.obj))

    def connect(self, svcname, svcnum, svcsys):
        """connect(svcname, svcnum, svcsys)

        Connect a plug widget to service provided by socket image.

        @param svcname: The service name to connect to set up by the socket.
        @type svcname: string
        @param svcnum: The service number to connect to (set up by socket).
        @type svcnum: int
        @param svcsys: Boolean to set if the service is a system one or not
            (set up by socket).
        @type svcsys: bool

        @return: (C{True} = success, C{False} = error)
        @rtype: bool

        """
        return bool(elm_plug_connect(self.obj, _cfruni(svcname), svcnum, svcsys))

    property image_object:
        """Get the basic Evas_Image object from this object (widget).

        This function allows one to get the underlying C{Object} of type
        Image from this elementary widget. It can be useful to do things
        like get the pixel data, save the image to a file, etc.

        @note: Be careful to not manipulate it, as it is under control of
            elementary.

        @type: L{Object}

        """
        def __get__(self):
            cdef evasImage img = evasImage()
            cdef Evas_Object *obj = elm_plug_image_object_get(self.obj)
            img.obj = obj
            return img

_elm_widget_type_register("plug", Plug)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPlug_Type # hack to install metaclass
_install_metaclass(&PyElementaryPlug_Type, ElementaryObjectMeta)
