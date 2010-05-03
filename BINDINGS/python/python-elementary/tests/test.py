#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

from test_bg import bg_plain_clicked, bg_image_clicked
from test_icon import icon_transparent_clicked
from test_box import box_vert_clicked, box_vert2_clicked, box_horiz_clicked
from test_button import buttons_clicked
from test_toggle import toggles_clicked
from test_table import table_clicked
from test_clock import clock_clicked
from test_hover import hover_clicked, hover2_clicked
from test_entry import entry_clicked, entry_scrolled_clicked
from test_notepad import notepad_clicked
from test_anchorview import anchorview_clicked
from test_anchorblock import anchorblock_clicked
from test_toolbar import toolbar_clicked
from test_hoversel import hoversel_clicked
from test_list import list_clicked, list2_clicked, list3_clicked
from test_inwin import inner_window_clicked
from test_check import check_clicked
from test_radio import radio_clicked
from test_pager import pager_clicked
from test_progressbar import progressbar_clicked
from test_fileselector import fileselector_clicked
from test_separator import separator_clicked
from test_scroller import scroller_clicked
from test_notify import notify_clicked
from test_menu import menu_clicked
from test_panel import panel_clicked
from test_layout import layout_clicked
from test_spinner import spinner_clicked
from test_genlist import genlist_clicked

#----- Main -{{{-
def destroy(obj, str1, str2, str3, str4):
    print "DEBUG: window destroy callback called!"
    print "DEBUG: str1='%s', str2='%s', str3='%s', str4='%s'" %(str1, str2,
                                                                str3, str4)
    elementary.exit()

if __name__ == "__main__":
    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.callback_destroy_add(destroy, "test1", "test2", str3="test3", str4="test4")

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

    items = [("Bg Plain", bg_plain_clicked),
             ("Bg Image", bg_image_clicked),
             ("Icon Transparent", icon_transparent_clicked),
             ("Box Vert", box_vert_clicked),
             ("Box Vert 2", box_vert2_clicked),
             ("Box Horiz", box_horiz_clicked),
             ("Buttons", buttons_clicked),
             ("Toggles", toggles_clicked),
             ("Table", table_clicked),
             ("Clock", clock_clicked),
             ("Layout", layout_clicked),
             ("Hover", hover_clicked),
             ("Hover 2", hover2_clicked),
             ("Entry", entry_clicked),
             ("Entry Scrolled", entry_scrolled_clicked),
             ("Notepad", notepad_clicked),
             ("Anchorview", anchorview_clicked),
             ("Anchorblock", anchorblock_clicked),
             ("Toolbar", toolbar_clicked),
             ("Hoversel", hoversel_clicked),
             ("List", list_clicked),
             ("List 2", list2_clicked),
             ("List 3", list3_clicked),
             ("InnerWindow", inner_window_clicked),
             ("Genlist", genlist_clicked),
             ("Checks", check_clicked),
             ("Radios", radio_clicked),
             ("Pager", pager_clicked),
             ("Progressbar", progressbar_clicked),
             ("File selector", fileselector_clicked),
             ("Separator", separator_clicked),
             ("Scroller", scroller_clicked),
             ("Spinner", spinner_clicked),
             ("Notify", notify_clicked),
             ("Menu", menu_clicked),
             ("Panel", panel_clicked)]



    li = elementary.List(win)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    box0.pack_end(li)
    li.show()

    for item in items:
        li.item_append(item[0], callback=item[1])

    li.go()

    win.resize(320,520)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
