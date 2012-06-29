#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Check -{{{-
def ck_1(obj):
    print(("test check 1 state:", obj.state))

def ck_2(obj):
    print(("test check 2 state:", obj.state))

def ck_never(obj):
    print("disabled check changed (should never happen unless you enable or set it)")

def ck_3(obj):
    print(("test check 3 state:", obj.state))

def ck_4(obj):
    print(("test check 4 state:", obj.state))

def check_clicked(obj):
    win = elementary.Window("check", elementary.ELM_WIN_BASIC)
    win.title_set("Check test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ck.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    ck.text_set("Icon sized to check")
    ck.content_set(ic)
    ck.state_set(True)
    ck.callback_changed_add(ck_1)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.resizable_set(0, 0)
    ck = elementary.Check(win)
    ck.text_set("Icon no scale")
    ck.content_set(ic)
    ck.callback_changed_add(ck_2)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ck = elementary.Check(win)
    ck.text_set("Label Only")
    ck.callback_changed_add(ck_3)
    bx.pack_end(ck)
    ck.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ck.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    ck.text_set("Disabled check")
    ck.content_set(ic)
    ck.state_set(True)
    ck.callback_changed_add(ck_never)
    bx.pack_end(ck)
    ck.disabled_set(True)
    ck.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ic.resizable_set(0, 0)
    ck = elementary.Check(win)
    ck.content_set(ic)
    ck.callback_changed_add(ck_4)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    check_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
