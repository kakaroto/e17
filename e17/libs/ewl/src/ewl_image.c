
#include <Ewl.h>

/*
#define _EBITS_INTERNAL 1
#include <Ebits_Private.h>
*/
static void __ewl_image_init(Ewl_Image * i);
static void __ewl_image_realize(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_image_configure(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void __ewl_image_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_image_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_image_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data);

static Ewl_Image_Type __ewl_image_get_type(const char *i);

Ewl_Widget *
ewl_image_load(const char *i)
{
	Ewl_Image *image;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, NULL);

	if (!strlen(i))
		return NULL;

	image = NEW(Ewl_Image, 1);

	ZERO(image, Ewl_Image, 1);
	__ewl_image_init(image);

	image->type = __ewl_image_get_type(i);
	image->path = strdup(i);

	DRETURN_PTR(EWL_WIDGET(image), DLEVEL_STABLE);
}

void
ewl_image_set_file(Ewl_Widget * w, const char *im)
{
	Ewl_Image *i;
	int ww = 0, hh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("im", im);

	i = EWL_IMAGE(w);

	IF_FREE(i->path);

	i->path = strdup(im);

	if (i->image)
	  {
		  switch (i->type)
		    {
		    case EWL_IMAGE_TYPE_NORMAL:
			    evas_hide(w->evas, i->image);
			    evas_unset_clip(w->evas, i->image);
			    evas_del_object(w->evas, i->image);
			    break;
		    case EWL_IMAGE_TYPE_EBITS:
			    ebits_hide(i->image);
			    ebits_unset_clip(i->image);
			    ebits_free(i->image);
			    break;
		    default:
			    break;
		    }

		  i->image = NULL;
	  }

	i->type = __ewl_image_get_type(i->path);

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  i->image = evas_add_image_from_file(w->evas, i->path);
		  evas_set_layer(w->evas, i->image, LAYER(w));
		  evas_set_clip(w->evas, i->image, w->fx_clip_box);
		  evas_get_image_size(w->evas, i->image, &ww, &hh);
		  evas_show(w->evas, i->image);
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  i->image = ebits_load(i->path);
		  ebits_add_to_evas(i->image, w->evas);
		  ebits_set_layer(i->image, LAYER(w));
		  ebits_set_clip(i->image, w->fx_clip_box);
		  ebits_show(i->image);
		  break;
	  default:
		  break;
	  }

	if (ww || hh)
		ewl_object_request_size(EWL_OBJECT(w), ww, hh);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_image_init(Ewl_Image * i)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);

	w = EWL_WIDGET(i);

	ewl_widget_init(w, NULL);
	ewl_widget_set_type(w, EWL_WIDGET_TYPE_IMAGE);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_image_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_image_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_image_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    __ewl_image_mouse_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			    __ewl_image_mouse_move, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_image_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;
	int ww = 0, hh = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  i->image = evas_add_image_from_file(w->evas, i->path);
		  evas_set_layer(w->evas, i->image, LAYER(w));
		  evas_set_clip(w->evas, i->image, w->fx_clip_box);
		  evas_get_image_size(w->evas, i->image, &ww, &hh);
		  evas_show(w->evas, i->image);
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  i->image = ebits_load(i->path);
		  ebits_add_to_evas(i->image, w->evas);
		  ebits_set_layer(i->image, LAYER(w));
		  ebits_set_clip(i->image, w->fx_clip_box);
		  ebits_show(i->image);
		  break;
	  default:
		  break;
	  }

	ewl_object_request_size(EWL_OBJECT(w), ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_image_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  evas_move(w->evas, i->image, CURRENT_X(w), CURRENT_Y(w));
		  evas_resize(w->evas, i->image, CURRENT_W(w), CURRENT_H(w));
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  ebits_move(i->image, CURRENT_X(w), CURRENT_Y(w));
		  ebits_resize(i->image, CURRENT_W(w), CURRENT_H(w));
		  break;
	  default:
		  break;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ewl_Image_Type
__ewl_image_get_type(const char *i)
{
	char str[8];
	int j, k, l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, -1);

	k = 0;
	l = strlen(i);
	memset(&str, 0, sizeof(char) * 8);

	for (j = l - 8; j < l; j++)
		str[k++] = i[j];

	if (!strncasecmp(str, ".bits.db", 8))
		return EWL_IMAGE_TYPE_EBITS;

	return EWL_IMAGE_TYPE_NORMAL;
}


void
__ewl_image_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;
	Ecore_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	ev = ev_data;

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  evas_event_button_down(w->evas, ev->x, ev->y, ev->button);
		  break;
	  default:
		  break;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_image_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;
	Ecore_Event_Mouse_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	ev = ev_data;

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  evas_event_button_up(w->evas, ev->x, ev->y, ev->button);
		  break;
	  default:
		  break;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_image_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;
	Ecore_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);
	ev = ev_data;

	switch (i->type)
	  {
	  case EWL_IMAGE_TYPE_NORMAL:
		  break;
	  case EWL_IMAGE_TYPE_EBITS:
		  evas_event_move(w->evas, ev->x, ev->y);
		  break;
	  default:
		  break;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
