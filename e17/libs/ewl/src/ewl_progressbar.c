#include <Ewl.h>

Ewl_Widget *ewl_progressbar_new();
void ewl_progressbar_init (Ewl_Progressbar * p);
void __ewl_progressbar_configure(Ewl_Widget * w, void *ev_data, void *user_data);



Ewl_Widget *ewl_progressbar_new() 
{
	Ewl_Progressbar *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	p = NEW(Ewl_Progressbar, 1);
	if (!p)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(p, Ewl_Progressbar, 1);

	ewl_progressbar_init(p);
	
	DRETURN_PTR(EWL_WIDGET(p), DLEVEL_STABLE);
}


void ewl_progressbar_init (Ewl_Progressbar * p)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", p);

	w = EWL_WIDGET(p);

	ewl_container_init(EWL_CONTAINER(w), "progressbar",
			NULL, NULL, NULL);

	p->bar = NEW(Ewl_Widget, 1);
	ewl_widget_init(p->bar, "bar");
	ewl_container_append_child(EWL_CONTAINER(p), p->bar);
	ewl_widget_show(p->bar);

	p->value = 0.0;
	
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, 
			__ewl_progressbar_configure, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __ewl_progressbar_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Progressbar *p;
	int             dx, dy;
	int             dw, dh;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	p = EWL_PROGRESSBAR(w);

	dx = CURRENT_X(p);
	dy = CURRENT_Y(p);
	dw = CURRENT_W(p);
	dh = CURRENT_H(p);

	ewl_object_request_geometry(EWL_OBJECT(p->bar), dx, dy, dw * p->value, dh);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

