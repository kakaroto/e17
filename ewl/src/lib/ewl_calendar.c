/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <time.h>
#include "ewl_base.h"
#include "ewl_calendar.h"
#include "ewl_icon.h"
#include "ewl_grid.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_calendar_grid_setup(Ewl_Calendar *cal);
static int ewl_calendar_leap_year_detect(unsigned int year);
static void ewl_calendar_highlight_today(struct tm *now, Ewl_Label *day,
                                                Ewl_Calendar *cal);
static void ewl_calendar_day_select_cb(Ewl_Widget *w, void *ev_data,
                                        void *user_data);
static void ewl_calendar_day_pick_cb(Ewl_Widget *w, void *ev_data,
                                        void *user_data);
static void ewl_calendar_prev_month_cb(Ewl_Widget *w, void *ev_data,
                                        void *user_data);
static void ewl_calendar_next_month_cb(Ewl_Widget *w, void *ev_data,
                                        void *user_data);
static void ewl_calendar_add_day_labels(Ewl_Calendar *ib);


static const char * const months[] = {"January", "February", "March", "April",
                        "May", "June", "July", "August", "September",
                        "October", "November", "December"};

static const int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 * @return Returns NULL on failure, a new Ewl_Calendar on success
 * @brief Creates a new Ewl_Calendar
 *
 * Creates a new Ewl_Calendar object
 */
Ewl_Widget *
ewl_calendar_new(void)
{
        Ewl_Calendar* ib;
        DENTER_FUNCTION(DLEVEL_STABLE);

        ib = NEW(Ewl_Calendar, 1);
        if (!ib)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_calendar_init(ib)) {
                ewl_widget_destroy(EWL_WIDGET(ib));
                ib = NULL;
        }

        DRETURN_PTR(EWL_WIDGET(ib), DLEVEL_STABLE);
}

/**
 * @param ib: The calendar widget to initialize
 * @return Returns FALSE on failure, a TRUE on success
 * @brief Init a new Ewl_Calendar to default values and callbacks, and set date to today
 */
int
ewl_calendar_init(Ewl_Calendar* ib)
{
        Ewl_Widget *w, *vbox, *top_hbox, *o;
        struct tm *ptr;
        time_t tm;
        const char *icon;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(ib, FALSE);

        w = EWL_WIDGET(ib);
        if (!ewl_box_init(EWL_BOX(ib)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_box_orientation_set(EWL_BOX(ib), EWL_ORIENTATION_HORIZONTAL);
        ewl_widget_appearance_set(w, EWL_CALENDAR_TYPE);
        ewl_widget_inherit(w, EWL_CALENDAR_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_FILL);

        vbox = ewl_vbox_new();
        ewl_container_child_append(EWL_CONTAINER(ib), vbox);
        ewl_widget_internal_set(vbox, TRUE);
        ewl_widget_show(vbox);

        top_hbox = ewl_hbox_new();
        ewl_container_child_append(EWL_CONTAINER(vbox), top_hbox);
        ewl_widget_internal_set(top_hbox, TRUE);
        ewl_widget_show(top_hbox);

        o = ewl_icon_new();
        ewl_container_child_append(EWL_CONTAINER(top_hbox), o);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_LEFT);
        icon = ewl_icon_theme_icon_path_get(EWL_ICON_GO_PREVIOUS,
                                                EWL_ICON_SIZE_SMALL);
        if (icon)
                ewl_icon_image_set(EWL_ICON(o), icon, EWL_ICON_GO_PREVIOUS);
        ewl_icon_alt_text_set(EWL_ICON(o), "<<");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_calendar_prev_month_cb, ib);
        ewl_widget_show(o);

        ib->month_label = ewl_label_new();
        ewl_object_fill_policy_set(EWL_OBJECT(ib->month_label),
                                                EWL_FLAG_FILL_HFILL |
                                                EWL_FLAG_FILL_VSHRINKABLE);
        ewl_object_alignment_set(EWL_OBJECT(ib->month_label),
                                                EWL_FLAG_ALIGN_CENTER);
        ewl_container_child_append(EWL_CONTAINER(top_hbox), ib->month_label);
        ewl_widget_show(ib->month_label);

        o = ewl_icon_new();
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_SHRINKABLE);
        ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_RIGHT);
        ewl_container_child_append(EWL_CONTAINER(top_hbox), o);
        icon = ewl_icon_theme_icon_path_get(EWL_ICON_GO_NEXT,
                                                EWL_ICON_SIZE_SMALL);
        if (icon)
                ewl_icon_image_set(EWL_ICON(o), icon, EWL_ICON_GO_NEXT);
        ewl_icon_alt_text_set(EWL_ICON(o), ">>");
        ewl_callback_append(o, EWL_CALLBACK_CLICKED,
                                        ewl_calendar_next_month_cb, ib);
        ewl_widget_show(o);

        ib->grid = ewl_grid_new();
        ewl_grid_dimensions_set(EWL_GRID(ib->grid), 7, 7);
        ewl_container_child_append(EWL_CONTAINER(vbox), EWL_WIDGET(ib->grid));
        ewl_object_fill_policy_set(EWL_OBJECT(ib->grid), EWL_FLAG_FILL_FILL);
        ewl_widget_show(ib->grid);

        /* Get the start time.. */
        tm = time(NULL);
        ptr = localtime(&tm);

        ib->cur_month = ptr->tm_mon;
        ib->cur_day  = ptr->tm_mday;
        ib->cur_year = ptr->tm_year + 1900;

        ewl_calendar_grid_setup(ib);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param cal: The calendar to get the date frm
 * @return Returns the string representing the selected date. This string
 * must be free'd
 * @brief Returns an ASCII formatted representation of the selected date
 * the user must freet this string.
 */
char *
ewl_calendar_ascii_time_get(Ewl_Calendar *cal)
{
        time_t tm;
        struct tm* month_start;
        char str[1024];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cal, NULL);
        DCHECK_TYPE_RET(cal, EWL_CALENDAR_TYPE, NULL);

        tm = time(NULL);
        month_start = localtime(&tm);
        month_start->tm_mday = cal->cur_day;
        month_start->tm_mon = cal->cur_month;
        month_start->tm_year = cal->cur_year - 1900;
        mktime(month_start);

        snprintf(str, sizeof(str), "%s", asctime(month_start));

        DRETURN_PTR(strdup(str), DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Calendar to get the day from
 * @return Returns the day currently selected in the calendar
 * @brief Returns the current day selected in the calendar
 */
int
ewl_calendar_day_get(Ewl_Calendar *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);
        DCHECK_TYPE_RET(c, EWL_CALENDAR_TYPE, FALSE);

        DRETURN_INT(c->cur_day, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Calendar to get the month from
 * @return Returns the month currently selected in the calendar
 * @brief Returns the month selected in the calendar
 */
int
ewl_calendar_month_get(Ewl_Calendar *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);
        DCHECK_TYPE_RET(c, EWL_CALENDAR_TYPE, FALSE);

        DRETURN_INT(c->cur_month, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Calendar to get the year from
 * @return Returns the current year selected in the calendar
 * @brief Returns the year currently selected in the calendar
 */
int
ewl_calendar_year_get(Ewl_Calendar *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);
        DCHECK_TYPE_RET(c, EWL_CALENDAR_TYPE, FALSE);

        DRETURN_INT(c->cur_year, DLEVEL_STABLE);
}

static void
ewl_calendar_grid_setup(Ewl_Calendar *cal)
{
        struct tm *date;
        char display_top[50];
        time_t tm;
        int cur_row, cur_col, cur_day, days = 30;
        Ewl_Widget *day_label;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cal);
        DCHECK_TYPE(cal, EWL_CALENDAR_TYPE);

        ewl_container_reset(EWL_CONTAINER(cal->grid));
        ewl_calendar_add_day_labels(cal);

        /* Make the initial display */
        snprintf(display_top, sizeof(display_top), "%s %d",
                                months[cal->cur_month], cal->cur_year);
        ewl_label_text_set(EWL_LABEL(cal->month_label), display_top);

        /* Get the DAY of the first day of this month */
        tm = time(NULL);
        date = localtime(&tm);
        date->tm_mday = 0;
        date->tm_mon = cal->cur_month;
        date->tm_year = cal->cur_year - 1900;
        mktime(date);

        /* Now add the days to this month */
        cur_row = 2;
        cur_col = date->tm_wday + 1;
        if (cur_col > 7) {
                cur_row = 2;
                cur_col = 1;
        }

        cur_day = 0;
        tm = time(NULL);
        date = localtime(&tm);

        days = mdays[cal->cur_month];
        /* If february, do leap years... */
        if (cal->cur_month == 1) {
                if (ewl_calendar_leap_year_detect(cal->cur_year))
                        days = 29;
                else
                        days = 28;
        }

        while (cur_day < days) {
                char day[3];

                if (cur_col > 7) {
                        cur_row++;
                        cur_col = 1;
                }

                snprintf(day, sizeof(day), "%d", cur_day + 1);
                day_label = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(day_label), day);
                ewl_object_alignment_set(EWL_OBJECT(day_label),
                                                        EWL_FLAG_ALIGN_RIGHT);
                ewl_callback_append(EWL_WIDGET(day_label),
                                        EWL_CALLBACK_MOUSE_DOWN,
                                        ewl_calendar_day_select_cb, cal);
                ewl_callback_append(EWL_WIDGET(day_label),
                                        EWL_CALLBACK_CLICKED,
                                        ewl_calendar_day_pick_cb, cal);

                ewl_container_child_append(EWL_CONTAINER(cal->grid), day_label);
                ewl_grid_child_position_set(EWL_GRID(cal->grid),
                                                day_label, cur_col - 1,
                                                cur_col - 1, cur_row - 1,
                                                cur_row - 1);
                ewl_calendar_highlight_today(date, EWL_LABEL(day_label), cal);
                ewl_widget_show(day_label);

                cur_col++;
                cur_day++;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_calendar_leap_year_detect(unsigned int year)
{
        int leap = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);

        /* FIXME: Properly handle leap year's prior to 1581. */
        if (year <= 1581) year = 1581;
        leap = (((year % 4 == 0) && (year % 100)) || (year % 400 == 0));

        DRETURN_INT(leap, DLEVEL_STABLE);
}

static void
ewl_calendar_highlight_today(struct tm *now, Ewl_Label *day,
                                        Ewl_Calendar *cal)
{
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(now);
        DCHECK_PARAM_PTR(day);
        DCHECK_PARAM_PTR(cal);
        DCHECK_TYPE(day, EWL_LABEL_TYPE);
        DCHECK_TYPE(cal, EWL_CALENDAR_TYPE);

        /* Get the day */
        i = atoi(ewl_label_text_get(EWL_LABEL(day)));
        if ((i == now->tm_mday) && ((now->tm_year + 1900) == cal->cur_year)
                        && (now->tm_mon == cal->cur_month)) {
                ewl_widget_color_set(EWL_WIDGET(day), 0, 0, 255, 255);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_calendar_day_select_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data)
{
        struct tm *now;
        time_t now_tm;
        int i;
        Ewl_Widget* it;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(user_data, EWL_CALENDAR_TYPE);

        now_tm = time(NULL);
        now = localtime(&now_tm);
        i = atoi(ewl_label_text_get(EWL_LABEL(w)));

        ewl_container_child_iterate_begin(EWL_CONTAINER(EWL_CALENDAR(user_data)->grid));
        while ((it = ewl_container_child_next(
                        EWL_CONTAINER(EWL_CALENDAR(user_data)->grid))) != NULL) {
                ewl_widget_color_set(EWL_WIDGET(it), 255, 255, 255, 255);
                ewl_calendar_highlight_today(now, EWL_LABEL(it), EWL_CALENDAR(user_data));
        }

        ewl_widget_color_set(w, 255, 0, 0, 255);
        EWL_CALENDAR(user_data)->cur_day = i;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_calendar_day_pick_cb(Ewl_Widget *w __UNUSED__, void *ev_data,
                                                void *user_data)
{
        Ewl_Event_Mouse_Down *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_WIDGET_TYPE);

        ev = ev_data;
        if (ev->clicks == 2)
                ewl_callback_call(EWL_WIDGET(user_data),
                                EWL_CALLBACK_VALUE_CHANGED);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_calendar_prev_month_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                                void *user_data)
{
        Ewl_Calendar *ib;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_CALENDAR_TYPE);

        ib = EWL_CALENDAR(user_data);
        ib->cur_month -= 1;
        if (ib->cur_month < 0) {
                ib->cur_month = 11;
                ib->cur_year--;
        }
        ewl_calendar_grid_setup(ib);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_calendar_next_month_cb(Ewl_Widget *w __UNUSED__, void *ev_data __UNUSED__,
                                                void *user_data)
{
        Ewl_Calendar *ib;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(user_data, EWL_CALENDAR_TYPE);

        ib = EWL_CALENDAR(user_data);
        ib->cur_month += 1;
        if (ib->cur_month > 11) {
                ib->cur_month = 0;
                ib->cur_year++;
        }
        ewl_calendar_grid_setup(ib);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_calendar_add_day_labels(Ewl_Calendar *ib)
{
        Ewl_Widget* day_label;
        char *days[] = {"M", "T", "W", "T", "F", "S", "S", NULL};
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(ib);
        DCHECK_TYPE(ib, EWL_CALENDAR_TYPE);

        for (i = 0; days[i] != NULL; i++)
        {
                day_label = ewl_label_new();
                ewl_label_text_set(EWL_LABEL(day_label), days[i]);
                ewl_container_child_append(EWL_CONTAINER(ib->grid),
                                                        day_label);
                ewl_object_alignment_set(EWL_OBJECT(day_label),
                                                        EWL_FLAG_ALIGN_CENTER);
                ewl_widget_show(day_label);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

