
#include <Ewl.h>


void __ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_hbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_hbox_configure_normal(Ewl_Widget * w, int *rh);
static void __ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f,
					 int rh);
static void __ewl_hbox_layout_children(Ewl_Widget * w);

static void __ewl_vbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_vbox_configure_normal(Ewl_Widget * w, int *rw);
static void __ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f,
					 int rw);
static void __ewl_vbox_layout_children(Ewl_Widget * w);

/**
 * ewl_box_new - alloacte and initialize a new box with orientation
 *
 * @o: the orientation for the box's layout
 *
 * Returns NULL on failure, or a newly allocated box on success.
 */
Ewl_Widget *
ewl_box_new(Ewl_Orientation o)
{
	Ewl_Box *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Box, 1);
	if (!b)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(b, 0, sizeof(Ewl_Box));
	ewl_box_init(b, o);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * ewl_box_init - initialize the box to starting values
 *
 * @b: the box to initialize
 * @o: the orientation for the box to layout child widgets
 *
 * Returns no value.
 */
void
ewl_box_init(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	/*
	 * Initialize the container portion of the box
	 */
	if (o == EWL_ORIENTATION_VERTICAL)
		ewl_container_init(EWL_CONTAINER(b),
				   "/appearance/box/vertical");
	else
		ewl_container_init(EWL_CONTAINER(b),
				   "/appearance/box/horizontal");

	ewl_object_set_fill_policy(EWL_OBJECT(b), EWL_FILL_POLICY_FILL);

	ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE,
			     __ewl_box_configure, NULL);

	/*
	 * Set the box's appropriate orientation
	 */
	b->orientation = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_box_set_orientation - change the specified box's orientation
 *
 * @b: the box to change the orientation
 * @o: the orientation to set for the box
 *
 * Returns no value.
 */
void
ewl_box_set_orientation(Ewl_Box * b, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	/*
	 * Set the orientation and reconfigure the widget so that child
	 * widgets now have the new orientation layout
	 */
	if (b->orientation == o)
		DRETURN(DLEVEL_STABLE);

	b->orientation = o;
	if (b->orientation == EWL_ORIENTATION_VERTICAL)
		ewl_widget_set_appearance(w, "/appearance/box/vertical");
	else
		ewl_widget_set_appearance(w, "/appearance/box/horizontal");

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_box_set_spacing - changes the spacing between the objects in the box
 *
 * @b: the box to change the spacing
 * @s: the spacing to put between the child widgets
 *
 * Returns no value.
 */
void
ewl_box_set_spacing(Ewl_Box * b, int s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

	b->spacing = s;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL)
		__ewl_vbox_configure_children(w);
	else
		__ewl_hbox_configure_children(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_vbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_vbox_configure_normal(w, &rh);

	if (f)
		__ewl_vbox_configure_fillers(w, f, rh);

	__ewl_vbox_layout_children(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewd_List *
__ewl_vbox_configure_normal(Ewl_Widget * w, int *rh)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rh = REQUEST_H(w) - tt - bb;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL) {
			if (!f)
				f = ewd_list_new();

			ewd_list_append(f, c);

			continue;
		}

		if (MAXIMUM_W(c) && MAXIMUM_W(c) < CURRENT_W(c))
			REQUEST_W(c) = MAXIMUM_W(c);
		else if (MINIMUM_W(c) && MINIMUM_W(c) > CURRENT_W(c))
			REQUEST_W(c) = MINIMUM_W(c);
		else
			REQUEST_W(c) = CURRENT_W(c);

		if (MAXIMUM_H(c) && MAXIMUM_H(c) < CURRENT_H(c))
			REQUEST_H(c) = MAXIMUM_H(c);
		else if (MINIMUM_H(c) && MINIMUM_H(c) > CURRENT_H(c))
			REQUEST_H(c) = MINIMUM_H(c);
		else
			REQUEST_H(c) = CURRENT_H(c);

		*rh -= REQUEST_H(c) + box->spacing;
		*rh -= c->object.padd.t + c->object.padd.b;
	}

	return f;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rh)
{
	Ewl_Box *b;
	Ewl_Widget *c;
	int nh, ll = 0, rr = 0, tt = 0, bb = 0;
	int children = 0, rchildren = 0;
	int nfh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	children = ewd_list_nodes(f);
	rchildren = children;

	nh = (rh - (rchildren - 1) * b->spacing) / children;

	if (children > 1) {
		ewd_list_goto_first(f);

		while ((c = ewd_list_next(f)) != NULL) {
			if (MAXIMUM_H(c) && MAXIMUM_H(c) < nh)
				nfh += MAXIMUM_H(c);
			else if (MINIMUM_H(c) && MINIMUM_H(c) > nh)
				nfh -= MINIMUM_H(c);
			else
				continue;

			--children;
		}

	}

	if (children)
		nh = ((rh - nfh) -
		      (rchildren - 1) * b->spacing) / children;

	ewd_list_goto_first(f);

	while ((c = ewd_list_next(f)) != NULL) {
		if (MAXIMUM_W(c) && MAXIMUM_W(c) < REQUEST_W(w) - ll - rr)
			REQUEST_W(c) = MAXIMUM_W(c);
		else if (MINIMUM_W(c)
			 && MINIMUM_W(c) > REQUEST_W(w) - ll - rr)
			REQUEST_W(c) = MINIMUM_W(c);
		else
			REQUEST_W(c) =
			    REQUEST_W(w) - ll - rr - c->object.padd.l -
			    c->object.padd.r;

		if (MAXIMUM_H(c) && MAXIMUM_H(c) < nh)
			REQUEST_H(c) = MAXIMUM_H(c);
		else if (MINIMUM_H(c) && MINIMUM_H(c) > nh)
			REQUEST_H(c) = MINIMUM_H(c);
		else
			REQUEST_H(c) =
			    nh - c->object.padd.t - c->object.padd.b;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_vbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int y, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	y = REQUEST_Y(w) + t;

	if (!EWL_CONTAINER(w)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		/*
		 * If the child is bigger then the box, align it left 
		 */
		if (REQUEST_W(c) > REQUEST_W(w) - l - r)
			REQUEST_X(c) = REQUEST_X(w) + l + c->object.padd.l;
		/*
		 * Left alignment 
		 */
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_LEFT)
			REQUEST_X(c) = REQUEST_X(w) + l + c->object.padd.l;
		/*
		 * Right 
		 */
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_RIGHT) {
			REQUEST_X(c) =
			    REQUEST_X(w) + REQUEST_W(w) - l +
			    c->object.padd.l;
			REQUEST_X(c) -= REQUEST_W(c);
		}
		/*
		 * Center
		 */
		else {
			REQUEST_X(c) = REQUEST_X(w) + c->object.padd.l;
			REQUEST_X(c) += (REQUEST_W(w) / 2) -
			    (REQUEST_W(c) / 2);
		}

		REQUEST_Y(c) = y + c->object.padd.t;
		/*
		 * Update y so the next child comes beneath it 
		 */
		y = REQUEST_Y(c) + REQUEST_H(c) + box->spacing +
		    c->object.padd.b;

		ewl_widget_configure(c);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



static void
__ewl_hbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rw;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_hbox_configure_normal(w, &rw);

	if (f)
		__ewl_hbox_configure_fillers(w, f, rw);

	__ewl_hbox_layout_children(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewd_List *
__ewl_hbox_configure_normal(Ewl_Widget * w, int *rw)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rw = REQUEST_W(w) - ll - rr;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL) {
			if (!f)
				f = ewd_list_new();

			ewd_list_append(f, c);

			continue;
		}

		if (MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			REQUEST_H(c) = MAXIMUM_H(c);
		else if (MINIMUM_H(c) > REQUEST_H(w))
			REQUEST_H(c) = MINIMUM_H(c);
		else
			REQUEST_H(c) = CURRENT_H(c);

		if (MAXIMUM_W(c) < CURRENT_W(c))
			REQUEST_W(c) = MAXIMUM_W(c);
		else if (MINIMUM_W(c) > CURRENT_W(c))
			REQUEST_W(c) = MINIMUM_W(c);
		else
			REQUEST_W(c) = CURRENT_W(c);

		*rw -= REQUEST_W(c) + box->spacing;
		*rw -= c->object.padd.l + c->object.padd.r;
	}

	return f;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rw)
{
	Ewl_Box *b;
	Ewl_Widget *c;
	int nw, ll = 0, rr = 0, tt = 0, bb = 0;
	int children = 0, rchildren = 0;
	int nfw = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	children = ewd_list_nodes(f);
	rchildren = children;

	nw = (rw - (rchildren - 1) * b->spacing) / children;

	if (children > 1) {
		ewd_list_goto_first(f);

		while ((c = ewd_list_next(f)) != NULL) {
			if (MAXIMUM_W(c) && MAXIMUM_W(c) < nw)
				nfw += MAXIMUM_W(c);
			else if (MINIMUM_W(c) && MINIMUM_W(c) > nw)
				nfw -= MINIMUM_W(c);
			else
				continue;

			--children;
		}

	}

	if (children)
		nw = ((rw - nfw) -
		      (rchildren - 1) * b->spacing) / children;

	ewd_list_goto_first(f);

	while ((c = ewd_list_next(f)) != NULL) {
		if (MAXIMUM_H(c) && MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			REQUEST_H(c) = MAXIMUM_H(c);
		else if (MINIMUM_H(c)
			 && MINIMUM_H(c) > REQUEST_W(w) - ll - rr)
			REQUEST_H(c) = MINIMUM_H(w);
		else
			REQUEST_H(c) =
			    REQUEST_H(w) - ll - rr - c->object.padd.t -
			    c->object.padd.b;

		if (MAXIMUM_W(c) && MAXIMUM_W(c) < nw)
			REQUEST_W(c) = MAXIMUM_W(c);
		else if (MINIMUM_W(c) && MINIMUM_W(c) > nw)
			REQUEST_W(c) = MINIMUM_W(c);
		else
			REQUEST_W(c) =
			    nw - c->object.padd.l - c->object.padd.r;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_hbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int x, l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	x = REQUEST_X(w) + l;

	if (!EWL_CONTAINER(w)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN(DLEVEL_STABLE);

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL) {
		/*
		 * If the child is bigger then the box, align it to the TOP 
		 */
		if (REQUEST_H(c) > REQUEST_H(w) - t - b)
			REQUEST_Y(c) = REQUEST_Y(w) + t + c->object.padd.t;
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_TOP)
			REQUEST_Y(c) = REQUEST_Y(w) + t + c->object.padd.t;
		/*
		 * Bottom 
		 */
		else if (EWL_OBJECT(c)->align & EWL_ALIGNMENT_BOTTOM) {
			REQUEST_Y(c) = REQUEST_Y(w) + REQUEST_H(w) - l - r;
			REQUEST_Y(c) -= REQUEST_H(c);
			REQUEST_Y(c) += c->object.padd.b;
		}
		/*
		 * Center or unknown 
		 */
		else {
			REQUEST_Y(c) = REQUEST_Y(w);
			REQUEST_Y(c) += (REQUEST_H(w) / 2) -
			    (REQUEST_H(c) / 2);
			REQUEST_Y(c) += c->object.padd.t;
		}

		REQUEST_X(c) = x + c->object.padd.l;

		x = REQUEST_X(c) + REQUEST_W(c) + box->spacing;
		x += c->object.padd.r;

		ewl_widget_configure(c);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
