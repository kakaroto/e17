
#ifndef __EWL_SEEKER_H__
#define __EWL_SEEKER_H__

typedef struct _ewl_seeker Ewl_Seeker;

#define EWL_SEEKER(seeker) ((Ewl_Seeker *) seeker)

struct _ewl_seeker {
	Ewl_Container   container;
	Ewl_Orientation orientation;
	Ewl_Widget     *dragbar;
	double          value;
	double          range;
	double          step;

	int             dragstart;
};

#define ewl_vseeker_new() ewl_seeker_new(EWL_ORIENTATION_VERTICAL);
#define ewl_hseeker_new() ewl_seeker_new(EWL_ORIENTATION_HORIZONTAL);

Ewl_Widget     *ewl_seeker_new(Ewl_Orientation orientation);
void            ewl_seeker_init(Ewl_Seeker * s, Ewl_Orientation orientation);

void            ewl_seeker_set_value(Ewl_Seeker * s, double v);
double          ewl_seeker_get_value(Ewl_Seeker * s);

void            ewl_seeker_set_range(Ewl_Seeker * s, double r);
double          ewl_seeker_get_range(Ewl_Seeker * s);

void            ewl_seeker_set_step(Ewl_Seeker * s, double step);
double          ewl_seeker_get_step(Ewl_Seeker * s);

void            ewl_seeker_decrease(Ewl_Seeker * s);
void            ewl_seeker_increase(Ewl_Seeker * s);

#endif				/* __EWL_SEEKER_H__ */
