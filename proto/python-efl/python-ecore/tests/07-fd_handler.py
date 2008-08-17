#!/usr/bin/env python

import ecore
import os


def cb_read1(fd_handler, a, b):
    data = os.read(fd_handler.fd, 50)
    print "ready for read1:", fd_handler, ", params: a=", a, ", b=", b
    return True


def cb_read2(fd_handler, a, b):
    print "ready for read2:", fd_handler, ", params: a=", a, ", b=", b
    return True


def cb_read3(fd_handler, a, b):
    print "ready for read3:", fd_handler, ", params: a=", a, ", b=", b
    return False


def timer_write(wfd):
    print "write to fd:", wfd
    os.write(wfd, "[some data]")
    return True

rfd, wfd = os.pipe()
fdh1 = ecore.fd_handler_add(rfd, ecore.ECORE_FD_READ, cb_read1, 123, b="xyz")
fdh2 = ecore.FdHandler(rfd, ecore.ECORE_FD_READ, cb_read2, 456, b="www")
fdh3 = ecore.FdHandler(rfd, ecore.ECORE_FD_READ, cb_read3, 789, b="stop next")

ecore.timer_add(0.2, timer_write, wfd)

print "before: fdh1=", fdh1
print "before: fdh2=", fdh2
print "before: fdh3=", fdh3

ecore.timer_add(1, ecore.main_loop_quit)
ecore.main_loop_begin()
print "main loop stopped"

print "after: fdh1=", fdh1
print "after: fdh2=", fdh2
print "after: fdh3=", fdh3

fdh1.delete()
fdh2.delete()
# fdh3 already deleted since it returned false
del fdh1
del fdh2
del fdh3

ecore.shutdown()
