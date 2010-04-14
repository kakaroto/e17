#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- AnchorView -{{{-
def my_anchorview_bt(bt, av):
    av.hover_end()

def my_anchorview_anchor(obj, ei, av):
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
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 2")
           bx.pack_end(bt)
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()
           bt.label_set("Top 3")
           bx.pack_end(bt)
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()
           ei.hover.content_set("top", bx)
           bx.show()
       if (ei.hover_bottom):
           bt = elementary.Button(obj)
           bt.label_set("Bot")
           ei.hover.content_set("bottom", bt)
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()
       if (ei.hover_left):
           bt = elementary.Button(obj)
           bt.label_set("Left")
           ei.hover.content_set("left", bt)
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()
       if (ei.hover_right):
           bt = elementary.Button(obj)
           bt.label_set("Right")
           ei.hover.content_set("right", bt)
           bt.callback_clicked_add(my_anchorview_bt, av)
           bt.show()


def anchorview_clicked(obj, it):
    win = elementary.Window("anchorview", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorview")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    av = elementary.AnchorView(win)
    av.hover_style_set("popout")
    av.hover_parent_set(win)
    av.text_set("This is an entry widget in this window that<br>"
                "uses markup <b>like this</> for styling and<br>"
                "formatting <em>like this</>, as well as<br>"
                "<a href=X><link>links in the text</></a>, so enter text<br>"
                "in here to edit it. By the way, links are<br>"
                "called <a href=anc-02>Anchors</a> so you will need<br>"
                "to refer to them this way.")
    av.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    av.callback_clicked_add(my_anchorview_anchor, av)
    win.resize_object_add(av)
    av.show()

    win.resize(320, 300)

    win.focus()
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    anchorview_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
