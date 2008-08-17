#!/usr/bin/env python

import dbus
import e_dbus
import ecore


def emit_signal(obj):
    obj.emitHelloSignal(dbus_interface="com.example.TestService")
    obj.Exit(dbus_interface="com.example.TestService")

    ecore.idler_add(ecore.main_loop_quit)
    return False


def hello_signal_handler(hello_string):
    print "Received signal (by connecting using remote " \
          "object) and it says: %s" % hello_string


def catchall_hello_signals_handler(hello_string):
    print "Received a hello signal and it says: %s" % hello_string


def catchall_testservice_interface_handler(hello_string, dbus_message):
    print "com.example.TestService interface says %s " \
          "when it sent signal %s" % (hello_string, dbus_message.get_member())


dbus_ml = e_dbus.DBusEcoreMainLoop()
bus = dbus.SessionBus(mainloop=dbus_ml)

obj = bus.get_object("com.example.TestService",
                     "/com/example/TestService/object")
obj.connect_to_signal("HelloSignal",
                      hello_signal_handler,
                      dbus_interface="com.example.TestService",
                      arg0="Hello")

bus.add_signal_receiver(catchall_hello_signals_handler,
                        dbus_interface = "com.example.TestService",
                        signal_name = "HelloSignal")
bus.add_signal_receiver(catchall_testservice_interface_handler,
                        dbus_interface = "com.example.TestService",
                        message_keyword="dbus_message")

ecore.timer_add(2.0, emit_signal, obj)
ecore.main_loop_begin()
