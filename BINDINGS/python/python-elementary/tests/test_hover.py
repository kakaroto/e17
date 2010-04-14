#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Hover -{{{-
def hover_bt1_clicked(bt, hv):
    hv.show()

def hover_clicked(obj, it):
    win = elementary.Window("hover", elementary.ELM_WIN_BASIC)
    win.title_set("Hover")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    hv = elementary.Hover(win)

    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.callback_clicked_add(hover_bt1_clicked, hv)
    bx.pack_end(bt)
    bt.show()
    hv.parent_set(win)
    hv.target_set(bt)

    bt = elementary.Button(win)
    bt.label_set("Popup")
    hv.content_set("middle", bt)
    bt.show()

    bx = elementary.Box(win)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bx.pack_end(ic)
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Top 1")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 2")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 3")
    bx.pack_end(bt)
    bt.show()

    bx.show()

    hv.content_set("top", bx)

    bt = elementary.Button(win)
    bt.label_set("Bottom")
    hv.content_set("bottom", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Left")
    hv.content_set("left", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Right")
    hv.content_set("right", bt)
    bt.show()

    bg.size_hint_min_set(160, 160)
    bg.size_hint_max_set(640, 640)

    win.resize(320, 320)
    win.show()
# }}}

#----- Hover2 -{{{-
def hover2_clicked(obj, it):
    win = elementary.Window("hover2", elementary.ELM_WIN_BASIC)
    win.title_set("Hover 2")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    hv = elementary.Hover(win)
    hv.style_set("popout")

    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.callback_clicked_add(hover_bt1_clicked, hv)
    bx.pack_end(bt)
    bt.show()
    hv.parent_set(win)
    hv.target_set(bt)

    bt = elementary.Button(win)
    bt.label_set("Popup")
    hv.content_set("middle", bt)
    bt.show()

    bx = elementary.Box(win)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bx.pack_end(ic)
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Top 1")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 2")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 3")
    bx.pack_end(bt)
    bt.show()

    bx.show()
    hv.content_set("top", bx)

    bt = elementary.Button(win)
    bt.label_set("Bot")
    hv.content_set("bottom", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Left")
    hv.content_set("left", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Right")
    hv.content_set("right", bt)
    bt.show()

    bg.size_hint_min_set(160, 160)
    bg.size_hint_max_set(640, 640)
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
    win.callback_destroy_add(destroy)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    box0 = elementary.Box(win)
    box0.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(box0)
    box0.show()

    fr = elementary.Frame(win)
    fr.label_set("Information")
    box0.pack_end(fr)
    fr.show()

    lb = elementary.Label(win)
    lb.label_set("Please select a test from the list below<br>"
                 "by clicking the test button to show the<br>"
                 "test window.")
    fr.content_set(lb)
    lb.show()

    items = [("Hover", hover_clicked),
             ("Hover 2", hover2_clicked)]

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
