#
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with Editje.  If not, see
# <http://www.gnu.org/licenses/>.
from os import system, remove, path, getcwd, chdir, listdir
from shutil import copyfile, move, rmtree
from tempfile import mkstemp, mkdtemp
from subprocess import Popen, PIPE
import re

import sysconfig

RESTORE = 1
REPLACE = 2

class SwapFile(object):

    def __init__(self):
        self.__new = False
        self.__filepath = ""
        self.__swapfile = ""
        self.__compiled_file = False
        self.__opened = False

    def open(self, mode=None):
        if self.__new:
            self.__swapfile = mkstemp(".swp", "editje_")[1]
            if not self.__filepath:
                self.__filepath = sysconfig.template_file_get("default")
            copyfile(self.__filepath, self.__swapfile)
            self.__compiled_file = True
            self.__filepath = ""
            self.__opened = True
            return

        if not self.__filepath:
            raise FileNotSet(self)

        if path.exists(self.__swapfile):
            if mode == RESTORE: # or filecmp.cmp(self.__filepath, self.__swapfile):
                self.__opened = True
                return
            elif mode != REPLACE:
                raise CacheAlreadyExists(self)

        self.__swap_create()
        self.__opened = True

    def __file_check(self):
        if self.__filepath.endswith(".edj"):
            self.__compiled_file = True
        elif self.__filepath.endswith(".edc"):
            self.__compiled_file = False
        else:
            raise UnknownFileType(self)

    def __swap_update(self):
        dir, file = path.split(self.__filepath)
        self.__swapfile = path.join(dir, "." + file + ".editje_swp.edj")

    def __swap_create(self):
        if self.__compiled_file:
            copyfile(self.__filepath, self.__swapfile)
        else:
            dir, file = path.split(self.__filepath)
            compiler = Popen(('edje_cc', file, self.__swapfile), cwd=dir, bufsize=-1, stderr=PIPE);
            err = compiler.communicate()[1]
            if not (path.exists(self.__swapfile) and
                    path.isfile(self.__swapfile)):
                raise CompileError(self, re.sub('\x1b.*?m', '', err))

    def save(self, filepath=None, mode=None):
        if not self.__opened:
            return

        if filepath:
            if path.exists(self.__swapfile) and mode != REPLACE:
                raise FileAlreadyExists(self)
            self.__filepath = filepath
            self.__file_check()
            swap = self.__swapfile
            self.__swap_update()
            move(swap, self.__swapfile)
        elif self.__new:
            raise FileNotSet(self)

        if self.__compiled_file:
            copyfile(self.__swapfile, self.__filepath)
        else:
            orig_dir = getcwd()
            temp_dir = mkdtemp(prefix="editje_")
            chdir(temp_dir)
            system('edje_decc ' + self.__swapfile + ' -no-build-sh -current-dir')
            copyfile("./generated_source.edc", self.__filepath)
            remove("./generated_source.edc")
            dir, file = path.split(self.__filepath)
            for file in listdir(temp_dir):
                copyfile(file, path.join(dir, file))
            chdir(orig_dir)
            rmtree(temp_dir)

    def close(self):
        if not self.__opened:
            return

        remove(self.__swapfile)
        self.__swapfile = None
        self.__new = True
        self.__filepath = ""

    def _file_get(self):
        return self.__filepath

    def _file_set(self, filepath):
        if self.__opened:
            return

        if not filepath:
            self.__filepath = ""
            self.__swapfile = ""
            return

        if not (path.exists(filepath) and path.isfile(filepath)):
            raise FileNotFound(self)

        self.__filepath = filepath

        self.__file_check()

        self.__swap_update()

    file = property(_file_get, _file_set)

    def _workfile_get(self):
        return self.__swapfile

    workfile = property(_workfile_get)

    def _new_set(self, value):
        if self.__opened:
            return

        self.__new = value

    def _new_get(self):
        return self.__new

    new = property(_new_get, _new_set)


class SwapFileError(Exception):
    def __init__(self, swapfile):
        self.swapfile = swapfile

class FileNotSet(SwapFileError):
    def __str__(self):
        return "File not set."

class UnknownFileType(SwapFileError):
    def __str__(self):
        return "Unknown File type."

class CacheAlreadyExists(SwapFileError):
    def __str__(self):
        return self.swapfile.file + " : Swap file (" + self.swapfile.workfile + ") exists."

class CompileError(SwapFileError):
    def __init__(self, swapfile, message):
        SwapFileError.__init__(self, swapfile)
        self.message = message
    def __str__(self):
        return self.swapfile.file + " : Compile Error\n" + self.message

class FileAlreadyExists(SwapFileError):
    def __str__(self):
        return self.swapfile.file + " : File exists."

class FileNotFound(SwapFileError):
    def __str__(self):
        return self.swapfile.file + " : File not found."
