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

class AnchorViewInfo:
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

cdef void _anchorview_callback(void *cbtl, c_evas.Evas_Object *o, void *event_info) with gil:
    cdef Elm_Entry_Anchorview_Info *ei
    l = <object>cbtl
    for cbt in l:
        try:
            obj, callback, args, kwargs = cbt
            if event_info is NULL:
                av = None
            else:
                ei = <Elm_Entry_Anchorview_Info*>event_info
                av = AnchorViewInfo()
                av.name = ei.name
                av.button = ei.button
                av.hover = Hover(None, <object>ei.hover);
                av.anchor = (ei.anchor.x, ei.anchor.y,
                             ei.anchor.w, ei.anchor.h)
                av.hover_parent = (ei.hover_parent.x, ei.hover_parent.y,
                                   ei.hover_parent.w, ei.hover_parent.h)
                av.hover_left = ei.hover_left
                av.hover_right = ei.hover_right
                av.hover_top = ei.hover_top
                av.hover_bottom = ei.hover_bottom
            callback(obj, av, *args, **kwargs)
        except Exception, e:
            traceback.print_exc()

cdef class AnchorView(Object):
    """
    This is just like the Anchorblock object, but provides a scroller to hold
    the text automatically.

    Signals that you can add callbacks for are:

    anchor,clicked - anchor called was clicked. event_info is anchor info -
    AnchorViewInfo
    This signal can be bound with "callback_clicked_add"
    """

    cdef object _cbs
    def __init__(self, c_evas.Object parent):
        """Initialize the Anchorview

        @parm: B{parent} Parent window
        """
        Object.__init__(self, parent.evas)
        self._set_obj(elm_anchorview_add(parent.obj))
        self._cbs = {}

    def callback_clicked_add(self, func, *args, **kwargs):
        """ Bound to signal "anchor, clicked"

        The callback should have the following signature:
            callback(obj, av, *args, **kargs)
        obj: the object raising the signal
        ab: AnchorViewInfo

        The value set is either the callback function or a tuple like
            (callback, *args, **kargs)
        """
        cdef object cbt

        if not callable(func):
            raise TypeError("callback is not callable")

        event = "anchor,clicked"

        cbt = (self, func, args, kwargs)

        if self._cbs.has_key(event):
            self._cbs[event].append(cbt)
        else:
            self._cbs[event] = [cbt]
            # register callback
            c_evas.evas_object_smart_callback_add(self.obj, event,
                                                  _anchorview_callback,
                                                  <void *>self._cbs[event])

    def callback_clicked_remove(self, func = None, *args, **kwargs):
        event = "anchor,clicked"
        if self._cbs and self._cbs.has_keys(event):
            if func is None:
                c_evas.evas_object_smart_callback_del(self.obj, event,
                                                      _anchorview_callback)
                self._cbs[event] = None
            else:
                for i, cbt in enumerate(self._cbs[event]):
                    if cbt is not None and (self, func, args, kwargs) == <object>cbt:
                        self._cbs[event][i] = None
                if not self._cbs[event]:
                    c_evas.evas_object_smart_callback_del(self.obj, event,
                                                          _anchorview_callback)

    def text_set(self, text):
        """
        Set the text markup of the anchorview

        This sets the text of the anchorview to be the text given as @p text. This
        text is in markup format with \<a href=XXX\> beginning an achor with the
        string link of 'XXX', and \</\> or \</a\> ending the link. Other markup can
        be used dependign on the style support.

        @param: B{text} The text to set, or None to clear
        """
        elm_anchorview_text_set(self.obj, text)

    def hover_parent_set(self, c_evas.Object parent):
        """
        Set the parent of the hover popup

        This sets the parent of the hover that anchorview will create. See hover
        objects for more information on this.

        @param: B{parent} The parent the hover should use
        """
        elm_anchorview_hover_parent_set(self.obj, parent.obj)

    def hover_style_set(self, style):
        """
        Set the style that the hover should use

        This sets the style for the hover that anchorview will create. See hover
        objects for more information

        @param: B{style} The style to use
        """
        elm_anchorview_hover_style_set(self.obj, style)

    def hover_end(self):
        """
        Stop the hover popup in the anchorview

        This will stop the hover popup in the anchorview if it is currently active.
        """
        elm_anchorview_hover_end(self.obj)

