# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Caio Marcelo de Oliveira Filho, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

#!/usr/bin/env python2

import c_evas

from c_evas import Canvas, SmartObject, ClippedSmartObject, Rectangle, Line, \
     Image, FilledImage, Gradient, Polygon, Text, Rect, EvasLoadError, \
     shutdown, render_method_lookup, render_method_list, \
     async_events_fd_get, async_events_process, \
     color_argb_premul, color_argb_unpremul, color_parse, \
     color_rgb_to_hsv, color_hsv_to_rgb, image_mask_fill


EVAS_CALLBACK_MOUSE_IN = 0
EVAS_CALLBACK_MOUSE_OUT = 1
EVAS_CALLBACK_MOUSE_DOWN = 2
EVAS_CALLBACK_MOUSE_UP = 3
EVAS_CALLBACK_MOUSE_MOVE = 4
EVAS_CALLBACK_MOUSE_WHEEL = 5
EVAS_CALLBACK_FREE = 6
EVAS_CALLBACK_KEY_DOWN = 7
EVAS_CALLBACK_KEY_UP = 8
EVAS_CALLBACK_FOCUS_IN = 9
EVAS_CALLBACK_FOCUS_OUT = 10
EVAS_CALLBACK_SHOW = 11
EVAS_CALLBACK_HIDE = 12
EVAS_CALLBACK_MOVE = 13
EVAS_CALLBACK_RESIZE = 14
EVAS_CALLBACK_RESTACK = 15
EVAS_CALLBACK_DEL = 16
EVAS_CALLBACK_HOLD = 17
EVAS_CALLBACK_CHANGED_SIZE_HINTS = 18
EVAS_CALLBACK_IMAGE_PRELOADED = 19

EVAS_BUTTON_NONE = 0
EVAS_BUTTON_DOUBLE_CLICK = 1
EVAS_BUTTON_TRIPLE_CLICK = 2

EVAS_EVENT_FLAG_NONE = 0
EVAS_EVENT_FLAG_ON_HOLD = 1 << 0

EVAS_RENDER_BLEND = 0
EVAS_RENDER_BLEND_REL = 1
EVAS_RENDER_COPY = 2
EVAS_RENDER_COPY_REL = 3
EVAS_RENDER_ADD = 4
EVAS_RENDER_ADD_REL = 5
EVAS_RENDER_SUB = 6
EVAS_RENDER_SUB_REL = 7
EVAS_RENDER_TINT = 8
EVAS_RENDER_TINT_REL = 9
EVAS_RENDER_MASK = 10
EVAS_RENDER_MUL = 11

EVAS_TEXTURE_REFLECT = 0
EVAS_TEXTURE_REPEAT = 1
EVAS_TEXTURE_RESTRICT = 2
EVAS_TEXTURE_RESTRICT_REFLECT = 3
EVAS_TEXTURE_RESTRICT_REPEAT = 4
EVAS_TEXTURE_PAD = 5

EVAS_COLOR_SPACE_ARGB = 0
EVAS_COLOR_SPACE_AHSV = 1

EVAS_COLORSPACE_ARGB8888 = 0 # ARGB 32 bits per pixel, high-byte is Alpha
EVAS_COLORSPACE_YCBCR422P601_PL = 1 # YCbCr 4:2:2 Planar, ITU.BT-601 specs.
EVAS_COLORSPACE_YCBCR422P709_PL = 2 # YCbCr 4:2:2 Planar, ITU.BT-709 specs.
EVAS_COLORSPACE_RGB565_A5P = 3 # 16bit rgb565 + Alpha plane (5/8 bits) at end

EVAS_FONT_HINTING_NONE = 0
EVAS_FONT_HINTING_AUTO = 1
EVAS_FONT_HINTING_BYTECODE = 2

EVAS_TEXT_STYLE_PLAIN = 0
EVAS_TEXT_STYLE_SHADOW = 1
EVAS_TEXT_STYLE_OUTLINE = 2
EVAS_TEXT_STYLE_SOFT_OUTLINE = 3
EVAS_TEXT_STYLE_GLOW = 4
EVAS_TEXT_STYLE_OUTLINE_SHADOW = 5
EVAS_TEXT_STYLE_FAR_SHADOW = 6
EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW = 7
EVAS_TEXT_STYLE_SOFT_SHADOW = 8
EVAS_TEXT_STYLE_FAR_SOFT_SHADOW = 9

EVAS_OBJECT_POINTER_MODE_AUTOGRAB = 0
EVAS_OBJECT_POINTER_MODE_NOGRAB = 1

EVAS_IMAGE_ROTATE_NONE = 0
EVAS_IMAGE_ROTATE_90 = 1
EVAS_IMAGE_ROTATE_180 = 2
EVAS_IMAGE_ROTATE_270 = 3

EVAS_ASPECT_CONTROL_NONE = 0
EVAS_ASPECT_CONTROL_NEITHER = 1
EVAS_ASPECT_CONTROL_HORIZONTAL = 2
EVAS_ASPECT_CONTROL_VERTICAL = 3
EVAS_ASPECT_CONTROL_BOTH = 4

c_evas.init()
