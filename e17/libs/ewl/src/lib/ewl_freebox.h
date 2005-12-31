#ifndef EWL_FREEBOX_H
#define EWL_FREEBOX_H

/**
 * @brief Callback to compare two widgets to determin which is first. If the
 * return is < 0 then a is first. If the return is > 0 then b is first. If
 * the return == 0 then they are equal 
 */
typedef int (*Ewl_Freebox_Comparator)(Ewl_Widget *a, Ewl_Widget *b);

#define EWL_FREEBOX(box) ((Ewl_Freebox *)box)

typedef struct Ewl_Freebox Ewl_Freebox;
struct Ewl_Freebox
{
	Ewl_Container container;
	Ewl_Freebox_Layout_Type layout;

	Ewl_Freebox_Comparator comparator;

	unsigned short sorted;
};

Ewl_Widget		*ewl_freebox_new(void);
int		 	 ewl_freebox_init(Ewl_Freebox *fb);

void		 	 ewl_freebox_layout_type_set(Ewl_Freebox *fb,
					Ewl_Freebox_Layout_Type type);
Ewl_Freebox_Layout_Type  ewl_freebox_layout_type_get(Ewl_Freebox *fb);

void			 ewl_freebox_comparator_set(Ewl_Freebox *fb,
					Ewl_Freebox_Comparator cmp);
Ewl_Freebox_Comparator	 ewl_freebox_comparator_get(Ewl_Freebox *fb);

void			 ewl_freebox_resort(Ewl_Freebox *fb);

/*
 * Internal callbacks, override at your risk
 */
void ewl_freebox_cb_configure(Ewl_Widget *w, void *ev, void *data);

void ewl_freebox_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_freebox_cb_child_show(Ewl_Container *c, Ewl_Widget *w);

#endif

