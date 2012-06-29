#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

def _pos_selected_cb(obj, event_info):
    print(("Selection: %s" % (event_info)))
    print(("Label selected: %s" % (obj.selected_label_get())))

    ipos = obj.indicator_pos_get()
    print("actionslider indicator pos: ")
    if ipos == elementary.ELM_ACTIONSLIDER_NONE:
        print("none!")
    elif ipos == elementary.ELM_ACTIONSLIDER_ALL:
        print("all!")
    else:
        if (ipos & elementary.ELM_ACTIONSLIDER_LEFT):
            print("left ")
        if (ipos & elementary.ELM_ACTIONSLIDER_CENTER):
            print("center ")
        if (ipos & elementary.ELM_ACTIONSLIDER_RIGHT):
            print("right ")
        print("")

    mpos = obj.magnet_pos_get()
    print("actionslider magnet pos: ")
    if mpos == elementary.ELM_ACTIONSLIDER_NONE:
        print("none!")
    elif mpos == elementary.ELM_ACTIONSLIDER_ALL:
        print("all!")
    else:
        if (mpos & elementary.ELM_ACTIONSLIDER_LEFT):
            print("left ")
        if (mpos & elementary.ELM_ACTIONSLIDER_CENTER):
            print("center ")
        if (mpos & elementary.ELM_ACTIONSLIDER_RIGHT):
            print("right ")
        print("")

    epos = obj.enabled_pos_get()
    print("actionslider enabled pos: ")
    if epos == elementary.ELM_ACTIONSLIDER_NONE:
        print("none!")
    elif epos == elementary.ELM_ACTIONSLIDER_ALL:
        print("all!")
    else:
        if (epos & elementary.ELM_ACTIONSLIDER_LEFT):
            print("left ")
        if (epos & elementary.ELM_ACTIONSLIDER_CENTER):
            print("center ")
        if (epos & elementary.ELM_ACTIONSLIDER_RIGHT):
            print("right ")
        print("")

def _position_change_magnetic_cb(obj, event_info):
    if event_info == "left":
        obj.magnet_pos_set(elementary.ELM_ACTIONSLIDER_LEFT)
    elif event_info == "right":
        obj.magnet_pos_set(elementary.ELM_ACTIONSLIDER_RIGHT)

def _magnet_enable_disable_cb(obj, event_info):
    if event_info == "left":
        obj.magnet_pos_set(elementary.ELM_ACTIONSLIDER_CENTER)
    elif event_info == "right":
        obj.magnet_pos_set(elementary.ELM_ACTIONSLIDER_NONE)

def actionslider_clicked(obj):
    win = elementary.Window("actionslider", elementary.ELM_WIN_BASIC)
    win.title_set("Actionslider")
    win.autodel_set(True)
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    bg.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bg)
    bg.show()

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.show()

    acts = elementary.Actionslider(win)
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.part_text_set("left", "Snooze")
    acts.part_text_set("center", "")
    acts.part_text_set("right", "Stop")
    acts.enabled_pos_set(elementary.ELM_ACTIONSLIDER_LEFT | elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.callback_pos_changed_add(_position_change_magnetic_cb)
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    acts = elementary.Actionslider(win)
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_CENTER)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_CENTER)
    acts.part_text_set("left", "Snooze")
    acts.part_text_set("center", "")
    acts.part_text_set("right", "Stop")
    acts.enabled_pos_set(elementary.ELM_ACTIONSLIDER_LEFT | elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    acts = elementary.Actionslider(win)
    acts.style_set("bar")
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_LEFT)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_CENTER | elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.enabled_pos_set(elementary.ELM_ACTIONSLIDER_CENTER | elementary.ELM_ACTIONSLIDER_RIGHT)
    acts.part_text_set("left", "")
    acts.part_text_set("center", "Accept")
    acts.part_text_set("right", "Reject")
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    acts = elementary.Actionslider(win)
    acts.style_set("bar")
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_LEFT)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_LEFT)
    acts.part_text_set("left", "")
    acts.part_text_set("center", "Accept")
    acts.part_text_set("right", "Reject")
    acts.text_set("Go")
    acts.callback_pos_changed_add(_position_change_magnetic_cb)
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    acts = elementary.Actionslider(win)
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_LEFT)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_ALL)
    acts.part_text_set("left", "Left")
    acts.part_text_set("center", "Center")
    acts.part_text_set("right", "Right")
    acts.text_set("Go");
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    acts = elementary.Actionslider(win)
    acts.size_hint_weight_set(evas.EVAS_HINT_EXPAND, 0.0)
    acts.size_hint_align_set(evas.EVAS_HINT_FILL, 0.0)
    acts.indicator_pos_set(elementary.ELM_ACTIONSLIDER_CENTER)
    acts.magnet_pos_set(elementary.ELM_ACTIONSLIDER_CENTER)
    acts.part_text_set("left", "Enable")
    acts.part_text_set("center", "Magnet")
    acts.part_text_set("right", "Disable")
    acts.callback_pos_changed_add(_magnet_enable_disable_cb)
    acts.callback_selected_add(_pos_selected_cb)
    bx.pack_end(acts)
    acts.show()

    win.resize(320, 400)
    win.show()

if __name__ == "__main__":
    elementary.init()

    actionslider_clicked(None)

    elementary.run()
    elementary.shutdown()
