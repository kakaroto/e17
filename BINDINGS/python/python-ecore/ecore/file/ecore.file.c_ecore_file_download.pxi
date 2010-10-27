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

# This file is included verbatim by c_ecore_file.pyx


cdef void _completion_cb(void *data, const_char_ptr file, int status) with gil:
    obj = <Download>data
    try:
        obj._exec_completion(file, status)
    except Exception, e:
        traceback.print_exc()

cdef int _progress_cb(void *data, const_char_ptr file, long int dltotal,
                    long int dlnow, long int ultotal, long int ulnow) with gil:
    obj = <Download>data
    try:
        return obj._exec_progress(file, dltotal, dlnow, ultotal, ulnow)
    except Exception, e:
        traceback.print_exc()


cdef class Download:
    """
    Download the given url to destination.
    You must provide the full url, including 'http://', 'ftp://' or 'file://'.
    If C{dst} already exist it will not be overwritten and the function will fail.
    Ecore must be compiled with CURL to download using http and ftp protocols.
    The C{status} param in the C{completion_cb} will be 0 if the download
    end successfully or 1 in case of failure.
    The C{progress_cb} must return 0 to continue normally or 1 to abort
    the download, note that no completion_cb will be called if you abort in
    this way.
    The constructor will raise a SystemError exception if the download don't
    start, this can happen if you don't have CURL compiled, if the destination
    exists or if the destination path don't exist or isn't writable.

    The callback signatures are:

        C{completion_cb(file, status, *args, **kargs)}
            
        C{progress_cb(file, dltotal, dlnow, uptotal, upnow, *args, **kargs): int}

    You can use the download feature as a class instance or you can use
    the legacy API.

    Instance example:
    
        import ecore.file
        dl = ecore.file.Download("http://your_url",
                                 "/path/to/destination", None, None)
        dl.abort()


    Legacy example:

        import ecore.file
        dl = ecore.file.download("http://your_url",
                                 "/path/to/destination", None, None)
        ecore.file.download_abort(dl)
    
    """
    def __init__(self, url, dst, completion_cb, progress_cb, *args, **kargs):
        """
        Constructor.

        @parm: B{url} The complete url to download
        @parm: B{dst} Where to download the file
        @parm: B{completion_cb} A callback called on download complete
        @parm: B{progress_cb} A callback called during the download operation
        """
        cdef Ecore_File_Download_Job *job

        if completion_cb is not None and not callable(completion_cb):
            raise TypeError("Parameter 'completion_cb' must be callable, or None")

        if progress_cb is not None and not callable(progress_cb):
            raise TypeError("Parameter 'progress_cb' must be callable, or None")

        self.completion_cb = completion_cb
        self.progress_cb = progress_cb
        self.args = args
        self.kargs = kargs

        started = ecore_file_download(url, dst,
                                  _completion_cb, #TODO _completion_cb if completion_cb else NULL
                                  _progress_cb,
                                  <void *>self, &job)
        if not started:
            raise SystemError("could not download '%s' to %s" % (url, dst))
            
        self.job = job
        Py_INCREF(self)

    def __str__(self):
        return "%s(completion_cb=%s, progress_cb=%s args=%s, kargs=%s)" % \
               (self.__class__.__name__, self.completion_cb,
                self.progress_cb, self.args, self.kargs)

    def __repr__(self):
        return ("%s(%#x, completion_cb=%s, progress_cb=%s, args=%s, kargs=%s, "
                "Ecore_File_Download_Job=%#x, refcount=%d)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self.completion_cb, self.progress_cb, self.args, self.kargs,
                <unsigned long>self.job, PY_REFCOUNT(self))

    def __dealloc__(self):
        if self.job != NULL:
            ecore_file_download_abort(self.job)
            self.job = NULL
        self.completion_cb = None
        self.progress_cb = None
        self.args = None
        self.kargs = None

    cdef object _exec_completion(self, const_char_ptr file, int status):
        if self.completion_cb:
            self.completion_cb(file, status, *self.args, **self.kargs)

    cdef object _exec_progress(self, const_char_ptr file, long int dltotal,
                            long int dlnow, long int ultotal, long int ulnow):
        if self.progress_cb:
            return self.progress_cb(file, dltotal, dlnow, ultotal, ulnow,
                                    *self.args, **self.kargs)
        return 0

    def abort(self):
        """Abort the download and free internal resources."""
        if self.job != NULL:
            ecore_file_download_abort(self.job)
            self.job = NULL
            Py_DECREF(self)


def download(url, dst, completion_cb, progress_cb, *args, **kargs):
    """
    L{Download} factory, for C-api compatibility.

    @parm: B{url} The complete url to download
    @parm: B{dst} Where to download the file

    C{completion_cb(file, status, *args, **kargs)}
    C{progress_cb(file, dltotal, dlnow, uptotal, upnow, *args, **kargs): int}

    @rtype: L{Download}
    """
    return Download(url, dst, completion_cb, progress_cb, *args, **kargs)

def download_abort(instance):
    """
    C-api compatibility
    Abort the given download an free internal resources
    """
    instance.abort()

def download_abort_all():
    """
    This will abort all the download currently in progrss, use with caution.
    """
    ecore_file_download_abort_all()

def download_protocol_available(protocol):
    """
    Check if the given download protocol is available, available protocols
    are: "http://", "ftp://" and "file://". Note that ecore can be
    compiled without CURL support and thus http and ftp could not be available

    @parm: B{protocol} The protocol to check, ex: "http://"
    @rtype: bool
    """
    return bool(ecore_file_download_protocol_available(protocol))
