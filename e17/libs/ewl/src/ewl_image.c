
#include <Ewl.h>

void            __ewl_image_realize(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void            __ewl_image_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            __ewl_image_mouse_down(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_image_mouse_up(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_image_mouse_move(Ewl_Widget * w, void *ev_data,
				       void *user_data);

Ewl_Image_Type  __ewl_image_get_type(const char *i);

/**
 * ewl_image_load - allocates a new image widget with specified image contents
 * @i: the path to the image to be displayed by the image widget
 *
 * Returns a pointer to the newly allocated image widget on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_image_load(const char *i)
{
	Ewl_Image      *image;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, NULL);

	if (!strlen(i))
		return NULL;

	image = NEW(Ewl_Image, 1);

	ZERO(image, Ewl_Image, 1);
	ewl_image_init(image);

	/*
	 * Determine the type of image to be loaded.
	 */
	image->type = __ewl_image_get_type(i);
	image->path = strdup(i);

	DRETURN_PTR(EWL_WIDGET(image), DLEVEL_STABLE);
}

/**
 * ewl_image_set_file - change the image file displayed by an image widget
 * @i: the image widget to change the displayed image
 * @im: the path to the new image to be displayed by @i
 *
 * Returns no value. Set the image displayed by @i to the one found at the
 * path @im.
 */
void ewl_image_set_file(Ewl_Image * i, const char *im)
{
	int             old_type;
	Ewl_Widget     *w;
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_PARAM_PTR("im", im);

	w = EWL_WIDGET(i);

	IF_FREE(i->path);

	i->path = strdup(im);

	win = ewl_window_find_window_by_widget(w);

	old_type = i->type;
	i->type = __ewl_image_get_type(i->path);

	/*
	 * Load the new image if widget has been realized
	 */
	if (REALIZED(w)) {
		/*
		 * Free the image if it had been loaded.
		 */
		if (i->image) {

			/*
			 * Type is important for using the correct free calls.
			 */
			if (old_type == EWL_IMAGE_TYPE_EBITS) {
				ebits_hide(i->image);
				ebits_unset_clip(i->image);
				ebits_free(i->image);
			} else {
				evas_hide(win->evas, i->image);
				evas_unset_clip(win->evas, i->image);
				evas_del_object(win->evas, i->image);
			}

			i->image = NULL;
		}

		/*
		 * Now draw the new image
		 */
		__ewl_image_realize(w, NULL, NULL);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_image_set_proportional - set boolean to determine how to scale
 * @i: the image to change proportional setting
 * @p: the boolean indicator of proportionality
 *
 * Returns no value. Changes the flag indicating if the image is scaled
 * proportionally.
 */
void
ewl_image_set_proportional(Ewl_Image *i, char p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("i", i);

	i->proportional = p;
	ewl_widget_configure(EWL_WIDGET(i));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_image_scale - scale image dimensions by a percentage
 * @i: the image to scale
 * @wp: the percentage to scale width
 * @hp: the percentage to scale height
 *
 * Returns no value. Scales the given image to @wp percent of preferred width
 * by @hp percent of preferred height. If @i->proportional is set to TRUE, the
 * lesser of @wp and @hp is applied for both directions.
 */
void
ewl_image_scale(Ewl_Image *i, double wp, double hp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);

	if (i->proportional) {
		if (wp < hp)
			hp = wp;
		else
			wp = hp;
	}

	i->sw = wp;
	i->sh = hp;

	ewl_object_set_preferred_w(EWL_OBJECT(i), wp * i->ow);
	ewl_object_set_preferred_h(EWL_OBJECT(i), hp * i->oh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_image_scale_to - scale image dimensions to a specific size
 * @i: the image to scale
 * @w: the size to scale width
 * @h: the size to scale height
 *
 * Returns no value. Scales the given image to @w by @hp. If @i->proportional
 * is set to TRUE, the image is scaled proportional to the lesser scale
 * percentage of preferred size.
 */
void
ewl_image_scale_to(Ewl_Image *i, int w, int h)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);

	/*
	 * Scale the image to be proportional inside the available space.
	 */
	if (i->ow && i->oh && i->proportional) {
		double wp, hp;

		wp = (double)w / (double)i->ow;
		hp = (double)h / (double)i->oh;

		if (wp < hp)
			hp = wp;
		else
			wp = hp;

		w = wp * i->ow;
		h = hp * i->oh;
	}

	i->sw = 1.0;
	i->sh = 1.0;
	ewl_object_set_preferred_size(EWL_OBJECT(i), w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_image_init - initialize an image widget to default values and callbacks
 * @i: the image widget to initialize
 *
 * Returns no value. Sets the fields and callbacks of @i to their default
 * values.
 */
void ewl_image_init(Ewl_Image * i)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);

	w = EWL_WIDGET(i);

	ewl_widget_init(w, NULL);

	/*
	 * Append necessary callbacks.
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_image_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_image_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_image_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, __ewl_image_mouse_up,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, __ewl_image_mouse_move,
			    NULL);

	i->sw = 1.0;
	i->sh = 1.0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	if (!i->path)
		DRETURN(DLEVEL_STABLE);

	win = ewl_window_find_window_by_widget(w);

	/*
	 * Load the image based on the type.
	 */
	if (i->type == EWL_IMAGE_TYPE_EBITS) {
		i->image = ebits_load(i->path);
		if (!i->image)
			return;

		ebits_add_to_evas(i->image, win->evas);
		ebits_set_layer(i->image, LAYER(w));
		ebits_set_clip(i->image, w->fx_clip_box);
		ebits_get_min_size(i->image, &i->ow, &i->oh);
		ebits_show(i->image);

	} else {
		i->image = evas_add_image_from_file(win->evas, i->path);
		if (!i->image)
			return;

		evas_set_layer(win->evas, i->image, LAYER(w));
		evas_set_clip(win->evas, i->image, w->fx_clip_box);
		evas_get_image_size(win->evas, i->image, &i->ow, &i->oh);
		evas_show(win->evas, i->image);
	}

	if (!i->ow)
		i->ow = 256;
	if (!i->oh)
		i->oh = 256;

	if (ewl_object_get_preferred_w(EWL_OBJECT(i)))
		ewl_image_scale_to(i, ewl_object_get_preferred_w(EWL_OBJECT(i)),
				ewl_object_get_preferred_h(EWL_OBJECT(i)));
	else
		ewl_image_scale(i, i->sw, i->sh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;

	i = EWL_IMAGE(w);
	if (!i->image)
		return;

	win = ewl_window_find_window_by_widget(w);

	if (i->type == EWL_IMAGE_TYPE_EBITS)
		ebits_set_layer(i->image, LAYER(w));
	else
		evas_set_layer(win->evas, i->image, LAYER(w));
}

void __ewl_image_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;
	int 		ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	if (!i->image)
		return;

	win = ewl_window_find_window_by_widget(w);

	ww = CURRENT_W(w) - (INSET_LEFT(w) + INSET_RIGHT(w));
	hh = CURRENT_H(w) - (INSET_TOP(w) + INSET_BOTTOM(w));

	/*
	 * Move the image into place based on type.
	 */
	if (i->type == EWL_IMAGE_TYPE_EBITS) {
		ebits_move(i->image, CURRENT_X(w) + INSET_LEFT(w),
				CURRENT_Y(w) + INSET_TOP(w));
		ebits_resize(i->image, i->sw * ww, i->sh * hh);
	} else {
		evas_move(win->evas, i->image, CURRENT_X(w), CURRENT_Y(w));
		evas_resize(win->evas, i->image, ww, hh);
		evas_set_image_fill(win->evas, i->image, 0, 0, i->sw * ww,
				i->sh * hh);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Determine the type of the file based on the filename.
 */
Ewl_Image_Type __ewl_image_get_type(const char *i)
{
	int             l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, -1);

	l = strlen(i);

	if (l >= 8 && !(strncasecmp((char *) i + l - 8, ".bits.db", 8)))
		return EWL_IMAGE_TYPE_EBITS;

	return EWL_IMAGE_TYPE_NORMAL;
}


void __ewl_image_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;
	Ecore_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	win = ewl_window_find_window_by_widget(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EBITS)
		evas_event_button_down(win->evas, ev->x, ev->y, ev->button);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;
	Ecore_Event_Mouse_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	win = ewl_window_find_window_by_widget(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EBITS)
		evas_event_button_up(win->evas, ev->x, ev->y, ev->button);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;
	Ecore_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	win = ewl_window_find_window_by_widget(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EBITS)
		evas_event_move(win->evas, ev->x, ev->y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
