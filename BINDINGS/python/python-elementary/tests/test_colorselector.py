#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Color Selector -{{{-
def cb_cs_changed(cs, rect):
    print("changed")
    (r, g, b, a) = cs.color
    r = (r * a) / 255
    g = (g * a) / 255
    b = (b * a) / 255
    rect.color = (r, g, b, a)

def cb_cs_item_sel(cs, item, rect):
    print("selected")
    (r, g, b, a) = item.color
    r = (r * a) / 255
    g = (g * a) / 255
    b = (b * a) / 255
    rect.color = (r, g, b, a)

def cb_cs_item_lp(cs, item, rect):
    print("longpressed")
    (r, g, b, a) = item.color
    r = (r * a) / 255
    g = (g * a) / 255
    b = (b * a) / 255
    rect.color = (r, g, b, a)

def colorselector_clicked(obj):
    win = elementary.Window("colorselector", elementary.ELM_WIN_BASIC)
    win.title = "ColorSelector test"
    win.autodel = True
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(vbox)
    vbox.show()

    fr = elementary.Frame(win)
    fr.text = "Color View"
    fr.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    fr.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(fr)
    fr.show()

    re = evas.Rectangle(win.evas)
    re.size_hint_min = (1, 100)
    fr.content = re
    re.show()

    fr = elementary.Frame(win)
    fr.text = "Color Selector"
    fr.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fr.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(fr)
    fr.show()

    cs = elementary.Colorselector(win)
    cs.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    cs.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    cs.callback_changed_add(cb_cs_changed, re)
    cs.callback_color_item_selected_add(cb_cs_item_sel, re)
    cs.callback_color_item_longpressed_add(cb_cs_item_lp, re)
    cs.color = (255, 160, 132, 255)
    fr.content = cs
    cs.show()

    re.color = cs.color
    cs.palette_color_add(255, 90, 18, 255)
    cs.palette_color_add(255, 213, 0, 255)
    cs.palette_color_add(146, 255, 11, 255)
    cs.palette_color_add(9, 186, 10, 255)
    cs.palette_color_add(86, 201, 242, 255)
    cs.palette_color_add(18, 83, 128, 255)
    cs.palette_color_add(140, 53, 238, 255)
    cs.palette_color_add(255, 145, 145, 255)
    cs.palette_color_add(255, 59, 119, 255)
    cs.palette_color_add(133, 100, 69, 255)
    cs.palette_color_add(255, 255, 119, 255)
    cs.palette_color_add(133, 100, 255, 255)

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.text = "Palette"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda btn: cs.mode_set(elementary.ELM_COLORSELECTOR_PALETTE))
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Components"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda btn: cs.mode_set(elementary.ELM_COLORSELECTOR_COMPONENTS))
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "Both"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda btn: cs.mode_set(elementary.ELM_COLORSELECTOR_BOTH))
    hbox.pack_end(bt)
    bt.show()

    win.resize(320, 550)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    colorselector_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
