#!/usr/bin/env python

import ecore
import ecore.evas
import edje
import sys
import os

ecore.init()
ecore.evas.init()
edje.init()

if "-x11" in sys.argv:
    ee = ecore.evas.SoftwareX11(w=800, h=480)
else:
    if ecore.evas.engine_type_supported_get("software_x11_16"):
        ee = ecore.evas.SoftwareX11_16(w=800, h=480)
    else:
        ee = ecore.evas.SoftwareX11(w=800, h=480)

canvas = ee.evas
edje_file = os.path.join(os.path.dirname(sys.argv[0]), "01-app_launcher.edj")

try:
    edje_obj = edje.Edje(canvas, file=edje_file, group="main")
except Exception, e: # should be EdjeLoadError, but it's wrong on python2.5
    raise SystemExit("Failed to load Edje file: %s" % edje_file)

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

parts = ("web_browser", "audio_player", "image_viewer", "video_player")
def icon_clicked_cb(edje_obj, emission, source, part):
    if edje_obj.part_state_get(part) == ("default", 0.0):
        edje_obj.signal_emit("select_" + part, "")
        for p in parts:
            if p != part:
                edje_obj.signal_emit("unselect_" + p, "")

for p in parts:
    edje_obj.signal_callback_add("mouse,clicked,1", p + "_area",
                                 icon_clicked_cb, p)

edje_obj.focus = True
ee.fullscreen = "-nofs" not in sys.argv
ee.show()
ecore.main_loop_begin()
