# Copyright (c) 2008 Simon Busch
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

import sys
import evas.c_evas
cimport evas.c_evas as c_evas
cimport evas.python
import traceback

def init():
    # Partly from python-etk
    cdef int argc, i, arg_len
    cdef char **argv, *arg
    argc_orig = argc = len(sys.argv)
    argv = <char **>PyMem_Malloc(argc * sizeof(char *))
    for i from 0 <= i < argc:
        arg = sys.argv[i]
        arg_len = len(sys.argv[i])
        argv[i] = <char *>PyMem_Malloc(arg_len + 1)
        memcpy(argv[i], arg, arg_len + 1)

    elm_init(argc, argv)

def shutdown():
    elm_shutdown()

def run():
    elm_run()

def exit():
    elm_exit()

flag = False    

cdef object _callback_mappings
_callback_mappings = dict()

cdef void _object_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _callback_mappings.get(<long>obj,None)
        if mapping is not None:
            func = mapping.get(<char*>data,None)
        
            if not callable(func):
                raise TypeError("func is not callable")
        
            func(mapping["__class__"],<char*>data)
    except Exception, e:
        traceback.print_exc()

cdef class Object(evas.c_evas.Object):
    """
    elementary.Object

    An abstract class to manage object and callback handling. All 
    widgets are based on this class
    """

    
    def _callback_add(self, event, func):
        """Add a callback for this object

        Add a function as new callback-function for a specified event. The
        function will be called, if the event occured.

        @parm: B{event} Name of the event
        @parm: B{func} Function should be called, when the event occured
        """
        if not callable(func):
            raise TypeError("func is not callable")
        
        # implement per object event <> func list in global var _callback_mappings
        # _object_callback looks for the object, saved in Evas_Object in the callback list
        # and calls every func that matched the event
        
        mapping = _callback_mappings.get(<long>self.obj, None)
        if mapping is None:
            mapping = dict()
            mapping["__class__"] =  self
            mapping[event] = func
            _callback_mappings[<long>self.obj] = mapping
        else:
            mapping[event] = func
            _callback_mappings[<long>self.obj] = mapping
        
        # register callback
        e = event
        c_evas.evas_object_smart_callback_add(self.obj, event, _object_callback, <char *>e)
            
    def _callback_remove(self, event):
        """Removes all callback functions for the event
        
        Will remove all callback functions for the specified event. 

        @parm: B{event} Name of the event whose events should be removed
        """

        mapping = _callback_mappings.get(<long>self.obj, None)
        if mapping is not None:
            mapping.pop(event)
            _callback_mappings[<long>self.obj] = mapping

cdef class Canvas(evas.c_evas.Canvas):
    def __init__(self, Object obj):
        self._set_obj(c_evas.evas_object_evas_get(obj.obj))

cdef class Window(Object):
    """
    Elementary Wind ow class
    
    This class represents a basic window.
    """

    def __init__(self, name, type):
        """
        @parm: B{name} Name of the window
        @parm: B{type} Type of the Window 
        """
        self._set_obj(elm_win_add(NULL, name, type))
    
    property canvas:
        def __get__(self):
            """
            Returns the canvas used by the window

            @rtype Canvas
            """
            cdef c_evas.Evas *e
            e = c_evas.evas_object_evas_get(self.obj)
            return evas.c_evas._Canvas_from_instance(<long>e)

    property destroy:
        def __set__(self, value):
            """
            Set the callback-function for the destroy-event

            @parm: B{value} Function which would be called, if the event occur
            """
            self._callback_add("delete-request",value)
        
    def resize_object_add(self,c_evas.Object obj):
        """
        Sets an object which sould be resized, when the window changes his size

        @parm: B{obj} Widget object
        """
        elm_win_resize_object_add(self.obj, obj.obj)

    def resize_object_del(self,c_evas.Object obj):
        """
        Removes a object from the resizing list

        @parm: B{obj} Widget object
        """
        elm_win_resize_object_del(self.obj, obj.obj)
    
    def title_set(self,title):
        """
        Sets the title of the window

        @parm: B{title} Title for the window
        """
        elm_win_title_set(self.obj, title)

    def activate(self):
        """Activates the window"""
        elm_win_activate(self.obj)
    
    def autodel_set(self,autodel):
        """
        Set the auto deletion property of the window

        @parm: B{audodel} Auto deletion property
        """
        elm_win_autodel_set(self.obj, autodel)

    def borderless_set(self, borderless):
        """
        Set the borderless property of the window

        @parm: B{borderless} Borderless property
        """
        elm_win_borderless_set(self.obj, borderless)

    def shaped_set(self,shaped):
        """Set the shaped property of the window

        @parm: B{shaped} Shaped property
        """
        elm_win_shaped_set(self.obj, shaped)

    def alpha_set(self,alpha):
        """
        Set the alpha value of the window

        @parm: b{alpha} Alpha value
        """
        elm_win_alpha_set(self.obj, alpha)
        
    def override_set(self, override):
        """
        Set the override property

        @parm: B{override} Override property
        """
        elm_win_override_set(self.obj, override)

    def keyboard_mode_set(self, mode):
        """
        Set the keyboard mode of the window

        @parm: B{mode} Keyboard mode
        """
        elm_win_keyboard_mode_set(self.obj, mode)

    def keyboard_win_set(self, is_keyboard):
        """
        Display the keyboard

        @parm: B{is_keyboard} Keyboard is visible?
        """
        elm_win_keyboard_win_set(self.obj, is_keyboard)

    # TODO
    """
    def xwindow_get(self):
        cdef Ecore_X_Window xwin
        xwin = elm_win_xwindow_get(self.obj)
        

        return Null
    """
    
cdef class Background(Object):
    """
    Background widget object
    
    This widget represent a background of a window. It holds a 
    image as background.
    """
    
    def __init__(self, c_evas.Object parent):
        """Initialize the background
        
        @parm: B{parent} Parent window
        """
        self._set_obj(elm_bg_add(parent.obj))
        
    def file_set(self, filename, group = ""):
        """
        Set the image for the background
        
        @parm: B{filename} filename of the background image
        @parm: B{group} 
        """
        elm_bg_file_set(self.obj, filename, group)

cdef class Icon(Object):
    """
    A simple icon widget

    If you need a widget which presents a icon and is clickable, this widget
    is the best option for you.
    """
    def __init__(self, c_evas.Object parent):
        """
        @parm: B{parent} Parent Object
        """
        self._set_obj(elm_icon_add(parent.obj))


    def file_set(self, filename, group = None):
        """
        Set the path of the image for the icon

        @parm: B{filename} Filename of the image
        """
        if group == None:
            elm_icon_file_set(self.obj, filename, NULL)
        else:
            elm_icon_file_set(self.obj, filename, group)
        
    property clicked:
        """
        If the event clicked occured, the specified callback function will be 
        called.

        @parm: B{value} Callback function
        """
        def __set__(self, value):
            self._callback_add("clicked",value)

    def standard_set(self,standard):
        elm_icon_standard_set(self.obj,standard)

    def smooth_set(self,smooth):
        elm_icon_smooth_set(self.obj,smooth)

    def no_scale_set(self,no_scale):
        elm_icon_no_scale_set(self.obj,no_scale)

    def scale_set(self,scale_up,scale_down):
        elm_icon_scale_set(self.obj,scale_up,scale_down)

    def fill_outside_set(self,fill_outside):
        elm_icon_fill_outside_set(self.obj,fill_outside)

cdef class Box(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_box_add(parent.obj))

    def horizontal_set(self,horizontal):
        elm_box_horizontal_set(self.obj,horizontal)

    def homogenous_set(self,homogenous):
        elm_box_homogenous_set(self.obj,homogenous)

    def pack_start(self, c_evas.Object obj):
        elm_box_pack_start(self.obj,obj.obj)
    
    def pack_end(self, c_evas.Object obj):
        elm_box_pack_end(self.obj, obj.obj)
    
    def pack_before(self, c_evas.Object obj, c_evas.Object before):
        elm_box_pack_before(self.obj,obj.obj,before.obj)

    def pack_after(self, c_evas.Object obj, c_evas.Object after):
        elm_box_pack_after(self.obj,obj.obj,after.obj)
    
cdef class Button(Object):
    def __init__(self,c_evas.Object parent):
        self._set_obj(elm_button_add(parent.obj))
        
    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)
    
    def label_set(self, label): 
        elm_button_label_set(self.obj, label)

    def icon_set(self, c_evas.Object icon):
        elm_button_icon_set(self.obj, icon.obj)

    def style_set(self, style):
        elm_button_style_set(self.obj, style)

cdef class Scroller(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_scroller_add(parent.obj))

    def content_set(self, c_evas.Object child):
        elm_scroller_content_set(self.obj, child.obj)

    def content_min_limit(self, w, h):
        elm_scroller_content_min_limit(self.obj, w, h)

cdef class Label(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_label_add(parent.obj))
        
    def label_set(self, label):
        elm_label_label_set(self.obj,label)
        
cdef class Toggle(Object):
    """
    A toggle widget
    """
    cdef int state_ptr
    
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_toggle_add(parent.obj))
        
        # set the state pointer for the toggle widget to always
        # know the current state
        self.state_ptr = 0
        elm_toggle_state_pointer_set(self.obj, &self.state_ptr)
        
        
    property changed:
        def __set__(self, value):
            self._callback_add("changed",value)
    
    def label_set(self, label):
        elm_toggle_label_set(self.obj, label)
        
    def icon_set(self, c_evas.Object icon):
        elm_toggle_icon_set(self.obj, icon.obj)
        
    def states_labels_set(self,onlabel, offlabel):
        elm_toggle_states_labels_set(self.obj, onlabel, offlabel)
        
    def state_set(self, state):
        if state == True:
            elm_toggle_state_set(self.obj, 1)
        elif state == False:
            elm_toggle_state_set(self.obj, 0)

    def state_get(self):
        if self.state_ptr == 1:
            return True
        return False
    
cdef class Frame(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_frame_add(parent.obj))
        
    def label_set(self, label):
        elm_frame_label_set(self.obj, label)
        
    def content_set(self, c_evas.Object content):
        elm_frame_content_set(self.obj, content.obj)

    def style_set(self, style):
        elm_frame_style_set(self.obj, style)

    def best_content_location_get(self, axis):
        elm_hover_best_content_location_get(self.obj, axis)
        
cdef class Table(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_table_add(parent.obj))
        
    def homogenous_set(self, homogenous):
        elm_table_homogenous_set(self.obj, homogenous)
        
    def pack(self, c_evas.Object subobj, x, y, w, h):
        elm_table_pack(self.obj, subobj.obj, x, y, w, h)
    
cdef class Clock(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_clock_add(parent.obj))
        
    def time_set(self, hours, minutes, seconds):
        elm_clock_time_set(self.obj, hours, minutes, seconds)
        
    def time_get(self):
        """
        Returns the current time of the clock

        @rtype: list of int
        """
        cdef int hrs
        cdef int min
        cdef int sec

        elm_clock_time_get(self.obj, &hrs, &min, &sec)

        return (hrs, min, sec)
        
    def edit_set(self, edit):
        elm_clock_edit_set(self.obj, edit)
        
    def show_am_pm_set(self, am_pm):
        elm_clock_show_am_pm_set(self.obj, am_pm)
        
    def show_seconds_set(self, seconds):
        elm_clock_show_seconds_set(self.obj, seconds)
        
cdef class Layout(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_layout_add(parent.obj))
        
    def file_set(self, filename, group):
        elm_layout_file_set(self.obj, filename, group)
        
    def content_set(self, swallow, c_evas.Object content):
        elm_layout_content_set(self.obj, swallow, content.obj)
    
    # TODO: need?
    #def edje_get(self):
    
cdef class Hover(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_hover_add(parent.obj))
        
    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)
        
    def target_set(self, c_evas.Object target):
        elm_hover_target_set(self.obj, target.obj)
        
    def parent_set(self, c_evas.Object parent):
        elm_hover_parent_set(self.obj, parent.obj)
        
    def content_set(self, swallow, c_evas.Object content):
        elm_hover_content_set(self.obj, swallow, content.obj)
        
    def style_set(self, style):
        elm_hover_style_set(self.obj, style)

    # TODO: best_content_location_get(...)
        
cdef class Entry(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_entry_add(parent.obj))

    property changed:
        def __set__(self, value):
            self._callback_add("changed",value)
            
    property selection_start:
        def __set__(self, value):
            self._callback_add("selection,start",value)
        
    property selection_changed:
        def __set__(self, value):
            self._callback_add("selection,changed",value)
    
    property selection_cleared:
        def __set__(self, value):
            self._callback_add("selection,cleared",value)
    
    property selection_paste:
        def __set__(self, value):
            self._callback_add("selection,paste",value)
            
    property selection_copy:
        def __set__(self, value):
            self._callback_add("selection,copy",value)
            
    property selection_cut:
        def __set__(self, value):
            self._callback_add("selection,cut",value)
            
    property cursor_changed:
        def __set__(self, value):
            self._callback_add("cursor,changed",value)
    
    property anchor_clicked:
        def __set__(self, value):
            self._callback_add("anchor,clicked",value)
            
    property activated:
        def __set__(self, value):
            self._callback_add("activated",value)
        
    def single_line_set(self, single_line):
        elm_entry_single_line_set(self.obj, single_line)
        
    def password_set(self, password):
        elm_entry_password_set(self.obj, password)
        
    def entry_set(self, entry):
        elm_entry_entry_set(self.obj, entry)
        
    def entry_get(self):
        return elm_entry_entry_get(self.obj)
        
    def selection_get(self):
        return elm_entry_selection_get(self.obj)
        
    def entry_insert(self, entry):
        elm_entry_entry_insert(self.obj, entry)
        
    def line_wrap_set(self, wrap):
        elm_entry_line_wrap_set(self.obj, wrap)
        
    def editable_set(self, editable):
        elm_entry_editable_set(self.obj, editable)

    def select_none(self):
        elm_entry_select_none(self.obj)

    def select_all(self):
        elm_entry_select_all(self.obj)

cdef class AnchorView(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_anchorview_add(parent.obj))
        
    property anchor_clicked:
        def __set__(self, value):
            self._callback_add("anchor,clicked",value)

    def text_set(self, text):
        elm_anchorview_text_set(self.obj, text)

    def hover_parent_set(self, c_evas.Object parent):
        elm_anchorview_hover_parent_set(self.obj, parent.obj)

    def hover_style_set(self, style):
        elm_anchorview_hover_style_set(self.obj, style)

    def hover_end(self):
        elm_anchorview_hover_end(self.obj)
        

cdef class AnchorBlock(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_anchorblock_add(parent.obj))
    
    property anchor_clicked:
        def __set__(self, value):
            self._callback_add("anchor,clicked",value)

    def text_set(self, text):
        elm_anchorblock_text_set(self.obj, text)

    def hover_parent_set(self, c_evas.Object parent):
        elm_anchorblock_hover_parent_set(self.obj, parent.obj)
    
    def hover_style_set(self, style):
        elm_anchorblock_hover_style_set(self.obj, style)

    def hover_end(self):
        elm_anchorblock_hover_end(self.obj)


cdef class Bubble(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_bubble_add(parent.obj))
        
    def label_set(self, label):
        elm_bubble_label_set(self.obj, label)
        
    def info_set(self, info):
        elm_bubble_info_set(self.obj, info)
        
    def content_set(self, c_evas.Object content):
        elm_bubble_content_set(self.obj, content.obj)
    
    def icon_set(self, c_evas.Object icon):
        elm_bubble_icon_set(self.obj, icon.obj)
        
    def corner_set(self, corner):
        elm_bubble_corner_set(self.obj, corner)

cdef class Photo(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_photo_add(parent.obj))
    
    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)
    
    def file_set(self, filename):
        elm_photo_file_set(self.obj, filename)

    def size_set(self, size):
        elm_photo_size_set(self.obj, size)


cdef object _hoversel_callback_mapping
_hoversel_callback_mapping = dict()

cdef void _hoversel_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _hoversel_callback_mapping.get(<long>event_info)
        if mapping is not None:
            callback = mapping["callback"] 
            if callback is not None and callable(callback):
                callback(mapping["class"], "clicked")
        else:
            print "ERROR: no callback available for the hoversel-item"
    except Exception, e:
        traceback.print_exc()

cdef class HoverselItem:
    """A item for the hoversel widget"""
    cdef Elm_Hoversel_Item *item

    def __init__(self, c_evas.Object hoversel, label, icon_file, icon_type, callback, data = None):
        self.item = elm_hoversel_item_add(hoversel.obj, label, icon_file, icon_type,_hoversel_callback, NULL)
       
        # Create the mapping
        mapping = dict()
        mapping["class"] = hoversel
        mapping["callback"] = callback
        _hoversel_callback_mapping[<long>self.item] = mapping

    def delete(self):
        """Delete the hoversel item"""
        elm_hoversel_item_del(self.item)

cdef class Hoversel(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_hoversel_add(parent.obj))

    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)
    
    property selected:
        def __set__(self, value):
            self._callback_add("selected", value)
            
    property dismissed:
        def __set__(self, value):
            self._callback_add("dismissed", value)
                
    def hover_parent_set(self, c_evas.Object parent):
        elm_hoversel_hover_parent_set(self.obj, parent.obj)
    
    def label_set(self, label):
        elm_hoversel_label_set(self.obj, label)
        
    def icon_set(self, c_evas.Object icon):
        elm_hoversel_icon_set(self.obj, icon.obj)
    
    def hover_end(self):
        elm_hoversel_hover_end(self.obj)
    
    def item_add(self, label, icon_file, icon_type, callback):
        return HoverselItem(self, label, icon_file, icon_type, callback)

cdef object _toolbar_callback_mapping
_toolbar_callback_mapping = dict()

cdef void _toolbar_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _toolbar_callback_mapping.get(<long>event_info)
        if mapping is not None:
            callback = mapping["callback"] 
            if callback is not None and callable(callback):
                callback(mapping["class"], "clicked")
        else:
            print "ERROR: no callback available for the item"
    except Exception, e:
        traceback.print_exc()

cdef class ToolbarItem:
    """
    A item for the toolbar
    """
    cdef Elm_Toolbar_Item *item

    def __new__(self):
        self.item = NULL

    def __init__(self, c_evas.Object toolbar, c_evas.Object icon, label, callback):
        if icon is not None:
            self.item = elm_toolbar_item_add(toolbar.obj, icon.obj, label, _toolbar_callback, NULL)
        else:
            self.item = elm_toolbar_item_add(toolbar.obj, NULL, label, _toolbar_callback, NULL)
        
        # Add a new callback in our mapping dict
        mapping = dict()
        mapping["class"] = self
        mapping["callback"] = callback
        _toolbar_callback_mapping[<long>self.item] = mapping

    def delete(self):
        """Delete the item"""
        elm_toolbar_item_del(self.item)

    def select(self):
        """Select the item"""
        elm_toolbar_item_select(self.item)

cdef class Toolbar(Object):
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_toolbar_add(parent.obj))

    def scrollable_set(self, scrollable):
        """
        Set the scrollable property

        @parm: L{scrollable}
        """
        if scrollable:
            elm_toolbar_scrollable_set(self.obj, 1)
        else:
            elm_toolbar_scrollable_set(self.obj, 0)
       
    def item_add(self, c_evas.Object icon, label, callback):
        """
        Adds a new item to the toolbar

        @note: Never pass the the same icon object to more than one item. For 
               a every item you must create a new icon!

        @parm: L{icon} icon for the item
        @parm: L{label} label for the item
        @parm: L{callback} function to click if the user clicked on the item
        """
        # Everything is done in the ToolbarItem class, because of wrapping the
        # C structures in python classes
        return ToolbarItem(self, icon, label, callback)

    property clicked:
        def __set__(self, value):
            """
            Set the callback function for the clicked-event

            @parm: L{value} callback function
            """
            self._callback_add("clicked", value)

cdef class List(Object):
    pass
