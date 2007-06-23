#!/usr/bin/env python

STOP_PULSING_TIMEOUT = 10.0

import ecore
import ecore.evas
import edje
import sys

ecore.init()
ecore.evas.init()
edje.init()

if "-x11" in sys.argv:
    ee = ecore.evas.SoftwareX11(w=800, h=480)
else:
    ee = ecore.evas.SoftwareX11_16(w=800, h=480)
canvas = ee.evas

edje_obj = edje.Edje(canvas, file="01-app_launcher.edj", group="main")
edje_obj.size = canvas.size
edje_obj.show()
ee.data["edje"] = edje_obj

def resize_cb(ee):
    r = ee.evas.rect
    ee.data["edje"].size = r.size
ee.callback_resize = resize_cb

def key_down_cb(bg, event):
    k = event.key
    if k == "Escape":
        ecore.main_loop_quit()
edje_obj.on_key_down_add(key_down_cb)

stop_pulsing_id = None
def stop_pulsing(edje_obj, icon):
    global stop_pulsing_id
    stop_pulsing_id = None
    edje_obj.signal_emit("stop_pulsing_" + icon, "")
    return False


parts = ("web_browser", "audio_player", "image_viewer", "video_player")
def icon_clicked_cb(edje_obj, emission, source):
    icon = source.replace("_area", "")
    if edje_obj.part_state_get(icon) == ("default", 0.0):
        global stop_pulsing_id
        if stop_pulsing_id is not None:
            stop_pulsing_id.stop()
        stop_pulsing_id = ecore.timer_add(STOP_PULSING_TIMEOUT,
                                          stop_pulsing, edje_obj, icon)
        edje_obj.signal_emit("select_" + icon, "")
        for p in parts:
            if p != icon:
                edje_obj.signal_emit("unselect_" + p, "")

for p in parts:
    edje_obj.signal_callback_add("mouse,clicked,1",
                                 p + "_area",
                                 icon_clicked_cb)

edje_obj.focus = True
ee.fullscreen = "-nofs" not in sys.argv
ee.show()
ecore.main_loop_begin()
