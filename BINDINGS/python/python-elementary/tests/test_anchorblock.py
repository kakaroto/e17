#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- AnchorBlock -{{{-
def my_anchorblock_bt(bt, ab):
    ab.hover_end()

def my_anchorblock_anchor(obj, ei, ab):
    if ei:
       bt = elementary.Button(obj)
       bt.label_set(ei.name)
       ei.hover.content_set("middle", bt)
       bt.show()
       if (ei.hover_top):
           bx = elementary.Box(obj)
           bt = elementary.Button(obj)
           bt.label_set("Top 1")
           bx.pack_end(bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 2")
           bx.pack_end(bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 3")
           bx.pack_end(bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()
           ei.hover.content_set("top", bx)
           bx.show()
       if (ei.hover_bottom):
           bt = elementary.Button(obj)
           bt.label_set("Bot")
           ei.hover.content_set("bottom", bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()
       if (ei.hover_left):
           bt = elementary.Button(obj)
           bt.label_set("Left")
           ei.hover.content_set("left", bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()
       if (ei.hover_right):
           bt = elementary.Button(obj)
           bt.label_set("Right")
           ei.hover.content_set("right", bt)
           bt.callback_clicked_add(my_anchorblock_bt, ab)
           bt.show()

def my_anchorblock_edge_left(obj):
    print "left"
def my_anchorblock_edge_right(obj):
    print "right"
def my_anchorblock_edge_top(obj):
    print "top"
def my_anchorblock_edge_bottom(obj):
    print "bottom"

def my_anchorblock_scroll(obj):
   (x, y, w, h) = obj.region_get()
   (vw, vh) = obj.child_size_get()
   print "scroll %ix%i +%i+%i in %ix%i" % (w, h, x, y, vw, vh)

def anchorblock_clicked(obj, it):
    win = elementary.Window("anchorblock", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorblock")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(sc)

    sc._callback_add("edge_left", my_anchorblock_edge_left)
    sc._callback_add("edge_right", my_anchorblock_edge_right)
    sc._callback_add("edge_top", my_anchorblock_edge_top)
    sc._callback_add("edge_bottom", my_anchorblock_edge_bottom)
    sc._callback_add("scroll", my_anchorblock_scroll)

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.content_set(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    bb = elementary.Bubble(win)
    bb.label_set("Message 3")
    bb.info_set("10:32 4/11/2008")
    bb.icon_set(ic)
    ic.show()
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    ab = elementary.AnchorBlock(win)
    ab.hover_style_set("popout")
    ab.hover_parent_set(win)
    ab.text_set("Hi there. This is the most recent message in the "
                "list of messages. It has one <a href=tel:+614321234>+61 432 1234</a> "
                "(phone number) to click on.")
    ab.callback_clicked_add(my_anchorblock_anchor, ab)
    bb.content_set(ab)
    ab.show()
    bx.pack_end(bb)
    bb.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    bb = elementary.Bubble(win)
    bb.label_set("Message 2")
    bb.info_set("7:16 27/10/2008")
    bb.icon_set(ic)
    ic.show()
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    ab = elementary.AnchorBlock(win)
    ab.hover_style_set("popout")
    ab.hover_parent_set(win)
    ab.text_set("Hey what are you doing? This is the second last message "
                "Hi there. This is the most recent message in the "
                "list. It's a longer one so it can wrap more and "
                "contains a <a href=contact:john>John</a> contact "
                "link in it to test popups on links. The idea is that "
                "all SMS's are scanned for things that look like phone "
                "numbers or names that are in your contacts list, and "
                "if they are, they become clickable links that pop up "
                "a menus of obvious actions to perform on this piece "
                "of information. This of course can be later explicitly "
                "done by links maybe running local apps or even being "
                "web URL's too that launch the web browser and point it "
                "to that URL.")
    ab.callback_clicked_add(my_anchorblock_anchor, ab)
    bb.content_set(ab)
    ab.show()
    bx.pack_end(bb)
    bb.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    bb = elementary.Bubble(win)
    bb.label_set("Message 1")
    bb.info_set("20:47 18/6/2008")
    bb.icon_set(ic)
    ic.show()
    bb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bb.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    ab = elementary.AnchorBlock(win)
    ab.hover_style_set("popout")
    ab.hover_parent_set(win)
    ab.text_set("This is a short message")
    ab.callback_clicked_add(my_anchorblock_anchor, ab)
    bb.content_set(ab)
    ab.show()
    bx.pack_end(bb)
    bb.show()

    sc.show()

    win.resize(320, 300)

    win.focus()
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    anchorblock_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
