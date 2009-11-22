# Copyright (c) 2008-2009 Simon Busch
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

class AnchorBlockInfo:
    def __init__(self):
        self.name = None
        self.button = 0
        self.hover = None
        self.anchor = (0, 0, 0, 0)
        self.hover_parent = (0, 0, 0, 0)
        self.hover_left = False
        self.hover_right = False
        self.hover_top = False
        self.hover_bottom = False

cdef void _anchorblock_callback(void *cbt, c_evas.Evas_Object *o, void *event_info) with gil:
    cdef Elm_Entry_Anchorblock_Info *ei
    try:
        (event, obj, callback, a, ka) = <object>cbt
        if event_info is NULL:
            ab = None
        else:
            ei = <Elm_Entry_Anchorblock_Info*>event_info
            ab = AnchorBlockInfo()
            ab.name = ei.name
            ab.button = ei.button
            ab.hover = Hover(None, <object>ei.hover);
            ab.anchor = (ei.anchor.x, ei.anchor.y,
                         ei.anchor.w, ei.anchor.h)
            ab.hover_parent = (ei.hover_parent.x, ei.hover_parent.y,
                               ei.hover_parent.w, ei.hover_parent.h)
            ab.hover_left = ei.hover_left
            ab.hover_right = ei.hover_right
            ab.hover_top = ei.hover_top
            ab.hover_bottom = ei.hover_bottom
        callback(obj, ab, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef class AnchorBlock(Object):
    """
    Anchorblock is for displaying tet that contains markup with anchors like:
    \<a href=1234\>something\</\> in it. These will be drawn differently and will
    be able to be clicked on by the user to display a popup. This popup then
    is intended to contain extra options such as "call", "add to contacts",
    "open web page" etc.

    Signals that you can add callbacks for are:

    anchor,clicked - anchor called was clicked. event_info is anchor info -
    AnchorBlockInfo
    This signal can be bound with the "ckicked" property
    """
    cdef object cbt
    def __init__(self, c_evas.Object parent):
        """Initialize the Anchorblock

        @parm: B{parent} Parent window
        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_anchorblock_add(parent.obj))

    property clicked:
        """ Clicked property. Bound to signal "anchor, clicked"

        The callback should have the following signature:
            callback(obj, ab, *args, **kargs)
        obj: the object raising the signal
        ab: AnchorBlockInfo

        The value set is either the callback function or a tuple like
            (callback, *args, **kargs)
        """
        def __set__(self, value):
            if self.cbt:
                c_evas.evas_object_smart_callback_del(self.obj, "anchor,clicked",
                                                      _anchorblock_callback)
                self.cbt = None
            if value:
                a = []
                ka = {}
                if type(value) == tuple:
                    cb = value[0]
                    a = value[1]
                    if len(value) == 3:
                        ka = value[2]
                    else:
                        if type(a) != tuple:
                            a = (a,)
                else:
                    cb = value

                if not callable(cb):
                    raise TypeError("callback is not callable")

                self.cbt = ("anchor,clicked", self, cb, a, ka)
                c_evas.evas_object_smart_callback_add(self.obj, "anchor,clicked",
                                                      _anchorblock_callback,
                                                      <void *>self.cbt)

    def text_set(self, text):
        """
        Set the text markup of the anchorblock

        This sets the text of the anchorblock to be the text given as @p text. This
        text is in markup format with \<a href=XXX\> beginning an achor with the
        string link of 'XXX', and \</\> or \</a\> ending the link. Other markup can
        be used dependign on the style support.

        @param: B{text} The text to set, or None to clear
        """
        elm_anchorblock_text_set(self.obj, text)

    def hover_parent_set(self, c_evas.Object parent):
        """
        Set the parent of the hover popup

        This sets the parent of the hover that anchorblock will create. See hover
        objects for more information on this.

        @param: B{parent} The parent the hover should use
        """
        elm_anchorblock_hover_parent_set(self.obj, parent.obj)

    def hover_style_set(self, style):
        """
        Set the style that the hover should use

        This sets the style for the hover that anchorblock will create. See hover
        objects for more information

        @param: B{style} The style to use
        """
        elm_anchorblock_hover_style_set(self.obj, style)

    def hover_end(self):
        """
        Stop the hover popup in the anchorblock

        This will stop the hover popup in the anchorblock if it is currently active.
        """
        elm_anchorblock_hover_end(self.obj)


