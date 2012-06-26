# encoding: utf-8
import os
from elementary import *
import edje
import ecore
import evas

from datetime import datetime

api = {
    "state" : 0,  # What state we are testing
    "box" : None     # box used in set_api_state
}

STATE_MARK_MONTHLY = 0
STATE_MARK_WEEKLY = 1
STATE_SUNDAY_HIGHLIGHT = 2
STATE_SELECT_DATE_DISABLED_WITH_MARKS = 3
STATE_SELECT_DATE_DISABLED_NO_MARKS = 4
API_STATE_LAST = 5

def set_api_state(api):
    items = api["box"].children
    if not items:
        return
    m = None

    if api["state"] == STATE_MARK_MONTHLY:
        cal = items[0]
        cal.min_max_year = (2010, 2011)
        the_time = datetime(2010, 12, 31)
        m = cal.mark_add("checked", the_time, ELM_CALENDAR_MONTHLY)
        cal.selected_time = the_time
    elif api["state"] == STATE_MARK_WEEKLY:
        cal = items[0]
        the_time = datetime(2010, 12, 26)
        if m is not None:
            m.delete()
        m = cal.mark_add("checked", the_time, ELM_CALENDAR_WEEKLY)
        cal.selected_time = the_time
    elif api["state"] == STATE_SUNDAY_HIGHLIGHT:
        cal = items[0]
        the_time = datetime(2010, 12, 25)
        # elm_calendar_mark_del(m)
        m = cal.mark_add("holiday", the_time, ELM_CALENDAR_WEEKLY)
        cal.selected_time = the_time
    elif api["state"] == STATE_SELECT_DATE_DISABLED_WITH_MARKS:
        cal = items[0]
        the_time = datetime(2011, 01, 01)
        cal.select_mode = ELM_CALENDAR_SELECT_MODE_NONE
        cal.selected_time = the_time
    elif api["state"] == STATE_SELECT_DATE_DISABLED_NO_MARKS:
        cal = items[0]
        the_time = datetime(2011, 02, 01)
        del cal.marks
        cal.select_mode = ELM_CALENDAR_SELECT_MODE_NONE
        cal.selected_time = the_time
    elif api["state"] == API_STATE_LAST:
        return
    else:
        return

def api_bt_clicked(bt, a):
    print("clicked event on API Button: api_state=<%d>\n" % a["state"])
    set_api_state(a)
    a["state"] = a["state"] + 1
    bt.text = "Next API function (%d)" % a["state"]
    if a["state"] == API_STATE_LAST:
        bt.disabled = True

# A simple test, just displaying calendar in it's default state
def calendar_clicked(obj):
    win = StandardWindow("calendar", "Calendar")
    win.autodel = True

    bxx = Box(win)
    win.resize_object_add(bxx)
    bxx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bxx.show()

    bx = Box(win)
    bx.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    api["box"] = bx
    bx.show()

    bt = Button(win)
    bt.text = "Next API function"
    bt.callback_clicked_add(api_bt_clicked, api)
    bxx.pack_end(bt)
    if api["state"] == API_STATE_LAST:
        bt.disabled = True
    bt.show()

    bxx.pack_end(bx)

    cal = Calendar(win)
    cal.first_day_of_week = ELM_DAY_MONDAY
    cal.size_hint_weight_set(evas.EVAS_HINT_EXPAND, evas.EVAS_HINT_EXPAND)
    bx.pack_end(cal)

    the_time = datetime(2010, 12, 31)
    cal.selected_time = the_time
    cal.min_max_year = (2010, 2012)

    cal.show()

    win.show()

"""
def print_cal_info(Evas_Object *cal, Evas_Object *en):
    char info[1024]
    double interval
    int year_min, year_max
    Eina_Bool sel_enabled
    const char **wds
    struct tm stm

    if (!elm_calendar_selected_time_get(cal, &stm))
      return

    interval = elm_calendar_interval_get(cal)
    elm_calendar_min_max_year_get(cal, &year_min, &year_max)
    sel_enabled = !!(elm_calendar_select_mode_get(cal) != ELM_CALENDAR_SELECT_MODE_NONE)
    wds = elm_calendar_weekdays_names_get(cal)

    snprintf(info, sizeof(info),
                "  Day: %i, Mon: %i, Year %i, WeekDay: %i<br/>"
                "  Interval: %0.2f, Year_Min: %i, Year_Max %i, Sel Enabled : %i<br/>"
                "  Weekdays: %s, %s, %s, %s, %s, %s, %s<br/>",
                stm.tm_mday, stm.tm_mon, stm.tm_year + 1900, stm.tm_wday,
                interval, year_min, year_max, sel_enabled,
                wds[0], wds[1], wds[2], wds[3], wds[4], wds[5], wds[6])

    elm_object_text_set(en, info)

def _print_cal_info_cb(void *data, Evas_Object *obj, void *event_info):
    _print_cal_info(obj, data)

static char *
_format_month_year(struct tm *stm)
    char buf[32]
    if (!strftime(buf, sizeof(buf), "%b %y", stm)) return NULL
    return strdup(buf)

# A test intended to cover all the calendar api and much use cases as possible
def test_calendar2(obj):
    Evas_Object *win, *bx, *bxh, *cal, *cal2, *cal3, *en
    Elm_Calendar_Mark *mark
    struct tm selected_time
    time_t current_time
    const char *weekdays[] =
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    }

    win = elm_win_util_standard_add("calendar2", "Calendar 2")
    elm_win_autodel_set(win, EINA_TRUE)

    bx = elm_box_add(win)
    evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    elm_win_resize_object_add(win, bx)
    evas_object_show(bx)

    bxh = elm_box_add(win)
    elm_box_horizontal_set(bxh, EINA_TRUE)
    evas_object_size_hint_weight_set(bxh, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_size_hint_align_set(bxh, EVAS_HINT_FILL, EVAS_HINT_FILL)
    evas_object_show(bxh)
    elm_box_pack_end(bx, bxh)

    cal = elm_calendar_add(win)
    evas_object_size_hint_weight_set(cal, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_size_hint_align_set(cal, EVAS_HINT_FILL, EVAS_HINT_FILL)
    evas_object_show(cal)
    elm_box_pack_end(bx, cal)

    cal2 = elm_calendar_add(win)
    evas_object_size_hint_weight_set(cal2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_size_hint_align_set(cal2, EVAS_HINT_FILL, EVAS_HINT_FILL)
    elm_calendar_select_mode_set(cal2, ELM_CALENDAR_SELECT_MODE_NONE)
    evas_object_show(cal2)
    elm_box_pack_end(bxh, cal2)

    cal3 = elm_calendar_add(win)
    evas_object_size_hint_weight_set(cal3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_size_hint_align_set(cal3, EVAS_HINT_FILL, EVAS_HINT_FILL)
    current_time = time(NULL) + 34 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_selected_time_set(cal3, &selected_time)
    current_time = time(NULL) + 1 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal3, "checked", &selected_time, ELM_CALENDAR_UNIQUE)
    elm_calendar_marks_clear(cal3)
    current_time = time(NULL)
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal3, "checked", &selected_time, ELM_CALENDAR_DAILY)
    elm_calendar_mark_add(cal3, "holiday", &selected_time, ELM_CALENDAR_DAILY)
    elm_calendar_marks_draw(cal3)
    evas_object_show(cal3)
    elm_box_pack_end(bxh, cal3)

    en = elm_entry_add(win)
    evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL)
    evas_object_show(en)
    elm_box_pack_end(bx, en)
    elm_entry_editable_set(en, EINA_FALSE)
    evas_object_show(win)

    elm_calendar_min_max_year_set(cal3, -1, -1)

    elm_calendar_weekdays_names_set(cal, weekdays)
    elm_calendar_first_day_of_week_set(cal, ELM_DAY_SATURDAY)
    elm_calendar_interval_set(cal, 0.4)
    elm_calendar_format_function_set(cal, _format_month_year)
    elm_calendar_min_max_year_set(cal, 2010, 2020)

    current_time = time(NULL) + 4 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal, "holiday", &selected_time, ELM_CALENDAR_ANNUALLY)

    current_time = time(NULL) + 1 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal, "checked", &selected_time, ELM_CALENDAR_UNIQUE)

    current_time = time(NULL) - 363 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal, "checked", &selected_time, ELM_CALENDAR_MONTHLY)

    current_time = time(NULL) - 5 * 84600
    localtime_r(&current_time, &selected_time)
    mark = elm_calendar_mark_add(cal, "holiday", &selected_time,
                      ELM_CALENDAR_WEEKLY)

    current_time = time(NULL) + 1 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_mark_add(cal, "holiday", &selected_time, ELM_CALENDAR_WEEKLY)

    elm_calendar_mark_del(mark)
    elm_calendar_marks_draw(cal)

    _print_cal_info(cal, en)
    evas_object_smart_callback_add(cal, "changed", _print_cal_info_cb, en)

def test_calendar3(obj):
    Evas_Object *win, *cal, *bxx
    struct tm selected_time
    time_t current_time

    win = elm_win_util_standard_add("calendar", "Calendar")
    elm_win_autodel_set(win, EINA_TRUE)

    bxx = elm_box_add(win)
    elm_win_resize_object_add(win, bxx)
    evas_object_size_hint_weight_set(bxx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    evas_object_show(bxx)

    cal = elm_calendar_add(win)
    elm_calendar_first_day_of_week_set(cal, ELM_DAY_THURSDAY)
    elm_calendar_select_mode_set(cal, ELM_CALENDAR_SELECT_MODE_ONDEMAND)
    current_time = time(NULL) + 34 * 84600
    localtime_r(&current_time, &selected_time)
    elm_calendar_selected_time_set(cal, &selected_time)
    evas_object_size_hint_weight_set(cal, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND)
    elm_box_pack_end(bxx, cal)

    evas_object_show(cal)

    evas_object_show(win)
"""
if __name__ == "__main__":
    elementary.init()

    calendar_clicked(None)

    elementary.run()
    elementary.shutdown()
