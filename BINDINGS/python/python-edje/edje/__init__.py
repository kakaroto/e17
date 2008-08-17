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
     extern_object_aspect_set, message_signal_process, EdjeLoadError, \
     Message, MessageSignal, MessageString, MessageInt, MessageFloat, \
     MessageStringSet, MessageIntSet, MessageFloatSet, MessageStringInt, \
     MessageStringFloat, MessageStringIntSet, MessageStringFloatSet


EDJE_ASPECT_CONTROL_NONE = 0
EDJE_ASPECT_CONTROL_NEITHER = 1
EDJE_ASPECT_CONTROL_HORIZONTAL = 2
EDJE_ASPECT_CONTROL_VERTICAL = 3
EDJE_ASPECT_CONTROL_BOTH = 4


c_edje.init()
