#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- mapbuf -{{{-

names = [ "Hello", "World", "Spam", "Egg", "Ham", "Good", "Bad", "Milk",
          "Smell", "Of", "Sky", "Gold", "Hole", "Pig", "And", "Calm"]

mb_list = []

def cb_btn_close(btn, win):
    win.delete()
    
def cb_ck_map(ck):
    for mb in mb_list:
        mb.enabled = ck.state
    
def cb_ck_alpha(ck):
    for mb in mb_list:
        mb.alpha = ck.state
    
def cb_ck_smooth(ck):
    for mb in mb_list:
        mb.smooth = ck.state
    
def cb_ck_fs(ck, win):
    win.fullscreen = ck.state

def mapbuf_clicked(obj, item=None):
    global mb_list

    win = elementary.Window("mapbuf", elementary.ELM_WIN_BASIC)
    win.title_set("Mapbuf test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    bg.file = "images/sky_04.jpg"
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    win.resize_object_add(vbox)
    vbox.show()

    # launcher
    sc = elementary.Scroller(win);
    sc.bounce = (True, False)
    sc.policy = (elementary.ELM_SCROLLER_POLICY_OFF, elementary.ELM_SCROLLER_POLICY_OFF)
    sc.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(sc)

    bx = elementary.Box(win)
    bx.horizontal = True
    bx.homogeneous = True
    bx.show()

    for k in range(8):
        tb = elementary.Table(win)
        tb.size_hint_align = (0.5, 0.5)
        tb.size_hint_weight = (0.0, 0.0)
        tb.show()

        pad = evas.Rectangle(win.evas)
        pad.color = (255, 255, 0, 255)
        pad.size_hint_min = (464, 4)
        pad.size_hint_weight = (0.0, 0.0)
        pad.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        pad.show()
        tb.pack(pad, 1, 0, 5, 1)

        pad = evas.Rectangle(win.evas)
        pad.color = (255, 255, 0, 255)
        pad.size_hint_min = (464, 4)
        pad.size_hint_weight = (0.0, 0.0)
        pad.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        pad.show()
        tb.pack(pad, 1, 11, 5, 1)

        pad = evas.Rectangle(win.evas)
        pad.color = (255, 255, 0, 255)
        pad.size_hint_min = (4, 4)
        pad.size_hint_weight = (0.0, 0.0)
        pad.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        pad.show()
        tb.pack(pad, 0, 1, 1, 10)

        pad = evas.Rectangle(win.evas)
        pad.color = (255, 255, 0, 255)
        pad.size_hint_min = (4, 4)
        pad.size_hint_weight = (0.0, 0.0)
        pad.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        pad.show()
        tb.pack(pad, 6, 1, 1, 10)

        mb = elementary.Mapbuf(win)
        mb_list.append(mb)
        mb.content = tb
        bx.pack_end(mb)
        mb.show()

        n = m = 0
        for j in range(5):
            for i in range(5):
                ic = elementary.Icon(win)
                ic.scale_set(0.5)
                ic.file_set("images/icon_%02d.png" % (n));
                ic.resizable_set(0, 0)
                ic.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
                ic.size_hint_align_set(0.5, 0.5)
                tb.pack(ic, 1 + i, 1 + (j * 2), 1, 1)
                ic.show()

                lb = elementary.Label(win)
                lb.style = "marker"
                lb.text = names[m]
                tb.pack(lb, 1 + i, 1 + (j * 2) + 1, 1, 1)
                lb.show()

                n = n + 1 if n < 23 else 0
                m = m + 1 if m < 15 else 0

    sc.content = bx
    sc.page_relative_set(1.0, 1.0)
    sc.show()

    # controls
    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.homogeneous = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    hbox.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    vbox.pack_start(hbox)
    hbox.show()

    ck = elementary.Check(win)
    ck.text = "Map"
    ck.state = False
    ck.callback_changed_add(cb_ck_map)
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.callback_changed_add(cb_ck_alpha)
    ck.text = "Alpha"
    ck.state = True
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.callback_changed_add(cb_ck_smooth)
    ck.text = "Smooth"
    ck.state = True
    hbox.pack_end(ck)
    ck.show()

    ck = elementary.Check(win)
    ck.callback_changed_add(cb_ck_fs, win)
    ck.text = "FS"
    ck.state = False
    hbox.pack_end(ck)
    ck.show()

    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_btn_close, win)
    bt.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    hbox.pack_end(bt)
    bt.show()

    win.resize(480, 800)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    mapbuf_clicked(None)

    elementary.run()
    elementary.shutdown()
    
# }}}
# vim:foldmethod=marker
