
#ifndef __EWL_SEEKER_H
#define __EWL_SEEKER_H

struct _ewl_seeker {
	Ewl_Widget			widget;
	Ewl_Orientation		orientation;
	Ebits_Object		ebits_bg;
	Ebits_Object		ebits_dragbar;
	unsigned int		value;
	unsigned int		range;
	int					start_drag;
};

typedef struct _ewl_seeker Ewl_Seeker;

#define EWL_SEEKER(seeker) ((Ewl_Seeker *) seeker)

Ewl_Widget	  * ewl_hseeker_new(void);
Ewl_Widget	  * ewl_vseeker_new(void);
Ewl_Widget	  * ewl_seeker_new_all(Ewl_Orientation orientation, int range);
void			ewl_seeker_set_value(Ewl_Widget * widget, int value);
int				ewl_seeker_get_value(Ewl_Widget * widget);
void			ewl_seeker_set_range(Ewl_Widget * widget, int range);
int				ewl_seeker_get_range(Ewl_Widget * widget);
void			ewl_seeker_set_orientation(Ewl_Widget * widget,
										   Ewl_Orientation orientation);

#endif
