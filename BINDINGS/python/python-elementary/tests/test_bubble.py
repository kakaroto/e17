#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Bubble -{{{-
def bubble_clicked(obj, item=None):
    win = elementary.Window("bubble", elementary.ELM_WIN_BASIC)
    win.title_set("Bubble")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    # bb 1
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    lb = elementary.Label(win)
    lb.text_set("Blah, Blah, Blah")

    bb = elementary.Bubble(win)
    bb.text_set("Message 1")
    bb.part_text_set("info", "Corner: top_left")
    bb.content_set(lb)
    bb.part_content_set("icon", ic)
    bb.pos = elementary.ELM_BUBBLE_POS_TOP_LEFT
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(bb)
    bb.show()

    # bb 2
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    lb = elementary.Label(win)
    lb.text_set("Blah, Blah, Blah")

    bb = elementary.Bubble(win)
    bb.text_set("Message 2")
    bb.part_text_set("info", "Corner: top_right")
    bb.content_set(lb)
    bb.part_content_set("icon", ic)
    bb.pos = elementary.ELM_BUBBLE_POS_TOP_RIGHT
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(bb)
    bb.show()

    # bb 3
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    lb = elementary.Label(win)
    lb.text_set("Blah, Blah, Blah")

    bb = elementary.Bubble(win)
    bb.text_set("Message 3")
    bb.part_text_set("info", "Corner: bottom_left")
    bb.content_set(ic)
    bb.pos = elementary.ELM_BUBBLE_POS_BOTTOM_LEFT
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(bb)
    bb.show()

    # bb 4
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    lb = elementary.Label(win)
    lb.text_set("Blah, Blah, Blah")

    bb = elementary.Bubble(win)
    bb.text_set("Message 4")
    bb.part_text_set("info", "Corner: bottom_right")
    bb.content_set(lb)
    bb.part_content_set("icon", ic)
    bb.pos = elementary.ELM_BUBBLE_POS_BOTTOM_RIGHT
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(bb)
    bb.show()

    win.resize(320, 320)
    win.show()
# }}}


#----- Main -{{{-
if __name__ == "__main__":
    def destroy(obj):
        elementary.exit()

    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.callback_delete_request_add(destroy)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    box0 = elementary.Box(win)
    box0.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(box0)
    box0.show()

    fr = elementary.Frame(win)
    fr.text_set("Information")
    box0.pack_end(fr)
    fr.show()

    lb = elementary.Label(win)
    lb.text_set("Please select a test from the list below<br>"
                 "by clicking the test button to show the<br>"
                 "test window.")
    fr.content_set(lb)
    lb.show()

    items = [("Bubble", bubble_clicked),
            ]

    li = elementary.List(win)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    box0.pack_end(li)
    li.show()

    for item in items:
        li.item_append(item[0], callback=item[1])

    li.go()

    win.resize(320,520)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
