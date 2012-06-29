#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Grid -{{{-
def cb_change(bt, grid):
    (x, y, w, h) = grid.pack_get(bt)
    grid.pack_set(bt, x - 2, y - 2, w + 4, h + 4)
    
def grid_clicked(obj):
    win = elementary.Window("grid", elementary.ELM_WIN_BASIC)
    win.title = "Grid test"
    win.autodel = True
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    gd = elementary.Grid(win)
    gd.size = (100, 100)
    win.resize_object_add(gd)
    gd.show()

    en = elementary.Entry(win)
    en.scrollable = True
    en.text = "Entry text 2"
    en.single_line = True
    gd.pack(en, 60, 20, 30, 10)
    en.show()

    bt = elementary.Button(win)
    bt.text = "Next API function"
    gd.pack(bt, 30, 0, 40, 10)
    bt.disabled = True
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Button"
    gd.pack(bt,  0,  0, 20, 20)
    bt.show()

    bt = elementary.Button(win);
    bt.text = "Button"
    gd.pack(bt, 10, 10, 40, 20)
    bt.show()

    bt = elementary.Button(win);
    bt.text = "Button"
    gd.pack(bt, 10, 30, 20, 50)
    bt.show()

    bt = elementary.Button(win);
    bt.text = "Button"
    gd.pack(bt, 80, 80, 20, 20)
    bt.show()

    bt = elementary.Button(win);
    bt.text = "Change"
    bt.callback_clicked_add(cb_change, gd)
    gd.pack(bt, 40, 40, 20, 20)
    bt.show()

    re = evas.Rectangle(win.evas)
    re.color = (128, 0, 0, 128)
    gd.pack(re, 40, 70, 20, 10)
    re.show()

    re = evas.Rectangle(win.evas)
    re.color = (0, 128, 0, 128)
    gd.pack(re, 60, 70, 10, 10)
    re.show()

    re = evas.Rectangle(win.evas)
    re.color = (0, 0, 128, 128)
    gd.pack(re, 40, 80, 10, 10)
    re.show()

    re = evas.Rectangle(win.evas)
    re.color = (128, 0, 128, 128)
    gd.pack(re, 50, 80, 10, 10)
    re.show()

    re = evas.Rectangle(win.evas)
    re.color = (128, 64, 0, 128)
    gd.pack(re, 60, 80, 10, 10)
    re.show()

    win.resize(480, 480)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    grid_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
