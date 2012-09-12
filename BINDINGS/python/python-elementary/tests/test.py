#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

from test_3d import evas3d_clicked
from test_actionslider import actionslider_clicked
from test_bg import bg_plain_clicked, bg_image_clicked
from test_box import box_vert_clicked, box_vert2_clicked, box_horiz_clicked
from test_box import box_layout_clicked, box_transition_clicked
from test_bubble import bubble_clicked
from test_button import buttons_clicked
from test_calendar import calendar_clicked
from test_check import check_clicked
from test_clock import clock_clicked
from test_colorselector import colorselector_clicked
from test_cursor import cursor_clicked, cursor2_clicked, cursor3_clicked
from test_ctxpopup import ctxpopup_clicked
from test_datetime import datetime_clicked
from test_dayselector import dayselector_clicked
from test_diskselector import diskselector_clicked
from test_entry import entry_clicked, entry_scrolled_clicked
from test_external import edje_external_button_clicked, edje_external_pbar_clicked, \
    edje_external_scroller_clicked, edje_external_slider_clicked, edje_external_video_clicked
from test_fileselector import fileselector_clicked, fileselector_button_clicked, fileselector_entry_clicked
from test_flip import flip_clicked, flip_interactive_clicked
from test_flipselector import flipselector_clicked
from test_frame import frame_clicked
from test_gengrid import gengrid_clicked
from test_genlist import genlist_clicked, genlist2_clicked, genlist3_clicked
from test_grid import grid_clicked
from test_hover import hover_clicked, hover2_clicked
from test_hoversel import hoversel_clicked
from test_icon import icon_clicked, icon_transparent_clicked
from test_index import index_clicked
from test_inwin import inner_window_clicked
from test_label import label_clicked
from test_layout import layout_clicked
from test_list import list_clicked, list2_clicked, list3_clicked
from test_mapbuf import mapbuf_clicked
from test_map import map_clicked
from test_map2 import map_overlays_clicked
from test_map3 import map_route_clicked
from test_menu import menu_clicked
from test_multibuttonentry import multibuttonentry_clicked
from test_naviframe import naviframe_clicked
from test_notify import notify_clicked
from test_panel import panel_clicked
from test_panes import panes_clicked
from test_photo import photo_clicked
from test_popup import popup_clicked
from test_progressbar import progressbar_clicked
from test_radio import radio_clicked
from test_scroller import scroller_clicked
from test_segment_control import segment_control_clicked
from test_separator import separator_clicked
from test_slideshow import slideshow_clicked
from test_spinner import spinner_clicked
from test_table import table_clicked
from test_thumb import thumb_clicked
from test_toolbar import toolbar_clicked
from test_tooltip import tooltip_clicked
from test_video import video_clicked
from test_web import web_clicked
from test_win import window_states_clicked


items = [("3D", [
            ("Evas Map 3D", evas3d_clicked),
        ]),
         ("Booleans", [
            ("Checks", check_clicked),
            #("Toggles", toggles_clicked),TODO make a toggle with check
        ]),
         ("Boundaries", [
            ("Bubble", bubble_clicked),
            ("Separator", separator_clicked),
        ]),
         ("Buttons", [
            ("Buttons", buttons_clicked),
        ]),
         ("Containers", [
            ("Box Horiz", box_horiz_clicked),
            ("Box Vert", box_vert_clicked),
            ("Box Vert2", box_vert2_clicked),
            ("Box Layout", box_layout_clicked),
            ("Box Layout Transition", box_transition_clicked),
            ("Table", table_clicked),
            ("Layout", layout_clicked),
            ("Grid", grid_clicked),
            ("Frame", frame_clicked),
        ]),
         ("Cursors", [
            ("Cursor", cursor_clicked),
            ("Cursor2", cursor2_clicked),
            ("Cursor3", cursor3_clicked),
        ]),
         ("Dividers", [
            ("Panel", panel_clicked),
            ("Panes", panes_clicked),
        ]),
         ("Effects", [
            ("Flip", flip_clicked),
            ("Flip Interactive", flip_interactive_clicked),
        ]),
         ("Entries", [
            ("Entry", entry_clicked),
            ("Entry Scrolled", entry_scrolled_clicked),
            ("MultiButtonEntry", multibuttonentry_clicked),
        ]),
         ("Edje External", [
            ("Ext Button", edje_external_button_clicked),
            ("Ext ProgressBar", edje_external_pbar_clicked),
            ("Ext Scroller", edje_external_scroller_clicked),
            ("Ext Slider", edje_external_slider_clicked),
            ("Ext Video", edje_external_video_clicked),
        ]),
         ("Geographic", [
            ("Map", map_clicked),
            ("Map Overlay", map_overlays_clicked),
            ("Map Route", map_route_clicked),
        ]),
         ("Images", [
            ("Icon", icon_clicked),
            ("Icon Transparent", icon_transparent_clicked),
            ("Photo", photo_clicked),
            ("Slideshow", slideshow_clicked),
            ("Thumb", thumb_clicked),
            ("Video", video_clicked),
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
         ("Naviframe", [
            ("Naviframe", naviframe_clicked),
        ]),
         ("Popups", [
            ("Hover", hover_clicked),
            ("Hover 2", hover2_clicked),
            ("Notify", notify_clicked),
            ("Tooltip", tooltip_clicked),
            ("Ctxpopup", ctxpopup_clicked),
            ("Popup", popup_clicked),
        ]),
         ("Range Values", [
            ("Spinner", spinner_clicked),
            ("Progressbar", progressbar_clicked),
        ]),
         ("Scroller", [
            ("Scroller", scroller_clicked),
        ]),
         ("Selectors", [
            ("Action Slider", actionslider_clicked),
            ("Color Selector", colorselector_clicked),
            ("Day Selector", dayselector_clicked),
            ("Disk Selector", diskselector_clicked),
            ("Flip Selector", flipselector_clicked),
            ("FS button", fileselector_button_clicked),
            ("FS entry", fileselector_entry_clicked),
            ("File Selector", fileselector_clicked),
            ("Hoversel", hoversel_clicked),
            ("Index", index_clicked),
            ("Menu", menu_clicked),
            ("Radios", radio_clicked),
            ("Segment Control", segment_control_clicked),
        ]),
         ("Stored Surface Buffer", [
            ("Launcher", mapbuf_clicked),
        ]),
         ("Text", [
            ("Label", label_clicked),
        ]),
         ("Times & Dates", [
            ("Calendar", calendar_clicked),
            ("Clock", clock_clicked),
            ("Datetime", datetime_clicked),
        ]),
        ("Toolbars", [
            ("Toolbar", toolbar_clicked),
        ]),
         ("Web", [
            ("Web", web_clicked),
        ]),
         ("Window / Background", [
            ("Bg Plain", bg_plain_clicked),
            ("Bg Image", bg_image_clicked),
            ("InnerWindow", inner_window_clicked),
            ("Window States", window_states_clicked),
        ])
        ]

#----- Main -{{{-
def menu_create(search, win):
    tbx.clear()
    for category in items:
        frame = elementary.Frame(win)
        frame.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
        frame.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
        frame.text = category[0]
        frame.show()
        tbx.pack_end(frame)

        tbx2 = elementary.Box(win)
        tbx2.layout_set(elementary.ELM_BOX_LAYOUT_FLOW_HORIZONTAL)
        tbx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
        tbx2.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
        frame.content_set(tbx2)
        tbx2.show()

        cnt = 0
        for test in category[1]:
            if (search == None) or (test[0].lower().find(search.lower()) > -1):
                bt = elementary.Button(win)
                bt.text = test[0]
                bt.callback_clicked_add(test[1])
                bt.show()
                tbx2.pack_end(bt)
                cnt += 1

        if cnt < 1:
            frame.delete()

def destroy(obj, str1, str2, str3, str4):
    print("DEBUG: window destroy callback called!")
    print(("DEBUG: str1='%s', str2='%s', str3='%s', str4='%s'" %(str1, str2,
                                                                str3, str4)))
    elementary.exit()

def cb_mirroring(toggle):
    elementary.Configuration().mirrored = toggle.state

def cb_filter(en, win):
    menu_create(en.text_get(), win)

if __name__ == "__main__":
    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.callback_delete_request_add(destroy, "test1", "test2", str3="test3", str4="test4")

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

    tg = elementary.Check(win)
    tg.style = "toggle"
    tg.text = "UI-Mirroring:"
    tg.callback_changed_add(cb_mirroring)
    box0.pack_end(tg)
    tg.show()

    bx1 = elementary.Box(win)
    bx1.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx1.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    bx1.horizontal_set(True)
    box0.pack_end(bx1)
    bx1.show()

    lb = elementary.Label(win)
    lb.text_set("Filter:")
    bx1.pack_end(lb)
    lb.show()

    en = elementary.Entry(win)
    en.single_line_set(True)
    en.scrollable_set(True)
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    en.callback_changed_add(cb_filter, win)
    bx1.pack_end(en)
    en.show()
    en.focus_set(True)

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

    menu_create(None, win)

    win.resize(320, 480)
    win.show()
    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
