# Copyright (C) 2009 by ProFUSION embedded systems
#
# This file is part of Python-Ethumb.
#
# Python-Ethumb is free software you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ethumb is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ethumb.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from *:
    ctypedef char* const_char_ptr "const char *"

cdef extern from "Eina.h":
    ctypedef int Eina_Bool
    ctypedef void (*Eina_Free_Cb)(void *data)

cdef extern from "Ethumb.h":
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

cdef extern from "Ethumb_Client.h":
    ctypedef struct Ethumb_Client
    ctypedef struct Ethumb_Exists
    ctypedef void (*Ethumb_Client_Connect_Cb)(void *data, Ethumb_Client *client, Eina_Bool success)
    ctypedef void (*Ethumb_Client_Die_Cb)(void *data, Ethumb_Client *client)
    ctypedef void (*Ethumb_Client_Generate_Cb)(void *data, Ethumb_Client *client, int id, const_char_ptr file, const_char_ptr key, const_char_ptr thumb_path, const_char_ptr thumb_key, Eina_Bool success)
    ctypedef void (*Ethumb_Client_Thumb_Exists_Cb)(void *data, Ethumb_Client *client, Ethumb_Exists *thread, Eina_Bool exists)
    ctypedef void (*Ethumb_Client_Generate_Cancel_Cb)(void *data, Eina_Bool success)

    int             ethumb_client_init()
    int             ethumb_client_shutdown()

    Ethumb_Client   *ethumb_client_connect(Ethumb_Client_Connect_Cb cb, void *data, Eina_Free_Cb free_data)
    void            ethumb_client_disconnect(Ethumb_Client *client)
    void            ethumb_client_on_server_die_callback_set(Ethumb_Client *client, Ethumb_Client_Die_Cb server_die_cb, void *data, Eina_Free_Cb free_data)

    void            ethumb_client_fdo_set(Ethumb_Client *client, int s)

    void            ethumb_client_size_set(Ethumb_Client *client, int tw, int th)
    void            ethumb_client_size_get(Ethumb_Client *client, int *tw, int *th)
    void            ethumb_client_format_set(Ethumb_Client *client, int f)
    int             ethumb_client_format_get(Ethumb_Client *client)
    void            ethumb_client_aspect_set(Ethumb_Client *client, int a)
    int             ethumb_client_aspect_get(Ethumb_Client *client)
    void            ethumb_client_orientation_set(Ethumb_Client *client, Ethumb_Thumb_Orientation o)
    Ethumb_Thumb_Orientation ethumb_client_orientation_get(Ethumb_Client *client)
    void            ethumb_client_crop_align_set(Ethumb_Client *client, float x, float y)
    void            ethumb_client_crop_align_get(Ethumb_Client *client, float *x, float *y)
    void            ethumb_client_quality_set(Ethumb_Client *client, int quality)
    int             ethumb_client_quality_get(Ethumb_Client *client)
    void            ethumb_client_compress_set(Ethumb_Client *client, int compress)
    int             ethumb_client_compress_get(Ethumb_Client *client)
    Eina_Bool       ethumb_client_frame_set(Ethumb_Client *client, const_char_ptr file, const_char_ptr group, const_char_ptr swallow)
    void            ethumb_client_dir_path_set(Ethumb_Client *client, const_char_ptr path)
    const_char_ptr  ethumb_client_dir_path_get(Ethumb_Client *client)
    void            ethumb_client_category_set(Ethumb_Client *client, const_char_ptr category)
    const_char_ptr  ethumb_client_category_get(Ethumb_Client *client)
    void            ethumb_client_video_time_set(Ethumb_Client *client, float time)
    void            ethumb_client_video_start_set(Ethumb_Client *client, float start)
    void            ethumb_client_video_interval_set(Ethumb_Client *client, float interval)
    void            ethumb_client_video_ntimes_set(Ethumb_Client *client, int ntimes)
    void            ethumb_client_video_fps_set(Ethumb_Client *client, int fps)
    void            ethumb_client_document_page_set(Ethumb_Client *client, int page)

    void            ethumb_client_ethumb_setup(Ethumb_Client *client)

    void            ethumb_client_thumb_path_set(Ethumb_Client *client, const_char_ptr path, const_char_ptr key)
    void            ethumb_client_thumb_path_get(Ethumb_Client *client, const_char_ptr *path, const_char_ptr *key)

    Eina_Bool       ethumb_client_file_set(Ethumb_Client *client, const_char_ptr path, const_char_ptr key)
    void            ethumb_client_file_get(Ethumb_Client *client, const_char_ptr *path, const_char_ptr *key)
    void            ethumb_client_file_free(Ethumb_Client *client)

    Ethumb_Exists   *ethumb_client_thumb_exists(Ethumb_Client *client, Ethumb_Client_Thumb_Exists_Cb exists_cb, void *data)
    void            ethumb_client_thumb_exists_cancel(Ethumb_Exists *exists)
    Eina_Bool       ethumb_client_thumb_exists_check(Ethumb_Exists *exists)
    int             ethumb_client_generate(Ethumb_Client *client, Ethumb_Client_Generate_Cb generated_cb, void *data, Eina_Free_Cb free_data)
    void            ethumb_client_generate_cancel(Ethumb_Client *client, int id, Ethumb_Client_Generate_Cancel_Cb cancel_cb, void *data, Eina_Free_Cb free_data)
    void            ethumb_client_generate_cancel_all(Ethumb_Client *client)

    ctypedef void (*Ethumb_Client_Async_Done_Cb)(Ethumb_Client *ethumbd, const_char_ptr thumb_path, const_char_ptr thumb_key, void *data)
    ctypedef void (*Ethumb_Client_Async_Error_Cb)(Ethumb_Client *ethumbd, void *data)

    ctypedef struct Ethumb_Client_Async

    Ethumb_Client_Async *ethumb_client_thumb_async_get(Ethumb_Client *client,
                            Ethumb_Client_Async_Done_Cb done,
                            Ethumb_Client_Async_Error_Cb error,
                            void *data)

    void            ethumb_client_thumb_async_cancel(Ethumb_Client *client, Ethumb_Client_Async *request)

cdef class Client:
    cdef Ethumb_Client *obj
    cdef object _on_connect_callback
    cdef object _on_server_die_callback
