#!/usr/bin/env python

import ecore

# basic read flags with line support
flags = ecore.ECORE_EXE_PIPE_READ | ecore.ECORE_EXE_PIPE_READ_LINE_BUFFERED

# simple ls -la output, monitor for add, del and data (stdout)
exe = ecore.Exe("ls -la", flags)
def on_add(x, event, a, b, c):
    print "ecore.Exe added:"
    print "   exe..:", x
    print "   event:", event
    print "   extra: arguments: a=%s, b=%s, c=%s" % (a, b, c)
    print

    assert x == exe
    assert isinstance(event, ecore.EventExeAdd)
    assert a == 1
    assert b == 2
    assert c == 3

def on_del(x, event):
    print "ecore.Exe deleted, exit!", x
    ecore.timer_add(1.0, ecore.main_loop_quit)

def on_data(x, event):
    assert x == exe
    assert isinstance(event, ecore.EventExeData)
    print "ecore.Exe data on stdout:"
    print "   Exe.....:", repr(exe)
    print "   Event...:", repr(event)
    for l in event.lines:
        print "         ", repr(l)
    print

exe.on_add_event_add(on_add, 1, c=3, b=2)
exe.on_del_event_add(on_del)
exe.on_data_event_add(on_data)

# use C-api like event handler, will catch all 3 Exe() instances
def catch_all_exe_add(event):
    print
    print ">>> EXE ADDED:", event
    print
    return 1

ecore.on_exe_add_event_add(catch_all_exe_add)
ecore.Exe("ls -l /", flags)


# start cat, send data to it, then read, then send again, then kill it
cat_pipe = ecore.Exe("cat", flags | ecore.ECORE_EXE_PIPE_WRITE)
def on_cat_pipe_add(x, event):
    x.send("123\nabc\nxyz\n")

def on_cat_pipe_data(x, event):
    assert event.lines == ["123", "abc", "xyz"]
    x.on_data_event_del(on_cat_pipe_data)

def on_cat_pipe_data2(x, event):
    print "cat pipe output:"
    print "\n".join(event.lines)
    print
    x.send("some\nmore\nlines!\n")
    if event.lines and event.lines[-1] == "lines!":
        x.on_data_event_del(on_cat_pipe_data2)
        x.kill() # otherwise it will stay there forever (will be detached)

cat_pipe.on_add_event_add(on_cat_pipe_add)
cat_pipe.on_data_event_add(on_cat_pipe_data)
cat_pipe.on_data_event_add(on_cat_pipe_data2)

ecore.main_loop_begin()
