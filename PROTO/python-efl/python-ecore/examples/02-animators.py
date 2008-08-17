#!/usr/bin/python

"""Ecore animator example.

Similar to timers, animators are functions that are scheduled to be
called back after some elapsed time, but instead of allocating one
timer per function with variable timeout, animators will be processed
all together using global "frametime" instead.

Provided callbacks should return 'True' in order to keep running after
being evoked and 'False' if it's their last run.

API is simple, either:

   ecore.animator_add(callback, sequential args..., named args...):
      => returns ecore.Animator object.

   ecore.Animator(callback, sequential args..., named args...):
      => returns ecore.Animator object.

   obj.stop() or obj.delete() => animator will not run anymore.
"""

import ecore

def animator1(a, b, p, q):
    """Demonstrates callbacks with multiple parameters.

    Note that animator for this callback is created with:

       ecore.animator_add(animator1, "arg1", 1234, q=890, p=567)

    the "arg1" and 1234 are sequential arguments will be stored in 'a'
    and 'b' due their order, while 567 and 890 will be correctly
    stored in 'p' and 'q' respectively even if they're given in the
    "wrong" order, because they are named parameters.
    """
    print "animator1: a=%r, b=%r, p=%r, q=%r" % (a, b, p, q)
    return True # keep running


def animator_run_once():
    print "animator_run_once"
    return False


def animator_run_once2():
    print "animator_run_once2 (no return value)"
    # no return means "return None" and None evaluates to "False"


def animator2():
    print "animator2"
    return True

counter1 = 0
def animator3(anim_obj):
    global counter1
    counter1 += 1
    print "animator3 will stop animator2 when", counter1, "== 5"
    if counter1 == 5:
        anim_obj.stop()
        return False
    else:
        return True


counter2 = 0
def animator_quit_program_after_10():
    global counter2
    counter2 += 1
    print "animator_quit_program_after_10: counter=", counter2
    if counter2 == 10:
        ecore.main_loop_quit()
        return False
    else:
        return True


if __name__ == "__main__":
    # animators will run in 60 frames per second
    ecore.animator_frametime_set(1.0 / 60.0)

    ecore.animator_add(animator1, "arg1", 1234, q=890, p=567)
    ecore.animator_add(animator_run_once)
    ecore.animator_add(animator_run_once2)
    ecore.animator_add(animator_quit_program_after_10)

    # keep animator object so we can stop it from the other animator
    o = ecore.animator_add(animator2)
    ecore.animator_add(animator3, o)

    # without a main loop, animators will not work!
    ecore.main_loop_begin()
