#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Panes -{{{-
def cb_panes(panes, event):
    print(("Event: %s" % (event)))

def panes_clicked(obj):
    win = elementary.Window("panes", elementary.ELM_WIN_BASIC)
    win.title_set("Panes test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    panes = elementary.Panes(win)
    win.resize_object_add(panes)
    panes.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    panes.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    panes.callback_clicked_add(cb_panes, "clicked")
    panes.callback_clicked_double_add(cb_panes, "clicked,double")
    panes.callback_press_add(cb_panes, "press")
    panes.callback_unpress_add(cb_panes, "unpress")
    panes.show()

    bt = elementary.Button(win)
    bt.text = "Left"
    panes.part_content_set("left", bt)
    bt.show()

    panes_h = elementary.Panes(win)
    panes_h.horizontal = True
    panes_h.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    panes_h.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    panes.part_content_set("right", panes_h)
    panes_h.show()

    bt = elementary.Button(win)
    bt.text = "Up"
    panes_h.part_content_set("left", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Down"
    panes_h.part_content_set("right", bt)
    bt.show()


    win.resize(320, 480)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    panes_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
