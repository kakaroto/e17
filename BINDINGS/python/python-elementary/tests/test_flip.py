#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Flip -{{{-
def my_flip_go(bt, fl, mode):
    fl.go(mode)

def my_flip_animate_begin(fl):
    print("Animation Begin")

def my_flip_animate_done(fl):
    print("Animation Done")

def flip_clicked(obj, item=None):
    # window
    win = elementary.Window("flip", elementary.ELM_WIN_BASIC)
    win.autodel_set(True)
    win.title_set("Flip")

    # background
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    # main vertical box
    box = elementary.Box(win)
    box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(box)
    box.show()

    # flip object
    fl = elementary.Flip(win)
    fl.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fl.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    fl.callback_animate_begin_add(my_flip_animate_begin)
    fl.callback_animate_done_add(my_flip_animate_done)
    box.pack_end(fl)
    fl.show()


    # front content (image)
    o = elementary.Background(win)
    o.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    o.file_set("images/sky_01.jpg")
    fl.part_content_set("front", o)
    o.show()

    # back content (layout)
    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fl.part_content_set("back", ly)
    ly.show()

    bt = elementary.Button(win)
    bt.text_set("Button 1")
    ly.part_content_set("element1", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 2")
    ly.part_content_set("element2", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 3")
    ly.part_content_set("element3", bt)
    bt.show()

    # flip buttons (first row)
    hbox = elementary.Box(win)
    hbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    hbox.horizontal_set(True)
    hbox.show()
    box.pack_end(hbox)
    count = 1

    for mode in [elementary.ELM_FLIP_ROTATE_X_CENTER_AXIS,
                 elementary.ELM_FLIP_ROTATE_Y_CENTER_AXIS,
                 elementary.ELM_FLIP_ROTATE_XZ_CENTER_AXIS,
                 elementary.ELM_FLIP_ROTATE_YZ_CENTER_AXIS]:
        bt = elementary.Button(win)
        bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        bt.text_set(str(count))
        bt.callback_clicked_add(my_flip_go, fl, mode)
        hbox.pack_end(bt)
        bt.show()
        count += 1

    # flip buttons (second row)
    hbox = elementary.Box(win)
    hbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    hbox.horizontal_set(True)
    hbox.show()
    box.pack_end(hbox)

    for mode in [elementary.ELM_FLIP_CUBE_LEFT,
                 elementary.ELM_FLIP_CUBE_RIGHT,
                 elementary.ELM_FLIP_CUBE_UP,
                 elementary.ELM_FLIP_CUBE_DOWN]:
        bt = elementary.Button(win)
        bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        bt.text_set(str(count))
        bt.callback_clicked_add(my_flip_go, fl, mode)
        hbox.pack_end(bt)
        bt.show()
        count += 1

    # flip buttons (third row)
    hbox = elementary.Box(win)
    hbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    hbox.horizontal_set(True)
    hbox.show()
    box.pack_end(hbox)

    for mode in [elementary.ELM_FLIP_PAGE_LEFT,
                 elementary.ELM_FLIP_PAGE_RIGHT,
                 elementary.ELM_FLIP_PAGE_UP,
                 elementary.ELM_FLIP_PAGE_DOWN]:
        bt = elementary.Button(win)
        bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        bt.text_set(str(count))
        bt.callback_clicked_add(my_flip_go, fl, mode)
        hbox.pack_end(bt)
        bt.show()
        count += 1

    # window show
    win.resize(320, 320)
    win.show()
# }}}


#----- Flip Interactive -{{{-
def my_cb_radios (rd, fl):
    print((rd.value_get()))
    fl.interaction_set(rd.value_get())

def flip_interactive_clicked(obj, item=None):
    # window
    win = elementary.Window("flip", elementary.ELM_WIN_BASIC)
    win.autodel_set(True)
    win.title_set("Flip")

    # background
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    # main vertical box
    box = elementary.Box(win)
    box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(box)
    box.show()

    # flip object
    fl = elementary.Flip(win)
    fl.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fl.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    fl.interaction_set(elementary.ELM_FLIP_INTERACTION_NONE);
    fl.interaction_direction_enabled_set(elementary.ELM_FLIP_DIRECTION_UP, True);
    fl.interaction_direction_enabled_set(elementary.ELM_FLIP_DIRECTION_DOWN, True);
    fl.interaction_direction_enabled_set(elementary.ELM_FLIP_DIRECTION_LEFT, True);
    fl.interaction_direction_enabled_set(elementary.ELM_FLIP_DIRECTION_RIGHT, True);
    fl.interaction_direction_hitsize_set(elementary.ELM_FLIP_DIRECTION_UP, 0.25);
    fl.interaction_direction_hitsize_set(elementary.ELM_FLIP_DIRECTION_DOWN, 0.25);
    fl.interaction_direction_hitsize_set(elementary.ELM_FLIP_DIRECTION_LEFT, 0.25);
    fl.interaction_direction_hitsize_set(elementary.ELM_FLIP_DIRECTION_RIGHT, 0.25);
    fl.callback_animate_begin_add(my_flip_animate_begin)
    fl.callback_animate_done_add(my_flip_animate_done)
    box.pack_end(fl)
    fl.show()

    # front content (image)
    o = elementary.Background(win)
    o.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    o.file_set("images/sky_01.jpg")
    fl.part_content_set("front", o)
    o.show()

    # back content (layout)
    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fl.part_content_set("back", ly)
    ly.show()

    bt = elementary.Button(win)
    bt.text_set("Button 1")
    ly.part_content_set("element1", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 2")
    ly.part_content_set("element2", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text_set("Button 3")
    ly.part_content_set("element3", bt)
    bt.show()


    # radio buttons
    rd = elementary.Radio(win)
    rd.state_value_set(elementary.ELM_FLIP_INTERACTION_NONE)
    rd.text_set("None")
    rd.callback_changed_add(my_cb_radios, fl)
    box.pack_end(rd)
    rd.show()
    rdg = rd

    rd = elementary.Radio(win)
    rd.state_value_set(elementary.ELM_FLIP_INTERACTION_ROTATE)
    rd.text_set("Rotate")
    rd.callback_changed_add(my_cb_radios, fl)
    rd.group_add(rdg)
    box.pack_end(rd)
    rd.show()

    rd = elementary.Radio(win)
    rd.state_value_set(elementary.ELM_FLIP_INTERACTION_CUBE)
    rd.text_set("Cube")
    rd.callback_changed_add(my_cb_radios, fl)
    rd.group_add(rdg)
    box.pack_end(rd)
    rd.show()

    rd = elementary.Radio(win)
    rd.state_value_set(elementary.ELM_FLIP_INTERACTION_PAGE)
    rd.text_set("Page")
    rd.callback_changed_add(my_cb_radios, fl)
    rd.group_add(rdg)
    box.pack_end(rd)
    rd.show()

    # window show
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

    items = [("Flip", flip_clicked),
             ("Flip Interactive", flip_interactive_clicked),
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
