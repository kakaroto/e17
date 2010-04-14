#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- Hoversel -{{{-
def hoversel_clicked(obj, it):
    win = elementary.Window("hoversel", elementary.ELM_WIN_BASIC)
    win.title_set("Hoversel")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("Labels")
    bt.item_add("Item 1")
    bt.item_add("Item 2")
    bt.item_add("Item 3")
    bt.item_add("Item 4 - Long Label Here")
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("Some Icons")
    bt.item_add("Item 1")
    bt.item_add("Item 2")
    bt.item_add("Item 3", "home", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 4", "close", elementary.ELM_ICON_STANDARD)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("All Icons")
    bt.item_add("Item 1", "apps", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 2", "arrow_down", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 3", "home", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 4", "close", elementary.ELM_ICON_STANDARD)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("All Icons")
    bt.item_add("Item 1", "apps", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 2", "images/logo_small.png", elementary.ELM_ICON_FILE)
    bt.item_add("Item 3", "home", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 4", "close", elementary.ELM_ICON_STANDARD)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("Disabled Hoversel")
    bt.item_add("Item 1", "apps", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 2", "close", elementary.ELM_ICON_STANDARD)
    bt.disabled_set(True)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Hoversel(win)
    bt.hover_parent_set(win)
    bt.label_set("Icon + Label")

    ic = elementary.Icon(win)
    ic.file_set("images/sky_03.jpg")
    bt.icon_set(ic)
    ic.show()

    bt.item_add("Item 1", "apps", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 2", "arrow_down", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 3", "home", elementary.ELM_ICON_STANDARD)
    bt.item_add("Item 4", "close", elementary.ELM_ICON_STANDARD)
    bt.size_hint_weight_set(0.0, 0.0)
    bt.size_hint_align_set(0.5, 0.5)
    bx.pack_end(bt)
    bt.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    hoversel_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
