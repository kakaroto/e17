#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Photo -{{{-
images = ["panel_01.jpg",
          "mystrale.jpg",
          "mystrale_2.jpg",
          "rock_02.jpg",
          "sky_01.jpg",
          "sky_02.jpg",
          "sky_03.jpg",
          "sky_04.jpg",
          "wood_01.jpg"]

def photo_clicked(obj):
    win = elementary.Window("photo", elementary.ELM_WIN_BASIC)
    win.title_set("Photo test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(sc)
    sc.show()

    elementary.need_ethumb()

    tb = elementary.Table(win)
    tb.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.content = tb
    tb.show()

    n = 0
    for j in range(12):
        for i in range(12):
            ph = elementary.Photo(win)
            name = "images/%s" % (images[n])
            n += 1
            if n >= 9: n = 0
            ph.aspect_fixed = False
            ph.size = 80
            if n == 8:
                ph.thumb = name
            else:
                ph.file = name
            ph.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
            ph.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
            ph.editable = True
            if n in [2, 3]:
                ph.fill_inside = True
                ph.style = "shadow"
            
            tb.pack(ph, i, j, 1, 1)
            ph.show()

    win.resize(300, 300)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    photo_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
