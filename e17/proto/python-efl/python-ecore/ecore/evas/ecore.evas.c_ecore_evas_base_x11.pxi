# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Carsten Haitzler
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
