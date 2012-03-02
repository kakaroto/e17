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
from test_table import table_clicked
from test_clock import clock_clicked
from test_hover import hover_clicked, hover2_clicked
from test_entry import entry_clicked
from test_anchorview import anchorview_clicked
from test_anchorblock import anchorblock_clicked
from test_toolbar import toolbar_clicked
from test_hoversel import hoversel_clicked
from test_list import list_clicked, list2_clicked, list3_clicked
from test_inwin import inner_window_clicked
from test_check import check_clicked
from test_radio import radio_clicked
from test_progressbar import progressbar_clicked
from test_fileselector import fileselector_clicked
from test_separator import separator_clicked
from test_scroller import scroller_clicked
from test_notify import notify_clicked
from test_menu import menu_clicked
from test_panel import panel_clicked
from test_layout import layout_clicked
from test_spinner import spinner_clicked
from test_genlist import genlist_clicked, genlist2_clicked, genlist3_clicked
from test_gengrid import gengrid_clicked
from test_flip import flip_clicked
from test_tooltip import tooltip_clicked
from test_cursor import cursor_clicked, cursor2_clicked, cursor3_clicked

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
    fr.text_set("Information")
    box0.pack_end(fr)
    fr.show()

    lb = elementary.Label(win)
    lb.text_set("Please select a test from the list below<br>"
                 "by clicking the test button to show the<br>"
                 "test window.")
    fr.content_set(lb)
    lb.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.bounce_set(False, True)
    sc.show()
    box0.pack_end(sc)

    tbx = elementary.Box(win)
    tbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    tbx.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    sc.content_set(tbx)
    tbx.show()

    items = [("Booleans", [
                ("Checks", check_clicked),
                #("Toggles", toggles_clicked),TODO make a toggle with check
            ]),
             ("Boundaries", [
                ("Separator", separator_clicked),
            ]),
             ("Buttons", [
                ("Buttons", buttons_clicked),
            ]),
             ("Containers", [
                ("Box Horiz", box_horiz_clicked),
                ("Box Vert", box_vert_clicked),
                ("Box Vert2", box_vert2_clicked),
                ("Table", table_clicked),
                ("Layout", layout_clicked),
            ]),
             ("Cursors", [
                ("Cursor", cursor_clicked),
                ("Cursor2", cursor2_clicked),
                ("Cursor3", cursor3_clicked),
            ]),
             ("Dividers", [
                ("Panel", panel_clicked),
            ]),
             ("Effects", [
                ("Flip", flip_clicked),
            ]),
             ("Entries", [
                ("Entry", entry_clicked),
#                ("Entry Scrolled", entry_scrolled_clicked),
            ]),
             ("Lists", [
                ("List", list_clicked),
                ("List 2", list2_clicked),
                ("List 3", list3_clicked),
                ("Genlist", genlist_clicked),
                ("Genlist 2", genlist2_clicked),
                ("Genlist Group", genlist3_clicked),
                ("Gengrid", gengrid_clicked),
            ]),
             ("Popups", [
                ("Hover", hover_clicked),
                ("Hover 2", hover2_clicked),
                ("Notify", notify_clicked),
                ("Tooltip", tooltip_clicked),
            ]),
             ("Range Values", [
                ("Spinner", spinner_clicked),
                ("Progressbar", progressbar_clicked),
            ]),
             ("Scroller", [
                ("Scroller", scroller_clicked),
            ]),
             ("Selectors", [
                ("File selector", fileselector_clicked),
                ("Hoversel", hoversel_clicked),
                ("Menu", menu_clicked),
                ("Radios", radio_clicked),
            ]),
#             ("Storyboard", [ TODO
#                ("Naviframe", pager_clicked),
#            ]),
             ("Text", [
                ("Anchorview", anchorview_clicked),
                ("Anchorblock", anchorblock_clicked),
            ]),
             ("Times & Dates", [
                ("Clock", clock_clicked),
            ]),
            ("Toolbars", [
                ("Toolbar", toolbar_clicked),
            ]),
             ("Window / Backgroud", [
                ("Bg Plain", bg_plain_clicked),
                ("Bg Image", bg_image_clicked),
                ("InnerWindow", inner_window_clicked),
                ("IconTransparent", icon_transparent_clicked),
            ])
            ]

    for category in items:
        frame = elementary.Frame(win)
        frame.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        frame.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        frame.text = category[0]
        frame.show()
        tbx.pack_end(frame)

        tbx2 = elementary.Box(win)
        tbx2.horizontal_set(True)
        tbx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        tbx2.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        frame.content_set(tbx2)
        tbx2.show()

        for test in category[1]:
            bt = elementary.Button(win)
            bt.text = test[0]
            bt.callback_clicked_add(test[1])
            bt.show()
            tbx2.pack_end(bt)

    win.resize(320,520)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
