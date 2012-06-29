 #!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas


#----- Pupup -{{{-
def cb_bnt_close(btn, popup):
    if "im" in popup.data:
        popup.data["im"].delete()
    popup.delete()

def cb_btn_restack(btn, popup):
    im = evas.FilledImage(popup.evas);
    im.file = "images/mystrale_2.jpg"
    im.move(40, 40)
    im.resize(500, 320)
    im.show()

    popup.data["im"] = im
    popup.raise_()

def cb_popup_center_text(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "This Popup has content area and timeout value is 3 seconds"
    popup.timeout = 3.0
    popup.show()

def cb_popup_center_title_text_1button(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "This Popup has content area and " \
                 "action area set, action area has one button Close"
    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button1", bt)
    popup.show()

def cb_popup_center_title_text_1button(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "This Popup has title area, content area and " \
                 "action area set, action area has one button Close"
    popup.part_text_set("title,text", "Title")
    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button1", bt)
    popup.show()

def cb_popup_center_title_text_block_clicked_event(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "This Popup has title area and content area. " \
                 "When clicked on blocked event region, popup gets deleted"
    popup.part_text_set("title,text", "Title")
    popup.callback_block_clicked_add(cb_bnt_close, popup)
    popup.show()

def cb_popup_bottom_title_text_3button(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "This Popup has title area, content area and " \
                 "action area set with content being character wrapped. " \
                 "action area has three buttons OK, Cancel and Close"
    popup.content_text_wrap_type = elementary.ELM_WRAP_CHAR
    popup.part_text_set("title,text", "Title")

    ic = elementary.Icon(win)
    ic.file = "images/logo_small.png"
    popup.part_content_set("title,icon", ic)

    bt = elementary.Button(win)
    bt.text = "OK"
    popup.part_content_set("button1", bt)

    bt = elementary.Button(win)
    bt.text = "Cancel"
    popup.part_content_set("button2", bt)

    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button3", bt)

    popup.show()

def cb_popup_center_title_content_3button(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.part_text_set("title,text", "Title")
    
    ic = elementary.Icon(win)
    ic.file = "images/logo_small.png"
    bt = elementary.Button(win)
    bt.text = "Content"
    bt.content = ic
    popup.content = bt

    bt = elementary.Button(win)
    bt.text = "OK"
    popup.part_content_set("button1", bt)

    bt = elementary.Button(win)
    bt.text = "Cancel"
    popup.part_content_set("button2", bt)

    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button3", bt)

    popup.show()

def cb_popup_center_title_item_3button(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.part_text_set("title,text", "Title")

    for i in range(1, 11):
        if i in [3, 5, 6]:
            ic = elementary.Icon(win)
            ic.file = "images/logo_small.png"
            popup.item_append("item"+str(i), ic)
        else:
            popup.item_append("item"+str(i))

    bt = elementary.Button(win)
    bt.text = "OK"
    popup.part_content_set("button1", bt)

    bt = elementary.Button(win)
    bt.text = "Cancel"
    popup.part_content_set("button2", bt)

    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button3", bt)

    popup.show()

def cb_popup_center_title_text_2button_restack(li, item, win):
    popup = elementary.Popup(win)
    popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    popup.text = "When you click the 'Restack' button, " \
                 "an image will be located under this popup"
    popup.part_text_set("title,text", "Title")

    bt = elementary.Button(win)
    bt.text = "Restack"
    bt.callback_clicked_add(cb_btn_restack, popup)
    popup.part_content_set("button1", bt)

    bt = elementary.Button(win)
    bt.text = "Close"
    bt.callback_clicked_add(cb_bnt_close, popup)
    popup.part_content_set("button3", bt)

    popup.show()

times = 0
g_popup = None
def cb_popup_center_text_1button_hide_show(li, item, win):
    global times
    global g_popup

    times += 1

    if g_popup is not None:
        g_popup.text = "You have checked this popup %d times." % times
        g_popup.show()
        return

    g_popup = elementary.Popup(win)
    g_popup.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    g_popup.text = "Hide this popup by using the button." \
                   "When you click list item again, you will see this popup again."

    bt = elementary.Button(win)
    bt.text = "Hide"
    bt.callback_clicked_add(lambda b: g_popup.hide())
    g_popup.part_content_set("button1", bt)

    g_popup.show()


def popup_clicked(obj):
    win = elementary.Window("popup", elementary.ELM_WIN_BASIC)
    win.title_set("Popup test")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    li = elementary.List(win)
    li.mode = elementary.ELM_LIST_LIMIT
    li.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    li.callback_selected_add(lambda li, it: it.selected_set(False))
    win.resize_object_add(li)
    li.show()

    li.item_append("popup-center-text", None, None,
                   cb_popup_center_text, win)
    li.item_append("popup-center-text + 1 button", None, None,
                   cb_popup_center_title_text_1button, win)
    li.item_append("popup-center-title + text + 1 button", None, None,
                   cb_popup_center_title_text_1button, win)
    li.item_append("popup-center-title + text (block,clicked handling)", None, None,
                   cb_popup_center_title_text_block_clicked_event, win)
    li.item_append("popup-bottom-title + text + 3 buttons", None, None,
                   cb_popup_bottom_title_text_3button, win)
    li.item_append("popup-center-title + content + 3 buttons", None, None,
                   cb_popup_center_title_content_3button, win)
    li.item_append("popup-center-title + items + 3 buttons", None, None,
                   cb_popup_center_title_item_3button, win)
    li.item_append("popup-center-title + text + 2 buttons (check restacking)", None, None,
                   cb_popup_center_title_text_2button_restack, win)
    li.item_append("popup-center-text + 1 button (check hide, show)", None, None,
                   cb_popup_center_text_1button_hide_show, win)

    li.go()
    win.resize(480, 800)
    win.show()
# }}}

#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    popup_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
