#!/usr/bin/env python

import ecore
import os


def cb_read(fd_handler, a, b):
    data = os.read(fd_handler.fd, 50)
    print "ready for read:", fd_handler, ", params: a=", a, ", b=", b
    return True

def timer_write(wfd):
    print "write to fd:", wfd
    os.write(wfd, "[some data]")
    return True

rfd, wfd = os.pipe()
fdh = ecore.fd_handler_add(rfd, ecore.ECORE_FD_READ, cb_read, 123, b="xyz")

ecore.timer_add(0.2, timer_write, wfd)

print "before: fdh=", fdh

ecore.timer_add(1, ecore.main_loop_quit)
ecore.main_loop_begin()
print "main loop stopped"

print "after: fdh=", fdh

fdh.delete()
del fdh

ecore.shutdown()
