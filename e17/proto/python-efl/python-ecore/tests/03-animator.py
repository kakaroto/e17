#!/usr/bin/env python

import ecore

def cb_true(n, t, a):
    print "cb_true:", n, t, a
    return True

def cb_false(n, t, a):
    print "cb_false:", n, t, a
    return False

ecore.animator_frametime_set(1.0/24.0)
assert ecore.animator_frametime_get() == 1.0/24.0

a0 = ecore.animator_add(cb_true, 123, "teste", a=456)
a1 = ecore.Animator(cb_false, 789, "bla", a="something in a")
a2 = ecore.animator_add(ecore.main_loop_quit)
print "before: a0=", a0
print "before: a1=", a1
print "before: a2=", a2

ecore.main_loop_begin()
print "main loop stopped"

print "after:  a0=", a0
print "after:  a1=", a1
print "after:  a2=", a2

a0.delete()
del a0
del a1 # already deleted since returned false
del a2 # already deleted since returned false

ecore.shutdown()
