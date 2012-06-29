#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Ctxpopup -{{{-

def cb_items(li, item):
    print(("ctxpopup item selected: %s" % (item.text)))

def item_new(cp, label, icon = None):
    if icon:
        ic = elementary.Icon(cp)
        ic.standard_set(icon)
        ic.resizable_set(False, False)
        return cp.item_append(label, ic, cb_items)
    else:
        return cp.item_append(label, None, cb_items)

def cb_btn(btn):
    cp = btn.data["ctxpopup"]
    if "img" in cp.data:
        return
    img = evas.FilledImage(btn.evas)
    img.file_set("images/sky_04.jpg")
    img.move(40, 40)
    img.resize(320, 320)
    img.show()

    cp = btn.data["ctxpopup"]
    cp.data["img"] = img
    cp.raise_()
    # NOTE: the '_' is wanted as 'raise' is a reserved word in py

def cb_dismissed(cp):
    if "img" in cp.data:
        cp.data["img"].delete()

def cb_item1(li, item):
    cp = elementary.Ctxpopup(li)
    it = item_new(cp, "Go to home folder", "home")
    it = item_new(cp, "Save file", "file")
    it = item_new(cp, "Delete file", "delete")
    it = item_new(cp, "Navigate to folder", "folder")
    it.disabled = True
    it = item_new(cp, "Edit entry", "edit")
    it = item_new(cp, "Sate date and time", "clock")
    it.disabled = True

    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()

def cb_item2(li, item):
    cp = elementary.Ctxpopup(li)
    it = item_new(cp, "", "home")
    it = item_new(cp, "", "file")
    it = item_new(cp, "", "delete")
    it = item_new(cp, "", "folder")
    it = item_new(cp, "", "edit")
    it.disabled = True
    it = item_new(cp, "", "clock")

    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()

def cb_item3(li, item):
    cp = elementary.Ctxpopup(li)
    it = item_new(cp, "Eina")
    it = item_new(cp, "Eet")
    it = item_new(cp, "Evas")
    it = item_new(cp, "Ecore")
    it.disabled = True
    it = item_new(cp, "Embryo")
    it = item_new(cp, "Edje")

    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()

def cb_item4(li, item):
    cp = elementary.Ctxpopup(li)
    cp.horizontal = True
    it = item_new(cp, "", "home")
    it = item_new(cp, "", "file")
    it = item_new(cp, "", "delete")
    it = item_new(cp, "", "folder")
    it = item_new(cp, "", "edit")
    it = item_new(cp, "", "clock")

    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()

def cb_item5(li, item):
    box = elementary.Box(li)
    box.size_hint_min = (150, 150)

    sc = elementary.Scroller(li)
    sc.bounce = (False, True)
    sc.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.show()

    bt = elementary.Button(li)
    bt.text = "Enlightenment"
    bt.size_hint_min = (140, 140)

    sc.content = bt
    box.pack_end(sc)

    cp = elementary.Ctxpopup(li)
    cp.content = box
    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()

def cb_item6(li, item):
    box = elementary.Box(li)
    box.size_hint_min = (200, 150)

    sc = elementary.Scroller(li)
    sc.bounce = (False, True)
    sc.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.show()

    bt = elementary.Button(li)
    bt.text = "Ctxpop will be on the top of layer"
    bt.callback_clicked_add(cb_btn)
    bt.size_hint_min = (190, 140)

    sc.content = bt
    box.pack_end(sc)

    cp = elementary.Ctxpopup(li)
    cp.callback_dismissed_add(cb_dismissed)
    cp.content = box
    (x, y) = li.evas.pointer_canvas_xy_get()
    cp.move(x, y)
    cp.show()
    bt.data["ctxpopup"] = cp

def ctxpopup_clicked(obj):
    win = elementary.Window("ctxpopup", elementary.ELM_WIN_BASIC)
    win.title = "Context popup test"
    win.autodel = True
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    win.resize_object_add(li)
    li.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.mode = elementary.ELM_LIST_COMPRESS
    li.show()

    li.item_append("Ctxpopup with icons and labels", callback=cb_item1)
    li.item_append("Ctxpopup with icons only", callback=cb_item2)
    li.item_append("Ctxpopup with labels only", callback=cb_item3)
    li.item_append("Ctxpopup at horizontal mode", callback=cb_item4)
    li.item_append("Ctxpopup with user content", callback=cb_item5)
    li.item_append("Ctxpopup with restacking", callback=cb_item6)
    li.go()

    win.resize(400, 400)
    win.show()
# }}}


#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    ctxpopup_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
