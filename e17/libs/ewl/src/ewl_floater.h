
/*
 * The floater is a container for widgets that need to be drawn on top
 * of multiple widgets (f.eks dropdownbox)
 *
 */

#ifndef __EWL_FLOATER_H__
#define __EWL_FLOATER_H__

typedef struct _ewl_floater Ewl_Floater;

#define EWL_FLOATER(floatwidget) ((Ewl_Floater *) floatwidget)


struct _ewl_floater {
	Ewl_Box         box;
	Ewl_Widget     *follows;
	int             x;
	int             y;
};


Ewl_Widget     *ewl_floater_new(Ewl_Widget * parent);
void            ewl_floater_set_position(Ewl_Floater * parent, int x, int y);
void            ewl_floater_init(Ewl_Floater * f, Ewl_Widget * parent);


#endif /* __EWL_FLOATER_H__ */
