#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Buttons -{{{-
def buttons_clicked(obj, it):
    win = elementary.Window("buttons", elementary.ELM_WIN_BASIC)
    win.title_set("Buttons")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    bt = elementary.Button(win)
    bt.label_set("Icon sized to button")
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bt = elementary.Button(win)
    bt.label_set("Icon no scale")
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Button only")
    bx.pack_end(bt)
    bt.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bt = elementary.Button(win)
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    buttons_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
