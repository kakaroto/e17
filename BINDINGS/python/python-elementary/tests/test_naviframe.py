#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Naviframe -{{{-
def content_new(parent, img):
    photo = elementary.Photo(parent)
    photo.file_set(img)
    photo.fill_inside_set(True)
    photo.style_set("shadow")
    return photo

def navi_pop(bt, nf):
    nf.item_pop()

def navi_promote(bt, nf, item):
    item.item_promote()

def title_visible(obj, item):
    item.title_visible = not item.title_visible

def page2(bt, nf):
    ic = elementary.Icon(nf)
    ic.file_set("images/icon_right_arrow.png")
    bt = elementary.Button(nf)
    bt.callback_clicked_add(page3, nf)
    bt.content_set(ic)

    content = content_new(nf, "images/plant_01.jpg");
    item = nf.item_push("Page 2", None, bt, content, "basic")
    item.part_text_set("subtitle", "Here is sub-title part!")

def page3(bt, nf):
    bt = elementary.Button(nf)
    bt.callback_clicked_add(navi_pop, nf)
    bt.text_set("Prev")

    bt2 = elementary.Button(nf)
    bt2.callback_clicked_add(page4, nf)
    bt2.text_set("Next")

    content = content_new(nf, "images/rock_01.jpg");
    item = nf.item_push("Page 3", bt, bt2, content, "basic")
    ic = elementary.Icon(nf)
    ic.file_set("images/logo_small.png")
    item.part_content_set("icon", ic)

def page4(bt, nf):
    ic = elementary.Icon(nf)
    ic.file_set("images/icon_right_arrow.png")
    bt = elementary.Button(nf)
    bt.callback_clicked_add(page5, nf)
    bt.content_set(ic)

    content = content_new(nf, "images/rock_02.jpg");
    item = nf.item_push("Page 4", None, bt, content, "basic")
    ic = elementary.Icon(nf)
    ic.file_set("images/logo_small.png")
    item.part_content_set("icon", ic)
    item.part_text_set("subtitle", "Title area visibility test")
    item.title_visible_set(False)
    content.callback_clicked_add(title_visible, item)

def page5(bt, nf):
    bt = elementary.Button(nf)
    bt.callback_clicked_add(navi_pop, nf)
    bt.text_set("Page 4")

    bt2 = elementary.Button(nf)
    bt2.callback_clicked_add(page6, nf)
    bt2.text_set("Page 6")

    content = content_new(nf, "images/sky_01.jpg");
    item = nf.item_insert_after(nf.top_item_get(), "Page 5", bt, bt2, content, "basic")
    item.part_text_set("subtitle", "This page is inserted without transition (TODO)")

def page6(bt, nf):
    bt = elementary.Button(nf)
    bt.callback_clicked_add(navi_pop, nf)
    bt.text_set("Page 5")

    bt2 = elementary.Button(nf)
    bt2.callback_clicked_add(page7, nf)
    bt2.text_set("Page 7")

    content = content_new(nf, "images/sky_03.jpg");
    item = nf.item_push("Page 6", bt, bt2, content, "overlap")
    item.part_text_set("subtitle", "Overlap style!")

def page7(bt, nf):
    bt = elementary.Button(nf)
    bt.callback_clicked_add(navi_pop, nf)
    bt.text_set("Page 6")

    bt2 = elementary.Button(nf)
    bt2.callback_clicked_add(navi_promote, nf, nf.data["page1"])
    bt2.text_set("Page 1")

    content = content_new(nf, "images/sky_02.jpg");
    item = nf.item_push("Page 7", bt, bt2, content, "overlap")
    item.part_text_set("subtitle", "Overlap style!")


def naviframe_clicked(obj):
    win = elementary.Window("naviframe", elementary.ELM_WIN_BASIC)
    win.title_set("Naviframe test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.show()

    nf = elementary.Naviframe(win)
    win.resize_object_add(nf)
    nf.show()

    bt = elementary.Button(win)
    bt.callback_clicked_add(page2, nf)
    bt.text_set("Next")

    content = content_new(nf, "images/logo.png")
    item = nf.item_push("Page 1", None, bt, content, "basic")
    nf.data["page1"] = item

    win.resize(400, 600)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    naviframe_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
