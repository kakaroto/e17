# This file is included verbatim by c_ecore_evas.pyx

cdef class BaseX11(EcoreEvas):
    def x_window_cursor_shape_set(self, int shape):
        """Set cursor to one pre-defined shape.

        Default value is ECORE_X_CURSOR_X, usual values are:
         - ECORE_X_CURSOR_ARROW
         - ECORE_X_CURSOR_CLOCK
         - ECORE_X_CURSOR_CROSS

        See ecore.x for available symbols.
        """
        cdef Ecore_X_Cursor x_cursor
        cdef Ecore_X_Window x_win
        x_cursor = ecore_x_cursor_shape_get(shape)
        x_win = self.window
        ecore_x_window_cursor_set(x_win, x_cursor)

    def x_window_virtual_keyboard_state_set(self, int type):
        cdef Ecore_X_Window x_win
        x_win = self.window
        ecore_x_e_virtual_keyboard_state_set(x_win, type)
