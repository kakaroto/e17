#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Notepad -{{{-
def notepad_clicked(obj, it):
    win = elementary.Window("notepad", elementary.ELM_WIN_BASIC)
    win.title_set("Notepad")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    l = elementary.Label(win)
    l.label_set("TODO")
    bx.pack_end(l)
    l.show()

    win.resize(320, 300)
    win.show()
# }}}
# vim:foldmethod=marker
