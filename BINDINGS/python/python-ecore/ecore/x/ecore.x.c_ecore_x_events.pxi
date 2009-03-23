# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_ecore_x.pyx

cdef object _charp_to_str(char *p):
    if p != NULL:
        return p
    else:
        return None


cdef class EventKey(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Key *obj
        obj = <Ecore_Event_Key*>o
        self.keyname = _charp_to_str(obj.keyname)
        self.key = _charp_to_str(obj.key)
        self.string = _charp_to_str(obj.string)
        self.compose = _charp_to_str(obj.compose)
        self.window = Window_from_xid(<unsigned long><void*>obj.window)
        self.root_window = Window_from_xid(<unsigned long><void*>obj.root_window)
        self.event_window = Window_from_xid(<unsigned long><void*>obj.event_window)
        self.timestamp = obj.timestamp
        self.modifiers = obj.modifiers
        self.same_screen = obj.same_screen
        return 1


cdef class EventPoint:
    def __init__(self, int x, int y):
        self.x = x
        self.y = y


cdef class EventMouseButton(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Mouse_Button *obj
        obj = <Ecore_Event_Mouse_Button*>o
        self.window = Window_from_xid(<unsigned long><void*>obj.window)
        self.root_window = Window_from_xid(<unsigned long><void*>obj.root_window)
        self.event_window = Window_from_xid(<unsigned long><void*>obj.event_window)
        self.timestamp = obj.timestamp
        self.modifiers = obj.modifiers
        self.buttons = obj.buttons
        self.double_click = obj.double_click
        self.triple_click = obj.triple_click
        self.same_screen = obj.same_screen
        self.x = obj.x
        self.y = obj.y
        self.root = EventPoint(obj.root.x, obj.root.y)
        return 1


cdef class EventMouseMove(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Mouse_Move *obj
        obj = <Ecore_Event_Mouse_Move *>o
        self.window = Window_from_xid(<unsigned long><void*>obj.window)
        self.root_window = Window_from_xid(<unsigned long><void*>obj.root_window)
        self.event_window = Window_from_xid(<unsigned long><void*>obj.event_window)
        self.timestamp = obj.timestamp
        self.modifiers = obj.modifiers
        self.same_screen = obj.same_screen
        self.x = obj.x
        self.y = obj.y
        self.root = EventPoint(obj.root.x, obj.root.y)
        return 1


cdef class EventMouseIO(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Mouse_IO *obj
        obj = <Ecore_Event_Mouse_IO *>o
        self.window = Window_from_xid(<unsigned long><void*>obj.window)
        self.event_window = Window_from_xid(<unsigned long><void*>obj.event_window)
        self.timestamp = obj.timestamp
        self.modifiers = obj.modifiers
        self.x = obj.x
        self.y = obj.y
        return 1


cdef class EventMouseWheel(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Event_Mouse_Wheel *obj
        obj = <Ecore_Event_Mouse_Wheel *>o
        self.window = Window_from_xid(<unsigned long><void*>obj.window)
        self.root_window = Window_from_xid(<unsigned long><void*>obj.root_window)
        self.event_window = Window_from_xid(<unsigned long><void*>obj.event_window)
        self.timestamp = obj.timestamp
        self.modifiers = obj.modifiers
        self.same_screen = obj.same_screen
        self.direction = obj.direction
        self.z = obj.z
        self.x = obj.x
        self.y = obj.y
        self.root = EventPoint(obj.root.x, obj.root.y)
        return 1


cdef class EventWindowFocusIn(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Focus_In *obj
        obj = <Ecore_X_Event_Window_Focus_In *>o
        self.win = Window_from_xid(obj.win)
        self.mode = obj.mode
        self.detail = obj.detail
        self.time = obj.time
        return 1


cdef class EventWindowFocusOut(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Focus_Out *obj
        obj = <Ecore_X_Event_Window_Focus_Out *>o
        self.win = Window_from_xid(obj.win)
        self.mode = obj.mode
        self.detail = obj.detail
        self.time = obj.time
        return 1


cdef class EventWindowKeymap(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Keymap *obj
        obj = <Ecore_X_Event_Window_Keymap *>o
        self.win = Window_from_xid(obj.win)
        return 1


cdef class EventWindowDamage(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Damage *obj
        obj = <Ecore_X_Event_Window_Damage *>o
        self.win = Window_from_xid(obj.win)
        self.x = obj.x
        self.y = obj.y
        self.w = obj.w
        self.h = obj.h
        self.count = obj.count
        self.time = obj.time
        return 1


cdef class EventWindowVisibilityChange(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Visibility_Change *obj
        obj = <Ecore_X_Event_Window_Visibility_Change *>o
        self.win = Window_from_xid(obj.win)
        self.fully_obscured = obj.fully_obscured
        self.time = obj.time
        return 1


cdef class EventWindowCreate(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Create *obj
        obj = <Ecore_X_Event_Window_Create *>o
        self.win = Window_from_xid(obj.win)
        self.override = obj.override
        self.time = obj.time
        return 1


cdef class EventWindowDestroy(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Destroy *obj
        obj = <Ecore_X_Event_Window_Destroy *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventWindowHide(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Hide *obj
        obj = <Ecore_X_Event_Window_Hide *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventWindowShow(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Show *obj
        obj = <Ecore_X_Event_Window_Show *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventWindowShowRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Show_Request *obj
        obj = <Ecore_X_Event_Window_Show_Request *>o
        self.win = Window_from_xid(obj.win)
        self.parent = Window_from_xid(obj.parent)
        self.time = obj.time
        return 1


cdef class EventWindowReparent(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Reparent *obj
        obj = <Ecore_X_Event_Window_Reparent *>o
        self.win = Window_from_xid(obj.win)
        self.parent = Window_from_xid(obj.parent)
        self.time = obj.time
        return 1


cdef class EventWindowConfigure(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Configure *obj
        obj = <Ecore_X_Event_Window_Configure *>o
        self.win = Window_from_xid(obj.win)
        self.above_win = Window_from_xid(obj.abovewin)
        self.x = obj.x
        self.y = obj.y
        self.w = obj.w
        self.h = obj.h
        self.border = obj.border
        self.override = obj.override
        self.from_wm = obj.from_wm
        self.time = obj.time
        return 1


cdef class EventWindowConfigureRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Configure_Request *obj
        obj = <Ecore_X_Event_Window_Configure_Request *>o
        self.win = Window_from_xid(obj.win)
        self.above_win = Window_from_xid(obj.abovewin)
        self.x = obj.x
        self.y = obj.y
        self.w = obj.w
        self.h = obj.h
        self.border = obj.border
        self.stack_mode = obj.detail
        self.value_mask = obj.value_mask
        self.time = obj.time
        return 1


cdef class EventWindowGravity(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Gravity *obj
        obj = <Ecore_X_Event_Window_Gravity *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventWindowResizeRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Resize_Request *obj
        obj = <Ecore_X_Event_Window_Resize_Request *>o
        self.win = Window_from_xid(obj.win)
        self.w = obj.w
        self.h = obj.h
        self.time = obj.time
        return 1


cdef class EventWindowStack(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Stack *obj
        obj = <Ecore_X_Event_Window_Stack *>o
        self.win = Window_from_xid(obj.win)
        self.event_win = Window_from_xid(obj.event_win)
        self.stack_mode = obj.detail
        self.time = obj.time
        return 1


cdef class EventWindowStackRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Stack_Request *obj
        obj = <Ecore_X_Event_Window_Stack_Request *>o
        self.win = Window_from_xid(obj.win)
        self.parent = Window_from_xid(obj.parent)
        self.stack_mode = obj.detail
        self.time = obj.time
        return 1


cdef class EventDestkopChange(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Prop_Desktop_Change *obj
        obj = <Ecore_X_Event_Window_Prop_Desktop_Change *>o
        self.win = Window_from_xid(obj.win)
        self.desktop = obj.desktop
        self.time = obj.time
        return 1


cdef class EventPing(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Ping *obj
        obj = <Ecore_X_Event_Ping *>o
        self.win = Window_from_xid(obj.win)
        self.event_win = Window_from_xid(obj.event_win)
        self.time = obj.time
        return 1


cdef class EventWindowStateRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_State_Request *obj
        obj = <Ecore_X_Event_Window_State_Request *>o
        self.win = Window_from_xid(obj.win)
        self.action = obj.action
        self.state1 = obj.state[0]
        self.state2 = obj.state[1]
        self.source = obj.source
        return 1


cdef class EventFrameExtentsRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Frame_Extents_Request *obj
        obj = <Ecore_X_Event_Frame_Extents_Request *>o
        self.win = Window_from_xid(obj.win)
        return 1


cdef class EventWindowShape(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Shape *obj
        obj = <Ecore_X_Event_Window_Shape *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventScreensaverNotify(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Screensaver_Notify *obj
        obj = <Ecore_X_Event_Screensaver_Notify *>o
        self.win = Window_from_xid(obj.win)
        self.on = obj.on
        self.time = obj.time
        return 1


cdef class EventSyncCounter(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Sync_Counter *obj
        obj = <Ecore_X_Event_Sync_Counter *>o
        self.time = obj.time
        return 1


cdef class EventSyncAlarm(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Sync_Alarm *obj
        obj = <Ecore_X_Event_Sync_Alarm *>o
        self.time = obj.time
        self.alarm = obj.alarm
        return 1


cdef class EventScreenChange(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Screen_Change *obj
        obj = <Ecore_X_Event_Screen_Change *>o
        self.win = Window_from_xid(obj.win)
        self.root = Window_from_xid(obj.root)
        self.width = obj.width
        self.height = obj.height
        return 1


cdef class EventWindowDeleteRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Delete_Request *obj
        obj = <Ecore_X_Event_Window_Delete_Request *>o
        self.win = Window_from_xid(obj.win)
        self.time = obj.time
        return 1


cdef class EventWindowMoveResizeRequest(ecore.c_ecore.Event):
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_X_Event_Window_Move_Resize_Request *obj
        obj = <Ecore_X_Event_Window_Move_Resize_Request *>o
        self.win = Window_from_xid(obj.win)
        self.x = obj.x
        self.y = obj.y
        self.button = obj.button
        self.source = obj.source
        return 1


cdef class _Events:
    property KEY_DOWN:
        def __get__(self):
            return ECORE_EVENT_KEY_DOWN

    property KEY_UP:
        def __get__(self):
            return ECORE_EVENT_KEY_UP

    property MOUSE_BUTTON_DOWN:
        def __get__(self):
            return ECORE_EVENT_MOUSE_BUTTON_DOWN

    property MOUSE_BUTTON_UP:
        def __get__(self):
            return ECORE_EVENT_MOUSE_BUTTON_UP

    property MOUSE_MOVE:
        def __get__(self):
            return ECORE_EVENT_MOUSE_MOVE

    property MOUSE_IN:
        def __get__(self):
            return ECORE_EVENT_MOUSE_IN

    property MOUSE_OUT:
        def __get__(self):
            return ECORE_EVENT_MOUSE_OUT

    property MOUSE_WHEEL:
        def __get__(self):
            return ECORE_EVENT_MOUSE_WHEEL

    property WINDOW_FOCUS_IN:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_FOCUS_IN

    property WINDOW_FOCUS_OUT:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_FOCUS_OUT

    property WINDOW_KEYMAP:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_KEYMAP

    property WINDOW_DAMAGE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_DAMAGE

    property WINDOW_VISIBILITY_CHANGE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE

    property WINDOW_CREATE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_CREATE

    property WINDOW_DESTROY:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_DESTROY

    property WINDOW_HIDE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_HIDE

    property WINDOW_SHOW:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_SHOW

    property WINDOW_SHOW_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_SHOW_REQUEST

    property WINDOW_REPARENT:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_REPARENT

    property WINDOW_CONFIGURE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_CONFIGURE

    property WINDOW_CONFIGURE_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_CONFIGURE_REQUEST

    property WINDOW_GRAVITY:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_GRAVITY

    property WINDOW_RESIZE_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_RESIZE_REQUEST

    property WINDOW_STACK:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_STACK

    property WINDOW_STACK_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_STACK_REQUEST

    property WINDOW_PROPERTY:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_PROPERTY

    property WINDOW_COLORMAP:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_COLORMAP

    property WINDOW_MAPPING:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_MAPPING

    property SELECTION_CLEAR:
        def __get__(self):
            return ECORE_X_EVENT_SELECTION_CLEAR

    property SELECTION_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_SELECTION_REQUEST

    property SELECTION_NOTIFY:
        def __get__(self):
            return ECORE_X_EVENT_SELECTION_NOTIFY

    property CLIENT_MESSAGE:
        def __get__(self):
            return ECORE_X_EVENT_CLIENT_MESSAGE

    property WINDOW_SHAPE:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_SHAPE

    property SCREENSAVER_NOTIFY:
        def __get__(self):
            return ECORE_X_EVENT_SCREENSAVER_NOTIFY

    property SYNC_COUNTER:
        def __get__(self):
            return ECORE_X_EVENT_SYNC_COUNTER

    property SYNC_ALARM:
        def __get__(self):
            return ECORE_X_EVENT_SYNC_ALARM

    property SCREEN_CHANGE:
        def __get__(self):
            return ECORE_X_EVENT_SCREEN_CHANGE

    property DAMAGE_NOTIFY:
        def __get__(self):
            return ECORE_X_EVENT_DAMAGE_NOTIFY

    property WINDOW_DELETE_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_DELETE_REQUEST

    property WINDOW_MOVE_RESIZE_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_MOVE_RESIZE_REQUEST

    property WINDOW_STATE_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_WINDOW_STATE_REQUEST

    property FRAME_EXTENTS_REQUEST:
        def __get__(self):
            return ECORE_X_EVENT_FRAME_EXTENTS_REQUEST

    property PING:
        def __get__(self):
            return ECORE_X_EVENT_PING

    property DESKTOP_CHANGE:
        def __get__(self):
            return ECORE_X_EVENT_DESKTOP_CHANGE

    property STARTUP_SEQUENCE_NEW:
        def __get__(self):
            return ECORE_X_EVENT_STARTUP_SEQUENCE_NEW

    property STARTUP_SEQUENCE_CHANGE:
        def __get__(self):
            return ECORE_X_EVENT_STARTUP_SEQUENCE_CHANGE

    property STARTUP_SEQUENCE_REMOVE:
        def __get__(self):
            return ECORE_X_EVENT_STARTUP_SEQUENCE_REMOVE

Events = _Events()


cdef int _x_events_registered = 0
import ecore.c_ecore

# Just call after ecore_x_init() is done, otherwise all event type will be 0.
cdef int x_events_register() except 0:
    global _x_events_registered
    if _x_events_registered != 0:
        return 1
    _x_events_registered = 1

    reg = ecore.c_ecore._event_mapping_register

    reg(ECORE_EVENT_KEY_DOWN, EventKey)
    reg(ECORE_EVENT_KEY_UP, EventKey)
    reg(ECORE_EVENT_MOUSE_BUTTON_DOWN, EventMouseButton)
    reg(ECORE_EVENT_MOUSE_BUTTON_UP, EventMouseButton)
    reg(ECORE_EVENT_MOUSE_MOVE, EventMouseMove)
    reg(ECORE_EVENT_MOUSE_IN, EventMouseIO)
    reg(ECORE_EVENT_MOUSE_OUT, EventMouseIO)
    reg(ECORE_EVENT_MOUSE_WHEEL, EventMouseWheel)
    reg(ECORE_X_EVENT_WINDOW_FOCUS_IN, EventWindowFocusIn)
    reg(ECORE_X_EVENT_WINDOW_FOCUS_OUT, EventWindowFocusOut)
    reg(ECORE_X_EVENT_WINDOW_KEYMAP, EventWindowKeymap)
    reg(ECORE_X_EVENT_WINDOW_DAMAGE, EventWindowDamage)
    reg(ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE, EventWindowVisibilityChange)
    reg(ECORE_X_EVENT_WINDOW_CREATE, EventWindowCreate)
    reg(ECORE_X_EVENT_WINDOW_DESTROY, EventWindowDestroy)
    reg(ECORE_X_EVENT_WINDOW_HIDE, EventWindowHide)
    reg(ECORE_X_EVENT_WINDOW_SHOW, EventWindowShow)
    reg(ECORE_X_EVENT_WINDOW_SHOW_REQUEST, EventWindowShowRequest)
    reg(ECORE_X_EVENT_WINDOW_REPARENT, EventWindowReparent)
    reg(ECORE_X_EVENT_WINDOW_CONFIGURE, EventWindowConfigure)
    reg(ECORE_X_EVENT_WINDOW_CONFIGURE_REQUEST, EventWindowConfigureRequest)
    reg(ECORE_X_EVENT_WINDOW_GRAVITY, EventWindowGravity)
    reg(ECORE_X_EVENT_WINDOW_RESIZE_REQUEST, EventWindowResizeRequest)
    reg(ECORE_X_EVENT_WINDOW_STACK, EventWindowStack)
    reg(ECORE_X_EVENT_WINDOW_STACK_REQUEST, EventWindowStackRequest)
##     reg(ECORE_X_EVENT_WINDOW_PROPERTY, EventWindowProperty)            # TODO
##     reg(ECORE_X_EVENT_WINDOW_COLORMAP, EventWindowColormap)            # TODO
##     reg(ECORE_X_EVENT_WINDOW_MAPPING, EventWindowMapping)              # ?
##     reg(ECORE_X_EVENT_SELECTION_CLEAR, EventSelectionClear)            # TODO
##     reg(ECORE_X_EVENT_SELECTION_REQUEST, EventSelectionRequest)        # TODO
##     reg(ECORE_X_EVENT_SELECTION_NOTIFY, EventSelectionNotify)          # TODO
##     reg(ECORE_X_EVENT_CLIENT_MESSAGE, EventClientMessage)
    reg(ECORE_X_EVENT_WINDOW_SHAPE, EventWindowShape)
    reg(ECORE_X_EVENT_SCREENSAVER_NOTIFY, EventScreensaverNotify)
    reg(ECORE_X_EVENT_SYNC_COUNTER, EventSyncCounter)
    reg(ECORE_X_EVENT_SYNC_ALARM, EventSyncAlarm)
    reg(ECORE_X_EVENT_SCREEN_CHANGE, EventScreenChange)
##     reg(ECORE_X_EVENT_DAMAGE_NOTIFY, EventDamage)
    reg(ECORE_X_EVENT_WINDOW_DELETE_REQUEST, EventWindowDeleteRequest)
    reg(ECORE_X_EVENT_WINDOW_MOVE_RESIZE_REQUEST, EventWindowMoveResizeRequest)
    reg(ECORE_X_EVENT_WINDOW_STATE_REQUEST, EventWindowStateRequest)
    reg(ECORE_X_EVENT_FRAME_EXTENTS_REQUEST, EventFrameExtentsRequest)
    reg(ECORE_X_EVENT_PING, EventPing)
    reg(ECORE_X_EVENT_DESKTOP_CHANGE, EventDestkopChange)
##     reg(ECORE_X_EVENT_STARTUP_SEQUENCE_NEW, EventSTartupSequenceNew)       #?
##     reg(ECORE_X_EVENT_STARTUP_SEQUENCE_CHANGE, EventStartupSequenceChange) #?
##     reg(ECORE_X_EVENT_STARTUP_SEQUENCE_REMOVE, EventStartupSequenceRemove) #?

    return 1


def on_key_down_add(func, *args, **kargs):
    """Creates an ecore event handler for ECORE_EVENT_KEY_DOWN.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_KEY_DOWN,
                                      func, *args, **kargs)

def on_key_up_add(func, *args, **kargs):
    """Creates an ecore event handler for ECORE_EVENT_KEY_UP.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_KEY_UP,
                                      func, *args, **kargs)


def on_mouse_button_down_add(func, *args, **kargs):
    """Creates an ecore event handler for ECORE_EVENT_MOUSE_BUTTON_DOWN.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_BUTTON_DOWN,
                                      func, *args, **kargs)

def on_mouse_button_up_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_MOUSE_BUTTON_UP.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_BUTTON_UP,
                                      func, *args, **kargs)

def on_mouse_move_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_MOUSE_MOVE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_MOVE,
                                      func, *args, **kargs)

def on_mouse_in_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_MOUSE_IN.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_IN,
                                      func, *args, **kargs)

def on_mouse_out_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_MOUSE_OUT.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_OUT,
                                      func, *args, **kargs)

def on_mouse_wheel_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EVENT_MOUSE_WHEEL.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_EVENT_MOUSE_WHEEL,
                                      func, *args, **kargs)

def on_window_focus_in_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_FOCUS_IN.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_FOCUS_IN,
                                      func, *args, **kargs)

def on_window_focus_out_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_FOCUS_OUT.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_FOCUS_OUT,
                                      func, *args, **kargs)

def on_window_keymap_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_KEYMAP.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_KEYMAP,
                                      func, *args, **kargs)

def on_window_damage_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_DAMAGE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_DAMAGE,
                                      func, *args, **kargs)

def on_window_visibility_change_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE,
                                      func, *args, **kargs)

def on_window_create_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_CREATE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_CREATE,
                                      func, *args, **kargs)

def on_window_destroy_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_DESTROY.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_DESTROY,
                                      func, *args, **kargs)

def on_window_hide_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_HIDE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_HIDE,
                                      func, *args, **kargs)

def on_window_show_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_SHOW.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_SHOW,
                                      func, *args, **kargs)

def on_window_show_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_SHOW_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_SHOW_REQUEST,
                                      func, *args, **kargs)

def on_window_reparent_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_REPARENT.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_REPARENT,
                                      func, *args, **kargs)

def on_window_configure_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_CONFIGURE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_CONFIGURE,
                                      func, *args, **kargs)

def on_window_configure_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_CONFIGURE_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_CONFIGURE_REQUEST,
                                      func, *args, **kargs)

def on_window_gravity_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_GRAVITY.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_GRAVITY,
                                      func, *args, **kargs)

def on_window_resize_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_RESIZE_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_RESIZE_REQUEST,
                                      func, *args, **kargs)

def on_window_stack_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_STACK.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_STACK,
                                      func, *args, **kargs)

def on_window_stack_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_STACK_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_STACK_REQUEST,
                                      func, *args, **kargs)

## def on_window_property_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_WINDOW_PROPERTY.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_PROPERTY,
##                                       func, *args, **kargs)

## def on_window_colormap_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_WINDOW_COLORMAP.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_COLORMAP,
##                                       func, *args, **kargs)

## def on_window_mapping_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_WINDOW_MAPPING.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_MAPPING,
##                                       func, *args, **kargs)

## def on_selection_clear_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_SELECTION_CLEAR.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SELECTION_CLEAR,
##                                       func, *args, **kargs)

## def on_selection_request_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_SELECTION_REQUEST.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SELECTION_REQUEST,
##                                       func, *args, **kargs)

## def on_selection_notify_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_SELECTION_NOTIFY.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SELECTION_NOTIFY,
##                                       func, *args, **kargs)

## def on_client_message_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_CLIENT_MESSAGE.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_CLIENT_MESSAGE,
##                                       func, *args, **kargs)

def on_window_shape_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_SHAPE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_SHAPE,
                                      func, *args, **kargs)

def on_screensaver_notify_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_SCREENSAVER_NOTIFY.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SCREENSAVER_NOTIFY,
                                      func, *args, **kargs)

def on_sync_counter_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_SYNC_COUNTER.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SYNC_COUNTER,
                                      func, *args, **kargs)

def on_sync_alarm_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_SYNC_ALARM.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SYNC_ALARM,
                                      func, *args, **kargs)

def on_screen_change_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_SCREEN_CHANGE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_SCREEN_CHANGE,
                                      func, *args, **kargs)

## def on_damage_notify_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_DAMAGE_NOTIFY.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_DAMAGE_NOTIFY,
##                                       func, *args, **kargs)

def on_window_delete_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_DELETE_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_DELETE_REQUEST,
                                      func, *args, **kargs)

def on_window_move_resize_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_MOVE_RESIZE_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_MOVE_RESIZE_REQUEST,
                                      func, *args, **kargs)

def on_window_state_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_WINDOW_STATE_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_WINDOW_STATE_REQUEST,
                                      func, *args, **kargs)

def on_frame_extents_request_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_FRAME_EXTENTS_REQUEST.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_FRAME_EXTENTS_REQUEST,
                                      func, *args, **kargs)

def on_ping_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_PING.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_PING,
                                      func, *args, **kargs)

def on_desktop_change_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_X_EVENT_DESKTOP_CHANGE.

       @see: L{ecore.EventHandler}
    """
    return ecore.c_ecore.EventHandler(ECORE_X_EVENT_DESKTOP_CHANGE,
                                      func, *args, **kargs)

## def on_startup_sequence_new_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_STARTUP_SEQUENCE_NEW.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_STARTUP_SEQUENCE_NEW,
##                                       func, *args, **kargs)

## def on_startup_sequence_change_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_STARTUP_SEQUENCE_CHANGE.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_STARTUP_SEQUENCE_CHANGE,
##                                       func, *args, **kargs)

## def on_startup_sequence_remove_add(func, *args, **kargs):
##     """Create an ecore event handler for ECORE_X_EVENT_STARTUP_SEQUENCE_REMOVE.

##        @see: L{ecore.EventHandler}
##     """
##     return ecore.c_ecore.EventHandler(ECORE_X_EVENT_STARTUP_SEQUENCE_REMOVE,
##                                       func, *args, **kargs)

