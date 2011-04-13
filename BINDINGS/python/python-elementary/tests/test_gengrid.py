#!/usr/bin/env python
import os
import random

import elementary
import edje
import ecore
import evas

images = ["panel_01.jpg", "plant_01.jpg", "rock_01.jpg", "rock_02.jpg",
        "sky_01.jpg", "sky_02.jpg", "sky_03.jpg", "sky_04.jpg", "wood_01.jpg"]

#----- common -{{{-
def gg_label_get(obj, part, item_data):
    return "Item # %s" % (item_data)

def gg_icon_get(obj, part, data):
    if part == "elm.swallow.icon":
        im = elementary.Image(obj)
        im.file_set("images/" + random.choice(images))
        im.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_BOTH, 1, 1)
        return im
    return None

def gg_state_get(obj, part, item_data):
    return False

def gg_del(obj, item_data):
    # commented out because this make clear() slow with many items
    # print "[item del] # %d - %s" % (item_data, obj)
    pass

def gg_sel(gg, ggi, *args, **kwargs):
    (x, y) = ggi.pos_get()
    print "[item selected] # %d  at pos %d %d" % (ggi.data, x, y)

def gg_unsel(gg, ggi, *args, **kwargs):
    print "[item unselected] # %d" % (ggi.data)

def gg_clicked_double(gg, ggi, *args, **kwargs):
    print "[item double clicked] # %d" % (ggi.data)
# -}}}-

#----- Gengrid -{{{-
def gengrid_clicked(obj, it):

    global item_count
    item_count = 25

    # window
    win = elementary.Window("gengrid", elementary.ELM_WIN_BASIC)
    win.title_set("Gengrid")
    win.autodel_set(True)

    # background
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    # main table
    tb = elementary.Table(win)
    win.resize_object_add(tb)
    tb.homogenous_set(0)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    tb.show()

    # gengrid
    itc = elementary.GengridItemClass(item_style="default",
                                       label_get_func=gg_label_get,
                                       icon_get_func=gg_icon_get,
                                       state_get_func=gg_state_get,
                                       del_func=gg_del)
    gg = elementary.Gengrid(win)
    gg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    gg.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    gg.horizontal_set(False)
    gg.bounce_set(False, True)
    gg.item_size_set(80, 80)
    gg.align_set(0.5, 0.0)
    tb.pack(gg, 0, 0, 6, 1)
    gg.callback_selected_add(gg_sel)
    gg.callback_unselected_add(gg_unsel)
    gg.callback_clicked_double_add(gg_clicked_double)
    gg.show()

    # add the first items
    for i in range(item_count):
        gg.item_append(itc, i, None)


    # multi select
    def multi_select_changed(bt, gg):
        gg.multi_select_set(bt.state)
        print gg.multi_select_get()

    bt = elementary.Check(win)
    bt.label = "MultiSelect"
    bt.state = gg.multi_select_get()
    bt.callback_changed_add(multi_select_changed, gg)
    tb.pack(bt, 0, 1, 1, 1)
    bt.show()

    # horizontal
    def horizontal_changed(bt, gg):
        gg.horizontal_set(bt.state)

    bt = elementary.Check(win)
    bt.label = "Horizontal"
    bt.callback_changed_add(horizontal_changed, gg)
    tb.pack(bt, 1, 1, 1, 1)
    bt.show()

    # always select mode
    def always_select_mode_changed(bt, gg):
        gg.always_select_mode_set(bt.state)
        print gg.always_select_mode_get()

    bt = elementary.Check(win)
    bt.label = "AlwaysSelectMode"
    bt.state = gg.always_select_mode_get()
    bt.callback_changed_add(always_select_mode_changed, gg)
    tb.pack(bt, 2, 1, 1, 1)
    bt.show()

    # no select mode
    def no_select_mode_changed(bt, gg):
        gg.no_select_mode_set(bt.state)
        print gg.no_select_mode_get()

    bt = elementary.Check(win)
    bt.label = "NoSelectMode"
    bt.state = gg.no_select_mode_get()
    bt.callback_changed_add(no_select_mode_changed, gg)
    tb.pack(bt, 3, 1, 1, 1)
    bt.show()

    # bounce h
    def bounce_h_changed(bt, gg):
        (h_bounce, v_bounce) = gg.bounce_get()
        gg.bounce_set(bt.state, v_bounce)
        print gg.bounce_get()

    bt = elementary.Check(win)
    bt.label = "BounceH"
    (h_bounce, v_bounce) = gg.bounce_get()
    bt.state = h_bounce
    bt.callback_changed_add(bounce_h_changed, gg)
    tb.pack(bt, 4, 1, 1, 1)
    bt.show()

    # bounce v
    def bounce_v_changed(bt, gg):
        (h_bounce, v_bounce) = gg.bounce_get()
        gg.bounce_set(h_bounce, bt.state)
        print gg.bounce_get()

    bt = elementary.Check(win)
    bt.label = "BounceV"
    (h_bounce, v_bounce) = gg.bounce_get()
    bt.state = v_bounce
    bt.callback_changed_add(bounce_v_changed, gg)
    tb.pack(bt, 5, 1, 1, 1)
    bt.show()

    # item size
    def item_size_w_changed(sl, gg):
        (w, h) = gg.item_size_get()
        gg.item_size_set(sl.value, h)
        print gg.item_size_get()

    def item_size_h_changed(sl, gg):
        (w, h) = gg.item_size_get()
        gg.item_size_set(w, sl.value)
        print gg.item_size_get()

    sl = elementary.Slider(win)
    sl.label_set("ItemSizeW")
    sl.min_max_set(0, 500)
    sl.indicator_format_set("%.0f")
    sl.unit_format_set("%.0f")
    sl.span_size_set(100)
    (w, h) = gg.item_size_get()
    sl.value = w
    sl.callback_changed_add(item_size_w_changed, gg)
    tb.pack(sl, 0, 2, 2, 1)
    sl.show()

    sl = elementary.Slider(win)
    sl.label_set("ItemSizeH")
    sl.min_max_set(0, 500)
    sl.indicator_format_set("%.0f")
    sl.unit_format_set("%.0f")
    sl.span_size_set(100)
    (w, h) = gg.item_size_get()
    sl.value = h
    sl.callback_changed_add(item_size_h_changed, gg)
    tb.pack(sl, 0, 3, 2, 1)
    sl.show()

    # align
    def alignx_changed(sl, gg):
        (ax, ay) = gg.align_get()
        gg.align_set(sl.value, ay)
        print gg.align_get()

    def aligny_changed(sl, gg):
        (ax, ay) = gg.align_get()
        gg.align_set(ax, sl.value)
        print gg.align_get()

    sl = elementary.Slider(win)
    sl.label_set("AlignX")
    sl.min_max_set(0.0, 1.0)
    sl.indicator_format_set("%.2f")
    sl.unit_format_set("%.2f")
    sl.span_size_set(100)
    (ax, ay) = gg.align_get()
    sl.value = ax
    sl.callback_changed_add(alignx_changed, gg)
    tb.pack(sl, 0, 4, 2, 1)
    sl.show()

    sl = elementary.Slider(win)
    sl.label_set("AlignY")
    sl.min_max_set(0.0, 1.0)
    sl.indicator_format_set("%.2f")
    sl.unit_format_set("%.2f")
    sl.span_size_set(100)
    (ax, ay) = gg.align_get()
    sl.value = ax
    sl.callback_changed_add(aligny_changed, gg)
    tb.pack(sl, 0, 5, 2, 1)
    sl.show()

    # select first
    def select_first_clicked(bt, gg):
        ggi = gg.first_item
        ggi.selected = not ggi.selected

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Select first")
    bt.callback_clicked_add(select_first_clicked, gg)
    tb.pack(bt, 2, 2, 1, 1)
    bt.show()

    # select last
    def select_last_clicked(bt, gg):
        ggi = gg.last_item
        ggi.selected = not ggi.selected

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Select last")
    bt.callback_clicked_add(select_last_clicked, gg)
    tb.pack(bt, 3, 2, 1, 1)
    bt.show()

    # selection del
    def seldel_clicked(bt, gg):
        for ggi in gg.selected_items_get():
            ggi.delete()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Sel del")
    bt.callback_clicked_add(seldel_clicked, gg)
    tb.pack(bt, 4, 2, 1, 1)
    bt.show()

    # clear
    def clear_clicked(bt, gg):
        global item_count
        item_count = 0
        gg.clear()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Clear")
    bt.callback_clicked_add(clear_clicked, gg)
    tb.pack(bt, 5, 2, 1, 1)
    bt.show()

    # show first/last
    def show_clicked(bt, gg, first):
        ggi = gg.first_item if first else gg.last_item
        ggi.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Show first")
    bt.callback_clicked_add(show_clicked, gg, True)
    tb.pack(bt, 2, 3, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Show last")
    bt.callback_clicked_add(show_clicked, gg, False)
    tb.pack(bt, 3, 3, 1, 1)
    bt.show()

    # bring-in first/last
    def bring_in_clicked(bt, gg, first):
        ggi = gg.first_item if first else gg.last_item
        ggi.bring_in()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("BringIn first")
    bt.callback_clicked_add(bring_in_clicked, gg, True)
    tb.pack(bt, 4, 3, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("BringIn last")
    bt.callback_clicked_add(bring_in_clicked, gg, False)
    tb.pack(bt, 5, 3, 1, 1)
    bt.show()

    # append
    def append_clicked(bt, gg, n):
        global item_count
        while n:
            item_count += 1
            gg.item_append(itc, item_count, None)
            n -= 1

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Append 1")
    bt.callback_clicked_add(append_clicked, gg, 1)
    tb.pack(bt, 2, 4, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Append 100")
    bt.callback_clicked_add(append_clicked, gg, 100)
    tb.pack(bt, 3, 4, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Append 1000")
    bt.callback_clicked_add(append_clicked, gg, 1000)
    tb.pack(bt, 4, 4, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Append 10000 :)")
    bt.callback_clicked_add(append_clicked, gg, 10000)
    tb.pack(bt, 5, 4, 1, 1)
    bt.show()

    # prepend
    def prepend_clicked(bt, gg):
        global item_count
        item_count += 1
        gg.item_prepend(itc, item_count)

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Prepend")
    bt.callback_clicked_add(prepend_clicked, gg)
    tb.pack(bt, 2, 5, 1, 1)
    bt.show()

    # insert_before
    def ins_before_clicked(bt, gg):
        global item_count
        item_count += 1
        before = gg.selected_item_get()
        if before:
            gg.item_insert_before(itc, item_count, before)
        else:
            print("nothing selected")

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Ins before")
    bt.callback_clicked_add(ins_before_clicked, gg)
    tb.pack(bt, 3, 5, 1, 1)
    bt.show()

    # insert_after
    def ins_after_clicked(bt, gg):
        global item_count
        item_count += 1
        after = gg.selected_item_get()
        if after:
            gg.item_insert_after(itc, item_count, after)
        else:
            print("nothing selected")

    bt = elementary.Button(win)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, 0)
    bt.label_set("Ins after")
    bt.callback_clicked_add(ins_after_clicked, gg)
    tb.pack(bt, 4, 5, 1, 1)
    bt.show()


    win.resize(480, 800)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    gengrid_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
