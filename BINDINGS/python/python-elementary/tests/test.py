#!/usr/bin/env python
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
    tb.pack(bt, 0, 1, 1, 1)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Button 5")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 2, 1, 1, 1)
    bt.show()
    
    bt = elementary.Button(win)
    bt.label_set("Button 6")
    bt.size_hint_weight_set(1.0, 1.0)
    bt.size_hint_align_set(-1.0, -1.0)
    tb.pack(bt, 0, 2, 1, 1)
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
    
def hover_bt1_clicked(obj, event, *args, **kargs):
    obj.show()

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
    bt.clicked = hover_bt1_clicked
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
    bt.clicked = hover2_bt_clicked
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
    bx.horizontal_set(True)
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
    
    sc = elementary.Scroller(win)
    sc.size_hint_weight_set(1.0, 1.0)
    sc.size_hint_align_set(-1.0, -1.0)
    bx.pack_end(sc)
    
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
    sc.content_set(en)
    en.show()

    sc.show()
    
    bx2 = elementary.Box(win)
    bx.horizontal_set(True)
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

def anchorview_clicked(obj, event, *args, **kargs):
    win = elementary.Window("anchorview", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorview")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()

def anchorblock_clicked(obj, event, *args, **kargs):
    win = elementary.Window("anchorblock", elementary.ELM_WIN_BASIC)
    win.title_set("Anchorblock")
    win.autodel_set(True)
    
    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight_set(1.0, 1.0)
    bg.show()

    bx = elementary.Box(win)
    win.resize_object_add(bx)
    bx.size_hint_weight_set(1.0, 1.0)
    bx.show()

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
    lb.label_set("Please select a test from the list belox<CR>" \
                 "by clicking the test button to show the<CR>" \
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
               ("Pager", pager_clicked)]
    
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

