#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Toolbar -{{{-
def tb_1(obj, it, ph):
    ph.file_set("images/panel_01.jpg")

def tb_2(obj, it, ph):
    ph.file_set("images/rock_01.jpg")

def tb_3(obj, it, ph):
    ph.file_set("images/wood_01.jpg")

def tb_4(obj, it, ph):
    ph.file_set("images/sky_03.jpg")

def tb_5(obj, it, ph):
    ph.file_set(None)

def toolbar_clicked(obj, it):
    win = elementary.Window("entry-scrolled", elementary.ELM_WIN_BASIC)
    win.title_set("Entry Scrolled")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    tb = elementary.Toolbar(win)
    tb.homogenous = False
    tb.size_hint_weight_set(0.0, 0.0)
    tb.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)

    ph1 = elementary.Photo(win)
    ph2 = elementary.Photo(win)
    ph3 = elementary.Photo(win)
    ph4 = elementary.Photo(win)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    item = tb.item_add(ic, "Hello", tb_1)
    item.disabled = True

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    item = tb.item_add(ic, "World,", tb_2, ph2)
    item.select()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "here", tb_3, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "comes", tb_4, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "python-elementary!", tb_5, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    item = tb.item_add(ic, "Menu", tb_5, ph4)
    item.menu_set(True)
    tb.menu_parent_set(win)
    menu = item.menu_get()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    menu.item_add(None, "Here", ic, tb_3, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    menu_item = menu.item_add(None, "Comes", ic, tb_4, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    menu.item_add(menu_item, "hey ho", ic, tb_4, ph4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    menu.item_add(None, "python-elementary", ic, tb_5, ph4)

    bx.pack_end(tb)
    tb.show()

    tb = elementary.Table(win)
    tb.size_hint_weight_set(0.0, evas.EVAS_HINT_EXPAND)
    tb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    ph1.size_set(40)
    ph1.file_set("images/plant_01.jpg")
    ph1.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ph1.size_hint_align_set(0.5, 0.5)
    tb.pack(ph1, 0, 0, 1, 1)
    ph1.show()

    ph2.size_set(80)
    ph2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ph2.size_hint_align_set(0.5, 0.5)
    tb.pack(ph2, 1, 0, 1, 1)
    ph2.show()

    ph3.size_set(40)
    ph3.file_set("images/sky_01.jpg")
    ph3.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ph3.size_hint_align_set(0.5, 0.5)
    tb.pack(ph3, 0, 1, 1, 1)
    ph3.show()

    ph4.size_set(60)
    ph4.file_set("images/sky_02.jpg")
    ph4.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ph4.size_hint_align_set(0.5, 0.5)
    tb.pack(ph4, 1, 1, 1, 1)
    ph4.show()

    bx.pack_end(tb)
    tb.show()

    win.resize(320, 300)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    toolbar_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
