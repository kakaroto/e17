# Copyright (C) 2009 Samsung Electronics.
#
# This file is part of Editje.
#
# Editje is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Editje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje. If not, see <http://www.gnu.org/licenses/>.

from os import remove, path, strerror
from shutil import copyfile
from tempfile import mkstemp, mkdtemp
import re
import errno

import sysconfig
from edjecc import EdjeCC, EdjeDeCC

_opened_files = dict()
_opened_swapfiles = dict()

RESTORE = 1
REPLACE = 2

EDITJE_SWP_SUFIX = ".editje_swp.edj"


def _swapfile_path_get(filepath):
    dir, file = path.split(filepath)
    return path.join(dir, "." + file + EDITJE_SWP_SUFIX)


def open_new(success_cb, fail_cb, origin=None, **kargs):
    #TODO: include checks
    swapfile = mkstemp(".swp", "editje_")[1]
    if not origin:
        origin = sysconfig.template_file_get("default")
    copyfile(origin, swapfile)
    success_cb(SwapFile("", swapfile, True), **kargs)


def open(filepath, success_cb, fail_cb, mode=None, **kargs):
    kargs["filepath"] = filepath

    if not path.exists(filepath):
        fail_cb(IOError(errno.ENOENT, strerror(errno.ENOENT), filepath),
                **kargs)
        return

    sf = _opened_files.get(filepath)
    if sf:
        success_cb(sf, **kargs)
        return

    sf = _opened_swapfiles.get(filepath)
    if sf or filepath.endswith(EDITJE_SWP_SUFIX):
        fail_cb(FileSwapTypeError(filepath), **kargs)
        return

    swapfile = _swapfile_path_get(filepath)
    if path.exists(swapfile):
        if mode == RESTORE:
            sf = SwapFile(filepath, swapfile)
            success_cb(sf, **kargs)
            return
        elif mode != REPLACE:
            fail_cb(IOError(errno.EEXIST, strerror(errno.EEXIST), swapfile),
                    **kargs)
            return

    if filepath.endswith(".edj"):
        _open_copy(filepath, swapfile, success_cb, fail_cb, kargs)
    elif filepath.endswith(".edc"):
        _open_compile(filepath, swapfile, success_cb, fail_cb, kargs)
    else:
        fail_cb(UnknownFileTypeError(filepath, swapfile), **kargs)


def _open_compile(filepath, swapfile, success_cb, fail_cb, kargs):
    def error_cb(edjecc, **kargs):
        msg = ""
        for l in edjecc.stderr:
            msg += l + "\n"
        fail_cb(CompileError(edjecc.edc, edjecc.edj, msg), **kargs)

    def ok_cb(edjecc, **kargs):
        sf = SwapFile(edjecc.edc, edjecc.edj, False)
        success_cb(sf, **kargs)

    EdjeCC(filepath, swapfile, ok_cb, error_cb, **kargs)


def _open_copy(filepath, swapfile, success_cb, fail_cb, kargs):
    #TODO: include checks
    copyfile(filepath, swapfile)
    sf = SwapFile(filepath, swapfile, True)
    success_cb(sf, **kargs)


class SwapFile(object):

    def __init__(self, filepath, swapfile, compiled=None):
        self.__filepath = filepath
        self.__swapfile = swapfile

        if compiled is None:
            self.__file_check()
        else:
            self.__compiled_file = compiled

        _opened_files[filepath] = self
        _opened_swapfiles[swapfile] = self

    def is_new(self):
        return not self.__filepath

    def __file_check(self):
        if self.__filepath.endswith(".edj"):
            self.__compiled_file = True
        elif self.__filepath.endswith(".edc"):
            self.__compiled_file = False
        else:
            raise UnknownFileTypeError(self.__filepath)

    def save(self, success_cb, fail_cb, filepath=None, mode=None):
        if filepath:
            opened = _opened_files.get(filepath)
            if opened and opened != self:
                fail_cb(FileOpenedError(self))
                return
            if self.__filepath in _opened_files:
                del _opened_files[self.__filepath]
            if path.exists(filepath) and mode != REPLACE:
                fail_cb(IOError(errno.EEXIST, strerror(errno.EEXIST),
                        filepath))
                return
            if not path.exists(path.dirname(filepath)):
                fail_cb(IOError(errno.ENOENT, strerror(errno.ENOENT),
                        filepath))
                return
            self.__filepath = filepath
            _opened_files[self.__filepath] = self
            try:
                self.__file_check()
            except Exception, e:
                fail_cb(e)
                return
        elif self.__filepath:
            filepath = self.__filepath
        else:
            fail_cb(self, FileNotSetError())
            return

        if self.__compiled_file:
            self.__save_copy(filepath, success_cb, fail_cb)
        else:
            self.__save_decc(filepath, success_cb, fail_cb)

    def __save_copy(self, filepath, success_cb, fail_cb):
        copyfile(self.__swapfile, self.__filepath)
        self.__filepath = filepath
        _opened_files[self.__filepath] = self
        success_cb(self)

    def __save_decc(self, filepath, success_cb, fail_cb):
        def decc_ok(decc):
            success_cb(self)

        def decc_error(decc):
            fail_cb(self)

        EdjeDeCC(self.__swapfile, filepath, decc_ok, decc_error)

    def close(self):
        if self.__filepath:
            del _opened_files[self.__filepath]
        del _opened_swapfiles[self.__swapfile]
        remove(self.__swapfile)
        self.__swapfile = None
        self.__filepath = ""

    def _file_get(self):
        return self.__filepath

    file = property(_file_get)

    def _workfile_get(self):
        return self.__swapfile

    workfile = property(_workfile_get)


class FileError(Exception):
    def __init__(self, file):
        self.file = file


class FileNotSetError(Exception):
    def __str__(self):
        return "File not set."


class UnknownFileTypeError(FileError):
    def __str__(self):
        return "Unknown File type."


class FileSwapTypeError(FileError):
    def __str__(self):
        return "Swap File type."


class CompileError(FileError):
    def __init__(self, file, message):
        FileError.__init__(self, file)
        self.message = message

    def __str__(self):
        return self.file + " : Compile Error\n" + self.message


class FileOpenedError(FileError):
    def __str__(self):
        return self.file + " : File opened."
