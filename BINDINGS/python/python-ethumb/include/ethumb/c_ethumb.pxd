# Copyright (C) 2009 by ProFUSION embedded systems
#
# This file is part of Python-Ethumb.
#
# Python-Ethumb is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ethumb is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ethumb.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from *:
    ctypedef char* const_char_ptr "const_char_ptr "

cdef extern from "Eina.h":
    ctypedef int Eina_Bool
    ctypedef void (*Eina_Free_Cb)(void *data)

cdef extern from "Ethumb.h":
    cdef struct Ethumb:
        int tw
        int th
        int format
        int aspect

    ctypedef void (*Ethumb_Generate_Cb)(void *data, Ethumb *e, Eina_Bool success)

    # Ethumb
    int                         ethumb_init()
    void                        ethumb_shutdown()
    Ethumb                      *ethumb_new()
    void                        ethumb_free(Ethumb *e)

    # Setup
    Eina_Bool                   ethumb_frame_set(Ethumb *e, const_char_ptr theme_file, const_char_ptr group, const_char_ptr swallow)
    void                        ethumb_frame_get(Ethumb *e, const_char_ptr *theme_file, const_char_ptr *group, const_char_ptr *swallow)

    void                        ethumb_thumb_dir_path_set(Ethumb *e, const_char_ptrpath)
    const_char_ptr              ethumb_thumb_dir_path_get(Ethumb *e)

    void                        ethumb_thumb_category_set(Ethumb *e, const_char_ptrcategory)
    const_char_ptr              ethumb_thumb_category_get(Ethumb *e)

    void                        ethumb_thumb_path_set(Ethumb *e, const_char_ptrpath, const_char_ptrkey)
    void                        ethumb_thumb_path_get(Ethumb *e, const_char_ptr*path, const_char_ptr*key)
    void                        ethumb_thumb_hash(Ethumb *e)
    void                        ethumb_thumb_hash_copy(Ethumb *dst, Ethumb *src)

    ctypedef enum Ethumb_Thumb_FDO_Size:
        ETHUMB_THUMB_NORMAL
        ETHUMB_THUMB_LARGE

    ctypedef enum Ethumb_Thumb_Format:
        ETHUMB_THUMB_FDO
        ETHUMB_THUMB_JPEG
        ETHUMB_THUMB_EET

    ctypedef enum Ethumb_Thumb_Aspect:
        ETHUMB_THUMB_KEEP_ASPECT
        ETHUMB_THUMB_IGNORE_ASPECT
        ETHUMB_THUMB_CROP

    ctypedef enum Ethumb_Thumb_Orientation:
        ETHUMB_THUMB_ORIENT_NONE
        ETHUMB_THUMB_ROTATE_90_CW
        ETHUMB_THUMB_ROTATE_180
        ETHUMB_THUMB_ROTATE_90_CCW
        ETHUMB_THUMB_FLIP_HORIZONTAL
        ETHUMB_THUMB_FLIP_VERTICAL
        ETHUMB_THUMB_FLIP_TRANSPOSE
        ETHUMB_THUMB_FLIP_TRANSVERSE
        ETHUMB_THUMB_ORIENT_ORIGINAL

    void                        ethumb_thumb_fdo_set(Ethumb *e, Ethumb_Thumb_FDO_Size s)

    void                        ethumb_thumb_size_set(Ethumb *e, int tw, int th)
    void                        ethumb_thumb_size_get(Ethumb *e, int *tw, int *th)

    void                        ethumb_thumb_format_set(Ethumb *e, Ethumb_Thumb_Format f)
    Ethumb_Thumb_Format         ethumb_thumb_format_get(Ethumb *e)

    void                        ethumb_thumb_aspect_set(Ethumb *e, Ethumb_Thumb_Aspect a)
    Ethumb_Thumb_Aspect         ethumb_thumb_aspect_get(Ethumb *e)

    void                        ethumb_thumb_orientation_set(Ethumb *e, Ethumb_Thumb_Orientation o)
    Ethumb_Thumb_Orientation    ethumb_thumb_orientation_get(Ethumb *e)

    void                        ethumb_thumb_crop_align_set(Ethumb *e, float x, float y)
    void                        ethumb_thumb_crop_align_get(Ethumb *e, float *x, float *y)

    void                        ethumb_thumb_quality_set(Ethumb *e, int quality)
    int                         ethumb_thumb_quality_get(Ethumb *e)

    void                        ethumb_thumb_compress_set(Ethumb *e, int compress)
    int                         ethumb_thumb_compress_get(Ethumb *e)

    void                        ethumb_video_start_set(Ethumb *e, float start)
    float                       ethumb_video_start_get(Ethumb *e)
    void                        ethumb_video_time_set(Ethumb *e, float time)
    float                       ethumb_video_time_get(Ethumb *e)
    void                        ethumb_video_interval_set(Ethumb *e, float interval)
    float                       ethumb_video_interval_get(Ethumb *e)
    void                        ethumb_video_ntimes_set(Ethumb *e, unsigned int ntimes)
    unsigned int                ethumb_video_ntimes_get(Ethumb *e)
    void                        ethumb_video_fps_set(Ethumb *e, unsigned int fps)
    unsigned int                ethumb_video_fps_get(Ethumb *e)

    # Basics
    void                        ethumb_document_page_set(Ethumb *e, unsigned int page)
    unsigned int                ethumb_document_page_get(Ethumb *e)

    Eina_Bool                   ethumb_file_set(Ethumb *e, const_char_ptr path, const_char_ptr key)
    void                        ethumb_file_get(Ethumb *e, const_char_ptr *path, const_char_ptr *key)
    void                        ethumb_file_free(Ethumb *e)

    Eina_Bool                   ethumb_generate(Ethumb *e, Ethumb_Generate_Cb finished_cb, void *data, Eina_Free_Cb free_data)
    Eina_Bool                   ethumb_exists(Ethumb *e)

    Ethumb                      *ethumb_dup(Ethumb *e)
    Eina_Bool                   ethumb_cmp(Ethumb *e1, Ethumb *e2)
    int                         ethumb_hash(void *key, int key_length)
    int                         ethumb_key_cmp(void *key1, int key1_length, void *key2, int key2_length)
    unsigned int                ethumb_length(void *key)


cdef class PyEthumb:
    cdef Ethumb *obj
