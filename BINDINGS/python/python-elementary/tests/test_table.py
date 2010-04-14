#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Table -{{{-
def table_clicked(obj, it):
    win = elementary.Window("table", elementary.ELM_WIN_BASIC)
    win.title_set("Table")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    tb = elementary.Table(win)
    win.resize_object_add(tb)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    tb.show()

    bt = elementary.Button(win)
    bt.label_set("Button 1")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 1, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 2, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 4")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 1, 2, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 5")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 2, 1, 1, 3)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 6")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 2, 2, 2)
    bt.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    table_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
