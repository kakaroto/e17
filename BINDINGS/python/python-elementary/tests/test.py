#!/usr/bin/env python
import os
import elementary
import edje
import evas

def destroy(obj, event, data):
    print "DEBUG: window destroy callback called!"
    print "DEBUG: data:"
    print data
    elementary.exit()
    
def bg_plain_clicked(obj, event, data):
    win = elementary.Window("bg plain", elementary.ELM_WIN_BASIC)
    win.title_set("Bg Plain")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    win.resize(320, 320)
    win.show()

def bg_image_clicked(obj, event, data):
    win = elementary.Window("bg-image", elementary.ELM_WIN_BASIC)
    win.title_set("Bg Image")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.file_set("images/plant_01.jpg")
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    win.size_hint_min_set(160, 160)
    win.size_hint_max_set(320, 320)
    win.resize(320, 320)
    win.show()

def icon_transparent_clicked(obj, event, data):
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
    

def box_vert_clicked(obj, event, *args, **kargs):
    win = elementary.Window("box-vert", elementary.ELM_WIN_BASIC)
    win.title_set("Box Vert")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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
    ic.size_hint_align_set(1.0, 0.5)
    bx.pack_end(ic)
    ic.show()
    
    win.show()


def box_horiz_clicked(obj, event, *args, **kargs):
    win = elementary.Window("box-horiz", elementary.ELM_WIN_BASIC)
    win.title_set("Box Horiz")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    bx.horizontal_set(True)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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
    ic.size_hint_align_set(1.0, 0.5)
    bx.pack_end(ic)
    ic.show()
    
    win.show()

def buttons_clicked(obj, event, *args, **kargs):
    win = elementary.Window("buttons", elementary.ELM_WIN_BASIC)
    win.title_set("Buttons")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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
    
  
def tg_changed(obj, event, *args, **kargs):
  print obj.state_get()

def toggles_clicked(obj, event, *args, **kargs):
    win = elementary.Window("toggles", elementary.ELM_WIN_BASIC)
    win.title_set("Toggles")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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

def table_clicked(obj, event, *args, **kargs):
    win = elementary.Window("table", elementary.ELM_WIN_BASIC)
    win.title_set("Table")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    tb = elementary.Table(win)
    win.resize_object_add(tb)
    tb.size_hint_weight_set(1.0, 1.0)
    tb.show()

    bt = elementary.Button(win)
    bt.label_set("Button 1")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 0, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 2")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 1, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 3")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 2, 0, 1, 1)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Button 4")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 0, 1, 2, 1)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Button 5")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 2, 1, 1, 3)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Button 6")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 0, 2, 2, 2)
    bt.show()
    
    win.show()

def clock_clicked(obj, event, *args, **kargs):
    win = elementary.Window("clock", elementary.ELM_WIN_BASIC)
    win.title_set("Clock")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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
    

def layout_clicked(obj, event, *args, **kargs):
    win = elementary.Window("layout", elementary.ELM_WIN_BASIC)
    win.title_set("Layout")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    ly = elementary.Layout(win)
    ly.file_set("test.edj", "layout")
    ly.size_hint_weight_set(1.0, 1.0)
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
    
def hover_bt1_clicked(obj, event, data):
    data.show()

def hover_clicked(obj, event, *args, **kargs):
    win = elementary.Window("hover", elementary.ELM_WIN_BASIC)
    win.title_set("Hover")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()
    
    hv = elementary.Hover(win)
    
    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.clicked = (hover_bt1_clicked, hv)
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

def hover2_bt_clicked(obj, event, *args, **kargs):
    pass

def hover2_clicked(obj, event, *args, **kargs):
    win = elementary.Window("hover2", elementary.ELM_WIN_BASIC)
    win.title_set("Hover 2")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()
    
    hv = elementary.Hover(win)
    hv.style_set("popout")
    
    bt = elementary.Button(win)
    bt.label_set("Button")
    bt.clicked = (hover_bt1_clicked, hv)
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
    

def my_entry_bt_1(obj, event, data):
    data.entry_set("")

def my_entry_bt_2(obj, event, data):
    str = data.entry_get()
    print "ENTRY: %s" % str
    
def my_entry_bt_3(obj, event, data):
    str = data.selection_get()
    print "SELECTION: %s" % str

def my_entry_bt_4(obj, event, data):
    data.entry_insert("Insert some <b>BOLD</> text")

def anchor_test(obj, event, data):
    data.entry_insert("ANCHOR CLICKED")

def entry_clicked(obj, event, *args, **kargs):
    win = elementary.Window("entry", elementary.ELM_WIN_BASIC)
    win.title_set("Entry")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
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
    en.size_hint_weight_set(1.0, 1.0)
    en.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(en)
    en.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.size_hint_weight_set(1.0, 0.0)
    bx2.size_hint_align_set(-1.0, -1.0)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.clicked = (my_entry_bt_1, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Print")
    bt.clicked = (my_entry_bt_2, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Selection")
    bt.clicked = (my_entry_bt_3, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Insert")
    bt.clicked = (my_entry_bt_4, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    en.focus()
    win.show()

def entry_scrolled_clicked(obj, event, *args, **kargs):
    win = elementary.Window("entry", elementary.ELM_WIN_BASIC)
    win.title_set("Entry Scrolled")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
    win.resize_object_add(bx)
    bx.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(1.0, 0.0)
    sc.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.single_line_set(True)
    en.entry_set("Disable entry")
    en.size_hint_weight_set(1.0, 0.0)
    en.size_hint_align_set(-1.0, 0.5)
    en.disabled_set(True)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(1.0, 0.0)
    sc.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.password_set(True)
    en.entry_set("Access denied, give up!")
    en.size_hint_weight_set(1.0, 0.0)
    en.size_hint_align_set(-1.0, 0.5)
    en.disabled_set(True)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(1.0, 1.0)
    sc.size_hint_align_set(-1.0, -1.0)
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
    en.size_hint_weight_set(1.0, 1.0)
    en.size_hint_align_set(-1.0, -1.0)
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
    sc.size_hint_weight_set(1.0, 0.0)
    sc.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.single_line_set(True)
    en.entry_set("This is a single line")
    en.size_hint_weight_set(1.0, 0.0)
    en.size_hint_align_set(-1.0, 0.5)
    en.select_all()
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.content_min_limit(0, 1)
    sc.policy_set(elementary.ELM_SCROLLER_POLICY_OFF,
                  elementary.ELM_SCROLLER_POLICY_OFF)
    sc.bounce_set(False, False)
    sc.size_hint_weight_set(1.0, 0.0)
    sc.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(sc)

    en = elementary.Entry(win)
    en.password_set(True)
    en.entry_set("Password here")
    en.size_hint_weight_set(1.0, 0.0)
    en.size_hint_align_set(-1.0, 0.0)
    sc.content_set(en)
    en.show()

    sc.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(1.0, 1.0)
    sc.size_hint_align_set(-1.0, -1.0)
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
    en._callback_add("anchor,clicked", (anchor_test, en))
    en.size_hint_weight_set(1.0, 1.0)
    en.size_hint_align_set(-1.0, -1.0)
    sc.content_set(en)
    en.show()

    sc.show()

    bx2 = elementary.Box(win)
    bx2.horizontal_set(True)
    bx2.size_hint_weight_set(1.0, 0.0)
    bx2.size_hint_align_set(-1.0, -1.0)

    bt = elementary.Button(win)
    bt.label_set("Clear")
    bt.clicked = (my_entry_bt_1, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Print")
    bt.clicked = (my_entry_bt_2, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Selection")
    bt.clicked = (my_entry_bt_3, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Insert")
    bt.clicked = (my_entry_bt_4, en)
    bt.size_hint_align_set(-1.0, -1.0)
    bt.size_hint_weight_set(1.0, 0.0)
    bx2.pack_end(bt)
    bt.show()

    bx.pack_end(bx2)
    bx2.show()

    win.resize(320, 300)

    win.focus()
    win.show()


def notepad_clicked(obj, event, *args, **kargs):
    win = elementary.Window("notepad", elementary.ELM_WIN_BASIC)
    win.title_set("Notepad")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()

def my_anchorview_bt(obj, event, data):
    data.hover_end()

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
           bt.clicked = (my_anchorview_bt, av)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 2")
           bx.pack_end(bt)
           bt.clicked = (my_anchorview_bt, av)
           bt.show()
           bt.label_set("Top 3")
           bx.pack_end(bt)
           bt.clicked = (my_anchorview_bt, av)
           bt.show()
           ei.hover.content_set("top", bx)
           bx.show()
       if (ei.hover_bottom):
           bt = elementary.Button(obj)
           bt.label_set("Bot")
           ei.hover.content_set("bottom", bt)
           bt.clicked = (my_anchorview_bt, av)
           bt.show()
       if (ei.hover_left):
           bt = elementary.Button(obj)
           bt.label_set("Left")
           ei.hover.content_set("left", bt)
           bt.clicked = (my_anchorview_bt, av)
           bt.show()
       if (ei.hover_right):
           bt = elementary.Button(obj)
           bt.label_set("Right")
           ei.hover.content_set("right", bt)
           bt.clicked = (my_anchorview_bt, av)
           bt.show()


def anchorview_clicked(obj, event, *args, **kargs):
    win = elementary.Window("anchorview", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorview")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
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
    av.size_hint_weight_set(1.0, 1.0)
    av.clicked = (my_anchorview_anchor, av)
    win.resize_object_add(av)
    av.show()

    win.resize(320, 300)

    win.focus()
    win.show()

def my_anchorblock_bt(obj, event, data):
    data.hover_end()

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
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 2")
           bx.pack_end(bt)
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()
           bt = elementary.Button(obj)
           bt.label_set("Top 3")
           bx.pack_end(bt)
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()
           ei.hover.content_set("top", bx)
           bx.show()
       if (ei.hover_bottom):
           bt = elementary.Button(obj)
           bt.label_set("Bot")
           ei.hover.content_set("bottom", bt)
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()
       if (ei.hover_left):
           bt = elementary.Button(obj)
           bt.label_set("Left")
           ei.hover.content_set("left", bt)
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()
       if (ei.hover_right):
           bt = elementary.Button(obj)
           bt.label_set("Right")
           ei.hover.content_set("right", bt)
           bt.clicked = (my_anchorblock_bt, ab)
           bt.show()

def my_anchorblock_edge_left(obj, event, data):
    print "left"
def my_anchorblock_edge_right(obj, event, data):
    print "right"
def my_anchorblock_edge_top(obj, event, data):
    print "top"
def my_anchorblock_edge_bottom(obj, event, data):
    print "bottom"

def my_anchorblock_scroll(obj, event, data):
   (x, y, w, h) = obj.region_get()
   (vw, vh) = obj.child_size_get()
   print "scroll %ix%i +%i+%i in %ix%i" % (w, h, x, y, vw, vh)

def anchorblock_clicked(obj, event, *args, **kargs):
    win = elementary.Window("anchorblock", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorblock")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(1.0, 1.0)
    win.resize_object_add(sc)

    sc._callback_add("edge_left", my_anchorblock_edge_left)
    sc._callback_add("edge_right", my_anchorblock_edge_right)
    sc._callback_add("edge_top", my_anchorblock_edge_top)
    sc._callback_add("edge_bottom", my_anchorblock_edge_bottom)
    sc._callback_add("scroll", my_anchorblock_scroll)

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.size_hint_align_set(-1.0, -1.0)
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
    bb.size_hint_weight_set(1.0, 0.0)
    bb.size_hint_align_set(-1.0, -1.0)
    ab = elementary.AnchorBlock(win)
    ab.hover_style_set("popout")
    ab.hover_parent_set(win)
    ab.text_set("Hi there. This is the most recent message in the "
                "list of messages. It has one <a href=tel:+614321234>+61 432 1234</a> "
                "(phone number) to click on.")
    ab.clicked = (my_anchorblock_anchor, ab)
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
    bb.size_hint_weight_set(1.0, 0.0)
    bb.size_hint_align_set(-1.0, -1.0)
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
    ab.clicked = (my_anchorblock_anchor, ab)
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
    bb.size_hint_weight_set(1.0, 0.0)
    bb.size_hint_align_set(-1.0, -1.0)
    ab = elementary.AnchorBlock(win)
    ab.hover_style_set("popout")
    ab.hover_parent_set(win)
    ab.text_set("This is a short message")
    ab.clicked = (my_anchorblock_anchor, ab)
    bb.content_set(ab)
    ab.show()
    bx.pack_end(bb)
    bb.show()

    sc.show()

    win.resize(320, 300)

    win.focus()
    win.show()

def tb_1(obj, event, *args, **kargs):
    print "toolbar-item: test 1"

def tb_2(obj, event, *args, **kargs):
    print "toolbar-item: test 2"

def tb_3(obj, event, *args, **kargs):
    print "toolbar-item: test 3"

def tb_4(obj, event, *args, **kargs):
    print "toolbar-item: test 4"

def tb_5(obj, event, *args, **kargs):
    print "toolbar-item: test 5"

def toolbar_clicked(obj, event, *args, **kargs):
    win = elementary.Window("entry-scrolled", elementary.ELM_WIN_BASIC)
    win.title_set("Entry Scrolled")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()
   
    
    ic = elementary.Icon(win)
    ic.file_set("images/logo_small.png")
    ic.scale_set(0, 0)
    ic.size_hint_align_set(0.5, 0.5)
    bx.pack_end(ic)
    ic.show()

    tb = elementary.Toolbar(win)
    tb.size_hint_weight_set(0.0, 0.0)
    tb.size_hint_align_set(-1.0, 0.0)
  
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

def my_pager_1(obj, event, data):
    data["pager"].content_promote(data["pg2"])

def my_pager_2(obj, event, data):
    data["pager"].content_promote(data["pg3"])

def my_pager_3(obj, event, data):
    data["pager"].content_promote(data["pg1"])

def my_pager_pop(obj, event, data):
    data["pager"].content_pop()

def pager_clicked(obj, event, data):
    win = elementary.Window("pager", elementary.ELM_WIN_BASIC)
    win.autodel_set(True)
    win.title_set("Pager")

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    pg = elementary.Pager(win)
    win.resize_object_add(pg)
    pg.show()

    info = dict()
    info["pager"] = pg

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
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
    bt.clicked = (my_pager_1, info)
    bx.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.clicked = (my_pager_pop, info)
    bx.pack_end(bt)
    bt.show()

    pg.content_push(bx)

    info["pg1"] = bx;
    
    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
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
    bt.clicked = (my_pager_2, info)
    bx.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.clicked = (my_pager_pop, info)
    bx.pack_end(bt)
    bt.show()

    pg.content_push(bx)
    
    info["pg2"] = bx
    
    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
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
    bt.clicked = (my_pager_3, info)
    bx.pack_end(bt)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Popme")
    bt.clicked = (my_pager_pop, info)
    bx.pack_end(bt)
    bt.show()
    
    pg.content_push(bx)
     
    info["pg3"] = bx

    win.resize(320, 320)
    win.show()

def check_clicked(obj, event, data):
    win = elementary.Window("check", elementary.ELM_WIN_BASIC)
    win.title_set("Check test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
    win.resize_object_add(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(1.0, 1.0)
    ck.size_hint_align_set(-1.0, 0.5)
    ck.label_set("Icon sized to check")
    ck.icon_set(ic)
    ck.state_set(True)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.scale_set(0, 0)
    ck = elementary.Check(win)
    ck.label_set("Icon no scale")
    ck.icon_set(ic)
    bx.pack_end(ck)
    ck.show()
    ic.show()

    ck = elementary.Check(win)
    ck.label_set("Label Only")
    bx.pack_end(ck)
    ck.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    ck = elementary.Check(win)
    ck.size_hint_weight_set(1.0, 1.0)
    ck.size_hint_align_set(-1.0, 0.5)
    ck.label_set("Disabled check")
    ck.icon_set(ic)
    ck.state_set(True)
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
    bx.pack_end(ck)
    ck.show()
    ic.show()

    win.show()

def radio_clicked(obj, event, data):
    win = elementary.Window("check", elementary.ELM_WIN_BASIC)
    win.title_set("Radio test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
    win.resize_object_add(bx)
    bx.show()

    ic = elementary.Icon(win)
    ic.file_set('images/logo_small.png')
    ic.size_hint_aspect_set(evas.EVAS_ASPECT_CONTROL_VERTICAL, 1, 1)
    rd = elementary.Radio(win)
    rd.state_value_set(0)
    rd.size_hint_weight_set(1.0, 1.0)
    rd.size_hint_align_set(-1.0, 0.5)
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

def inner_window_clicked(obj, event, data):
    win = elementary.Window("inner-window", elementary.ELM_WIN_BASIC)
    win.title_set("InnerWindow test")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    iw = elementary.InnerWindow(win)
    iw.show()

    win.resize(320, 320)
    win.show()
    
def spinner_clicked(obj, event, data):
    win = elementary.Window("spinner", elementary.ELM_WIN_BASIC)
    win.title_set("Spinner test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(1.0, 1.0)
    win.resize_object_add(bx)
    bx.show()

    sp = elementary.Spinner(win)
    sp.label_format_set("%1.1f units")
    sp.step_set(1.3)
    sp.wrap_set(1)
    sp.min_max_set(-50.0, 250.0)
    sp.size_hint_align_set(-1.0, 0.5)
    sp.size_hint_weight_set(1.0, 1.0)
    bx.pack_end(sp)
    sp.show()

    sp2 = elementary.Spinner(win)
    sp2.label_format_set("Disabled %.0f")
    sp2.disabled_set(1)
    sp2.min_max_set(-50.0, 250.0)
    sp2.size_hint_align_set(-1.0, 0.5)
    sp2.size_hint_weight_set(1.0, 1.0)
    bx.pack_end(sp2)
    sp2.show()

    win.show()

def fileselector_clicked(obj, event, data):
    win = elementary.Window("fileselector", elementary.ELM_WIN_BASIC)
    win.title_set("File selector test")
    win.autodel_set(True)

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    vbox = elementary.Box(win)
    win.resize_object_add(vbox)
    vbox.size_hint_weight_set(1.0, 1.0)
    vbox.show()

    fs = elementary.Fileselector(win)
    fs.is_save_set(True)
    fs.expandable_set(False)
    fs.path_set(os.getenv("HOME"))
    fs.size_hint_weight_set(1.0, 1.0)
    fs.size_hint_align_set(-1.0, -1.0)
    vbox.pack_end(fs)
    fs.show()

    def fs_cb_done(obj, event_info, data):
        if event_info is not None:
            print "Selected file:", event_info
        else:
            data.delete()
    fs.done = (fs_cb_done, win)
    def fs_cb_selected(obj, event_info, data):
        print "Selected file:", event_info
        print "or:", obj.selected_get()
    fs.selected = (fs_cb_selected, win)

    hbox = elementary.Box(win)
    hbox.horizontal_set(True)
    vbox.pack_end(hbox)
    hbox.show()

    bt = elementary.Button(win)
    bt.label_set("Toggle is_save")
    def bt_cb_is_save(obj, event, data):
        print "Toggle is save"
        data.is_save_set(not data.is_save_get())
    bt.clicked = (bt_cb_is_save, fs)
    hbox.pack_end(bt)
    bt.show()

    bt = elementary.Button(win)
    bt.label_set("sel get")
    def bt_cb_sel_get(obj, event, data):
        print "Get Selected:", data.selected_get()
    bt.clicked = (bt_cb_sel_get, fs)
    hbox.pack_end(bt)
    bt.show()

    win.resize(240, 350)
    win.show()

if __name__ == "__main__":
    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.destroy = (destroy, ("test", "test1"))
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()
    
    box0 = elementary.Box(win)
    box0.size_hint_weight_set(1.0, 1.0)
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
               ("Pager", pager_clicked),
               ("Radio", radio_clicked),
               ("Check", check_clicked),
               ("InnerWindow", inner_window_clicked),
               ("Spinner", spinner_clicked),
               ("File selector", fileselector_clicked)]
    
    
    
    li = elementary.List(win)
    li.size_hint_weight_set(1.0, 1.0)
    li.size_hint_align_set(-1.0, -1.0)
    box0.pack_end(li)
    li.show()
    
    for item in items:
        """
        it = elementary.ListItem()
        it.append(li, item[0], None, None, item[1])
        """
        li.item_append(item[0], None, None, item[1])
        
    li.go()

    """
    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(1.0, 1.0)
    sc.size_hint_align_set(-1.0, -1.0)
    box0.pack_end(sc)
    sc.show()

    box1 = elementary.Box(win)
    box1.size_hint_weight_set(1.0, 1.0)
    sc.content_set(box1)
    box1.show()

    buttons = [("Bg Plain", bg_plain_clicked), 
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
               ("Pager", pager_clicked)]
    
    
    for btn in buttons:
        bt = elementary.Button(win)
        bt.clicked = btn[1]
        bt.label_set(btn[0])
        bt.size_hint_align_set(-1.0, 0.0)
        box1.pack_end(bt)
        bt.show()
    """
    
    win.resize(320,520)
    win.show()
    elementary.run()
    elementary.shutdown()

