# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Emotion.
#
# Python-Emotion is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Emotion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Emotion.  If not, see <http://www.gnu.org/licenses/>.

cimport evas.c_evas
import evas.c_evas


cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Emotion.h":
    ctypedef enum Emotion_Module:
        EMOTION_MODULE_XINE = 0
        EMOTION_MODULE_GSTREAMER

    ctypedef enum Emotion_Event:
        EMOTION_EVENT_MENU1 = 0
        EMOTION_EVENT_MENU2
        EMOTION_EVENT_MENU3
        EMOTION_EVENT_MENU4
        EMOTION_EVENT_MENU5
        EMOTION_EVENT_MENU6
        EMOTION_EVENT_MENU7
        EMOTION_EVENT_UP
        EMOTION_EVENT_DOWN
        EMOTION_EVENT_LEFT
        EMOTION_EVENT_RIGHT
        EMOTION_EVENT_SELECT
        EMOTION_EVENT_NEXT
        EMOTION_EVENT_PREV
        EMOTION_EVENT_ANGLE_NEXT
        EMOTION_EVENT_ANGLE_PREV
        EMOTION_EVENT_FORCE
        EMOTION_EVENT_0
        EMOTION_EVENT_1
        EMOTION_EVENT_2
        EMOTION_EVENT_3
        EMOTION_EVENT_4
        EMOTION_EVENT_5
        EMOTION_EVENT_6
        EMOTION_EVENT_7
        EMOTION_EVENT_8
        EMOTION_EVENT_9
        EMOTION_EVENT_10

    ctypedef enum Emotion_Meta_Info:
        EMOTION_META_INFO_TRACK_TITLE
        EMOTION_META_INFO_TRACK_ARTIST
        EMOTION_META_INFO_TRACK_ALBUM
        EMOTION_META_INFO_TRACK_YEAR
        EMOTION_META_INFO_TRACK_GENRE
        EMOTION_META_INFO_TRACK_COMMENT
        EMOTION_META_INFO_TRACK_DISC_ID
        EMOTION_META_INFO_TRACK_COUNT

    cdef enum Emotion_Channel_Settings:
        EMOTION_CHANNEL_AUTO = -1
        EMOTION_CHANNEL_DEFAULT = 0

    evas.c_evas.Evas_Object *emotion_object_add(evas.c_evas.Evas *evas)
    void emotion_object_module_option_set(evas.c_evas.Evas_Object *obj, char *opt, char *val)
    evas.c_evas.Evas_Bool emotion_object_init(evas.c_evas.Evas_Object *obj, char *module_filename)

    void emotion_object_file_set(evas.c_evas.Evas_Object *obj, char *filename)
    char *emotion_object_file_get(evas.c_evas.Evas_Object *obj)

    void emotion_object_play_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool play)
    evas.c_evas.Evas_Bool emotion_object_play_get(evas.c_evas.Evas_Object *obj)

    void emotion_object_position_set(evas.c_evas.Evas_Object *obj, double sec)
    double emotion_object_position_get(evas.c_evas.Evas_Object *obj)

    evas.c_evas.Evas_Bool emotion_object_video_handled_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Bool emotion_object_audio_handled_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Bool emotion_object_seekable_get(evas.c_evas.Evas_Object *obj)
    double emotion_object_play_length_get(evas.c_evas.Evas_Object *obj)
    void emotion_object_size_get(evas.c_evas.Evas_Object *obj, int *iw, int *ih)
    void emotion_object_smooth_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool smooth)
    evas.c_evas.Evas_Bool emotion_object_smooth_scale_get(evas.c_evas.Evas_Object *obj)
    double emotion_object_ratio_get(evas.c_evas.Evas_Object *obj)

    void emotion_object_event_simple_send(evas.c_evas.Evas_Object *obj, Emotion_Event ev)

    void emotion_object_audio_volume_set(evas.c_evas.Evas_Object *obj, double vol)
    double emotion_object_audio_volume_get(evas.c_evas.Evas_Object *obj)
    void emotion_object_audio_mute_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool mute)
    evas.c_evas.Evas_Bool emotion_object_audio_mute_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_audio_channel_count(evas.c_evas.Evas_Object *obj)
    char *emotion_object_audio_channel_name_get(evas.c_evas.Evas_Object *obj, int channel)
    void emotion_object_audio_channel_set(evas.c_evas.Evas_Object *obj, int channel)
    int emotion_object_audio_channel_get(evas.c_evas.Evas_Object *obj)
    void emotion_object_video_mute_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool mute)
    evas.c_evas.Evas_Bool emotion_object_video_mute_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_video_channel_count(evas.c_evas.Evas_Object *obj)
    char *emotion_object_video_channel_name_get(evas.c_evas.Evas_Object *obj, int channel)
    void emotion_object_video_channel_set(evas.c_evas.Evas_Object *obj, int channel)
    int emotion_object_video_channel_get(evas.c_evas.Evas_Object *obj)
    void emotion_object_spu_mute_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool mute)
    evas.c_evas.Evas_Bool emotion_object_spu_mute_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_spu_channel_count(evas.c_evas.Evas_Object *obj)
    char *emotion_object_spu_channel_name_get(evas.c_evas.Evas_Object *obj, int channel)
    void emotion_object_spu_channel_set(evas.c_evas.Evas_Object *obj, int channel)
    int emotion_object_spu_channel_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_chapter_count(evas.c_evas.Evas_Object *obj)
    void emotion_object_chapter_set(evas.c_evas.Evas_Object *obj, int chapter)
    int emotion_object_chapter_get(evas.c_evas.Evas_Object *obj)
    char *emotion_object_chapter_name_get(evas.c_evas.Evas_Object *obj, int chapter)
    void emotion_object_play_speed_set(evas.c_evas.Evas_Object *obj, double speed)
    double emotion_object_play_speed_get(evas.c_evas.Evas_Object *obj)

    void emotion_object_eject(evas.c_evas.Evas_Object *obj)

    char *emotion_object_title_get(evas.c_evas.Evas_Object *obj)
    char *emotion_object_progress_info_get(evas.c_evas.Evas_Object *obj)
    double emotion_object_progress_status_get(evas.c_evas.Evas_Object *obj)
    char *emotion_object_ref_file_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_ref_num_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_spu_button_count_get(evas.c_evas.Evas_Object *obj)
    int emotion_object_spu_button_get(evas.c_evas.Evas_Object *obj)
    char *emotion_object_meta_info_get(evas.c_evas.Evas_Object *obj, Emotion_Meta_Info meta)


cdef class Emotion(evas.c_evas.Object):
    cdef object _emotion_callbacks
