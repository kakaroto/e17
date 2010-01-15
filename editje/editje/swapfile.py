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
            print self.__filepath, self.__swapfile
            copyfile(self.__filepath, self.__swapfile)
            self.__compiled_file = True
            self.__filepath = ""
            self.__opened = True
            return

        if path.exists(self.__swapfile):
            if mode == RESTORE:
                return
            elif mode != REPLACE:
                raise Exception("Cache exists")

        self.__swap_create()
        self.__opened = True

    def __file_check(self):
        if self.__filepath.endswith(".edj"):
            self.__compiled_file = True
        elif self.__filepath.endswith(".edc"):
            self.__compiled_file = False
        else:
            raise Exception("Unknown format")

    def __swap_update(self):
        dir, file = path.split(self.__filepath)
        self.__swapfile = path.join(dir, "." + file + ".editje_swp.edj")

    def __swap_create(self):
        print self.__compiled_file
        if self.__compiled_file:
            copyfile(self.__filepath, self.__swapfile)
        else:
            orig_dir = getcwd()
            dir, file = path.split(self.__filepath)
            chdir(dir)
            system('edje_cc ' + file + ' ' + self.__swapfile);
            chdir(orig_dir)
            if not (path.exists(self.__swapfile) and
                    path.isfile(self.__swapfile)):
                raise Exception("Compiler Error")

    def save(self, filepath=None, mode=None):
        if not self.__opened:
            return

        if filepath:
            if path.exists(self.__swapfile) and mode != REPLACE:
                raise Exception("File exists")
            self.__filepath = filepath
            self.__file_check()
            swap = self.__swapfile
            self.__swap_update()
            move(swap, self.__swapfile)
        elif self.__new:
            raise Exception("Filename not defined")

        if self.__compiled_file:
            copyfile(self.__swapfile, self.__filepath)
        else:
            orig_dir = getcwd()
            temp_dir = mkdtemp(prefix="editje_")
            chdir(temp_dir)
            system('edje_decc ' + self.__swapfile + ' -no-build-sh -current-dir')
            move("./generated_source.edc", self.__filepath)
            dir, file = path.split(self.__filepath)
            for file in listdir(temp_dir):
                move(file, dir)
            chdir(orig_dir)
            rmtree(temp_dir)

    def close(self):
        if not self.__opened:
            return

        remove(self.__swapfile)
        self.__swapfile = None
        self.__new = True
        self.__filepath = ""

    def _filename_get(self):
        return self.__filepath

    def _filename_set(self, filepath):
        if self.__opened:
            return

        self.__filepath = filepath
        if not filepath:
            self.__swapfile = ""
            return

        if not (path.exists(filepath) and path.isfile(filepath)):
            raise Exception("File not found")

        self.__file_check()

        self.__swap_update()

    filename = property(_filename_get, _filename_set)

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
