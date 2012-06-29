#!/usr/bin/env python
# encoding: utf-8
import elementary
import evas

def thumb_clicked(obj):
    if not elementary.need_ethumb():
        print("Ethumb not available!")
        return

    images = (
        "panel_01.jpg",
        "plant_01.jpg",
        "rock_01.jpg",
        "rock_02.jpg",
        "sky_01.jpg",
        "sky_02.jpg",
        "sky_03.jpg",
        "sky_04.jpg",
        "wood_01.jpg",
        "mystrale.jpg",
        "mystrale_2.jpg"
    )

    win = elementary.StandardWindow("thumb", "Thumb")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    tb = elementary.Table(win)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

    n = 0
    for j in range(12):
        for i in range(12):
            th = elementary.Thumb(win)
            n = (n + 1) % 11
            th.file = "images/%s" % (images[n])
            th.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
            th.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
            tb.pack(th, i, j, 1, 1)
            th.editable = True
            th.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(sc)

    sc.content_set(tb)
    tb.show()
    sc.show()

    win.resize(600, 600)
    win.show()

if __name__ == "__main__":
    elementary.init()

    thumb_clicked(None)

    elementary.run()
    elementary.shutdown()
