#include <Ewl.h>
#include <time.h>
#include "ewl_debug.h"
#include <Evas.h>
#include "ewl_macros.h"
#include <stdlib.h>
#include <assert.h>

char *months[] = { "January","February","March","April","May","June","July","August","September","October","November","December" };

int mdays[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

static int ewl_calendar_leap_year_detect(unsigned int year)
{
    assert(year > 1581);
    return(((year % 4 == 0) && (year % 100)) || (year % 400 == 0));
}

void ewl_calendar_highlight_today(struct tm* now, Ewl_Label* day, Ewl_Calendar* cal) {
	/*Get the day*/
	int i = atoi(ewl_label_text_get(EWL_LABEL(day)));
	if (i == now->tm_mday && now->tm_year+1900 == cal->cur_year && now->tm_mon == cal->cur_month) {
			ewl_widget_color_set(EWL_WIDGET(day), 0,0,255,255);
			/*printf("Doing..\n");	*/
	} else {
	/*	printf("%d %d %d ---- %d %d %d\n", now->tm_mday, now->tm_mon, now->tm_year+1900, i, cal->cur_month, cal->cur_year);*/
	}
}


void ewl_calendar_day_select (Ewl_Widget *w, void *ev_data, void *user_data) {
	struct tm* now;
	time_t now_tm;
	int i;

	
	now_tm = time(NULL);
	now = localtime(&now_tm);
	i = atoi(ewl_label_text_get(EWL_LABEL(w)));

	ewl_container_child_iterate_begin(EWL_CONTAINER(EWL_CALENDAR(user_data)->grid));
	

	Ewl_Widget* it;
	while ( (it = ewl_container_child_next(EWL_CONTAINER(EWL_CALENDAR(user_data)->grid))) != NULL ) {
			ewl_widget_color_set(EWL_WIDGET(it), 255,255,255,255);
			ewl_calendar_highlight_today(now, EWL_LABEL(it), EWL_CALENDAR(user_data));
	}

	
	ewl_widget_color_set(w, 255,0,0,255);
	EWL_CALENDAR(user_data)->cur_day = i;

}

void ewl_calendar_day_pick (Ewl_Widget *w, void *ev_data, void *user_data) {
	ewl_callback_call(EWL_WIDGET(user_data), EWL_CALLBACK_VALUE_CHANGED);
}

void ewl_calendar_prev_month_cb (Ewl_Widget *w, void *ev_data, void *user_data) {
        Ewl_Calendar* ib = EWL_CALENDAR(user_data);

	/*printf ("Go to prev month\n");		*/
	ib->cur_month -= 1;
	if (ib->cur_month < 0) { ib->cur_month = 11; ib->cur_year--; }
	ewl_calendar_grid_setup(ib);
}

void ewl_calendar_next_month_cb (Ewl_Widget *w, void *ev_data, void *user_data) {
        Ewl_Calendar* ib = EWL_CALENDAR(user_data);

	/*printf ("Go to next month\n");*/
	ib->cur_month += 1;
	if (ib->cur_month > 11) { ib->cur_month = 0; ib->cur_year++; }
	ewl_calendar_grid_setup(ib);
}

void ewl_calendar_add_day_labels(Ewl_Calendar* ib) {
	Ewl_Widget* day_label;
	
	/* Add the days*/
	day_label = ewl_label_new("M");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 1, 1, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("T");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 2, 2, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("W");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 3, 3, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("T");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 4, 4, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("F");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 5, 5, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("S");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 6, 6, 1, 1);
	ewl_widget_show(day_label);

	day_label = ewl_label_new("S");
	ewl_grid_add(EWL_GRID(ib->grid), day_label, 7, 7, 1, 1);
	ewl_widget_show(day_label);
}

/**
 * @param cal: The calendar to get the date frm
 * @param str: a pre-initialized char* pointer to insert the date into
 * @return none 
 * @brief Returns an ASCII formatted representation of the selected date
 *
 * Inserts an ASCII formatted string of the currently selected date into the char* str pointer
 */
void ewl_calendar_ascii_time_get(Ewl_Calendar* cal, char* str) {
	time_t tm;
	struct tm* month_start;
	
	tm = time(NULL);
	month_start = localtime(&tm);
	month_start->tm_mday = cal->cur_day;
	month_start->tm_mon = cal->cur_month;
	month_start->tm_year = cal->cur_year-1900;
	mktime(month_start);

	strcpy(str,asctime(month_start));

}


void ewl_calendar_grid_setup(Ewl_Calendar* cal) {
	struct tm* month_start;
	struct tm* now;
	char display_top[50];
	time_t tm;
	time_t now_tm;
	int today = 0;
	int cur_row, cur_col, cur_day, days=30;
	Ewl_Widget *day_label;

	ewl_grid_reset(EWL_GRID(cal->grid),7,7);
	ewl_calendar_add_day_labels(cal);

	/*Get the start time..*/
	/*tm = time(NULL);
	ptr = localtime(&tm);
	printf("%d\n", ptr->tm_mon);*/

	/*Make the initial display..*/
	sprintf(display_top, "%s %d", months[cal->cur_month], cal->cur_year);
	ewl_label_text_set(EWL_LABEL(cal->month_label), display_top);
	today = cal->cur_day;
	

	/*Get the DOW of the first day of this month */
	tm = time(NULL);
	month_start = localtime(&tm);
	month_start->tm_mday = 0;
	month_start->tm_mon = cal->cur_month;
	month_start->tm_year = cal->cur_year-1900;
	mktime(month_start);
	/*printf ("First day of this month (%d), year (%d), DOW is %d\n", cal->cur_month, cal->cur_year-1900, month_start->tm_wday);*/

	/* Now add the days to this month */
	cur_row = 2;
	cur_col = month_start->tm_wday + 1;
	/*printf("Col: %d\n", cur_col);*/
	if (cur_col > 7) {
		cur_row=2;
		cur_col=1;
	}
	
	cur_day = 0;
	now_tm = time(NULL);
	now = localtime(&now_tm);

	days = mdays[cal->cur_month];
	/*If february, do leap years...*/
	if (cal->cur_month == 1) {
		if (ewl_calendar_leap_year_detect(cal->cur_year)) {
			days = 29;
		} else {
			days = 28;
		}
		
	}
	
	while (cur_day < days) {
		char day[3];
		
		if (cur_col > 7) {
			cur_row++;
			cur_col = 1;
		}

		sprintf(day, "%d", cur_day+1); 
		day_label = ewl_label_new(day);
		ewl_callback_append(EWL_WIDGET(day_label), EWL_CALLBACK_MOUSE_DOWN,ewl_calendar_day_select, cal);
		ewl_callback_append(EWL_WIDGET(day_label), EWL_CALLBACK_DOUBLE_CLICKED,ewl_calendar_day_pick, cal);

		ewl_grid_add(EWL_GRID(cal->grid), day_label, cur_col, cur_col, cur_row, cur_row);
		ewl_calendar_highlight_today(now, EWL_LABEL(day_label), cal);
		/*printf("Day: %d:%d, Year: %d:%d, Month: %d:%d\n",now->tm_mday,cur_day,  now->tm_year+1900,cal->cur_year,  now->tm_mon, cal->cur_month);*/
		ewl_widget_show(day_label);

		cur_col++;
		cur_day++;
		
	}

}


/**
 * @return Returns NULL on failure, a new Ewl_Calendar on success
 * @brief Creates a new Ewl_Calendar
 *
 * Creates a new Ewl_Calendar object
 */
Ewl_Widget *ewl_calendar_new() {
	Ewl_Calendar* ib;
	DENTER_FUNCTION (DLEVEL_STABLE);

	ib = NEW(Ewl_Calendar, 1);
	if (!ib) {
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_calendar_init(ib)) {
		printf("Failed calendar init...\n");
		FREE(ib);
		ib = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(ib), DLEVEL_STABLE);
}

/**
 * @param ib: The calendar widget to initialize
 * @return Returns NULL on failure, a new Ewl_IconBox on success
 * @brief Init a new Ewl_Calendar to default values and callbacks, and set date to today
 */
int ewl_calendar_init(Ewl_Calendar* ib) {
	Ewl_Widget *w;
	Ewl_Widget *vbox;
	Ewl_Widget *top_hbox;
	Ewl_Widget *prev_button;
	Ewl_Widget *next_button;
	
	
	struct tm *ptr;
	
	time_t tm;
	

	/*printf("Configuring the calendar..\n");*/
	
	

	w = EWL_WIDGET(ib);
	
	if (!ewl_box_init(EWL_BOX(ib), EWL_ORIENTATION_HORIZONTAL))
			DRETURN_INT(FALSE, DLEVEL_STABLE);

	/* Init ewl setup */
	ewl_widget_appearance_set(EWL_WIDGET(ib), "calendar");
	ewl_widget_inherit(EWL_WIDGET(w), "calendar");

	
	ib->grid = ewl_grid_new(7,7);
	ib->month_label = ewl_label_new("Disp");

	/*printf("Getting date..");*/
	/*Get the start time..*/
	tm = time(NULL);
	ptr = localtime(&tm);
	/*printf("%d\n", ptr->tm_mon);*/
	ib->cur_month = ptr->tm_mon;
	ib->cur_day  = ptr->tm_mday;
	ib->cur_year = ptr->tm_year + 1900;
	/*printf("..done\n");*/

	ewl_calendar_grid_setup(ib);


	prev_button = ewl_button_new("<");
	next_button = ewl_button_new(">");
	top_hbox = ewl_hbox_new();
	vbox = ewl_vbox_new();

	/*printf("Showing widgets..\n");	*/
	ewl_widget_show(vbox);
	ewl_widget_show(top_hbox);
	ewl_widget_show(ib->grid);
	/*printf("Showing widgets..\n");*/
	ewl_widget_show(ib->month_label);
	ewl_widget_show(next_button);
	ewl_widget_show(prev_button);
	/*printf("Showing widgets..\n");*/
	ewl_container_child_append(EWL_CONTAINER(vbox), top_hbox);
	ewl_container_child_append(EWL_CONTAINER(top_hbox), prev_button);
	/*printf("Showing widgets..\n");*/
	ewl_container_child_append(EWL_CONTAINER(top_hbox), ib->month_label);
	ewl_container_child_append(EWL_CONTAINER(top_hbox), next_button);	
	/*printf("..done\n");*/

	ewl_container_child_append(EWL_CONTAINER(vbox), ib->grid);
	ewl_object_maximum_size_set(EWL_OBJECT(prev_button), 20,10);
	ewl_object_maximum_size_set(EWL_OBJECT(next_button), 20,10);
	ewl_object_maximum_h_set(EWL_OBJECT(ib->month_label), 10);
	ewl_object_minimum_w_set(EWL_OBJECT(vbox), 150);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_VSHRINK);
	ewl_object_fill_policy_set(EWL_OBJECT(ib->grid), EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(ib), EWL_FLAG_FILL_FILL);
	ewl_object_minimum_h_set(EWL_OBJECT(ib->grid), 100);


	
	ewl_container_child_append(EWL_CONTAINER(ib), vbox);

	
	//Add the prev/next callbacks
	ewl_callback_append(prev_button, EWL_CALLBACK_MOUSE_DOWN, ewl_calendar_prev_month_cb, ib);
	ewl_callback_append(next_button, EWL_CALLBACK_MOUSE_DOWN, ewl_calendar_next_month_cb, ib);

	/*printf("Setup the calendar...\n");*/
	DRETURN_INT(TRUE, DLEVEL_STABLE);

}


