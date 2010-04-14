#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Pager -{{{-
def my_pager_1(bt, pg, info):
    pg.content_promote(info["pg2"])

def my_pager_2(bt, pg, info):
    pg.content_promote(info["pg3"])

def my_pager_3(bt, pg, info):
    pg.content_promote(info["pg1"])

def my_pager_pop(obj, pg, info):
    pg.content_pop()

def pager_clicked(obj, it):
    win = elementary.Window("pager", elementary.ELM_WIN_BASIC)
    win.autodel_set(True)
    win.title_set("Pager")

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    pg = elementary.Pager(win)
    win.resize_object_add(pg)
    pg.show()

    info = dict()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    lb = elementary.Label(win)
    lb.label_set("This is page 1 in a pager stack.<br>"
                       "<br>"
                       "So what is a pager stack? It is a stack<br>"
                       "of pages that hold widgets in it. The<br>"
                       "pages can be pushed and popped on and<br>"
                       "off the stack, activated and otherwise<br>"
                       "activated if already in the stack<br>"
                       "(activated means promoted to the top of<br>"
                       "the stack).<br>"
                       "<br>"
                       "The theme may define the animation how<br>"
                       "show and hide of pages.")

    bx.pack_end(lb)
    lb.show()

    bt = elementary.Button(win)
    bt.label_set("Flip to 2")
    bt.callback_clicked_add(my_pager_1, pg, info)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.callback_clicked_add(my_pager_pop, pg, info)
    bx.pack_end(bt)
    bt.show()

    pg.content_push(bx)

    info["pg1"] = bx;

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    lb = elementary.Label(win)
    lb.label_set("This is page 2 in a pager stack.<br>"
                       "<br>"
                       "This is just like the previous page in<br>"
                       "the pager stack.")

    bx.pack_end(lb)
    lb.show()

    bt = elementary.Button(win)
    bt.label_set("Flip to 3")
    bt.callback_clicked_add(my_pager_2, pg, info)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.callback_clicked_add(my_pager_pop, pg, info)
    bx.pack_end(bt)
    bt.show()

    pg.content_push(bx)

    info["pg2"] = bx

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    lb = elementary.Label(win)
    lb.label_set("This is page 3 in a pager stack.<br>"
                       "<br>"
                       "This is just like the previous page in<br>"
                       "the pager stack.")

    bx.pack_end(lb)
    lb.show()

    bt = elementary.Button(win)
    bt.label_set("Flip to 1")
    bt.callback_clicked_add(my_pager_3, pg,info)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.callback_clicked_add(my_pager_pop, pg,info)
    bx.pack_end(bt)
    bt.show()

    pg.content_push(bx)

    info["pg3"] = bx

    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    pager_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
