
#include <Ewl.h>


void __ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_hbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_hbox_configure_normal(Ewl_Widget * w, int *rh);
static void __ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rh);
static void __ewl_hbox_layout_children(Ewl_Widget * w);

static void __ewl_vbox_configure_children(Ewl_Widget * w);
static Ewd_List *__ewl_vbox_configure_normal(Ewl_Widget * w, int *rw);
static void __ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rw);
static void __ewl_vbox_layout_children(Ewl_Widget * w);

/*
 * Create and initialize a new box container with the specified orientation
 */
Ewl_Widget *
ewl_box_new(Ewl_Orientation o)
{
	Ewl_Box *b;

	DENTER_FUNCTION;

	b = NEW(Ewl_Box, 1);
	if (!b)
		DRETURN_PTR(NULL);
	
	memset(b, 0, sizeof(Ewl_Box));
	ewl_box_init(b, o);

	DRETURN_PTR(EWL_WIDGET(b));
}

/*
 * Initialize the specified box to sane starting values and the given
 * orientation
 */
void
ewl_box_init(Ewl_Box * b, Ewl_Orientation o)
{
        Ewl_Widget *w;
        
        DENTER_FUNCTION;
        DCHECK_PARAM_PTR("b", b);

	w = EWL_WIDGET(b);

        /*
         * Initialize the container portion of the box
         */
        if (o == EWL_ORIENTATION_VERTICAL)
                ewl_container_init(EWL_CONTAINER(b), "/appearance/box/vertical");
        else
                ewl_container_init(EWL_CONTAINER(b), "/appearance/box/horizontal");
        
        ewl_object_set_fill_policy(EWL_OBJECT(b), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_box_configure, NULL);

        /*
         * Set the box's appropriate orientation
         */     
        b->orientation = o;

	DLEAVE_FUNCTION;
}

/*
 * Change the specified box's orientation
 */
void
ewl_box_set_orientation(Ewl_Widget * w, Ewl_Orientation o)
{
        Ewl_Box *b;

        DENTER_FUNCTION;
        DCHECK_PARAM_PTR("w", w);

        b = EWL_BOX(w);

        /* 
         * Set the orientation and reconfigure the widget so that child
         * widgets now have the new orientation layout
         */
        if (b->orientation == o)
                DRETURN;

        b->orientation = o;
        if (b->orientation == EWL_ORIENTATION_VERTICAL)
                ewl_widget_set_appearance(w, "/appearance/box/vertical");
        else
                ewl_widget_set_appearance(w, "/appearance/box/horizontal");

        ewl_widget_configure(w);

        DLEAVE_FUNCTION;
}

/*
 * Adjust the spacing between the objects in the box
 */
void
ewl_box_set_spacing(Ewl_Widget * w, unsigned int s)
{
	Ewl_Box *b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	b->spacing = s;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_box_child_set_padding(Ewl_Widget * w, int xp, int yp)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (xp == 0)
		ewl_widget_del_data(w, "/x_padding");
	else
		ewl_widget_set_data(w, "/x_padding", (void *) xp);
	
	if (yp == 0)
		ewl_widget_del_data(w, "/y_padding");
	else
		ewl_widget_set_data(w, "/y_padding", (void *) yp);

	DLEAVE_FUNCTION;
}

void
__ewl_box_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
        DENTER_FUNCTION;
        DCHECK_PARAM_PTR("w", w);
        
        if (EWL_BOX(w)->orientation == EWL_ORIENTATION_VERTICAL)
                __ewl_vbox_configure_children(w);
        else    
                __ewl_hbox_configure_children(w);
                
        DLEAVE_FUNCTION;
}       

static void
__ewl_vbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rh;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_vbox_configure_normal(w, &rh);

	if (f)
		__ewl_vbox_configure_fillers(w, f, rh);

	__ewl_vbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_vbox_configure_normal(Ewl_Widget * w, int *rh)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int xp = 0, yp = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rh = REQUEST_H(w) - tt - bb;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL)
		    {
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

		  REQUEST_H(c) = CURRENT_H(c);

		  *rh -= REQUEST_H(c) + box->spacing + yp;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rh)
{
	Ewl_Box *b;
	Ewl_Widget *c;
	int nh, ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	b = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	ewd_list_goto_first(f);

	nh = (rh - ((ewd_list_nodes(f) - 1) * b->spacing)) / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (MAXIMUM_W(c) && MAXIMUM_W(c) < REQUEST_W(w) - ll - rr)
			  REQUEST_W(c) = MAXIMUM_W(c);
		  else if (MINIMUM_W(c) && MINIMUM_W(c) > REQUEST_W(w) - ll -rr)
		  	  REQUEST_W(c) = MINIMUM_W(c);
		  else
			  REQUEST_W(c) = REQUEST_W(w) - ll - rr - xp;

		  if (MAXIMUM_H(c) && MAXIMUM_H(c) < nh)
			  REQUEST_H(c) = MAXIMUM_H(c);
		  else if (MINIMUM_H(c) && MINIMUM_H(c) > nh)
		  	  REQUEST_H(c) = MINIMUM_H(c);
		  else
			  REQUEST_H(c) = nh - yp;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_vbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int y, l = 0, r = 0, t = 0, b = 0;
	int xp, yp;
	int min_w = 0, min_h = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	min_h -= box->spacing;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	y = REQUEST_Y(w) + t;

	if (!EWL_CONTAINER(w)->children ||
		ewd_list_is_empty(EWL_CONTAINER(w)->children))
		DRETURN;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  /* If the child is bigger then the box, align it left */
		  if (REQUEST_W(c) > REQUEST_W(w) - l - r)
		  	REQUEST_X(c) = REQUEST_X(w) + l + xp;
		  /* Left alignment */
		  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_LEFT)
			    REQUEST_X(c) = REQUEST_X(w) + l + xp;
		  /* Right */
		  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_RIGHT)
		    {
		 	    REQUEST_X(c) = REQUEST_X(w) + REQUEST_W(w) - l + xp;
			    REQUEST_X(c) -= REQUEST_W(c);
		    }
		  /* Center or unknown */
                  else
                    {
                            REQUEST_X(c) = REQUEST_X(w) + xp;
                            REQUEST_X(c) += (REQUEST_W(w) / 2) -
			    			(REQUEST_W(c) / 2);
                    }

		  REQUEST_Y(c) = y + yp;
		  /* Update y so the next child comes beneath it */
		  y = REQUEST_Y(c) + REQUEST_H(c) + box->spacing;

		  ewl_widget_configure(c);

		  if (MINIMUM_W(c) > min_w)
			  min_w += MINIMUM_W(c);

		  min_h += MINIMUM_H(c) + box->spacing;
	  }

	ewl_object_set_minimum_size(EWL_OBJECT(w), min_w, min_h);

	DLEAVE_FUNCTION;
}



static void
__ewl_hbox_configure_children(Ewl_Widget * w)
{
	Ewd_List *f;
	int rw;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	f = __ewl_hbox_configure_normal(w, &rw);

	if (f)
		__ewl_hbox_configure_fillers(w, f, rw);

	__ewl_hbox_layout_children(w);

	DLEAVE_FUNCTION;
}

static Ewd_List *
__ewl_hbox_configure_normal(Ewl_Widget * w, int *rw)
{
	Ewd_List *f = NULL;
	Ewl_Box *box;
	Ewl_Widget *c;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	*rw = REQUEST_W(w) - ll - rr;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		xp = (int) ewl_widget_get_data(c, "/x_padding");
		yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (EWL_OBJECT(c)->fill & EWL_FILL_POLICY_FILL)
		    {
			    if (!f)
				    f = ewd_list_new();

			    ewd_list_append(f, c);

			    continue;
		    }

		  if (MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			  REQUEST_H(c) = MAXIMUM_H(c) - yp;
		  else if (MINIMUM_W(c) > REQUEST_W(w))
		  	  REQUEST_W(c) = MINIMUM_W(c);
		  else
			  REQUEST_H(c) = REQUEST_H(w) - tt - bb - yp;

		  REQUEST_W(c) = CURRENT_W(c);

		  *rw -= REQUEST_W(c) + box->spacing - xp;
	  }

	return f;

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_configure_fillers(Ewl_Widget * w, Ewd_List * f, int rw)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int nw, ll = 0, rr = 0, tt = 0, bb = 0;
	int xp, yp;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	ewd_list_goto_first(f);

	nw = (rw - (ewd_list_nodes(f) - 1) * box->spacing) / ewd_list_nodes(f);

	while ((c = ewd_list_next(f)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");

		  if (MAXIMUM_H(c) && MAXIMUM_H(c) < REQUEST_H(w) - tt - bb)
			  REQUEST_H(c) = MAXIMUM_H(c);
		  else
			  REQUEST_H(c) = REQUEST_H(w) - ll - bb - yp;

		  if (MAXIMUM_W(c) && MAXIMUM_W(c) < nw)
			  REQUEST_W(c) = MAXIMUM_W(c);
		  else
			  REQUEST_W(c) = nw - xp;
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_hbox_layout_children(Ewl_Widget * w)
{
	Ewl_Box *box;
	Ewl_Widget *c;
	int x, l = 0, r = 0, t = 0, b = 0;
	int xp, yp;
	int min_w = 0, min_h = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	box = EWL_BOX(w);

	min_w -= box->spacing;

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	x = REQUEST_X(w) + l;

	ewd_list_goto_first(EWL_CONTAINER(w)->children);

	while ((c = ewd_list_next(EWL_CONTAINER(w)->children)) != NULL)
	  {
		  xp = (int) ewl_widget_get_data(c, "/x_padding");
		  yp = (int) ewl_widget_get_data(c, "/y_padding");


		  /* If the child is bigger then the box, align it to the TOP */
		  if (REQUEST_H(c) > REQUEST_H(w) - t - b)
		  	  REQUEST_Y(c) = REQUEST_Y(w) + t + yp;
		  /* Top alignment */
		  if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_TOP)
			  REQUEST_Y(c) = REQUEST_Y(w) + t + yp;
		  /* Bottom */
                  else if (EWL_OBJECT(c)->align == EWL_ALIGNMENT_BOTTOM)
                          REQUEST_Y(c) = (REQUEST_Y(w) + REQUEST_H(w)) -
                                  REQUEST_H(c) - r + yp;
		  /* Center or unknown */
		  else
		    {
			    REQUEST_Y(c) = REQUEST_Y(w) + yp;
			    REQUEST_Y(c) += (REQUEST_H(w) / 2) -
			    			(REQUEST_H(c) / 2);
		    }

		  REQUEST_X(c) = x + xp;
		  x = REQUEST_X(c) + REQUEST_W(c) + box->spacing;

		  ewl_widget_configure(c);

		  min_w += MINIMUM_W(c) + box->spacing;

		  if (MINIMUM_H(c) > min_h)
		  	min_h += MINIMUM_H(c);
	  }

	ewl_object_set_minimum_size(EWL_OBJECT(w), min_w, min_h);

	DLEAVE_FUNCTION;
}
