#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Layout -{{{-
def _event(*args, **kargs):
    print((args, kargs))

def layout_clicked(obj):
    win = elementary.Window("layout", elementary.ELM_WIN_BASIC)
    win.title_set("Layout")
    win.elm_event_callback_add(_event)
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(ly)
    ly.show()

    bt = elementary.Button(win)
    bt.text_set("Button 1")
    ly.part_content_set("element1", bt)
    bt.elm_event_callback_add(_event)
    bt.elm_event_callback_del(_event)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 2")
    ly.part_content_set("element2", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 3")
    ly.part_content_set("element3", bt)
    bt.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    layout_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
