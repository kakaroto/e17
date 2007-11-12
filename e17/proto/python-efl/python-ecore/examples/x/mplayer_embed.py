#!/usr/bin/env python

import sys
import os
import ecore
import ecore.x
from subprocess import Popen, PIPE

try:
    filename = sys.argv[1]
except IndexError, e:
    raise SystemExit("Usage: %s <filename>" % sys.argv[0])

main_window = ecore.x.Window(w=800, h=600)
main_window.background_color_set(0xffff, 0, 0)
main_window.show()

sub_window = ecore.x.Window(main_window, 10, 10, 780, 580)
sub_window.background_color_set(0, 0, 0xffff)
sub_window.show()

cmd = ["/usr/bin/mplayer", "-slave", "-nomouseinput", "-quiet",
       "-wid", str(sub_window.xid), filename]
p = Popen(cmd, stdin=PIPE, stdout=PIPE, close_fds=True)

def handle_read(fd_handler, file):
    line = file.read(1)
    r = bool(line and not fd_handler.has_error())
    if not r:
        ecore.main_loop_quit()
    return r

def on_configure(event, main_window, sub_window):
    if event.win == main_window:
        sub_window.resize(event.w - 20, event.h - 20)
    return True
ecore.x.on_window_configure_add(on_configure, main_window, sub_window)

ecore.fd_handler_add(p.stdout, ecore.ECORE_FD_ALL, handle_read, p.stdout)
ecore.main_loop_begin()
