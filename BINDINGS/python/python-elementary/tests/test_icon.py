#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

def aspect_fixed_cb(obj, ic):
    ic.aspect_fixed_set(obj.state_get())

def fill_outside_cb(obj, ic):
    ic.fill_outside_set(obj.state_get())

def smooth_cb(obj, ic):
    ic.smooth_set(obj.state_get())

def bt_clicked(obj):
    win = elementary.Window("preload-prescale", elementary.ELM_WIN_BASIC)
    win.title_set("Preload & Prescale")
    win.autodel_set(True)

    bg = elementary.Background(win)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bg)
    bg.show()

    ic = elementary.Icon(win)
    win.resize_object_add(ic)
    ic.file_set("images/insanely_huge_test_image.jpg")

    ic.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ic.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    ic.resizable_set(True, True)
    ic.aspect_fixed_set(True)
    ic.preload_disabled_set(True)
    ic.prescale_set(True)
    ic.show()

    win.resize(350, 350)
    win.show()

#----- Icon -{{{-
def icon_clicked(obj):
    win = elementary.Window("icon test", elementary.ELM_WIN_BASIC)
    win.title_set("Icon Test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bg)
    bg.show()

    box = elementary.Box(win)
    win.resize_object_add(box)
    box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    box.show()

    content_box = elementary.Box(win)
    win.resize_object_add(content_box)
    content_box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    content_box.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    box.pack_end(content_box)
    content_box.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo.png")
    ic.resizable_set(True, True)
    ic.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ic.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    content_box.pack_end(ic)
    ic.show()

    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    content_box.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    box.pack_end(hbox)
    hbox.show()

    # Test Aspect Fixed
    tg = elementary.Check(win)
    tg.text_set("Aspect Fixed")
    tg.state_set(True)
    tg.callback_changed_add(aspect_fixed_cb, ic)
    hbox.pack_end(tg)
    tg.show()

    # Test Fill Outside
    tg = elementary.Check(win)
    tg.text_set("Fill Outside")
    tg.callback_changed_add(fill_outside_cb, ic)
    hbox.pack_end(tg)
    tg.show()

    # Test Smooth
    tg = elementary.Check(win)
    tg.text_set("Smooth")
    tg.state_set(True)
    tg.callback_changed_add(smooth_cb, ic)
    hbox.pack_end(tg)
    tg.show()

    # Test Preload, Prescale
    bt = elementary.Button(win)
    bt.text_set("Preload & Prescale")
    bt.callback_clicked_add(bt_clicked)
    hbox.pack_end(bt)
    bt.show()

    win.resize(400, 400)
    win.show()
# }}}

#----- Icon Transparent -{{{-
def icon_transparent_clicked(obj):
    win = elementary.Window("icon-transparent", elementary.ELM_WIN_BASIC)
    win.title_set("Icon Transparent")
    win.autodel_set(True)
    win.alpha_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    icon = elementary.Icon(win)
    icon.file_set("images/logo.png")
    icon.resizable_set(0, 0)
    win.resize_object_add(icon)
    icon.show()

    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    icon_transparent_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
