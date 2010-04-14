#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- List -{{{-
def my_list_show_it(obj, it):
    it.show()

def list_clicked(obj, it):
    win = elementary.Window("list", elementary.ELM_WIN_BASIC)
    win.title_set("List")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    win.resize_object_add(li)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(1, 1)
    it1 = li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    li.item_append(".", ic, ic2)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)

    bx = elementary.Box(win)
    bx.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()
    li.item_append("are")

    li.item_append("you")
    it3 = li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    it4 = li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Is this label long enough?")
    it5 = li.item_append("Maybe this one is even longer so we can test long long items.")

    li.go()

    li.show()

    tb2 = elementary.Table(win)
    tb2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(tb2)

    bt = elementary.Button(win)
    bt.label_set("Hello")
    bt.callback_clicked_add(my_list_show_it, it1)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 0, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("How")
    bt.callback_clicked_add(my_list_show_it, it2)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 1, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("doing")
    bt.callback_clicked_add(my_list_show_it, it3)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 2, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Here")
    bt.callback_clicked_add(my_list_show_it, it4)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 3, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Maybe this...")
    bt.callback_clicked_add(my_list_show_it, it5)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 4, 1, 1);
    bt.show()

    tb2.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- List2 -{{{-
def my_list2_clear(bt, li):
    li.clear()

def my_list2_sel(obj, it):
    it = obj.selected_item_get()
    if it is not None:
        it.selected_set(False)

def list2_clicked(obj, it):
    win = elementary.Window("list-2", elementary.ELM_WIN_BASIC)
    win.title_set("List 2")
    win.autodel_set(True)

    bg = elementary.Background(win)
    bg.file_set('images/plant_01.jpg')
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    li = elementary.List(win)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.horizontal_mode_set(elementary.ELM_LIST_LIMIT)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    it = li.item_append("Hello", ic, callback=my_list2_sel)
    it.selected_set(True)
    ic = elementary.Icon(win)
    ic.scale_set(0, 0)
    ic.file_set('images/logo_small.png')
    li.item_append("world", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    li.item_append(".", ic)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx2.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx2.pack_end(ic)
    ic.show()

    li.item_append("are", bx2)

    li.item_append("you")
    li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Longer label.")

    li.go()

    bx.pack_end(li)
    li.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.homogenous_set(True)
    bx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.callback_clicked_add(my_list2_clear, li)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- List3 -{{{-
def list3_clicked(obj, it):
    win = elementary.Window("list-3", elementary.ELM_WIN_BASIC)
    win.title_set("List 3")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    win.resize_object_add(li)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.horizontal_mode_set(elementary.ELM_LIST_COMPRESS)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    li.item_append("world", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    li.item_append(".", ic)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)

    bx = elementary.Box(win)
    bx.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()

    li.item_append("are", bx)
    li.item_append("you")
    li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Is this label long enough?")
    it5 = li.item_append("Maybe this one is even longer so we can test long long items.")

    li.go()

    li.show()

    win.resize(320, 300)
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

    items = [("List", list_clicked),
             ("List 2", list2_clicked),
             ("List 3", list3_clicked)]

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
