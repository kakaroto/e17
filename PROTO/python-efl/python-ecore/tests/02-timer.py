#!/usr/bin/env python

import ecore

def cb(n, t, a):
    print "cb:", n, t, a
    return True

t0 = ecore.timer_add(0.2, cb, 123, "teste", a=456)
t1 = ecore.timer_add(1, ecore.main_loop_quit)
t2 = ecore.Timer(0.5, cb, 789, "bla", a="something in a")
print "before: t0=", t0
print "before: t1=", t1
print "before: t2=", t2

ecore.main_loop_begin()
print "main loop stopped"

print "after:  t0=", t0
print "after:  t1=", t1
print "after:  t2=", t2

t0.delete()
del t0
del t1 # already deleted since returned false
t2.delete()
del t2

ecore.shutdown()
