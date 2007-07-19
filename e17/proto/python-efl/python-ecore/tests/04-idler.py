#!/usr/bin/env python

import ecore

def cb_true(n, t, a):
    print "cb_true:", n, t, a
    return True

def cb_false(n, t, a):
    print "cb_false:", n, t, a
    return False

i0 = ecore.idler_add(cb_true, 123, "teste", a=456)
i1 = ecore.Idler(cb_false, 789, "bla", a="something in a")
print "before: i0=", i0
print "before: i1=", i1

ecore.timer_add(1, ecore.main_loop_quit)

ecore.main_loop_begin()
print "main loop stopped"

print "after:  i0=", i0
print "after:  i1=", i1

i0.delete()
del i0
del i1 # already deleted since returned false

ecore.shutdown()
