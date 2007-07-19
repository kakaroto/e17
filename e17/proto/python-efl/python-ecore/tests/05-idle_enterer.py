#!/usr/bin/env python

import ecore

def cb_true(n, t, a):
    print "cb_true:", n, t, a
    return True

def cb_false(n, t, a):
    print "cb_false:", n, t, a
    return False

def cb_idle():
    print "idle..."
    return True

def sleeper():
    import time
    print "sleep 0.1s"
    time.sleep(0.1)
    return True

i0 = ecore.idle_enterer_add(cb_true, 123, "teste", a=456)
i1 = ecore.IdleEnterer(cb_false, 789, "bla", a="something in a")
print "before: i0=", i0
print "before: i1=", i1

ecore.timer_add(1, ecore.main_loop_quit)
ecore.timer_add(0.1, sleeper)
ecore.idler_add(cb_idle)

ecore.main_loop_begin()
print "main loop stopped"

print "after:  i0=", i0
print "after:  i1=", i1

i0.delete()
del i0
del i1 # already deleted since returned false

ecore.shutdown()
