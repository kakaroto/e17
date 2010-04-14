#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Separator -{{{-
def separator_clicked(obj, it):
    win = elementary.Window("separators", elementary.ELM_WIN_BASIC)
    win.title_set("Separators")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx0 = elementary.Box(win)
    bx0.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.horizontal_set(True)
    win.resize_object_add(bx0)
    bx0.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.pack_end(bx)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("Left upper corner")
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    sp.horizontal_set(True)
    bx.pack_end(sp)
    sp.show()

    bt = elementary.Button(win)
    bt.label_set("Left lower corner")
    bt.disabled_set(True)
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    bx0.pack_end(sp)
    sp.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.pack_end(bx)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("Right upper corner")
    bt.disabled_set(True)
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    sp.horizontal_set(True)
    bx.pack_end(sp)
    sp.show()

    bt = elementary.Button(win)
    bt.label_set("Right lower corner")
    bx.pack_end(bt)
    bt.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    separator_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
