
#ifndef __EWL_SPINNER_H
#define __EWL_SPINNER_H

struct _ewl_spinner {
	Ewl_Widget widget;
	double min_val;
	double max_val;
	double value;
	int digits;
	Ewl_Widget *entry;
	Ewl_Widget *button_increase;
	Ewl_Widget *button_decrease;
};

typedef struct _ewl_spinner Ewl_Spinner;

#define EWL_SPINNER(spinner) ((Ewl_Spinner *) spinner)

Ewl_Widget *ewl_spinner_new();
void ewl_spinner_set_value(Ewl_Widget * widget, double value);
double ewl_spinner_get_value(Ewl_Widget * widget);
void ewl_spinner_set_digits(Ewl_Widget * widget, int digits);
void ewl_spinner_set_min_val(Ewl_Widget * widget, double val);
void ewl_spinner_set_max_val(Ewl_Widget * widget, double val);

#endif
