#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- BG Plain -{{{-
def bg_plain_clicked(obj, it):
    win = elementary.Window("bg plain", elementary.ELM_WIN_BASIC)
    win.title_set("Bg Plain")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- BG Image -{{{-
def bg_image_clicked(obj, it):
    win = elementary.Window("bg-image", elementary.ELM_WIN_BASIC)
    win.title_set("Bg Image")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.file_set("images/plant_01.jpg")
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    win.size_hint_min_set(160, 160)
    win.size_hint_max_set(320, 320)
    win.resize(320, 320)
    win.show()
# }}}

#----- Icon Transparent -{{{-
def icon_transparent_clicked(obj, it):
    win = elementary.Window("icon-transparent", elementary.ELM_WIN_BASIC)
    win.title_set("Icon Transparent")
    win.autodel_set(True)
    win.alpha_set(True)

    icon = elementary.Icon(win)
    icon.file_set("images/logo.png")
    icon.scale_set(0, 0)
    win.resize_object_add(icon)
    icon.show()

    win.show()
# }}}

#----- Box Vert -{{{-
def box_vert_clicked(obj, it):
    win = elementary.Window("box-vert", elementary.ELM_WIN_BASIC)
    win.title_set("Box Vert")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(evas.EVAS_HINT_EXPAND, 0.5)
    bx.pack_end(ic)
    ic.show()

    win.show()
# }}}

#----- Box Horiz -{{{-
def box_horiz_clicked(obj, it):
    win = elementary.Window("box-horiz", elementary.ELM_WIN_BASIC)
    win.title_set("Box Horiz")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.horizontal_set(True)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()

    win.show()
# }}}

#----- Buttons -{{{-
def buttons_clicked(obj, it):
    win = elementary.Window("buttons", elementary.ELM_WIN_BASIC)
    win.title_set("Buttons")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    bt = elementary.Button(win)
    bt.label_set("Icon sized to button")
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bt = elementary.Button(win)
    bt.label_set("Icon no scale")
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Button only")
    bx.pack_end(bt)
    bt.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bt = elementary.Button(win)
    bt.icon_set(ic)
    bx.pack_end(bt)
    bt.show()
    ic.show()

    win.show()
# }}}

#----- Toggles -{{{-
def toggles_clicked(obj, it):
    win = elementary.Window("toggles", elementary.ELM_WIN_BASIC)
    win.title_set("Toggles")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    tg = elementary.Toggle(win)
    tg.label_set("Icon sized to toggle")
    tg.icon_set(ic)
    tg.state_set(True)
    tg.states_labels_set("Yes", "No")
    bx.pack_end(tg)
    tg.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    tg = elementary.Toggle(win)
    tg.label_set("Icon no scale")
    tg.icon_set(ic)
    bx.pack_end(tg)
    tg.show()
    ic.show()

    tg = elementary.Toggle(win)
    tg.label_set("Label Only")
    tg.states_labels_set("Big", "Small")
    bx.pack_end(tg)
    tg.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    tg = elementary.Toggle(win)
    tg.icon_set(ic)
    bx.pack_end(tg)
    tg.show()
    ic.show()

    win.show()
# }}}

#----- Table -{{{-
def table_clicked(obj, it):
    win = elementary.Window("table", elementary.ELM_WIN_BASIC)
    win.title_set("Table")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    tb = elementary.Table(win)
    win.resize_object_add(tb)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    tb.show()

    bt = elementary.Button(win)
    bt.label_set("Button 1")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 1, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 2, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 4")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 1, 2, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 5")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 2, 1, 1, 3)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 6")
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    tb.pack(bt, 0, 2, 2, 2)
    bt.show()

    win.show()
# }}}

#----- Clock -{{{-
def clock_clicked(obj, it):
    win = elementary.Window("clock", elementary.ELM_WIN_BASIC)
    win.title_set("Clock")
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
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.show_am_pm_set(True)
    bx.pack_end(ck)
    ck.show()

    print ck.time_get()

    ck = elementary.Clock(win)
    ck.show_seconds_set(True)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.show_seconds_set(True)
    ck.show_am_pm_set(True)
    bx.pack_end(ck)
    ck.show()

    ck = elementary.Clock(win)
    ck.edit_set(True)
    ck.show_seconds_set(True)
    ck.show_am_pm_set(True)
    ck.time_set(10, 11, 12)
    bx.pack_end(ck)
    ck.show()

    win.show()
# }}}

#----- Layout -{{{-
def layout_clicked(obj, it):
    win = elementary.Window("layout", elementary.ELM_WIN_BASIC)
    win.title_set("Layout")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(ly)
    ly.show()

    bt = elementary.Button(win)
    bt.label_set("Button 1")
    ly.content_set("element1", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    ly.content_set("element2", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    ly.content_set("element3", bt)
    bt.show()

    win.show()
# }}}

#----- Hover -{{{-
def hover_bt1_clicked(bt, hv):
    hv.show()

def hover_clicked(obj, it):
    win = elementary.Window("hover", elementary.ELM_WIN_BASIC)
    win.title_set("Hover")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    hv = elementary.Hover(win)

    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.callback_clicked_add(hover_bt1_clicked, hv)
    bx.pack_end(bt)
    bt.show()
    hv.parent_set(win)
    hv.target_set(bt)

    bt = elementary.Button(win)
    bt.label_set("Popup")
    hv.content_set("middle", bt)
    bt.show()

    bx = elementary.Box(win)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bx.pack_end(ic)
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Top 1")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 2")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 3")
    bx.pack_end(bt)
    bt.show()

    bx.show()

    hv.content_set("top", bx)

    bt = elementary.Button(win)
    bt.label_set("Bottom")
    hv.content_set("bottom", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Left")
    hv.content_set("left", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Right")
    hv.content_set("right", bt)
    bt.show()

    bg.size_hint_min_set(160, 160)
    bg.size_hint_max_set(640, 640)

    win.resize(320, 320)
    win.show()
# }}}

#----- Hover2 -{{{-
def hover2_clicked(obj, it):
    win = elementary.Window("hover2", elementary.ELM_WIN_BASIC)
    win.title_set("Hover 2")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    hv = elementary.Hover(win)
    hv.style_set("popout")

    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.callback_clicked_add(hover_bt1_clicked, hv)
    bx.pack_end(bt)
    bt.show()
    hv.parent_set(win)
    hv.target_set(bt)

    bt = elementary.Button(win)
    bt.label_set("Popup")
    hv.content_set("middle", bt)
    bt.show()

    bx = elementary.Box(win)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    bx.pack_end(ic)
    ic.show()

    bt = elementary.Button(win)
    bt.label_set("Top 1")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 2")
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top 3")
    bx.pack_end(bt)
    bt.show()

    bx.show()
    hv.content_set("top", bx)

    bt = elementary.Button(win)
    bt.label_set("Bot")
    hv.content_set("bottom", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Left")
    hv.content_set("left", bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Right")
    hv.content_set("right", bt)
    bt.show()

    bg.size_hint_min_set(160, 160)
    bg.size_hint_max_set(640, 640)
    win.resize(320, 320)
    win.show()
# }}}

#----- Entry -{{{-
def my_entry_bt_1(bt, en):
    en.entry_set("")

def my_entry_bt_2(bt, en):
    str = en.entry_get()
    print "ENTRY: %s" % str

def my_entry_bt_3(bt, en):
    str = en.selection_get()
    print "SELECTION: %s" % str

def my_entry_bt_4(bt, en):
    en.entry_insert("Insert some <b>BOLD</> text")

def my_entry_anchor_test(obj, en, *args, **kwargs):
    en.entry_insert("ANCHOR CLICKED")

def entry_clicked(obj, it):
    win = elementary.Window("entry", elementary.ELM_WIN_BASIC)
    win.title_set("Entry")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    en = elementary.Entry(win)
    en.line_wrap_set(False)
    en.entry_set("This is an entry widget in this window that<br>"
                 "uses markup <b>like this</> for styling and<br>"
                 "formatting <em>like this</>, as well as<br>"
                 "<a href=X><link>links in the text</></a>, so enter text<br>"
                 "in here to edit it. By the way, links are<br>"
                 "called <a href=anc-02>Anchors</a> so you will need<br>"
                 "to refer to them this way.")
    en.callback_anchor_clicked_add(my_entry_anchor_test, en)
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(en)
    en.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.callback_clicked_add(my_entry_bt_1, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Print")
    bt.callback_clicked_add(my_entry_bt_2, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Selection")
    bt.callback_clicked_add(my_entry_bt_3, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Insert")
    bt.callback_clicked_add(my_entry_bt_4, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    en.focus()
    win.show()
# }}}

#----- Entry Scrolled -{{{-
def entry_scrolled_clicked(obj, it):
    win = elementary.Window("entry", elementary.ELM_WIN_BASIC)
    win.title_set("Entry Scrolled")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.single_line_set(True)
    en.entry_set("Disable entry")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    en.disabled_set(True)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.password_set(True)
    en.entry_set("Access denied, give up!")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    en.disabled_set(True)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_ON,
                  elementary.ELM_SCROLLER_POLICY_ON)
    sc.bounce_set(False, True)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    #TODO
    #en.context_menu_item_add("Hello", None, elementary.ELM_ICON_NONE, None, None)
    #en.context_menu_item_add("World", None, elementary.ELM_ICON_NONE, None, None)
    en.entry_set(
               "Multi-line disabled entry widget :)<br>"
               "We can use markup <b>like this</> for styling and<br>"
               "formatting <em>like this</>, as well as<br>"
               "<a href=X><link>links in the text</></a>, but it won't be editable or clickable.")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    en.disabled_set(True)
    sc.content_set(en)
    en.show()

    sc.show()

    sp = elementary.Separator(win)
    sp.horizontal_set(True)
    bx.pack_end(sp)
    sp.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.single_line_set(True)
    en.entry_set("This is a single line")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    en.select_all()
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.password_set(True)
    en.entry_set("Password here")
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    sc.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_ON,
                  elementary.ELM_SCROLLER_POLICY_ON)
    sc.bounce_set(False, True)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.entry_set("This is an entry widget in this window that<br>"
                "uses markup <b>like this</> for styling and<br>"
                "formatting <em>like this</>, as well as<br>"
                "<a href=X><link>links in the text</></a>, so enter text<br>"
                "in here to edit it. By the way, links are<br>"
                "called <a href=anc-02>Anchors</a> so you will need<br>"
                "to refer to them this way. At the end here is a really long line to test line wrapping to see if it works. But just in case this line is not long enough I will add more here to really test it out, as Elementary really needs some good testing to see if entry widgets work as advertised.")
    en.callback_anchor_clicked_add(my_entry_anchor_test, en)
    en.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    en.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    sc.content_set(en)
    en.show()

    sc.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.callback_clicked_add(my_entry_bt_1, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Print")
    bt.callback_clicked_add(my_entry_bt_2, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Selection")
    bt.callback_clicked_add(my_entry_bt_3, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Insert")
    bt.callback_clicked_add(my_entry_bt_4, en)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    win.resize(320, 300)

    win.focus()
    win.show()
# }}}

#----- Notepad -{{{-
def notepad_clicked(obj, it):
    win = elementary.Window("notepad", elementary.ELM_WIN_BASIC)
    win.title_set("Notepad")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    l = elementary.Label(win)
    l.label_set("TODO")
    bx.pack_end(l)
    l.show()

    win.resize(320, 300)
    win.show()
# }}}

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

#----- Toolbar -{{{-
def tb_1(obj, it):
    print "toolbar-item: test 1"

def tb_2(obj, it):
    print "toolbar-item: test 2"

def tb_3(obj, it):
    print "toolbar-item: test 3"

def tb_4(obj, it):
    print "toolbar-item: test 4"

def tb_5(obj, it):
    print "toolbar-item: test 5"

def toolbar_clicked(obj, it):
    win = elementary.Window("entry-scrolled", elementary.ELM_WIN_BASIC)
    win.title_set("Entry Scrolled")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    tb = elementary.Toolbar(win)
    tb.size_hint_weight_set(0.0, 0.0)
    tb.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    item = tb.item_add(ic, "Hello", tb_1)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    item = tb.item_add(ic, "World,", tb_2)
    item.select()

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "here", tb_3)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "comes", tb_4)

    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    tb.item_add(ic, "python-elementary!", tb_5)

    bx.pack_end(tb)
    tb.show()

    win.resize(320, 320)
    win.show()
# }}}

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

#----- List -{{{-
def my_list_show_it(obj, it):
    it.show()

def list_clicked(obj, it):
    win = elementary.Window("list", elementary.ELM_WIN_BASIC)
    win.title_set("List")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    win.resize_object_add(li)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(1, 1)
    it1 = li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    li.item_append(".", ic)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)
    print it2.base_get()

    bx = elementary.Box(win)
    bx.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()
    li.item_append("are")

    li.item_append("you")
    it3 = li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    it4 = li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Is this label long enough?")
    it5 = li.item_append("Maybe this one is even longer so we can test long long items.")

    li.go()

    li.show()

    tb2 = elementary.Table(win)
    tb2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(tb2)

    bt = elementary.Button(win)
    bt.label_set("Hello")
    bt.callback_clicked_add(my_list_show_it, it1)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 0, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("How")
    bt.callback_clicked_add(my_list_show_it, it2)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 1, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("doing")
    bt.callback_clicked_add(my_list_show_it, it3)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 2, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Here")
    bt.callback_clicked_add(my_list_show_it, it4)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 3, 1, 1);
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Maybe this...")
    bt.callback_clicked_add(my_list_show_it, it5)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.5)
    tb2.pack(bt, 0, 4, 1, 1);
    bt.show()

    tb2.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- List2 -{{{-
def my_list2_clear(bt, li):
    li.clear()

def my_list2_sel(obj, it):
    it = obj.selected_item_get()
    if it is not None:
        it.selected_set(False)

def list2_clicked(obj, it):
    win = elementary.Window("list-2", elementary.ELM_WIN_BASIC)
    win.title_set("List 2")
    win.autodel_set(True)

    bg = elementary.Background(win)
    bg.file_set('images/plant_01.jpg')
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    li = elementary.List(win)
    li.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.horizontal_mode_set(elementary.ELM_LIST_LIMIT)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    it = li.item_append("Hello", ic, callback=my_list2_sel)
    it.selected_set(True)
    ic = elementary.Icon(win)
    ic.scale_set(0, 0)
    ic.file_set('images/logo_small.png')
    li.item_append("world", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    li.item_append(".", ic)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx2.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx2.pack_end(ic)
    ic.show()

    li.item_append("are", bx2)

    li.item_append("you")
    li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Longer label.")

    li.go()

    bx.pack_end(li)
    li.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.homogenous_set(True)
    bx2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.callback_clicked_add(my_list2_clear, li)
    bt.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- List3 -{{{-
def list3_clicked(obj, it):
    win = elementary.Window("list-3", elementary.ELM_WIN_BASIC)
    win.title_set("List 3")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    win.resize_object_add(li)
    li.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.horizontal_mode_set(elementary.ELM_LIST_COMPRESS)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    li.item_append("Hello", ic)
    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    li.item_append("world", ic)
    ic = elementary.Icon(win)
    ic.standard_set("edit")
    ic.scale_set(0, 0)
    li.item_append(".", ic)

    ic = elementary.Icon(win)
    ic.standard_set("delete")
    ic.scale_set(0, 0)
    ic2 = elementary.Icon(win)
    ic2.standard_set("clock")
    ic2.scale_set(0, 0)
    it2 = li.item_append("How", ic, ic2)

    bx = elementary.Box(win)
    bx.horizontal_set(True)

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.0)
    bx.pack_end(ic)
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.0, evas.EVAS_HINT_EXPAND)
    bx.pack_end(ic)
    ic.show()

    li.item_append("are", bx)
    li.item_append("you")
    li.item_append("doing")
    li.item_append("out")
    li.item_append("there")
    li.item_append("today")
    li.item_append("?")
    li.item_append("Here")
    li.item_append("are")
    li.item_append("some")
    li.item_append("more")
    li.item_append("items")
    li.item_append("Is this label long enough?")
    it5 = li.item_append("Maybe this one is even longer so we can test long long items.")

    li.go()

    li.show()

    win.resize(320, 300)
    win.show()
# }}}

#----- Inner Window -{{{-
def inner_window_clicked(obj, it):
    win = elementary.Window("inner-window", elementary.ELM_WIN_BASIC)
    win.title_set("InnerWindow test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    iw = elementary.InnerWindow(win)
    iw.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Check -{{{-
def ck_1(obj):
    print "test check 1"

def ck_2(obj):
    print "test check 2"

def ck_never(obj):
    print "disabled check changed (should never happen unless you enable or set it)"

def ck_3(obj):
    print "test check 3"

def ck_4(obj):
    print "test check 4"

def check_clicked(obj, it):
    win = elementary.Window("check", elementary.ELM_WIN_BASIC)
    win.title_set("Check test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ck.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    ck.label_set("Icon sized to check")
    ck.icon_set(ic)
    ck.state_set(True)
    ck.callback_changed_add(ck_1)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ck = elementary.Check(win)
    ck.label_set("Icon no scale")
    ck.icon_set(ic)
    ck.callback_changed_add(ck_2)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ck = elementary.Check(win)
    ck.label_set("Label Only")
    ck.callback_changed_add(ck_3)
    bx.pack_end(ck)
    ck.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ck.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    ck.label_set("Disabled check")
    ck.icon_set(ic)
    ck.state_set(True)
    ck.callback_changed_add(ck_never)
    bx.pack_end(ck)
    ck.disabled_set(True)
    ck.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ic.scale_set(0, 0)
    ck = elementary.Check(win)
    ck.icon_set(ic)
    ck.callback_changed_add(ck_4)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    win.show()
# }}}

#----- Radio -{{{-
def radio_clicked(obj, it):
    win = elementary.Window("radio", elementary.ELM_WIN_BASIC)
    win.title_set("Radio test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    rd = elementary.Radio(win)
    rd.state_value_set(0)
    rd.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    rd.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    rd.label_set("Icon sized to radio")
    rd.icon_set(ic)
    bx.pack_end(rd)
    rd.show()
    ic.show()
    rdg = rd

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    rd = elementary.Radio(win)
    rd.state_value_set(1)
    rd.group_add(rdg)
    rd.label_set("Icon no scale")
    rd.icon_set(ic)
    bx.pack_end(rd)
    rd.show()
    ic.show()

    rd = elementary.Radio(win)
    rd.state_value_set(2)
    rd.group_add(rdg)
    rd.label_set("Label Only")
    bx.pack_end(rd)
    rd.show()

    rd = elementary.Radio(win)
    rd.state_value_set(3)
    rd.group_add(rdg)
    rd.label_set("Disabled")
    rd.disabled_set(True)
    bx.pack_end(rd)
    rd.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    rd = elementary.Radio(win)
    rd.state_value_set(4)
    rd.group_add(rdg)
    rd.icon_set(ic)
    bx.pack_end(rd)
    rd.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    rd = elementary.Radio(win)
    rd.state_value_set(5)
    rd.group_add(rdg)
    rd.icon_set(ic)
    rd.disabled_set(True)
    bx.pack_end(rd)
    rd.show()
    ic.show()

    rdg.value_set(2)

    win.show()
# }}}

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

# ----- Progressbar {{{
my_progressbar_run = False
my_progressbar_timer = None

def my_progressbar_value_set(pb1, pb2, pb3, pb4, pb5, pb6, pb7):
    progress = pb1.value_get()
    if progress < 1.0:
        progress += 0.0123
    else:
        progress = 0.0
    pb1.value_set(progress)
    pb4.value_set(progress)
    pb3.value_set(progress)
    pb6.value_set(progress)
    if progress < 1.0:
        return ecore.ECORE_CALLBACK_RENEW
    global my_progressbar_run
    my_progressbar_run = False
    return ecore.ECORE_CALLBACK_CANCEL

def my_progressbar_test_start(obj, *args, **kwargs):
    (pb1, pb2, pb3, pb4, pb5, pb6, pb7) = args
    pb2.pulse(True)
    pb5.pulse(True)
    pb7.pulse(True)
    global my_progressbar_run
    global my_progressbar_timer
    if not my_progressbar_run:
        my_progressbar_timer = ecore.timer_add(0.1, my_progressbar_value_set,
                                               *args)
        my_progressbar_run = True

def my_progressbar_test_stop(obj, pb1, pb2, pb3, pb4, pb5, pb6, pb7):
    pb2.pulse(False)
    pb5.pulse(False)
    pb7.pulse(False)
    global my_progressbar_run
    global my_progressbar_timer
    if my_progressbar_run:
        my_progressbar_timer.delete()
        my_progressbar_run = False

def my_progressbar_destroy(obj, *args):
    my_progressbar_test_stop(None, *args)
    obj.delete()

def progressbar_clicked(obj, it):
    win = elementary.Window("progressbar", elementary.ELM_WIN_BASIC)
    win.title_set("Progressbar test")

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.show()

    pb1 = elementary.Progressbar(win)
    pb1.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb1.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    bx.pack_end(pb1)
    pb1.show()

    pb2 = elementary.Progressbar(win)
    pb2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb2.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb2.label_set("Infinite bounce")
    pb2.pulse_set(True)
    bx.pack_end(pb2)
    pb2.show()

    ic1 = elementary.Icon(win)
    ic1.file_set('images/logo_small.png')
    ic1.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)

    pb3 = elementary.Progressbar(win)
    pb3.label_set("Label")
    pb3.icon_set(ic1)
    pb3.inverted_set(True)
    pb3.unit_format_set("%1.1f units")
    pb3.span_size_set(200)
    pb3.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb3.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(pb3)
    ic1.show()
    pb3.show()

    hbx = elementary.Box(win)
    hbx.horizontal_set(True)
    hbx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    bx.pack_end(hbx)
    hbx.show()

    pb4 = elementary.Progressbar(win)
    pb4.horizontal_set(False)
    pb4.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    pb4.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.pack_end(pb4)
    pb4.span_size_set(60)
    pb4.label_set("percent")
    pb4.show()

    pb5 = elementary.Progressbar(win)
    pb5.horizontal_set(False)
    pb5.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb5.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    pb5.span_size_set(80)
    pb5.pulse_set(True)
    pb5.unit_format_set(None)
    pb5.label_set("Infinite bounce")
    hbx.pack_end(pb5)
    pb5.show()

    ic2 = elementary.Icon(win)
    ic2.file_set('images/logo_small.png')
    ic2.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_HORIZONTAL, 1, 1)

    pb6 = elementary.Progressbar(win)
    pb6.horizontal_set(False)
    pb6.label_set("Label")
    pb6.icon_set(ic2)
    pb6.inverted_set(True)
    pb6.unit_format_set("%1.2f%%")
    pb6.span_size_set(200)
    pb6.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb6.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    hbx.pack_end(pb6)
    ic2.show()
    pb6.show()

    pb7 = elementary.Progressbar(win)
    pb7.style_set("wheel")
    pb7.label_set("Style: wheel")
    pb7.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    pb7.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(pb7)
    pb7.show()

    bt_bx = elementary.Box(win)
    bt_bx.horizontal_set(True)
    bt_bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(bt_bx)
    bt_bx.show()

    pbt = (pb1, pb2, pb3, pb4, pb5, pb6, pb7)

    bt = elementary.Button(win)
    bt.label_set("Start")
    bt.callback_clicked_add(my_progressbar_test_start, *pbt)
    bt_bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Stop")
    bt.callback_clicked_add(my_progressbar_test_stop, *pbt)
    bt_bx.pack_end(bt)
    bt.show()

    win.callback_destroy_add(my_progressbar_destroy, *pbt)
    win.show()
# }}}

#----- FileSelector -{{{-
def fileselector_clicked(obj, it):
    win = elementary.Window("fileselector", elementary.ELM_WIN_BASIC)
    win.title_set("File selector test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    vbox = elementary.Box(win)
    win.resize_object_add(vbox)
    vbox.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    vbox.show()

    fs = elementary.Fileselector(win)
    fs.is_save_set(True)
    fs.expandable_set(False)
    fs.path_set(os.getenv("HOME"))
    fs.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    fs.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    vbox.pack_end(fs)
    fs.show()

    def fs_cb_done(fs, selected, win):
        win.delete()
    fs.callback_done_add(fs_cb_done, win)
    def fs_cb_selected(fs, selected, win):
        print "Selected file:", selected
        print "or:", fs.selected_get()
    fs.callback_selected_add(fs_cb_selected, win)

    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.label_set("Toggle is_save")
    def bt_cb_is_save(bt, fs):
        print "Toggle is save"
        fs.is_save_set(not fs.is_save_get())
    bt.callback_clicked_add(bt_cb_is_save, fs)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("sel get")
    def bt_cb_sel_get(bt, fs):
        print "Get Selected:", fs.selected_get()
    bt.callback_clicked_add(bt_cb_sel_get, fs)
    hbox.pack_end(bt)
    bt.show()

    win.resize(240, 350)
    win.show()
# }}}

#----- Separator -{{{-
def separator_clicked(obj, it):
    win = elementary.Window("separators", elementary.ELM_WIN_BASIC)
    win.title_set("Separators")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx0 = elementary.Box(win)
    bx0.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.horizontal_set(True)
    win.resize_object_add(bx0)
    bx0.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.pack_end(bx)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("Left upper corner")
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    sp.horizontal_set(True)
    bx.pack_end(sp)
    sp.show()

    bt = elementary.Button(win)
    bt.label_set("Left lower corner")
    bt.disabled_set(True)
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    bx0.pack_end(sp)
    sp.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx0.pack_end(bx)
    bx.show()

    bt = elementary.Button(win)
    bt.label_set("Right upper corner")
    bt.disabled_set(True)
    bx.pack_end(bt)
    bt.show()

    sp = elementary.Separator(win)
    sp.horizontal_set(True)
    bx.pack_end(sp)
    sp.show()

    bt = elementary.Button(win)
    bt.label_set("Right lower corner")
    bx.pack_end(bt)
    bt.show()

    win.show()
# }}}

#----- Scroller -{{{-
def my_scroller_go_300_300(bt, sc):
    sc.region_bring_in(300, 300, 318, 318)

def my_scroller_go_900_300(bt, sc):
    sc.region_bring_in(900, 300, 318, 318)

def my_scroller_go_300_900(bt, sc):
    sc.region_bring_in(300, 900, 318, 318)

def my_scroller_go_900_900(obj, sc):
    sc.region_bring_in(900, 900, 318, 318)

def scroller_clicked(obj, it):
    win = elementary.Window("scroller", elementary.ELM_WIN_BASIC)
    win.title_set("Scroller")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    tb = elementary.Table(win)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)

    img = ["images/panel_01.jpg",
           "images/plant_01.jpg",
           "images/rock_01.jpg",
           "images/rock_02.jpg",
           "images/sky_01.jpg",
           "images/sky_02.jpg",
           "images/sky_03.jpg",
           "images/sky_04.jpg",
           "images/wood_01.jpg"]

    n = 0
    for j in range(12):
        for i in range(12):
            bg2 = elementary.Background(win)
            bg2.file_set(img[n])

            n = n + 1
            if n >= 9:
                n = 0
            bg2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
            bg2.size_hint_align_set(evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
            bg2.size_hint_min_set(318, 318)
            tb.pack(bg2, i, j, 1, 1)
            bg2.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(sc)

    sc.content_set(tb)
    tb.show()

    sc.page_relative_set(1.0, 1.0)
    sc.show()

    tb2 = elementary.Table(win)
    tb2.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(tb2)

    bt = elementary.Button(win)
    bt.label_set("to 300 300")
    bt.callback_clicked_add(my_scroller_go_300_300, sc)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.1, 0.1)
    tb2.pack(bt, 0, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("to 900 300")
    bt.callback_clicked_add(my_scroller_go_900_300, sc)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.1)
    tb2.pack(bt, 1, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("to 300 900")
    bt.callback_clicked_add(my_scroller_go_300_900, sc)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.1, 0.9)
    tb2.pack(bt, 0, 1, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("to 900 900")
    bt.callback_clicked_add(my_scroller_go_900_900, sc)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.9, 0.9)
    tb2.pack(bt, 1, 1, 1, 1)
    bt.show()

    tb2.show()

    win.resize(320, 320)
    win.show()
# }}}

#----- Spinner -{{{-
def spinner_clicked(obj, it):
    win = elementary.Window("spinner", elementary.ELM_WIN_BASIC)
    win.title_set("Spinner test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    sp = elementary.Spinner(win)
    sp.label_format_set("%1.1f units")
    sp.step_set(1.3)
    sp.wrap_set(1)
    sp.min_max_set(-50.0, 250.0)
    sp.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    sp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(sp)
    sp.show()

    sp = elementary.Spinner(win)
    sp.label_format_set("%1.1f units")
    sp.step_set(1.3)
    sp.wrap_set(1)
    sp.style_set("vertical");
    sp.min_max_set(-50.0, 250.0)
    sp.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    sp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(sp)
    sp.show()

    sp = elementary.Spinner(win)
    sp.label_format_set("Disabled %.0f")
    sp.disabled_set(1)
    sp.min_max_set(-50.0, 250.0)
    sp.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    sp.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(sp)
    sp.show()

    win.show()
# }}}

#----- Notify -{{{-
def notify_close(bt, notify):
    notify.hide()

def notify_show(bt, win, orient):
    notify = elementary.Notify(win)
    notify.repeat_events_set(False)
    notify.timeout_set(5)
    notify.orient_set(orient)

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.horizontal_set(True)
    notify.content_set(bx)
    bx.show()

    lb = elementary.Label(win)
    lb.label_set("Text notification")
    bx.pack_end(lb)
    lb.show()

    bt = elementary.Button(win)
    bt.label_set("Close")
    bt.callback_clicked_add(notify_close, notify)
    bx.pack_end(bt)
    bt.show()
    notify.show()

def notify_clicked(obj, it):
    win = elementary.Window("notify", elementary.ELM_WIN_BASIC)
    win.title_set("Notify test")
    win.autodel_set(True)
    win.size_hint_min_set(160, 160)
    win.size_hint_max_set(320, 320)
    win.resize(320, 320)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    tb = elementary.Table(win)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(tb)
    tb.show()

    bt = elementary.Button(win)
    bt.label_set("Top")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_TOP)
    tb.pack(bt, 1, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Bottom")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_BOTTOM)
    tb.pack(bt, 1, 2, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Left")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_LEFT)
    tb.pack(bt, 0, 1, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top Left")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_TOP_LEFT)
    tb.pack(bt, 0, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Bottom Left")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_BOTTOM_LEFT)
    tb.pack(bt, 0, 2, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Right")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_RIGHT)
    tb.pack(bt, 2, 1, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Top Right")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_TOP_RIGHT)
    tb.pack(bt, 2, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Bottom Right")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_BOTTOM_RIGHT)
    tb.pack(bt, 2, 2, 1, 1)
    bt.show()

    win.show()

# }}}

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
               ("Checks", check_clicked),
               ("Radios", radio_clicked),
               ("Pager", pager_clicked),
               ("Progressbar", progressbar_clicked),
               ("File selector", fileselector_clicked),
               ("Separator", separator_clicked),
               ("Scroller", scroller_clicked),
               ("Spinner", spinner_clicked),
               ("Notify", notify_clicked)]



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
