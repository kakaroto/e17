# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

#!/usr/bin/env python

import c_edje

from c_edje import Edje, shutdown, frametime_set, frametime_get, \
     freeze, thaw, fontset_append_get, fontset_append_set, \
     file_collection_list, file_group_exists, \
     file_data_get, file_cache_set, file_cache_get, file_cache_flush, \
     collection_cache_set, collection_cache_get, collection_cache_flush, \
     color_class_set, color_class_del, color_class_list, \
     text_class_set, text_class_del, text_class_list, \
     extern_object_min_size_set, extern_object_max_size_set, \
     extern_object_aspect_set, available_modules_get, message_signal_process, \
     module_load, external_param_info_get, external_type_get, \
     EdjeLoadError, Message, MessageSignal, MessageString, MessageInt, \
     MessageFloat, MessageStringSet, MessageIntSet, MessageFloatSet, \
     MessageStringInt, MessageStringFloat, MessageStringIntSet, \
     MessageStringFloatSet, ExternalParam, ExternalParamInfo, \
     ExternalParamInfoInt, ExternalParamInfoInt, ExternalParamInfoDouble, \
     ExternalParamInfoString, ExternalParamInfoBool, ExternalType, \
     ExternalIterator

EDJE_MESSAGE_NONE = 0
EDJE_MESSAGE_SIGNAL = 1
EDJE_MESSAGE_STRING = 2
EDJE_MESSAGE_INT = 3
EDJE_MESSAGE_FLOAT = 4
EDJE_MESSAGE_STRING_SET = 5
EDJE_MESSAGE_INT_SET = 6
EDJE_MESSAGE_FLOAT_SET = 7
EDJE_MESSAGE_STRING_INT = 8
EDJE_MESSAGE_STRING_FLOAT = 9
EDJE_MESSAGE_STRING_INT_SET = 10
EDJE_MESSAGE_STRING_FLOAT_SET = 11

EDJE_ASPECT_CONTROL_NONE = 0
EDJE_ASPECT_CONTROL_NEITHER = 1
EDJE_ASPECT_CONTROL_HORIZONTAL = 2
EDJE_ASPECT_CONTROL_VERTICAL = 3
EDJE_ASPECT_CONTROL_BOTH = 4

EDJE_DRAG_DIR_NONE = 0
EDJE_DRAG_DIR_X = 1
EDJE_DRAG_DIR_Y = 2
EDJE_DRAG_DIR_XY = 3

EDJE_LOAD_ERROR_NONE = 0
EDJE_LOAD_ERROR_GENERIC = 1
EDJE_LOAD_ERROR_DOES_NOT_EXIST = 2
EDJE_LOAD_ERROR_PERMISSION_DENIED = 3
EDJE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED = 4
EDJE_LOAD_ERROR_CORRUPT_FILE = 5
EDJE_LOAD_ERROR_UNKNOWN_FORMAT = 6
EDJE_LOAD_ERROR_INCOMPATIBLE_FILE = 7
EDJE_LOAD_ERROR_UNKNOWN_COLLECTION = 8

EDJE_PART_TYPE_NONE      = 0
EDJE_PART_TYPE_RECTANGLE = 1
EDJE_PART_TYPE_TEXT      = 2
EDJE_PART_TYPE_IMAGE     = 3
EDJE_PART_TYPE_SWALLOW   = 4
EDJE_PART_TYPE_TEXTBLOCK = 5
EDJE_PART_TYPE_GRADIENT  = 6
EDJE_PART_TYPE_GROUP     = 7
EDJE_PART_TYPE_BOX       = 8
EDJE_PART_TYPE_TABLE     = 9
EDJE_PART_TYPE_EXTERNAL  = 10
EDJE_PART_TYPE_LAST      = 11

EDJE_TEXT_EFFECT_NONE                = 0
EDJE_TEXT_EFFECT_PLAIN               = 1
EDJE_TEXT_EFFECT_OUTLINE             = 2
EDJE_TEXT_EFFECT_SOFT_OUTLINE        = 3
EDJE_TEXT_EFFECT_SHADOW              = 4
EDJE_TEXT_EFFECT_SOFT_SHADOW         = 5
EDJE_TEXT_EFFECT_OUTLINE_SHADOW      = 6
EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW = 7
EDJE_TEXT_EFFECT_FAR_SHADOW          = 8
EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW     = 9
EDJE_TEXT_EFFECT_GLOW                = 10
EDJE_TEXT_EFFECT_LAST                = 11

EDJE_ACTION_TYPE_NONE          = 0
EDJE_ACTION_TYPE_STATE_SET     = 1
EDJE_ACTION_TYPE_ACTION_STOP   = 2
EDJE_ACTION_TYPE_SIGNAL_EMIT   = 3
EDJE_ACTION_TYPE_DRAG_VAL_SET  = 4
EDJE_ACTION_TYPE_DRAG_VAL_STEP = 5
EDJE_ACTION_TYPE_DRAG_VAL_PAGE = 6
EDJE_ACTION_TYPE_SCRIPT        = 7
EDJE_ACTION_TYPE_FOCUS_SET     = 8
EDJE_ACTION_TYPE_LUA_SCRIPT    = 9
EDJE_ACTION_TYPE_LAST          = 10

EDJE_TWEEN_MODE_NONE       = 0
EDJE_TWEEN_MODE_LINEAR     = 1
EDJE_TWEEN_MODE_SINUSOIDAL = 2
EDJE_TWEEN_MODE_ACCELERATE = 3
EDJE_TWEEN_MODE_DECELERATE = 4
EDJE_TWEEN_MODE_LAST       = 5

EDJE_EXTERNAL_PARAM_TYPE_INT    = 0
EDJE_EXTERNAL_PARAM_TYPE_DOUBLE = 1
EDJE_EXTERNAL_PARAM_TYPE_STRING = 2
EDJE_EXTERNAL_PARAM_TYPE_BOOL   = 3
EDJE_EXTERNAL_PARAM_TYPE_CHOICE = 4
EDJE_EXTERNAL_PARAM_TYPE_MAX    = 5

c_edje.init()
