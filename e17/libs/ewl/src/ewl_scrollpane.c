
#include <Ewl.h>


void
__ewl_container_configure_clip_box(Ewl_Widget * w, void *ev_data,
				   void *user_data);

void __ewl_scrollpane_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_scrollpane_configure(Ewl_Widget * w, void *ev_data,
				void *user_data);
void __ewl_scrollpane_box_configure(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void __ewl_scrollpane_scrollbar_value_changed(Ewl_Widget * w, void *ev_data,
					      void *user_data);
void __ewl_scrollpane_box_configure_clip_box(Ewl_Widget * w, void *ev_data,
					     void *user_data);


Ewl_Widget *
ewl_scrollpane_new(void)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);

	s = NEW(Ewl_ScrollPane, 1);

	ZERO(s, Ewl_ScrollPane, 1);

	s->box = ewl_vbox_new();
	s->hscrollbar = ewl_hscrollbar_new();
	s->vscrollbar = ewl_vscrollbar_new();

	ewl_scrollpane_init(s);

	ewl_container_append_child(EWL_CONTAINER(s), s->hscrollbar);
	ewl_container_append_child(EWL_CONTAINER(s), s->vscrollbar);
	ewl_container_append_child(EWL_CONTAINER(s), s->box);

	ewl_container_set_forward(EWL_CONTAINER(s), EWL_CONTAINER(s->box));

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_UNSTABLE);
}

void
ewl_scrollpane_init(Ewl_ScrollPane * s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_UNSTABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);

	ewl_container_init(EWL_CONTAINER(s),
			   "/appearance/scrollpane/default");
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_scrollpane_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollpane_configure, NULL);

	ewl_callback_append(s->box, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollpane_box_configure, s);

	ewl_callback_append(s->hscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollpane_scrollbar_value_changed, s);
	ewl_callback_append(s->vscrollbar, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_scrollpane_scrollbar_value_changed, s);

	ewl_callback_del(s->box, EWL_CALLBACK_CONFIGURE,
			 __ewl_container_configure_clip_box);
	ewl_callback_append(s->box, EWL_CALLBACK_CONFIGURE,
			    __ewl_scrollpane_box_configure_clip_box, s);

	DLEAVE_FUNCTION(DLEVEL_UNSTABLE);
}

void
__ewl_scrollpane_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_ScrollPane *s;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLPANE(w);

	ewl_widget_realize(s->box);
	ewl_widget_realize(s->hscrollbar);
	ewl_widget_realize(s->vscrollbar);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollpane_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_ScrollPane *s;
	int hx, hy, hw, hh, vx, vy, vw, vh, bx, by, bw, bh;
	double hfp, vfp, hv, vv;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SCROLLPANE(w);

	hx = CURRENT_X(w);
	hy = CURRENT_Y(w) + CURRENT_H(w);
	hw = CURRENT_W(w);
	hh = CURRENT_H(s->hscrollbar);
	hy -= hh;

	vx = CURRENT_X(w) + CURRENT_W(w);
	vy = CURRENT_Y(w);
	vw = CURRENT_W(s->vscrollbar);
	vh = CURRENT_H(w);
	vx -= vw;

	hw -= vw;
	vh -= hh;

	bx = CURRENT_X(w);
	by = CURRENT_Y(w);
	bw = CURRENT_W(w) - vw;
	bh = CURRENT_H(w) - hh;

	ewl_object_request_size(EWL_OBJECT(s->box), bw, bh);
	ewl_object_request_geometry(EWL_OBJECT(s->hscrollbar), hx, hy, hw,
				    hh);
	ewl_object_request_geometry(EWL_OBJECT(s->vscrollbar), vx, vy, vw,
				    vh);

	ewl_widget_configure(s->hscrollbar);
	ewl_widget_configure(s->vscrollbar);
	ewl_widget_configure(s->box);

	if (s->box_size.w <= bw)
		hfp = 1;
	else if (s->box_size.w > bw)
		hfp = (double) bw / (double) s->box_size.w;

	if (s->box_size.h <= bh)
		vfp = 1;
	else if (s->box_size.h > bh)
		vfp = (double) bh / (double) s->box_size.h;

	ewl_scrollbar_set_fill_percentage(EWL_SCROLLBAR(s->hscrollbar), hfp);
	ewl_scrollbar_set_fill_percentage(EWL_SCROLLBAR(s->vscrollbar), vfp);

	hv = ewl_scrollbar_get_value(EWL_SCROLLBAR(s->hscrollbar));
	vv = ewl_scrollbar_get_value(EWL_SCROLLBAR(s->vscrollbar));

	bx -= (s->box_size.w - bw) * hv;
	by -= (s->box_size.h - bh) * vv;

	ewl_object_request_geometry(EWL_OBJECT(s->box), bx, by,
				    s->box_size.w, s->box_size.h);
	ewl_widget_configure(s->box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollpane_box_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_ScrollPane *s;
	Ewl_Widget *child;
	int ww = 0, hh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	if (!EWL_CONTAINER(w)->children
	    || ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	s = user_data;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((child = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  if (EWL_BOX(w)->orientation == EWL_ORIENTATION_HORIZONTAL)
		    {
			    ww += CURRENT_W(child);
			    ww += EWL_OBJECT(child)->padd.l;
			    ww += EWL_OBJECT(child)->padd.r;
			    ww += EWL_BOX(w)->spacing;

			    if (CURRENT_H(child) + EWL_OBJECT(child)->padd.t +
				EWL_OBJECT(child)->padd.b > hh)
			      {
				      hh = CURRENT_H(child);
				      hh += EWL_OBJECT(child)->padd.t;
				      hh += EWL_OBJECT(child)->padd.b;
			      }
		    }
		  else
		    {
			    if (CURRENT_W(child) + EWL_OBJECT(child)->padd.l +
				EWL_OBJECT(child)->padd.r > ww)
			      {
				      ww = CURRENT_W(child);
				      ww += EWL_OBJECT(child)->padd.l;
				      ww += EWL_OBJECT(child)->padd.r;
			      }

			    hh += CURRENT_H(child);
			    hh += EWL_OBJECT(child)->padd.t;
			    hh += EWL_OBJECT(child)->padd.b;
			    hh += EWL_BOX(w)->spacing;
		    }
	  }

	s->box_size.w = ww;
	s->box_size.h = hh;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollpane_scrollbar_value_changed(Ewl_Widget * w, void *ev_data,
					 void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_widget_configure(user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_scrollpane_box_configure_clip_box(Ewl_Widget * w, void *ev_data,
					void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	if (EWL_CONTAINER(w)->clip_box)
	  {
		  Ewl_ScrollPane *s;
		  int bx, by, bw, bh;

		  s = user_data;

		  bx = CURRENT_X(s);
		  by = CURRENT_Y(s);
		  bw = CURRENT_W(s) - CURRENT_W(s->vscrollbar);
		  bh = CURRENT_H(s) - CURRENT_H(s->hscrollbar);

		  evas_move(w->evas, EWL_CONTAINER(w)->clip_box, bx, by);
		  evas_resize(w->evas, EWL_CONTAINER(w)->clip_box, bw, bh);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
