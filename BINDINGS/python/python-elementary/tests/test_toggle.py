#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Toggles -{{{-
def toggles_clicked(obj, it):
    win = elementary.Window("toggles", elementary.ELM_WIN_BASIC)
    win.title_set("Toggles")
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
    tg = elementary.Toggle(win)
    tg.label_set("Icon sized to toggle")
    tg.icon_set(ic)
    tg.state_set(True)
    tg.states_labels_set("Yes", "No")
    bx.pack_end(tg)
    tg.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    tg = elementary.Toggle(win)
    tg.label_set("Icon no scale")
    tg.icon_set(ic)
    bx.pack_end(tg)
    tg.show()
    ic.show()

    tg = elementary.Toggle(win)
    tg.label_set("Label Only")
    tg.states_labels_set("Big", "Small")
    bx.pack_end(tg)
    tg.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    tg = elementary.Toggle(win)
    tg.icon_set(ic)
    bx.pack_end(tg)
    tg.show()
    ic.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    toggles_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
