#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Frame -{{{-
def frame_clicked(obj):
    win = elementary.Window("frame", elementary.ELM_WIN_BASIC)
    win.title_set("Frame test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(vbox)
    vbox.show()

    # frame 1
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    
    fr = elementary.Frame(win)
    fr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    fr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fr.text_set("Frame title")
    fr.content_set(ic)
    vbox.pack_end(fr)
    fr.show()

    # frame 2 (collapsable)
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")

    fr = elementary.Frame(win)
    fr.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    fr.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fr.autocollapse_set(True)
    fr.text_set("Frame collapsable (BROKEN)")
    fr.content_set(ic)
    vbox.pack_end(fr)
    fr.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    frame_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
