 #!/usr/bin/env python
# encoding: utf-8
import os
from elementary import *
import edje
import ecore
import evas


def sel_cb(fp, *args):
    if args:
        print("label of selected item is: %s\n" % args[0].text)

def unsel_cb(bt, fp):
    fp.selected_item.selected = False

def last_cb(bt, fp):
    fp.last_item.selected = True

def third_from_end_cb(bt, fp):
    fp.last_item.prev.prev.selected = True

def underflow_cb(fp):
    print("underflow!\n")

def overflow_cb(fp):
    print("overflow!\n")

def slider_change_cb(sl, fp):
    fp.first_interval = sl.value

def flip_next_cb(bt, fp):
    fp.next()

def flip_prev_cb(bt, fp):
    fp.prev()

def flipselector_clicked(obj):
    lbl = (
        "Elementary",
        "Evas",
        "Eina",
        "Edje",
        "Eet",
        "Ecore",
        "Efreet",
        "Edbus"
    )

    win = StandardWindow("flipselector", "Flip Selector")
    win.autodel = True

    bx = Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    fp = FlipSelector(win)
    fp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fp.callback_selected_add(sel_cb)
    fp.callback_underflowed_add(overflow_cb)
    fp.callback_overflowed_add(underflow_cb)
    for i in range(0, len(lbl)):
        it = fp.item_append(lbl[i])
        if (i == 3):
            edje_it = it
    edje_it.selected = True
    bx.pack_end(fp)
    fp.show()

    bx2 = Box(win)
    bx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx2.horizontal = True
    bx.pack_end(bx2)
    bx2.show()

    fp = FlipSelector(win)
    fp.callback_underflowed_add(overflow_cb)
    fp.callback_overflowed_add(underflow_cb)
    fp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    for i in range(1990, 2100):
        fp.item_append(str(i), sel_cb)

    bt = Button(win)
    bt.text = "Flip Prev"
    bt.callback_clicked_add(flip_prev_cb, fp)

    bx2.pack_end(bt)
    bt.show()

    bx2.pack_end(fp)
    fp.show()

    bt = Button(win)
    bt.text = "Flip Next"
    bt.callback_clicked_add(flip_next_cb, fp)
    bx2.pack_end(bt)
    bt.show()

    sl = Slider(win)
    sl.text = "Flip Iterval:"
    sl.unit_format = "%1.2f"
    sl.min_max = (0.0, 3.0)
    sl.value = (0.85)
    sl.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sl.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(sl)
    sl.show()
    sl.callback_changed_add(slider_change_cb, fp)

    bt = Button(win)
    bt.text = "Select Last"
    bt.callback_clicked_add(last_cb, fp)
    bx.pack_end(bt)
    bt.show()

    bt = Button(win)
    bt.text = "Select 2097"
    bt.callback_clicked_add(third_from_end_cb, fp)
    bx.pack_end(bt)
    bt.show()

    bt = Button(win)
    bt.text = "Unselect year"
    bt.callback_clicked_add(unsel_cb, fp)
    bx.pack_end(bt)
    bt.show()

    win.show()

if __name__ == "__main__":
    elementary.init()

    flipselector_clicked(None)

    elementary.run()
    elementary.shutdown()
