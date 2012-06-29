#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- MultiButtonEntry -{{{-
counter = 0

def cb_item_selected(mbe, item):
    print "cb!!!!!!!!!!!" # TODO this is never called
    print mbe
    print item

def cb_btn_item_prepend(btn, mbe):
    global counter

    counter += 1
    item = mbe.item_prepend("item #%d" % (counter), cb_item_selected, "PippO")

def cb_btn_item_append(btn, mbe):
    global counter

    counter += 1
    item = mbe.item_append("item #%d" % (counter), cb_item_selected, "PippO")

def cb_btn_item_insert_after(btn, mbe):
    global counter

    counter += 1
    after = mbe.selected_item
    item = mbe.item_insert_after(after, "item #%d" % (counter), cb_item_selected)

def cb_btn_item_insert_before(btn, mbe):
    global counter

    counter += 1
    before = mbe.selected_item
    item = mbe.item_insert_before(before, "item #%d" % (counter), cb_item_selected)

def cb_btn_clear2(btn, mbe):
    for item in mbe.items:
        item.delete()

def cb_filter1(mbe, text):
    return True

def multibuttonentry_clicked(obj, item=None):
    win = elementary.Window("multibuttonentry", elementary.ELM_WIN_BASIC)
    win.title_set("MultiButtonEntry test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    vbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(vbox)
    vbox.show()

    sc = elementary.Scroller(win)
    sc.bounce = (False, True)
    sc.policy = (elementary.ELM_SCROLLER_POLICY_OFF, elementary.ELM_SCROLLER_POLICY_AUTO)
    sc.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.pack_end(sc)
    sc.show()

    mbe = elementary.MultiButtonEntry(win)
    mbe.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    mbe.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    mbe.text = "To: "
    mbe.part_text_set("guide", "Tap to add recipient")
    mbe.filter_append(cb_filter1)
    sc.content = mbe
    mbe.show()

    print mbe.entry

    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.text = "item_append"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(cb_btn_item_append, mbe)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "item_prepend"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(cb_btn_item_prepend, mbe)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "item_insert_after"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(cb_btn_item_insert_after, mbe)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "item_insert_before"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(cb_btn_item_insert_before, mbe)
    hbox.pack_end(bt)
    bt.show()


    hbox = elementary.Box(win)
    hbox.horizontal = True
    hbox.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.text = "delete selected item"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda btn: mbe.selected_item.delete())
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "clear"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda bt: mbe.clear())
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "clear2"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(cb_btn_clear2, mbe)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.text = "toggle expand"
    bt.size_hint_align = (evas.EVAS_HINT_FILL, 0.0)
    bt.size_hint_weight = (evas.EVAS_HINT_EXPAND, 0.0)
    bt.callback_clicked_add(lambda btn: mbe.expanded_set(not mbe.expanded_get()))
    hbox.pack_end(bt)
    bt.show()

    mbe.focus = True
    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    multibuttonentry_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
