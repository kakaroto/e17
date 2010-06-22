# Copyright (C) 2010 Boris Faure
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

cdef extern from "Ecore_Win32.h":
    ####################################################################
    # Data Types
    #
    ctypedef struct Ecore_Win32_Window
    ctypedef void Ecore_Win32_Cursor

    ctypedef enum Ecore_Win32_Window_State:
        ECORE_WIN32_WINDOW_STATE_ICONIFIED
        ECORE_WIN32_WINDOW_STATE_MODAL
        ECORE_WIN32_WINDOW_STATE_STICKY
        ECORE_WIN32_WINDOW_STATE_MAXIMIZED_VERT
        ECORE_WIN32_WINDOW_STATE_MAXIMIZED_HORZ
        ECORE_WIN32_WINDOW_STATE_MAXIMIZED
        ECORE_WIN32_WINDOW_STATE_SHADED
        ECORE_WIN32_WINDOW_STATE_HIDDEN
        ECORE_WIN32_WINDOW_STATE_FULLSCREEN
        ECORE_WIN32_WINDOW_STATE_ABOVE
        ECORE_WIN32_WINDOW_STATE_BELOW
        ECORE_WIN32_WINDOW_STATE_DEMANDS_ATTENTION
        ECORE_WIN32_WINDOW_STATE_UNKNOWN

    ctypedef enum Ecore_Win32_Window_Type:
        ECORE_WIN32_WINDOW_TYPE_DESKTOP
        ECORE_WIN32_WINDOW_TYPE_DOCK
        ECORE_WIN32_WINDOW_TYPE_TOOLBAR
        ECORE_WIN32_WINDOW_TYPE_MENU
        ECORE_WIN32_WINDOW_TYPE_UTILITY
        ECORE_WIN32_WINDOW_TYPE_SPLASH
        ECORE_WIN32_WINDOW_TYPE_DIALOG
        ECORE_WIN32_WINDOW_TYPE_NORMAL
        ECORE_WIN32_WINDOW_TYPE_UNKNOWN

    ctypedef enum Ecore_Win32_Cursor_Shape:
        ECORE_WIN32_CURSOR_SHAPE_APP_STARTING
        ECORE_WIN32_CURSOR_SHAPE_ARROW
        ECORE_WIN32_CURSOR_SHAPE_CROSS
        ECORE_WIN32_CURSOR_SHAPE_HAND
        ECORE_WIN32_CURSOR_SHAPE_HELP
        ECORE_WIN32_CURSOR_SHAPE_I_BEAM
        ECORE_WIN32_CURSOR_SHAPE_NO
        ECORE_WIN32_CURSOR_SHAPE_SIZE_ALL
        ECORE_WIN32_CURSOR_SHAPE_SIZE_NESW
        ECORE_WIN32_CURSOR_SHAPE_SIZE_NS
        ECORE_WIN32_CURSOR_SHAPE_SIZE_NWSE
        ECORE_WIN32_CURSOR_SHAPE_SIZE_WE
        ECORE_WIN32_CURSOR_SHAPE_UP_ARROW
        ECORE_WIN32_CURSOR_SHAPE_WAIT

    ####################################################################
    # Functions
    #
    int    ecore_win32_init()
    int    ecore_win32_shutdown()
    int    ecore_win32_screen_depth_get()
    long   ecore_win32_current_time_get()
    void   ecore_win32_message_loop_begin()

    Ecore_Win32_Window *ecore_win32_window_new(Ecore_Win32_Window *parent,
                                               int                 x,
                                               int                 y,
                                               int                 width,
                                               int                 height)
    Ecore_Win32_Window *ecore_win32_window_override_new(Ecore_Win32_Window *parent,
                                                        int                 x,
                                                        int                 y,
                                                        int                 width,
                                                        int                 height)

    void ecore_win32_window_free(Ecore_Win32_Window *window)

    void *ecore_win32_window_hwnd_get(Ecore_Win32_Window *window)

    void ecore_win32_window_move(Ecore_Win32_Window *window,
                                  int                 x,
                                  int                 y)

    void ecore_win32_window_resize(Ecore_Win32_Window *window,
                                    int                 width,
                                    int                 height)

    void ecore_win32_window_move_resize(Ecore_Win32_Window *window,
                                         int                 x,
                                         int                 y,
                                         int                 width,
                                         int                 height)

    void ecore_win32_window_geometry_get(Ecore_Win32_Window *window,
                                          int                *x,
                                          int                *y,
                                          int                *width,
                                          int                *height)

    void ecore_win32_window_size_get(Ecore_Win32_Window *window,
                                      int                *width,
                                      int                *height)

    void ecore_win32_window_size_min_set(Ecore_Win32_Window *window,
                                          unsigned int        min_width,
                                          unsigned int        min_height)

    void ecore_win32_window_size_min_get(Ecore_Win32_Window *window,
                                          unsigned int       *min_width,
                                          unsigned int       *min_height)

    void ecore_win32_window_size_max_set(Ecore_Win32_Window *window,
                                          unsigned int        max_width,
                                          unsigned int        max_height)

    void ecore_win32_window_size_max_get(Ecore_Win32_Window *window,
                                          unsigned int       *max_width,
                                          unsigned int       *max_height)

    void ecore_win32_window_size_base_set(Ecore_Win32_Window *window,
                                           unsigned int        base_width,
                                           unsigned int        base_height)

    void ecore_win32_window_size_base_get(Ecore_Win32_Window *window,
                                           unsigned int       *base_width,
                                           unsigned int       *base_height)

    void ecore_win32_window_size_step_set(Ecore_Win32_Window *window,
                                           unsigned int        step_width,
                                           unsigned int        step_height)

    void ecore_win32_window_size_step_get(Ecore_Win32_Window *window,
                                           unsigned int       *step_width,
                                           unsigned int       *step_height)

    void ecore_win32_window_show(Ecore_Win32_Window *window)

    void ecore_win32_window_hide(Ecore_Win32_Window *window)

    void ecore_win32_window_raise(Ecore_Win32_Window *window)

    void ecore_win32_window_lower(Ecore_Win32_Window *window)

    void ecore_win32_window_title_set(Ecore_Win32_Window *window,
                                      char         *title)

    void ecore_win32_window_focus_set(Ecore_Win32_Window *window)

    void ecore_win32_window_iconified_set(Ecore_Win32_Window *window,
                                           int                 on)

    void ecore_win32_window_borderless_set(Ecore_Win32_Window *window,
                                            int                 on)

    void ecore_win32_window_fullscreen_set(Ecore_Win32_Window *window,
                                            int                 on)

    void ecore_win32_window_shape_set(Ecore_Win32_Window *window,
                                       unsigned short      width,
                                       unsigned short      height,
                                       unsigned char      *mask)

    void ecore_win32_window_cursor_set(Ecore_Win32_Window *window,
                                        Ecore_Win32_Cursor *cursor)

    void ecore_win32_window_state_set(Ecore_Win32_Window       *window,
                                       Ecore_Win32_Window_State *state,
                                       unsigned int              num)

    void ecore_win32_window_state_request_send(Ecore_Win32_Window      *window,
                                                Ecore_Win32_Window_State state,
                                                unsigned int             set)

    void ecore_win32_window_type_set(Ecore_Win32_Window      *window,
                                      Ecore_Win32_Window_Type  type)


    Ecore_Win32_Cursor *ecore_win32_cursor_new(void *pixels_and,
                                               void *pixels_xor,
                                               int         width,
                                               int         height,
                                               int         hot_x,
                                               int         hot_y)

    void                ecore_win32_cursor_free(Ecore_Win32_Cursor *cursor)

    Ecore_Win32_Cursor *ecore_win32_cursor_shape_get(Ecore_Win32_Cursor_Shape shape)

    int                 ecore_win32_cursor_size_get()



    int ecore_win32_dnd_init()
    int ecore_win32_dnd_shutdown()
    int ecore_win32_dnd_begin(char *data,
                              int         size)

    #int ecore_win32_dnd_register_drop_target(Ecore_Win32_Window                 *window,
    #                                         Ecore_Win32_Dnd_DropTarget_Callback callback)
    void ecore_win32_dnd_unregister_drop_target(Ecore_Win32_Window *window)
