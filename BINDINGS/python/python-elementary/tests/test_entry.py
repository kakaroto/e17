#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

#----- common -{{{-
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
# -}}}-

#----- Entry -{{{-
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

#----- Main -{{{-
if __name__ == "__main__":
    def destroy(obj):
        elementary.exit()

    elementary.init()
    win = elementary.Window("test", elementary.ELM_WIN_BASIC)
    win.title_set("python-elementary test application")
    win.callback_destroy_add(destroy)

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

    items = [("Entry", entry_clicked),
             ("Entry Scrolled", entry_scrolled_clicked)]

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
