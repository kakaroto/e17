
#ifndef __EWL_SEEKER_H__
#define __EWL_SEEKER_H__

typedef struct _ewl_seeker Ewl_Seeker;
#define EWL_SEEKER(seeker) ((Ewl_Seeker *) seeker)

struct _ewl_seeker {
	Ewl_Container container;
	Ewl_Orientation orientation;
	Ewl_Widget * drag_button;
	double value;
	double range;
	double jump;

	int dragstart;
};

Ewl_Widget *ewl_seeker_new(Ewl_Orientation orientation);
#define ewl_vseeker_new() ewl_seeker_new(EWL_ORIENTATION_VERTICAL);
#define ewl_hseeker_new() ewl_seeker_new(EWL_ORIENTATION_HORIZONTAL);

void ewl_seeker_set_value(Ewl_Widget * w, double v);
double ewl_seeker_get_value(Ewl_Widget * w);
void ewl_seeker_set_range(Ewl_Widget * w, double r);
double ewl_seeker_get_range(Ewl_Widget * w);

#endif				/* __EWL_SEEKER_H__ */
