
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
		 * Now draw the new 
		 */
		__ewl_image_realize(w, NULL, NULL);
	}

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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;
	int             ww = 0, hh = 0;

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
		ebits_add_to_evas(i->image, win->evas);
		ebits_set_layer(i->image, LAYER(w));
		ebits_set_clip(i->image, w->fx_clip_box);
		ebits_show(i->image);
	} else {
		i->image = evas_add_image_from_file(win->evas, i->path);
		evas_set_layer(win->evas, i->image, LAYER(w));
		evas_set_clip(win->evas, i->image, w->fx_clip_box);
		evas_get_image_size(win->evas, i->image, &ww, &hh);
		evas_show(win->evas, i->image);
	}

	ewl_object_set_preferred_size(EWL_OBJECT(w), ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_image_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image      *i;
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	win = ewl_window_find_window_by_widget(w);

	/*
	 * Move the image into place based on type.
	 */
	if (i->type == EWL_IMAGE_TYPE_EBITS) {
		ebits_move(i->image, CURRENT_X(w), CURRENT_Y(w));
		ebits_resize(i->image, CURRENT_W(w), CURRENT_H(w));
	} else {
		evas_move(win->evas, i->image, CURRENT_X(w), CURRENT_Y(w));
		evas_resize(win->evas, i->image, CURRENT_W(w), CURRENT_H(w));
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
