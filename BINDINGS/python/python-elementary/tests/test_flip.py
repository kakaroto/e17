#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Flip -{{{-
def my_flip_1(bt, fl):
    fl.go(elementary.ELM_FLIP_ROTATE_Y_CENTER_AXIS)

def my_flip_2(bt, fl):
    fl.go(elementary.ELM_FLIP_ROTATE_X_CENTER_AXIS)

def my_flip_3(bt, fl):
    fl.go(elementary.ELM_FLIP_ROTATE_XZ_CENTER_AXIS)

def my_flip_4(bt, fl):
    fl.go(elementary.ELM_FLIP_ROTATE_YZ_CENTER_AXIS)

def my_flip_5(bt, fl):
    fl.go(elementary.ELM_FLIP_CUBE_LEFT)

def my_flip_6(bt, fl):
    fl.go(elementary.ELM_FLIP_CUBE_RIGHT)



def flip_clicked(obj, it):
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
    box.pack_end(fl)
    fl.show()


    # front content (image)
    o = elementary.Background(win)
    o.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    o.file_set("images/sky_01.jpg")
    fl.content_front_set(o)
    o.show()

    # back content (layout)
    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fl.content_back_set(ly)
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

    # flip buttons
    box2 = elementary.Box(win)
    box2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    box2.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    box2.horizontal_set(1)
    box2.show()
    box.pack_end(box2)
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("1")
    bt.callback_clicked_add(my_flip_1, fl)
    box2.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("2")
    bt.callback_clicked_add(my_flip_2, fl)
    box2.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("3")
    bt.callback_clicked_add(my_flip_3, fl)
    box2.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("4")
    bt.callback_clicked_add(my_flip_4, fl)
    box2.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("5")
    bt.callback_clicked_add(my_flip_5, fl)
    box2.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.label_set("6")
    bt.callback_clicked_add(my_flip_6, fl)
    box2.pack_end(bt)
    bt.show()

    # window show
    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    flip_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
