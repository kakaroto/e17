#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Box Vert -{{{-
def box_vert_clicked(obj, it):
    win = elementary.Window("box-vert", elementary.ELM_WIN_BASIC)
    win.title_set("Box Vert")
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
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(evas.EVAS_HINT_EXPAND, 0.5)
    bx.pack_end(ic)
    ic.show()

    win.show()
# }}}

#----- Box Vert 2 -{{{-
def boxvert2_del_cb(bt, bx):
    bx.unpack(bt)
    bt.move(0, 0)
    bt.color_set(128, 64, 0, 128)

def box_vert2_clicked(obj, it):
    win = elementary.Window("box-vert2", elementary.ELM_WIN_BASIC)
    win.title_set("Box Vert 2")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(0.0, 0.0)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("Button 1")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 4")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 5")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    win.show()
# }}}

#----- Box Horiz -{{{-
def box_horiz_clicked(obj, it):
    win = elementary.Window("box-horiz", elementary.ELM_WIN_BASIC)
    win.title_set("Box Horiz")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.horizontal_set(True)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()

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

    items = [("Box Vert", box_vert_clicked),
             ("Box Vert 2", box_vert2_clicked),
             ("Box Horiz", box_horiz_clicked)]

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
