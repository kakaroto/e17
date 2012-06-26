#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

def my_bt_open(bt, file, *args, **kwargs):
    video = args[0]

    if (file):
        video.file = file
        video.play()

def notify_show(video, event, no):
    no.show()

def notify_block(video, event, no):
    no.timeout = 0.0
    no.show()

def notify_unblock(video, event, no):
    no.timeout = 3.0
    no.show()

def video_clicked(obj):
    win = elementary.Window("video", elementary.ELM_WIN_BASIC)
    win.title = "video"
    win.autodel = True
    win.alpha = True # Needed to turn video fast path on

    bg = elementary.Background(win)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bg)
    bg.show()

    video = elementary.Video(win)
    video.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(video)
    video.show()

    notify = elementary.Notify(win)
    notify.orient = elementary.ELM_NOTIFY_ORIENT_BOTTOM
    notify.timeout = 3.0

    player = elementary.Player(win)
    player.content = video
    notify.content = player
    player.show()

    tb = elementary.Table(win)
    tb.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(tb)

    bt = elementary.FileselectorButton(win)
    bt.text = "Select Video"
    bt.callback_file_chosen_add(my_bt_open, video)
    bt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bt.size_hint_align_set(0.5, 0.1)
    tb.pack(bt, 0, 0, 1, 1)
    bt.show()

    tb.show()

    video.event_callback_add(evas.EVAS_CALLBACK_MOUSE_MOVE, notify_show, notify)
    video.event_callback_add(evas.EVAS_CALLBACK_MOUSE_IN, notify_block, notify)
    video.event_callback_add(evas.EVAS_CALLBACK_MOUSE_OUT, notify_unblock, notify)

    win.resize(800, 600)
    win.show()

if __name__ == "__main__":
    elementary.init()

    video_clicked(None)

    elementary.run()
    elementary.shutdown()
