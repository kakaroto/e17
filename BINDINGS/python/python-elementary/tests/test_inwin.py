#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Inner Window -{{{-
def inner_window_clicked(obj, it):
    win = elementary.Window("inner-window", elementary.ELM_WIN_BASIC)
    win.title_set("InnerWindow test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    iw = elementary.InnerWindow(win)
    iw.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    inner_window_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
