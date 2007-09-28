#!/usr/bin/env python

import dbus
import e_dbus
import ecore


def print_name_owner_changed(obj, old, new):
    print "Obj = '%s', Old = '%s', New = '%s'" % (obj, old, new)


dbus_ml = e_dbus.DBusEcoreMainLoop()
bus = dbus.SystemBus(mainloop=dbus_ml)

bus.add_signal_receiver(print_name_owner_changed, "NameOwnerChanged")
ecore.main_loop_begin()
