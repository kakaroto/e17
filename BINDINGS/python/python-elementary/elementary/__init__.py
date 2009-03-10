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

import c_elementary

from c_elementary import Object, Window, Background, Icon, \
    Box, Button, Scroller, Label, Toggle, Frame, Table, \
    Clock, Layout, Hover, Entry, AnchorView, AnchorBlock, Bubble, \
    Photo, Hoversel, Toolbar, ToolbarItem, List, ListItem, Slider, \
    init, shutdown, run, exit

ELM_WIN_BASIC = 0
ELM_WIN_DIALOG_BASIC = 1

ELM_WIN_KEYBOARD_OFF = 0
ELM_WIN_KEYBOARD_ON = 1
ELM_WIN_KEYBOARD_ALPHA = 2
ELM_WIN_KEYBOARD_NUMERIC = 3
ELM_WIN_KEYBOARD_PIN = 4
ELM_WIN_KEYBOARD_PHONE_NUMBER = 5
ELM_WIN_KEYBOARD_HEX = 6
ELM_WIN_KEYBOARD_TERMINAL = 7
ELM_WIN_KEYBOARD_PASSWORD = 8

ELM_HOVER_AXIS_NONE = 0
ELM_HOVER_AXIS_HORIZONTAL = 1
ELM_HOVER_AXIS_VERTICAL = 2
ELM_HOVER_AXIS_BOTH = 3

ELM_TEXT_FORMAT_PLAIN_UTF8 = 0
ELM_TEXT_FORMAT_MARKUP_UTF8 = 1

ELM_ICON_NONE = 0
ELM_ICON_FILE = 1
ELM_ICON_STANDARD = 2
   
ELM_LIST_COMPRESS = 0
ELM_LIST_SCROLL = 1
ELM_LIST_LIMIT = 2

ELM_GENLIST_ITEM_NONE = 0
ELM_GENLIST_ITEM_SUBITEMS = 1
