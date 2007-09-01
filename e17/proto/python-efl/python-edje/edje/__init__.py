#!/usr/bin/env python

import c_edje

from c_edje import Edje, shutdown, frametime_set, frametime_get, \
     freeze, thaw, fontset_append_get, fontset_append_set, \
     file_collection_list, file_group_exists, \
     file_data_get, file_cache_set, file_cache_get, file_cache_flush, \
     collection_cache_set, collection_cache_get, collection_cache_flush, \
     color_class_set, color_class_del, color_class_list, \
     text_class_set, text_class_del, text_class_list, \
     message_signal_process, EdjeLoadError, \
     Message, MessageSignal, MessageString, MessageInt, MessageFloat, \
     MessageStringSet, MessageIntSet, MessageFloatSet, MessageStringInt, \
     MessageStringFloat, MessageStringIntSet, MessageStringFloatSet

c_edje.init()
