#!/usr/bin/env python
# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

#----- Dayselector -{{{-
def cb_changed(ds):
    print("\nSelected Days:")
    print(("Sun:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_SUN)))
    print(("Mon:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_MON)))
    print(("Tue:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_TUE)))
    print(("Wed:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_WED)))
    print(("Thu:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_THU)))
    print(("Fri:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_FRI)))
    print(("Sat:", ds.day_selected_get(elementary.ELM_DAYSELECTOR_SAT)))

def dayselector_clicked(obj):
    win = elementary.Window("dayselector", elementary.ELM_WIN_BASIC)
    win.title = "Dayselector test"
    win.autodel = True
    if obj is None:
        win.callback_delete_request_add(lambda o: elementary.exit())

    bg = elementary.Background(win)
    win.resize_object_add(bg)
    bg.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bg.show()

    box = elementary.Box(win)
    box.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    box.size_hint_align = (evas.EVAS_HINT_FILL, evas.EVAS_HINT_FILL)
    win.resize_object_add(box)
    box.show()
    
    # default
    ds = elementary.Dayselector(win)
    ds.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ds.size_hint_align = (evas.EVAS_HINT_FILL, 0.5)
    box.pack_end(ds)
    ds.show()
    ds.callback_dayselector_changed_add(cb_changed)

    # Sunday first
    ds = elementary.Dayselector(win)
    ds.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ds.size_hint_align = (evas.EVAS_HINT_FILL, 0.5)
    box.pack_end(ds)
    ds.show()
    ds.callback_dayselector_changed_add(cb_changed)
    try:
        sunday = ds.part_content_get("day0")
        sunday.signal_emit("elm,type,weekend,style1", "")
    except:
        print("BUG HERE !!!")

    # Monday first
    ds = elementary.Dayselector(win)
    ds.size_hint_weight = (evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    ds.size_hint_align = (evas.EVAS_HINT_FILL, 0.5)
    ds.callback_dayselector_changed_add(cb_changed)
    box.pack_end(ds)
    ds.show()
    ds.week_start = elementary.ELM_DAY_MONDAY
    try:
        sunday = ds.part_content_get("day0")
        sunday.signal_emit("elm,type,weekend,style1", "")
    except:
        print("BUG HERE !!!")


    win.resize(350, 120)
    win.show()
# }}}


#----- Main -{{{-
if __name__ == "__main__":
    elementary.init()

    dayselector_clicked(None)

    elementary.run()
    elementary.shutdown()
# }}}
# vim:foldmethod=marker
