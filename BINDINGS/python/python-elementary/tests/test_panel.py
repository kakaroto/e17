#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Panel -{{{-
def panel_clicked(obj, it):
    win = elementary.Window("panel", elementary.ELM_WIN_BASIC)
    win.title_set("Panel test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    panel = elementary.Panel(win)
    panel.orient = elementary.ELM_PANEL_ORIENT_LEFT
    panel.size_hint_weight_set(0.0, evas.EVAS_HINT_EXPAND);
    panel.size_hint_align_set(0.0, evas.EVAS_HINT_FILL);

    bt = elementary.Button(win)
    bt.label_set("HIDE ME :)")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND);
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL);
    bt.show()

    panel.content_set(bt)

    bx.pack_end(panel)
    panel.show()

    win.resize(300, 300)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    panel_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
