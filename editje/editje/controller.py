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


class Controller(object):
    def __init__(self, parent):
        self.parent = parent
        self._view_load()

    # VIEW
    def _view_load(self):
        self._view = View(self, self.parent.view)

    def _view_get(self):
        return self._view

    view = property(_view_get)

    # TITLE
    def _title_set(self, value):
        self._view.title = value

    def _title_get(self):
        return self._view.title

    title = property(_title_get, _title_set)


class View(object):
    def __init__(self, controller, parent_view):
        self.controller = controller
        self.parent_view = parent_view

    # Title
    def _title_set(self, value):
        pass

    def _title_get(self):
        pass

    title = property(_title_get, _title_set)

    # Theme
    def _theme_get(self):
        return self.parent_view.theme

    theme = property(_theme_get)
