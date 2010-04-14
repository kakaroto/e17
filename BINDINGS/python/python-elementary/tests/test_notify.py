#!/usr/bin/env python
import os
import elementary
import edje
import ecore
import evas

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
    bt.label_set("Center")
    bt.callback_clicked_add(notify_show, win, elementary.ELM_NOTIFY_ORIENT_CENTER)
    tb.pack(bt, 1, 1, 1, 1)
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
if __name__ == "__main__":
    elementary.init()

    notify_clicked(None, None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
