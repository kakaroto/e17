
#include <Ewl.h>

/*
#define _EBITS_INTERNAL 1
#include <Ebits_Private.h>
*/
static void __ewl_image_init(Ewl_Image * i);
static void __ewl_image_realize(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_image_show(Ewl_Widget * w, void *ev_data,
			     void *user_data);
static void __ewl_image_hide(Ewl_Widget * w, void *ev_data,
			     void *user_data);
static void __ewl_image_configure(Ewl_Widget * w, void *ev_data,
				  void *user_data);

static Ewl_Image_Type __ewl_image_get_type(const char *i);

Ewl_Widget *
ewl_image_load(const char *i)
{
	Ewl_Image *image;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("i", i, NULL);

	if (!strlen(i))
		return NULL;

	image = NEW(Ewl_Image, 1);

	__ewl_image_init(image);

	image->type = __ewl_image_get_type(i);
	image->path = strdup(i);

	DRETURN_PTR(EWL_WIDGET(image));
}

void
ewl_image_set_layer(Ewl_Widget * w, int l)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("l", l);

	i = EWL_IMAGE(w);

	LAYER(w) = l;

	/*
	 * If we aren't realized.. we haven't loaded the image yet
	 * so we'll just let the realize callback set the layer..
	 * on realization =)
	 */
	if (!REALIZED(w))
		DRETURN;

	switch (i->type) {
	case EWL_IMAGE_TYPE_NORMAL:
		evas_set_layer(w->evas, i->image, l);
		break;
	case EWL_IMAGE_TYPE_EBITS:
		ebits_set_layer(i->image, l);
		break;
	default:
		break;

	}

	DLEAVE_FUNCTION;
}

int
ewl_image_get_layer(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);

	DRETURN_INT(LAYER(w));
}

void
ewl_image_lower(Ewl_Widget * w)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	--LAYER(w);

	if (!REALIZED(w))
		DRETURN;

	switch (i->type) {
	case EWL_IMAGE_TYPE_NORMAL:
		evas_lower(w->evas, i->image);
		break;
	case EWL_IMAGE_TYPE_EBITS:
		ebits_lower(i->image);
		break;
	default:
		break;
	}

	DLEAVE_FUNCTION;
}

void
ewl_image_raise(Ewl_Widget * w)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	++LAYER(w);

	if (!REALIZED(w))
		DRETURN;

	switch (i->type) {
	case EWL_IMAGE_TYPE_NORMAL:
		evas_raise(w->evas, i->image);
		break;
	case EWL_IMAGE_TYPE_EBITS:
		ebits_raise(i->image);
		break;
	default:
		break;
	}

	DLEAVE_FUNCTION;
}

void
ewl_image_move(Ewl_Widget * w, double x, double y)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	REQUEST_X(w) = x;
	REQUEST_Y(w) = y;

	if (!REALIZED(w))
		DRETURN ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_image_get_position(Ewl_Widget * w, double *x, double *y)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (x)
		*x = CURRENT_X(w);

	if (y)
		*y = CURRENT_Y(w);

	DLEAVE_FUNCTION;
}

void
ewl_image_resize(Ewl_Widget * w, double w2, double h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	REQUEST_W(w) = w2;
	REQUEST_H(w) = h;

	DLEAVE_FUNCTION;
}

void
ewl_image_get_size(Ewl_Widget * w, double *w2, double *h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (w2)
		*w2 = CURRENT_W(w);

	if (h)
		*h = CURRENT_H(w);

	DLEAVE_FUNCTION;
}

void
ewl_image_set_geometry(Ewl_Widget * w,
		       double x, double y, double w2, double h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	REQUEST_X(w) = x;
	REQUEST_Y(w) = y;
	REQUEST_W(w) = w2;
	REQUEST_H(w) = h;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_image_get_geometry(Ewl_Widget * w,
		       double *x, double *y, double *w2, double *h)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
ewl_image_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	i->color.r = r;
	i->color.g = g;
	i->color.b = b;
	i->color.a = a;

	if (!REALIZED(w))
		DRETURN;

	switch (i->type) {
	case EWL_IMAGE_TYPE_NORMAL:
		evas_set_color(w->evas, i->image, r, g, b, a);
		break;
	case EWL_IMAGE_TYPE_EBITS:
		break;
	default:
		break;
	}

	DLEAVE_FUNCTION;
}

void
ewl_image_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	if (r)
		*r = i->color.r;
	if (g)
		*g = i->color.g;
	if (b)
		*b = i->color.b;
	if (a)
		*a = i->color.a;

	DLEAVE_FUNCTION;
}

void
ewl_image_set_border(Ewl_Widget * w, int t, int b, int l, int r)
{

}

void
ewl_image_get_border(Ewl_Widget * w, int *t, int *b, int *l, int *r)
{

}

static void
__ewl_image_init(Ewl_Image * i)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("i", i);

	memset(i, 0, sizeof(Ewl_Image));

	w = EWL_WIDGET(i);

	ewl_widget_init(w, NULL);
	ewl_callback_clear(w);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_image_realize,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_SHOW, __ewl_image_show, NULL);
	ewl_callback_append(w, EWL_CALLBACK_HIDE, __ewl_image_hide, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_image_configure, NULL);

	memset(&i->color, 255, sizeof(i->color));

	DLEAVE_FUNCTION;
}

static void
__ewl_image_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	switch (i->type) {
		case EWL_IMAGE_TYPE_NORMAL:
			i->image = evas_add_image_from_file(w->evas, i->path);
			evas_set_layer(w->evas, i->image, LAYER(w));
			evas_set_clip(w->evas, i->image, w->fx_clip_box);
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

	DLEAVE_FUNCTION;
}

static void
__ewl_image_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_image_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{

}

static void
__ewl_image_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Image *i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	i = EWL_IMAGE(w);

	switch (i->type) {
	case EWL_IMAGE_TYPE_NORMAL:
		evas_move(w->evas, i->image, REQUEST_X(w), REQUEST_Y(w));
		evas_resize(w->evas, i->image, REQUEST_W(w), REQUEST_H(w));
		break;
	case EWL_IMAGE_TYPE_EBITS:
		ebits_move(i->image, REQUEST_X(w), REQUEST_Y(w));
		ebits_resize(i->image, REQUEST_W(w), REQUEST_H(w));
		break;
	default:
		DERROR
		    ("__ewl_image_configure: loading an unknown image\n");
		break;
	}

	ewl_object_set_current_geometry(EWL_OBJECT(w),
					REQUEST_X(w), REQUEST_Y(w),
					REQUEST_W(w), REQUEST_H(w));

	DLEAVE_FUNCTION;
}

static Ewl_Image_Type
__ewl_image_get_type(const char *i)
{
	char str[8];
	int j, k, l;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("i", i, -1);

	k = 0;
	l = strlen(i);
	memset(str, 0, sizeof(char) * 8);

	for (j = l - 8; j < l; j++)
		str[k++] = i[j];

	if (!strncasecmp(str, ".bits.db", 8))
		return EWL_IMAGE_TYPE_EBITS;

	return EWL_IMAGE_TYPE_NORMAL;
}
