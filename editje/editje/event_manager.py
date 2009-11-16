#
# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.

class Manager:
    queue = []

    def __init__(self):
        self._callbacks = {}

    def event_callback_add(self, event, callback):
        list = self._callbacks.get(event)
        if not list:
            list = []
            self._callbacks[event] = list
        elif callback in list:
            return
        list.append(callback)

    def event_callback_del(self, event, callback):
        list = self._callbacks.get(event)
        if list and callback in list:
            list.remove(callback)

    def event_callbacks_clear(self, event):
        del self._callbacks[event]

    def event_emit(self, event, data=None):
#        print "EMIT", event, self, data
        queue = len(self.queue)
        list = self._callbacks.get(event)
        if list:
            for callback in list:
                self.queue.append((event, callback, self, data))
        if not queue:
            while self.queue:
                event, callback, emissor, data = self.queue.pop(0)
#                print "\t", event, callback, emissor, data
                callback(emissor, data)
