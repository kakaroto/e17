# Copyright (C) 2010 Samsung Electronics.
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

import ecore

from os import remove, path, getcwd, chdir, listdir
from shutil import copyfile, rmtree
from tempfile import mkdtemp


class EdjeCC(object):
    def __init__(self, edc, edj, success_cb, fail_cb,
                 images="images", fonts="fonts", *args, **kargs):
        exe_flags = ecore.ECORE_EXE_PIPE_ERROR | \
                    ecore.ECORE_EXE_PIPE_ERROR_LINE_BUFFERED
        self.stdout = []
        self.stderr = []

        self.edc = edc
        self.edj = edj
        self._success = success_cb
        self._fail = fail_cb
        self._args = args
        self._kargs = kargs

        orig_dir = getcwd()
        chdir(path.split(edc)[0])
        cmd = "edje_cc -id %s -fd %s %s %s" % (images, fonts, edc, edj)
        self._exe = ecore.Exe(cmd, exe_flags)
        chdir(orig_dir)

        self._exe.on_add_event_add(self._on_add)
        self._exe.on_del_event_add(self._on_del)
        self._exe.on_data_event_add(self._on_data, self.stdout)
        self._exe.on_error_event_add(self._on_data, self.stderr)

    def _on_add(self, exe, event):
        return

    def _on_data(self, exe, event, buffer):
        for l in event.lines:
            buffer.append(l)

    def _on_del(self, exe, event):
        if event.exit_code == 0:
            self._success(self, *self._args, **self._kargs)
        else:
            self._fail(self, *self._args, **self._kargs)


class EdjeDeCC(object):
    def __init__(self, edj, edc, success_cb, fail_cb, *args, **kargs):
        exe_flags = ecore.ECORE_EXE_PIPE_ERROR | \
                    ecore.ECORE_EXE_PIPE_ERROR_LINE_BUFFERED
        self.stdout = []
        self.stderr = []

        self.edc = edc
        self.edj = edj
        self._success = success_cb
        self._fail = fail_cb
        self._args = args
        self._kargs = kargs

        orig_dir = getcwd()
        self._temp_dir = mkdtemp(prefix="editje_")
        chdir(self._temp_dir)
        cmd = 'edje_decc ' + edj + ' -no-build-sh -current-dir'
        self._exe = ecore.Exe(cmd, exe_flags)
        chdir(orig_dir)

        self._exe.on_add_event_add(self._on_add)
        self._exe.on_del_event_add(self._on_del)
        self._exe.on_data_event_add(self._on_data, self.stdout)
        self._exe.on_error_event_add(self._on_data, self.stderr)

    def _on_add(self, exe, event):
        return

    def _on_data(self, exe, event, buffer):
        for l in event.lines:
            buffer.append(l)

    def _on_del(self, exe, event):
        if event.exit_code == 0:
            gen_src = path.join(self._temp_dir, "generated_source.edc")
            copyfile(gen_src, self.edc)
            remove(gen_src)
            work_dir, edc_file = path.split(self.edc)
            for sfile in listdir(self._temp_dir):
                nfile = path.join(work_dir, sfile)
                sfile = path.join(self._temp_dir, sfile)
                copyfile(sfile, nfile)
            rmtree(self._temp_dir)
            self._success(self, *self._args, **self._kargs)
        else:
            rmtree(self._temp_dir)
            self._fail(self, *self._args, **self._kargs)
