#!/usr/bin/env python

import c_edje

init = c_edje.init
shutdown = c_edje.shutdown

frametime_set = c_edje.frametime_set
frametime_get = c_edje.frametime_get

freeze = c_edje.freeze
thaw = c_edje.thaw

fontset_append_get = c_edje.fontset_append_get
fontset_append_set = c_edje.fontset_append_set

file_collection_list = c_edje.file_collection_list
file_group_exists = c_edje.file_group_exists

file_data_get = c_edje.file_data_get

file_cache_set = c_edje.file_cache_set
file_cache_get = c_edje.file_cache_get
file_cache_flush = c_edje.file_cache_flush

collection_cache_set = c_edje.collection_cache_set
collection_cache_get = c_edje.collection_cache_get
collection_cache_flush = c_edje.collection_cache_flush

color_class_set = c_edje.color_class_set
color_class_del = c_edje.color_class_del
color_class_list = c_edje.color_class_list

text_class_set = c_edje.text_class_set
text_class_del = c_edje.text_class_del
text_class_list = c_edje.text_class_list

message_signal_process = c_edje.message_signal_process

EdjeLoadError = c_edje.EdjeLoadError

Message = c_edje.Message
MessageSignal = c_edje.MessageSignal
MessageString = c_edje.MessageString
MessageInt = c_edje.MessageInt
MessageFloat = c_edje.MessageFloat
MessageStringSet = c_edje.MessageStringSet
MessageIntSet = c_edje.MessageIntSet
MessageFloatSet = c_edje.MessageFloatSet
MessageStringInt = c_edje.MessageStringInt
MessageStringFloat = c_edje.MessageStringFloat
MessageStringIntSet = c_edje.MessageStringIntSet
MessageStringFloatSet = c_edje.MessageStringFloatSet


class Edje(c_edje.Edje):
    def __new__(type, canvas, file=None, group=None,
                size=None, pos=None, geometry=None, color=None, name=None):
        obj = c_edje.Edje.__new__(type, canvas)
        obj._new_obj()
        if file:
            obj.file_set(file, group)
        if size:
            obj.size_set(*size)
        if pos:
            obj.pos_set(*pos)
        if geometry:
            obj.geometry_set(*geometry)
        if color:
            obj.color_set(*c_evas.color_parse(color))
        if name:
            obj.name_set(name)
        if not size and not geometry:
            w, h = obj.size_min_get()
            obj.size_set(w, h)
        return obj
