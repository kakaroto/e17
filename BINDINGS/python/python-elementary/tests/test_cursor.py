#!/usr/bin/env python
import os
import elementary
from elementary import cursors
import edje
import ecore
import evas

#----- Cursors -{{{-
def cursor_clicked(obj, it):
    win = elementary.Window("cursors", elementary.ELM_WIN_BASIC)
    win.title_set("Cursors")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ck = elementary.Clock(win)
    ck.cursor_set(cursors.ELM_CURSOR_CLOCK)
    bx.pack_end(ck)
    ck.show()

    bt = elementary.Button(win)
    bt.label_set("Coffee Mug")
    bt.cursor_set(cursors.ELM_CURSOR_COFFEE_MUG)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Cursor unset")
    bt.cursor_set(cursors.ELM_CURSOR_BOGOSITY)
    bt.cursor_unset()
    bx.pack_end(bt)
    bt.show()

    lst = elementary.List(win)
    lst.item_append("watch over list")
    lst.item_append("watch over list")
    lst.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    lst.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    lst.cursor_set(cursors.ELM_CURSOR_WATCH)
    bx.pack_end(lst)
    lst.go()
    lst.show()

    en = elementary.ScrolledEntry(win)
    en.single_line_set(True)
    en.entry_set("Xterm cursor")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    en.cursor_set(cursors.ELM_CURSOR_XTERM)
    bx.pack_end(en)
    en.show()

    win.resize(320, 480)
    win.show()
# }}}

#----- Cursors2 -{{{-
def cursor2_clicked(obj, it):
    win = elementary.Window("cursors", elementary.ELM_WIN_BASIC)
    win.title_set("Cursors 2")
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
    ti = tb.item_append("folder-new", "Bogosity", None, None)
    ti.cursor_set(cursors.ELM_CURSOR_BOGOSITY)
    ti = tb.item_append("clock", "Unset", None, None)
    ti.cursor_set(cursors.ELM_CURSOR_BOGOSITY)
    ti.cursor_unset()
    ti = tb.item_append("document-print", "Xterm", None, None)
    ti.cursor_set(cursors.ELM_CURSOR_XTERM)
    tb.size_hint_weight_set(0.0, 0.0)
    tb.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    bx.pack_end(tb)
    tb.show()

    lst = elementary.List(win)
    li = lst.item_append("cursor bogosity")
    li.cursor_set(cursors.ELM_CURSOR_BOGOSITY)
    li = lst.item_append("cursor unset")
    li.cursor_set(cursors.ELM_CURSOR_BOGOSITY)
    li.cursor_unset()
    li = lst.item_append("cursor xterm")
    li.cursor_set(cursors.ELM_CURSOR_XTERM)
    lst.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    lst.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(lst)
    lst.go()
    lst.show()

    win.resize(320, 480)
    win.show()
# }}}

#----- Cursors3 -{{{-
def cursor3_clicked(obj, it):
    win = elementary.Window("cursors", elementary.ELM_WIN_BASIC)
    win.title_set("Cursors 3")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("hand1")
    bt.cursor_set(cursors.ELM_CURSOR_HAND1)
    bt.cursor_engine_only_set(False)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("hand2 x")
    bt.cursor_set(cursors.ELM_CURSOR_HAND2)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("hand2")
    bt.cursor_set(cursors.ELM_CURSOR_HAND2)
    bt.cursor_engine_only_set(False)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("hand3")
    bt.cursor_set("hand3")
    bt.cursor_engine_only_set(False)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("hand3")
    bt.cursor_set("hand3")
    bt.cursor_engine_only_set(False)
    bt.cursor_style_set("transparent")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("not existent")
    bt.cursor_set("hand4")
    bt.cursor_engine_only_set(False)
    bx.pack_end(bt)
    bt.show()

    elementary.cursor_engine_only_set(0)
    bt = elementary.Button(win)
    bt.label_set("hand 2 engine only config false")
    bt.cursor_set("hand2")
    bx.pack_end(bt)
    bt.show()

    elementary.cursor_engine_only_set(1)
    bt = elementary.Button(win)
    bt.label_set("hand 2 engine only config true")
    bt.cursor_set("hand2")
    bx.pack_end(bt)
    bt.show()

    lst = elementary.List(win)
    li = lst.item_append("cursor hand2 x")
    li.cursor_set(cursors.ELM_CURSOR_HAND2)
    li = lst.item_append("cursor hand2")
    li.cursor_set(cursors.ELM_CURSOR_HAND2)
    li.cursor_engine_only_set(False)
    li = lst.item_append("cursor hand3")
    li.cursor_set("hand3")
    li.cursor_engine_only_set(False)
    li = lst.item_append("cursor hand3 transparent")
    li.cursor_set("hand3")
    bt.cursor_style_set("transparent")
    li.cursor_engine_only_set(False)
    lst.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    lst.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(lst)
    lst.go()
    lst.show()

    win.resize(320, 480)
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

    items = [("Cursor", cursor_clicked),
             ("Cursor 2", cursor2_clicked),
             ("Cursor 3", cursor3_clicked)]

    li = elementary.List(win)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    box0.pack_end(li)
    li.show()

    for item in items:
        li.item_append(item[0], callback=item[1])

    li.go()

    win.resize(320, 480)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
