#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Window -{{{-
def cb_alpha(bt, win, bg, on):
    win.alpha = on
    if on:
        bg.hide()
    else:
        bg.show()

def cb_rot(bt, win, ck, rot):
    if ck.state:
        win.rotation_with_resize_set(rot)
    else:
        win.rotation = rot

def cb_win_moved(win):
    print(("MOVE - win geom:", win.geometry))

def window_states_clicked(obj):
    win = elementary.Window("window-states", elementary.ELM_WIN_BASIC)
    win.title = "Window States test"
    win.autodel = True
    win.callback_moved_add(cb_win_moved)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(vbox)
    vbox.show()

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    for state in [True, False]:
        bt = elementary.Button(win)
        bt.text = "Alpha " + ("On" if state else "Off")
        bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
        bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
        bt.callback_clicked_add(cb_alpha, win, bg, state)
        hbox.pack_end(bt)
        bt.show()

    sl = elementary.Slider(win)
    sl.text = "Visual test"
    sl.indicator_format = "%3.0f"
    sl.min_max = (50, 150)
    sl.value = 50
    sl.inverted = True
    sl.size_hint_align = (0.5, evas.EVAS_HINT_FILL)
    sl.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(sl)
    sl.show()

    ck = elementary.Check(win)
    ck.text = "Resize on rotate"
    ck.size_hint_align = (0.0, 0.0)
    vbox.pack_end(ck)
    ck.show()

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    for rot in [0, 90, 180, 270]:
        bt = elementary.Button(win)
        bt.text = "Rot " + str(rot)
        bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
        bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
        bt.callback_clicked_add(cb_rot, win, ck, rot)
        hbox.pack_end(bt)
        bt.show()

    win.resize(280, 400)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    window_states_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
