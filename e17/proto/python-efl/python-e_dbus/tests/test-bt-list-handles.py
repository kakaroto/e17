#!/usr/bin/env python

import sys
import dbus
import e_dbus
import ecore


def print_remote_handles(adapter, bdaddr):
    handles = [hex(int(h)) \
               for h in list(adapter.GetRemoteServiceHandles(bdaddr, ""))]
    print "Handles:", handles
    ecore.idler_add(ecore.main_loop_quit)
    return False


if len(sys.argv) != 2:
    print "USAGE: %s <bdaddr>" % sys.argv[0]
    sys.exit(1)

dbus_ml = e_dbus.DBusEcoreMainLoop()
bus = dbus.SystemBus(mainloop=dbus_ml);

obj = bus.get_object("org.bluez", "/org/bluez/hci0")
adapter = dbus.Interface(obj, "org.bluez.Adapter")

ecore.idler_add(print_remote_handles, adapter, sys.argv[1])
ecore.main_loop_begin()
