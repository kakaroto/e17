#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Inner Window -{{{-
def inner_window_clicked(obj):
    win = elementary.Window("inner-window", elementary.ELM_WIN_BASIC)
    win.title_set("InnerWindow test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    lb = elementary.Label(win)
    lb.text_set("This is an \"inwin\" - a window in a<br/>"
                "window. This is handy for quick popups<br/>"
                "you want centered, taking over the window<br/>"
                "until dismissed somehow. Unlike hovers they<br/>"
                "don't hover over their target.")

    iw = elementary.InnerWindow(win)
    iw.content_set(lb)
    iw.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    inner_window_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
