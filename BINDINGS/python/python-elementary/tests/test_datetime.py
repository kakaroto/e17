# encoding: utf-8
import os
import elementary
import edje
import ecore
import evas

from datetime import datetime

def changed_cb(obj):
    printf("Datetime value is changed\n")

def datetime_clicked(obj):
    win = elementary.StandardWindow("dt", "DateTime")
    win.autodel = True

    bx = elementary.Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    win.resize_object_add(bx)
    bx.horizontal = False
    bx.show()
    bx.size_hint_min_set(360, 240)

    dt = elementary.Datetime(bx)
    dt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    dt.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    dt.field_visible_set(elementary.ELM_DATETIME_HOUR, False)
    dt.field_visible_set(elementary.ELM_DATETIME_MINUTE, False)
    dt.field_visible_set(elementary.ELM_DATETIME_AMPM, False)
    bx.pack_end(dt)
    dt.show()

    dt = elementary.Datetime(bx)
    dt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    dt.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)
    dt.field_visible_set(elementary.ELM_DATETIME_YEAR, False)
    dt.field_visible_set(elementary.ELM_DATETIME_MONTH, False)
    dt.field_visible_set(elementary.ELM_DATETIME_DATE, False)
    bx.pack_end(dt)
    dt.show()

    dt = elementary.Datetime(bx)
    dt.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    dt.size_hint_align_set(evas.EVAS_HINT_FILL, 0.5)

    # get the current local time
    time1 = datetime.now()
    # set the max year as 2030 and the remaining fields are equal to current time values
    time1.replace(year = 130)
    dt.value_max = time1
    # set the min time limit as "1980 January 10th 02:30 PM"
    time1.replace(year = 80, month = 4, day = 10, hour = 14, minute = 30)
    dt.value_min = time1
    # minutes can be input only in between 15 and 45
    dt.field_limit = (elementary.ELM_DATETIME_MINUTE, 15, 45)
    dt.callback_changed_add(changed_cb)
    bx.pack_end(dt)
    dt.show()

    win.show()

if __name__ == "__main__":
    elementary.init()

    datetime_clicked(None)

    elementary.run()
    elementary.shutdown()
