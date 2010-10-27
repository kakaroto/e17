# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

import evas.c_evas


cdef extern from *:
    ctypedef char* const_char_ptr "const char *"

cdef extern from "Eina.h":
    ctypedef unsigned char Eina_Bool


cdef extern from "Ecore_File.h":
    ####################################################################
    # Enumerations
    #

    ####################################################################
    # Structures
    #
    ctypedef struct Ecore_File_Download_Job
    
    
    ####################################################################
    # Functions
    #
    int ecore_file_init()
    int ecore_file_shutdown()

    Eina_Bool ecore_file_download(const_char_ptr url, const_char_ptr dst,
                            void (*completion_cb)(void *data,
                                                  const_char_ptr file,
                                                  int status),
                            int (*progress_cb)(void *data,
                                               const_char_ptr file,
                                               long int dltotal,
                                               long int dlnow,
                                               long int ultotal,
                                               long int ulnow),
                            void *data,
                            Ecore_File_Download_Job **job_ret)

    void ecore_file_download_abort(Ecore_File_Download_Job *job)
    void ecore_file_download_abort_all()
    Eina_Bool ecore_file_download_protocol_available(const_char_ptr protocol)



cdef class Download:
    cdef Ecore_File_Download_Job *job
    cdef readonly object completion_cb
    cdef readonly object progress_cb
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec_completion(self, const_char_ptr file, int status)
    cdef object _exec_progress(self, const_char_ptr file,
                               long int dltotal, long int dlnow,
                               long int ultotal, long int ulnow)
