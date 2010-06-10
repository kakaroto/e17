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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.


class Manager(object):
    queue = []

    def __init__(self):
        self.__callbacks = {}

    def callback_add(self, event, callback):
        callbacks_list = self.__callbacks.get(event)
        if not callbacks_list:
            callbacks_list = []
            self.__callbacks[event] = callbacks_list
        elif callback in callbacks_list:
            return
        callbacks_list.append(callback)

    def callback_del(self, event, callback):
        callbacks_list = self.__callbacks.get(event)
        if callbacks_list and callback in callbacks_list:
            callbacks_list.remove(callback)

    def callbacks_clear(self, event):
        del self.__callbacks[event]

    def event_emit(self, event, data=None):
        # print "EMIT", event, self, data
        queue = len(self.queue)
        callbacks_list = self.__callbacks.get(event)
        if callbacks_list:
            for callback in callbacks_list:
                self.queue.append((event, callback, self, data))
        if not queue:
            while self.queue:
                event, callback, emissor, data = self.queue.pop(0)
                # print "\t", event, callback, emissor, data
                callback(emissor, data)
