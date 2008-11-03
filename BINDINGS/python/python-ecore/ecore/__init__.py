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

#!/usr/bin/env python2

import c_ecore

from c_ecore import shutdown, time_get, loop_time_get, timer_add, \
     main_loop_begin, main_loop_quit, main_loop_iterate, event_handler_add, \
     animator_add, animator_frametime_set, animator_frametime_get, \
     idler_add, idle_enterer_add, idle_exiter_add, fd_handler_add, \
     Animator, Timer, Idler, IdleExiter, IdleEnterer, FdHandler, \
     Event, EventHandler


ECORE_FD_NONE = 0
ECORE_FD_READ = 1
ECORE_FD_WRITE = 2
ECORE_FD_ERROR = 4
ECORE_FD_ALL = 7


class MainLoop(object):
    @staticmethod
    def begin():
        main_loop_begin()

    @staticmethod
    def quit():
        main_loop_quit()

    @staticmethod
    def iterate():
        main_loop_iterate()

c_ecore.init()
