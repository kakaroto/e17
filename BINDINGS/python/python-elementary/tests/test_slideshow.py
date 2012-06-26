#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

img1 = "images/logo.png"
img2 = "images/plant_01.jpg"
img3 = "images/rock_01.jpg"
img4 = "images/rock_02.jpg"
img5 = "images/sky_01.jpg"
img6 = "images/sky_04.jpg"
img7 = "images/wood_01.jpg"
img8 = "images/mystrale.jpg"
img9 = "images/mystrale_2.jpg"

def notify_show(no, event, *args, **kwargs):
    no = args[0]
    no.show()

def next(bt, ss):
    ss.next()

def previous(bt, ss):
    ss.previous()

def mouse_in(bx, event, *args, **kwargs):
    no = args[0]
    no.timeout = 0.0
    no.show()

def mouse_out(bx, event, *args, **kwargs):
    no = args[0]
    no.timeout = 3.0

def hv_select(hv, hvit, ss, transition):
    ss.transition = transition
    hv.text = transition

def layout_select(hv, hvit, ss, layout):
    ss.layout = layout
    hv.text = layout

def start(bt, ss, sp, bt_start, bt_stop):
    ss.timeout = sp.value
    bt_start.disabled = True
    bt_stop.disabled = False

def stop(bt, ss, sp, bt_start, bt_stop):
    ss.timeout = 0.0
    bt_start.disabled = False
    bt_stop.disabled = True

def spin(sp, ss):
    if (ss.timeout > 0):
        ss.timeout = sp.value

def slide_transition(ss, slide_it, slide_last_it):
    if (slide_last_it == slide_it):
        print("Reaches to End of slides\n")

class ssClass(elementary.SlideshowItemClass):
    def get(self, obj, *args, **kwargs):
        photo = elementary.Photo(obj)
        photo.file = args[0]
        photo.fill_inside = True
        photo.style = "shadow"

        return photo

def slideshow_clicked(obj):
    win = elementary.StandardWindow("slideshow", "Slideshow")
    win.autodel = True

    ss = elementary.Slideshow(win)
    ss.loop = True
    win.resize_object_add(ss)
    ss.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ss.show()

    ssc = ssClass()
    ss.item_add(ssc, img1)
    ss.item_add(ssc, img2)
    ss.item_add(ssc, img3)
    ss.item_add(ssc, img4)
    ss.item_add(ssc, img9)
    ss.item_add(ssc, img5)
    ss.item_add(ssc, img6)
    ss.item_add(ssc, img7)
    slide_last_it = ss.item_add(ssc, img8)
    ss.callback_transition_end_add(slide_transition, slide_last_it)

    no = elementary.Notify(win)
    no.orient = elementary.ELM_NOTIFY_ORIENT_BOTTOM
    no.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(no)
    no.timeout = 3.0

    bx = elementary.Box(win)
    bx.horizontal = True
    no.content = bx
    bx.show()

    bx.event_callback_add(evas.EVAS_CALLBACK_MOUSE_IN, mouse_in, no)
    bx.event_callback_add(evas.EVAS_CALLBACK_MOUSE_OUT, mouse_out, no)

    bt = elementary.Button(win)
    bt.text = "Previous"
    bt.callback_clicked_add(previous, ss)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Next"
    bt.callback_clicked_add(next, ss)
    bx.pack_end(bt)
    bt.show()

    hv = elementary.Hoversel(win)
    bx.pack_end(hv)
    hv.hover_parent = win
    for transition in ss.transitions:
        hv.item_add(transition, None, 0, hv_select, ss, transition)
    hv.item_add("None", None, 0, hv_select, ss, None)
    hv.text_set(ss.transitions[0])
    hv.show()

    hv = elementary.Hoversel(win)
    bx.pack_end(hv)
    hv.hover_parent = win
    for layout in ss.layouts:
         hv.item_add(layout, None, 0, layout_select, ss, layout)
    hv.text = ss.layout
    hv.show()

    sp = elementary.Spinner(win)
    sp.label_format = "%2.0f secs."
    sp.callback_changed_add(spin, ss)
    sp.step = 1
    sp.min_max = (1, 30)
    sp.value = 3
    bx.pack_end(sp)
    sp.show()

    bt_start = elementary.Button(win)
    bt_stop = elementary.Button(win)

    bt = bt_start
    bt.text = "Start"
    bt.callback_clicked_add(start, ss, sp, bt_start, bt_stop)
    bx.pack_end(bt)
    bt.show()

    bt = bt_stop
    bt.text = "Stop"
    bt.callback_clicked_add(stop, ss, sp, bt_start, bt_stop)
    bx.pack_end(bt)
    bt.disabled = True
    bt.show()

    ss.event_callback_add(evas.EVAS_CALLBACK_MOUSE_UP, notify_show, no)
    ss.event_callback_add(evas.EVAS_CALLBACK_MOUSE_MOVE, notify_show, no)

    win.resize(500, 400)
    win.show()

if __name__ == "__main__":
    elementary.init()

    slideshow_clicked(None)

    elementary.run()
    elementary.shutdown()
