#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Box Vert -{{{-
def box_vert_clicked(obj, item=None):
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
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.0, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
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

def box_vert2_clicked(obj, item=None):
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
    bt.text_set("Button 1")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 2")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 3")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 4")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 5")
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.callback_clicked_add(boxvert2_del_cb, bx)
    bx.pack_end(bt)
    bt.show()

    win.show()
# }}}

#----- Box Horiz -{{{-
def box_horiz_clicked(obj, item=None):
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
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()

    win.show()
# }}}

#----- Box Layout -{{{-
layout_list = ["horizontal","vertical","homogeneous_vertical",
    "homogeneous_horizontal", "homogeneous_max_size_horizontal",
    "homogeneous_max_size_vertical", "flow_horizontal", "flow_vertical", "stack"]
current_layout = elementary.ELM_BOX_LAYOUT_HORIZONTAL

def box_layout_button_cb(obj, box):
    global current_layout

    current_layout += 1
    if current_layout >= len(layout_list):
        current_layout = 0
    obj.text_set("layout: %s" % layout_list[current_layout])
    box.layout_set(current_layout)

def box_layout_clicked(obj, item=None):
    win = elementary.Window("box-layout", elementary.ELM_WIN_BASIC)
    win.title_set("Box Layout")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    win.resize_object_add(vbox)
    vbox.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.show()

    bx = elementary.Box(win)
    bx.layout_set(elementary.ELM_BOX_LAYOUT_HORIZONTAL)
    bx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(bx)
    bx.show()

    sep = elementary.Separator(win)
    sep.horizontal_set(True)
    vbox.pack_end(sep)
    sep.show()

    bt = elementary.Button(win)
    bt.text_set("layout: %s" % layout_list[current_layout])
    bt.callback_clicked_add(box_layout_button_cb, bx)
    vbox.pack_end(bt)
    bt.show()

    for i in range(5):
        ic = elementary.Icon(win)
        ic.file_set("images/logo_small.png")
        ic.resizable_set(0, 0)
        ic.size_hint_align_set(0.5, 0.5)
        bx.pack_end(ic)
        ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(1.0, 1.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.resizable_set(0, 0)
    ic.size_hint_align_set(0.0, 0.0)
    bx.pack_end(ic)
    ic.show()

    win.show()
# }}}

#----- Box Layout Transition-{{{-
def box_transition_button_cb(obj, box):
    global current_layout

    from_ly = current_layout
    current_layout += 1
    if current_layout >= len(layout_list):
        current_layout = 0

    obj.text_set("layout: %s" % layout_list[current_layout])
    box.layout_transition(0.4, from_ly, current_layout)

def box_transition_clicked(obj, item=None):
    win = elementary.Window("box-layout-transition", elementary.ELM_WIN_BASIC)
    win.title_set("Box Layout Transition")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    win.resize_object_add(vbox)
    vbox.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.show()

    bx = elementary.Box(win)
    bx.layout_set(elementary.ELM_BOX_LAYOUT_HORIZONTAL)
    bx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(bx)
    bx.show()

    sep = elementary.Separator(win)
    sep.horizontal_set(True)
    vbox.pack_end(sep)
    sep.show()

    bt = elementary.Button(win)
    bt.text_set("layout: %s" % layout_list[current_layout])
    bt.callback_clicked_add(box_transition_button_cb, bx)
    vbox.pack_end(bt)
    bt.show()

    for i in range(4):
        ic = elementary.Icon(win)
        ic.file_set("images/logo_small.png")
        ic.resizable_set(0, 0)
        ic.size_hint_align_set(0.5, 0.5)
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

    items = [("Box Vert", box_vert_clicked),
             ("Box Vert 2", box_vert2_clicked),
             ("Box Horiz", box_horiz_clicked),
             ("Box Layout", box_layout_clicked),
             ("Box Layout Transition", box_transition_clicked)]

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
