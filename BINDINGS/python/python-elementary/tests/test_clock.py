#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Clock -{{{-
def clock_clicked(obj):
    win = elementary.Window("clock", elementary.ELM_WIN_BASIC)
    win.title_set("Clock")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ck = elementary.Clock(win)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.show_am_pm_set(True)
    bx.pack_end(ck)
    ck.show()

    print((ck.time_get()))

    ck = elementary.Clock(win)
    ck.show_seconds_set(True)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.show_seconds_set(True)
    ck.show_am_pm_set(True)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.edit_set(True)
    ck.show_seconds_set(True)
    ck.show_am_pm_set(True)
    ck.time_set(10, 11, 12)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.edit_set(True)
    ck.show_seconds_set(True)
    ck.edit_mode = elementary.ELM_CLOCK_EDIT_HOUR_DECIMAL | \
                   elementary.ELM_CLOCK_EDIT_MIN_DECIMAL | \
                   elementary.ELM_CLOCK_EDIT_SEC_DECIMAL
    bx.pack_end(ck)
    ck.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    clock_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
