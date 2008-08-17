#!/usr/bin/env python

import ecore
import ecore.x

# method 1
main_window = ecore.x.Window(w=800, h=600)
main_window.background_color_set(0xffff, 0, 0)
main_window.show()

sub_window = ecore.x.Window(main_window, 10, 10, 780, 580)
sub_window.background_color_set(0, 0, 0xffff)
sub_window.show()

def cb_on_configure(event, main_window, sub_window):
    if event.win == main_window:
        sub_window.resize(event.w - 20, event.h - 20)
    return True
ecore.x.on_window_configure_add(cb_on_configure, main_window, sub_window)


# method 2: inheritance
class Window(ecore.x.Window):
    def __init__(self, w, h):
        ecore.x.Window.__init__(self, w=w, h=h)
        self.background_color_set(0xffff, 0, 0)
        self.sub_window = ecore.x.Window(self, 10, 10, w - 20, h - 20)
        self.sub_window.background_color_set(0, 0, 0xffff)
        self.sub_window.show()
        ecore.x.on_window_configure_add(self._cb_on_configure)

    def _cb_on_configure(self, event):
        if event.win == self:
            self.sub_window.resize(event.w - 20, event.h - 20)
        return True

other_window = Window(400, 300)
other_window.show()

ecore.main_loop_begin()
