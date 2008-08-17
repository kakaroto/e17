#!/usr/bin/env python

import dbus
import e_dbus
import ecore


def disc_started_signal():
        print "Signal: DiscoveryStarted()"


def rem_dev_found_signal(address, cls, rssi):
        print "Signal: RemoteDeviceFound(%s, %s, %s)" % (address, cls, rssi)


def rem_dev_name_signal(address, name):
        print "Signal: RemoteNameUpdated(%s, %s)" % (address, name)


def disc_completed_signal():
        print "Signal: DiscoveryCompleted()"
        ecore.idler_add(ecore.main_loop_quit)


dbus_ml = e_dbus.DBusEcoreMainLoop()
bus = dbus.SystemBus(mainloop=dbus_ml)

bus.add_signal_receiver(disc_started_signal,
                        "DiscoveryStarted",
                        "org.bluez.Adapter",
                        "org.bluez",
                        "/org/bluez/hci0")
bus.add_signal_receiver(rem_dev_found_signal,
                        "RemoteDeviceFound",
                        "org.bluez.Adapter",
                        "org.bluez",
                        "/org/bluez/hci0")
bus.add_signal_receiver(rem_dev_name_signal,
                        "RemoteNameUpdated",
                        "org.bluez.Adapter",
                        "org.bluez",
                        "/org/bluez/hci0")
bus.add_signal_receiver(disc_completed_signal,
                        "DiscoveryCompleted",
                        "org.bluez.Adapter",
                        "org.bluez",
                        "/org/bluez/hci0")

obj = bus.get_object("org.bluez", "/org/bluez/hci0")
adapter = dbus.Interface(obj, "org.bluez.Adapter")

adapter.DiscoverDevices()
ecore.main_loop_begin()
