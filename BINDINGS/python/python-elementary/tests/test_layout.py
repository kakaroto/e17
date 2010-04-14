#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Layout -{{{-
def layout_clicked(obj, it):
    win = elementary.Window("layout", elementary.ELM_WIN_BASIC)
    win.title_set("Layout")
    win.autodel_set(True)

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
    bt.label_set("Button 1")
    ly.content_set("element1", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    ly.content_set("element2", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    ly.content_set("element3", bt)
    bt.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    layout_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
