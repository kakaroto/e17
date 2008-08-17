#!/usr/bin/python

"""Ecore timer example.

Timers are used to schedule events (function call-backs) within Ecore's
main loop  based on elapsed time.

Provided callbacks should return 'True' in order to keep running after
being evoked and 'False' if it's their last run.

API is simple, either:

   ecore.timer_add(timeout, callback, sequential args..., named args...):
      => returns ecore.Timer object.

   ecore.Timer(timeout, callback, sequential args..., named args...):
      => returns ecore.Timer object.

   obj.interval => property used to get and set timeout interval.
   obj.stop() or obj.delete() => timer will not run anymore.
"""

import ecore

def print_every_500ms(a, b, p, q):
    """Demonstrates callbacks with multiple parameters.

    Note that timer for this callback is created with:

       ecore.timer_add(0.5, print_every_500ms, "arg1", 1234, q=890, p=567)

    the "arg1" and 1234 are sequential arguments will be stored in 'a'
    and 'b' due their order, while 567 and 890 will be correctly
    stored in 'p' and 'q' respectively even if they're given in the
    "wrong" order, because they are named parameters.
    """
    print "every 500ms: a=%r, b=%r, p=%r, q=%r" % (a, b, p, q)
    return True # keep running


counter = 0
def print_every_600ms_twice():
    global counter
    counter += 1
    print "every 600ms, run:", counter
    if counter == 2:
        return False # stop running
    else:
        return True # keep running


def print_once_after_700ms():
    print "once after 700ms"
    # no return means "return None" and None evaluates to "False"


def print_every_1s(a, b, p):
    print "every 1s:", a, b, p
    return True


def quit_after_3s():
    ecore.main_loop_quit()
    return False


counter2 = 0
def print_every_300ms():
    global counter2
    counter2 += 1
    print "every 300ms, run:", counter2
    return True # keep running, stop_print_every_300ms will stop me


def stop_print_every_300ms(my_timer):
    print "stopping 'print_every_300ms' timer"
    my_timer.delete()


if __name__ == "__main__":
    # Use C-like API
    ecore.timer_add(0.5, print_every_500ms, "arg1", 1234, q=890, p=567)
    ecore.timer_add(0.6, print_every_600ms_twice)
    ecore.timer_add(0.7, print_once_after_700ms)

    # Use pythonic API, ecore.Timer() is a class
    ecore.Timer(1.0, print_every_1s, "arg1.1", 1234.1, p=567.1)

    # keep timer object so we can stop it from the other timer
    my_timer = ecore.timer_add(0.3, print_every_300ms)
    ecore.timer_add(1.5, stop_print_every_300ms, my_timer)

    # regular timer using C-like API, will trigger ecore.main_loop_quit()
    ecore.timer_add(3.0, quit_after_3s)

    # without a main loop, timers will not work
    ecore.main_loop_begin()
