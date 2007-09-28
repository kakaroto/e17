#!/usr/bin/env python

import dbus
import e_dbus
import dbus.service
import ecore


class TestObject(dbus.service.Object):
    def __init__(self, conn, object_path="/com/example/TestService/object"):
        dbus.service.Object.__init__(self, conn, object_path)

    @dbus.service.signal("com.example.TestService")
    def HelloSignal(self, message):
        # The signal is emitted when this method exits
        # You can have code here if you wish
        pass

    @dbus.service.method("com.example.TestService")
    def emitHelloSignal(self):
        #you emit signals by calling the signal"s skeleton method
        self.HelloSignal("Hello")
        return "Signal emitted"

    @dbus.service.method("com.example.TestService",
                         in_signature="", out_signature="")
    def Exit(self):
        ecore.idler_add(ecore.main_loop_quit)


dbus_ml = e_dbus.DBusEcoreMainLoop()
session_bus = dbus.SessionBus(mainloop=dbus_ml)

name = dbus.service.BusName("com.example.TestService", session_bus)
obj = TestObject(session_bus)

ecore.main_loop_begin()
