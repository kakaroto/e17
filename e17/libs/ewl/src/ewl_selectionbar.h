
#ifndef __EWL_SELECTIONBAR_H__
#define __EWL_SELECTIONBAR_H__

typedef struct _ewl_selectionbar Ewl_Selectionbar;

#define EWL_SELECTIONBAR(selectionbar) ((Ewl_Selectionbar *) selectionbar)

struct _ewl_selectionbar {
	Ewl_Floater     floater;

	struct {
		Ewl_Widget     *top;
		Ewl_Widget     *bottom;
	} scroller;


	int             h, w;
	int             mouse_x;	/* store old mouse x position for animation */
	int             OPEN;	/* Boolean, open or closed */
};

Ewl_Widget     *ewl_selectionbar_new(Ewl_Widget * parent);
void            ewl_selectionbar_init(Ewl_Selectionbar * s,
				      Ewl_Widget * parent);
void            ewl_selectionbar_set_size(Ewl_Selectionbar * s, int w, int h1,
					  int h2);


#endif /* __EWL_SELECTIONBAR_H__ */
