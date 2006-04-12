#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

#ifdef BUILD_EPSILON_SUPPORT
#include <Epsilon.h>
#include <Epsilon_Request.h>
#endif

static Ecore_Event_Handler *ewl_image_epsilon_handler = NULL;

static Ewl_Image_Type  ewl_image_type_get(const char *i);
static int ewl_image_thumbnail_complete_cb(void *data, int type, void *event);
static void ewl_image_thumb_destroy_cb(Ewl_Widget *w, void *ev, void *data);

/**
 * @return Returns a pointer to a new image widget on success, NULL on failure.
 * @brief Load an image widget with specified image contents
 *
 * The @a k parameter is primarily used for loading edje groups or keyed data
 * in an image.
 */
Ewl_Widget *
ewl_image_new(void)
{
	Ewl_Image *image;

	DENTER_FUNCTION(DLEVEL_STABLE);

	image = NEW(Ewl_Image, 1);
	if (!image)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_image_init(image)) {
		ewl_widget_destroy(EWL_WIDGET(image));
		image = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(image), DLEVEL_STABLE);
}

/**
 * @param i: the image widget to initialize
 * @return Returns no value.
 * @brief Initialize an image widget to default values and callbacks
 *
 * Sets the fields and callbacks of @a i to their default values.
 */
int
ewl_image_init(Ewl_Image *i)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, FALSE);

	w = EWL_WIDGET(i);

	if (!ewl_widget_init(w))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_IMAGE_TYPE);
	ewl_widget_inherit(w, EWL_IMAGE_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

	/*
	 * Append necessary callbacks.
	 */
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_image_reveal_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_image_obscure_cb,
			    NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_image_destroy_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_image_configure_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, ewl_image_mouse_down_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, ewl_image_mouse_up_cb,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, ewl_image_mouse_move_cb,
			    NULL);

	i->sw = 1.0;
	i->sh = 1.0;

	i->cs = 0;

	i->tile.x = 0;
	i->tile.y = 0;
	i->tile.w = 0;
	i->tile.h = 0;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param i: The image to set the file into
 * @param path: The path to set into the image
 * @return Returns no value.
 * @brief Sets the path to the image file
 */
void
ewl_image_file_path_set(Ewl_Image *i, const char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_PARAM_PTR("path", path);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	ewl_image_file_set(i, path, i->key);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image widget to get the file of
 * @return Returns the currently set filename
 * @brief get the filename this image uses
 */
const char *
ewl_image_file_path_get(Ewl_Image *i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, NULL);
	DCHECK_TYPE_RET("i", i, EWL_IMAGE_TYPE, NULL);

	DRETURN_PTR(i->path, DLEVEL_STABLE);
}

/**
 * @param i: The image to set the key into
 * @param key: The key to set into the image
 * @return Returns no value.
 * @brief Sets the key to use for the image
 */
void
ewl_image_file_key_set(Ewl_Image *i, const char *key)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	if (i->path)
		ewl_image_file_set(i, i->path, key);
	else
	{
		IF_FREE(i->key);
		if (key) i->key = strdup(key);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: The image to get the key from
 * @return Returns the key used for this image
 */
const char *
ewl_image_file_key_get(Ewl_Image *i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, NULL);
	DCHECK_TYPE_RET("i", i, EWL_IMAGE_TYPE, NULL);

	DRETURN_PTR(i->key, DLEVEL_STABLE);
}

/**
 * @param i: the image widget to change the displayed image
 * @param im: the path to the new image to be displayed by @a i
 * @param key: the key in the file for the image
 * @return Returns no value.
 * @brief Change the image file displayed by an image widget
 *
 * Set the image displayed by @a i to the one found at the path @a im. If an
 * edje is used, a minimum size should be specified in the edje or the code.
 */
void
ewl_image_file_set(Ewl_Image *i, const char *im, const char *key)
{
	int old_type;
	Ewl_Widget *w;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	w = EWL_WIDGET(i);
	emb = ewl_embed_widget_find(w);

	if (i->path != im) {
		IF_FREE(i->path);
	}

	if (i->key != key) {
		IF_FREE(i->key);
	}

	/*
	 * Determine the type of image to be loaded.
	 */
	old_type = i->type;
	if (im) {
		i->type = ewl_image_type_get(im);
		i->path = strdup(im);
		if (key)
			i->key = strdup(key);
	}
	else
		i->type = EWL_IMAGE_TYPE_NORMAL;

	/*
	 * Load the new image if widget has been realized
	 */
	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to change constrain setting
 * @param size: the minimum constrain size
 * @return Returns no value.
 * @brief Set a size which, if the image is bigger than, scale proportionally
 *
 * Sets a size to scale to proportionally if the image exceeds this size
 */
void
ewl_image_constrain_set(Ewl_Image *i, unsigned int size)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	i->cs = size;
	if (size) {
		ewl_object_preferred_inner_w_set(EWL_OBJECT(i), size);
		ewl_object_preferred_inner_h_set(EWL_OBJECT(i), size);
	}
	else if (i->sw != 1.0 || i->sh != 1.0) {
		ewl_image_scale_set(i, i->sw, i->sh);
	}
	else if (i->aw != i->ow || i->ah != i->oh) {
		ewl_image_size_set(i, i->aw, i->ah);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: The Ewl_Image to get the consrain size from
 * @return Returns the current constrain size on the image
 * @brief Get the current constrain value set on the image
 */
unsigned int
ewl_image_constrain_get(Ewl_Image *i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, 0);
	DCHECK_TYPE_RET("i", i, EWL_IMAGE_TYPE, 0);

	DRETURN_INT(i->cs, DLEVEL_STABLE);
}

/**
 * @param i: the image to change proportional setting
 * @param p: the boolean indicator of proportionality
 * @return Returns no value.
 * @brief Set boolean to determine how to scale
 *
 * Changes the flag indicating if the image is scaled proportionally.
 */
void
ewl_image_proportional_set(Ewl_Image *i, char p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	i->proportional = p;
	ewl_widget_configure(EWL_WIDGET(i));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to get the proportional setting value
 * @return Returns TRUE if the image is forced proportional, FALSE otherwise.
 * @brief Get boolean to determine how to scale
 */
char
ewl_image_proportional_get(Ewl_Image *i)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, FALSE);
	DCHECK_TYPE_RET("i", i, EWL_IMAGE_TYPE, FALSE);

	DRETURN_INT(i->proportional, DLEVEL_STABLE);
}

/**
 * @param i: the image to scale
 * @param wp: the percentage to scale width
 * @param hp: the percentage to scale height
 * @brief Scale image dimensions by a percentage
 *
 * @return Returns no value.
 * Scales the given image to @a wp percent of preferred width
 * by @a hp percent of preferred height. If @a i->proportional is set to TRUE,
 * the lesser of @a wp and @a hp is applied for both directions.
 */
void
ewl_image_scale_set(Ewl_Image *i, double wp, double hp)
{
	int aw, ah;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	i->sw = wp;
	i->sh = hp;

	/*
	 * Use set bounds if available, otherwise original image size.
	 */
	if (i->cs) {
		aw = ah = i->cs;
	}
	else {
		if (i->aw)
			aw = i->aw;
		else
			aw = i->ow;

		if (i->ah)
			ah = i->ah;
		else
			ah = i->oh;
	}

	/*
	 * Check for proportional scaling and adjust to fit.
	 */
	if (i->proportional) {
		if (wp < hp)
			hp = wp;
		else
			hp = wp;
	}

	ewl_object_preferred_inner_w_set(EWL_OBJECT(i), wp * aw);
	ewl_object_preferred_inner_h_set(EWL_OBJECT(i), hp * ah);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to retrieve the current scale
 * @param wp: stores the percentage to scale width
 * @param hp: stores the percentage to scale height
 * @brief Retrieve the percentage an image is scaled.
 *
 * @return Returns no value.
 */
void
ewl_image_scale_get(Ewl_Image *i, double *wp, double *hp)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	if (wp)
		*wp = i->sw;
	if (hp)
		*hp = i->sh;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to scale
 * @param w: the size to scale width
 * @param h: the size to scale height
 * @return Returns no value.
 * @brief Scale image dimensions to a specific size
 *
 * Scales the given image to @a w by @a hp. If @a i->proportional
 * is set to TRUE, the image is scaled proportional to the lesser scale
 * percentage of preferred size.
 */
void
ewl_image_size_set(Ewl_Image *i, int w, int h)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	i->aw = w;
	i->ah = h;
	ewl_object_preferred_inner_size_set(EWL_OBJECT(i), w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to scale
 * @param w: the size to scale width
 * @param h: the size to scale height
 * @return Returns no value.
 * @brief Scale image dimensions to a specific size
 *
 * Scales the given image to @a w by @a hp. If @a i->proportional
 * is set to TRUE, the image is scaled proportional to the lesser scale
 * percentage of preferred size.
 */
void
ewl_image_size_get(Ewl_Image *i, int *w, int *h)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	if (w)
		*w = i->aw;
	if (h)
		*h = i->ah;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to tile
 * @param x: the x position of the top right corner
 * @param y: the y position of the top right corner
 * @param w: the width of the tile
 * @param h: the height of the tile
 * @return Returns no value
 * @brief Tile the image with the given start position and given size
 *
 * Tiles the image across the available area, starting the image at the
 * given position and with the given size.
 */
void
ewl_image_tile_set(Ewl_Image *i, int x, int y, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("i", i);
	DCHECK_TYPE("i", i, EWL_IMAGE_TYPE);

	i->tile.set = 1;
	i->tile.x = x;
	i->tile.y = y;
	i->tile.w = w;
	i->tile.h = h;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: the image to thumbnail
 * @return Returns a thumbnailed image widget on success, NULL on failure.
 * @brief Create a widget representing a thumbnailed version of the image.
 */
Ewl_Widget *
ewl_image_thumbnail_get(Ewl_Image *i)
{
	Ewl_Widget *thumb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, NULL);
	DCHECK_TYPE_RET("i", i, EWL_IMAGE_TYPE, NULL);

#ifdef BUILD_EPSILON_SUPPORT
	if (i->path && (i->type == EWL_IMAGE_TYPE_NORMAL)) {
		thumb = ewl_image_thumbnail_new();
		if (thumb) {
			ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(thumb),
					(char *)ewl_image_file_path_get(i));
			EWL_IMAGE_THUMBNAIL(thumb)->orig = EWL_WIDGET(i);
		}
	}
#endif

	DRETURN_PTR(thumb, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Image_Thumbnail widget
 * @brief This will create and initialize a new Ewl_Image_Thumbnail widget
 */
Ewl_Widget *
ewl_image_thumbnail_new(void)
{
	Ewl_Image_Thumbnail *thumb;

	DENTER_FUNCTION(DLEVEL_STABLE);

	thumb = NEW(Ewl_Image_Thumbnail, 1);
	if (!ewl_image_thumbnail_init(EWL_IMAGE_THUMBNAIL(thumb))) {
		ewl_widget_destroy(EWL_WIDGET(thumb));
		thumb = NULL;
	}

	DRETURN_PTR(thumb, DLEVEL_STABLE);
}

/**
 * @param image: The Ewl_Image_Thumbnail to init
 * @return Returns TRUE if successfully initialized, FALSE otherwise
 * @brief Initialize an Ewl_Image_Thumbnail to default values.
 */
int
ewl_image_thumbnail_init(Ewl_Image_Thumbnail *image)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("image", image, FALSE);

	if (!ewl_image_init(EWL_IMAGE(image)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(image),
					EWL_IMAGE_THUMBNAIL_TYPE);
	ewl_widget_inherit(EWL_WIDGET(image), EWL_IMAGE_THUMBNAIL_TYPE);

	ewl_callback_prepend(EWL_WIDGET(image), EWL_CALLBACK_DESTROY,
			    ewl_image_thumb_destroy_cb, NULL);

#ifdef BUILD_EPSILON_SUPPORT
	if (!ewl_image_epsilon_handler) {
		epsilon_thumb_init();
		ewl_image_epsilon_handler =
			ecore_event_handler_add(EPSILON_EVENT_DONE,
					ewl_image_thumbnail_complete_cb,
					NULL);
	}
#endif

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/** 
 * @param thumb: The thumbnail to request
 * @param path: The path to the image
 * @return Returns no value.
 * @brief This will request the generation of a thumbnail for the image
 * given by @a path.
 */
void
ewl_image_thumbnail_request(Ewl_Image_Thumbnail *thumb, char *path)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("thumb", thumb);
	DCHECK_TYPE("thumb", thumb, EWL_IMAGE_THUMBNAIL_TYPE);
	DCHECK_PARAM_PTR("path", path);

#ifdef BUILD_EPSILON_SUPPORT
	thumb->thumb = epsilon_add(path, NULL, EPSILON_THUMB_NORMAL, thumb);
#else
	thumb->thumb = NULL;
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_image_thumb_destroy_cb(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Image_Thumbnail *thumb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	thumb = EWL_IMAGE_THUMBNAIL(w);
	if (thumb->thumb) {
#ifdef BUILD_EPSILON_SUPPORT
		epsilon_del(thumb->thumb);
#endif
		thumb->thumb = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_image_thumbnail_complete_cb(void *data __UNUSED__, int type __UNUSED__, 
								void *event)
{
#ifdef BUILD_EPSILON_SUPPORT
	Ewl_Image_Thumbnail *thumb;
	Epsilon_Request *ev;
#endif

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("event", event, FALSE);

#ifdef BUILD_EPSILON_SUPPORT
	ev = event;
	thumb = ev->data;

	if (thumb) {
		ewl_image_file_path_set(EWL_IMAGE(thumb), ev->dest);
		thumb->thumb = NULL;
	}
#endif

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_image_reveal_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;
	int ww, hh;
	double sw, sh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	emb = ewl_embed_widget_find(w);

	/*
	 * Load the image based on the type.
	 */
	if (i->type == EWL_IMAGE_TYPE_EDJE) {
		if (!i->image)
			i->image = ewl_embed_object_request(emb, "edje");
		if (!i->image)
			i->image = edje_object_add(emb->evas);
		if (!i->image)
			DRETURN(DLEVEL_STABLE);

		if (i->path)
			edje_object_file_set(i->image, i->path, i->key);
		edje_object_size_min_get(i->image, &i->ow, &i->oh);
	} else {
		if (!i->image)
			i->image = ewl_embed_object_request(emb, EWL_IMAGE_TYPE);
		if (!i->image)
			i->image = evas_object_image_add(emb->evas);
		if (!i->image)
			DRETURN(DLEVEL_STABLE);

		if (i->path)
			evas_object_image_file_set(i->image, i->path, i->key);
		evas_object_image_size_get(i->image, &i->ow, &i->oh);
	}



	evas_object_layer_set(i->image, ewl_widget_layer_sum_get(w));
	if (w->fx_clip_box)
		evas_object_clip_set(i->image, w->fx_clip_box);

	evas_object_pass_events_set(i->image, TRUE);
	evas_object_show(i->image);

	if (!i->ow)
		i->ow = 1;
	if (!i->oh)
		i->oh = 1;

	/*
	 * Bound the scales when proportional.
	 */
	if (i->proportional) {
		if (i->sw < i->sh)
			sh = i->sw;
		else
			sw = i->sh;
	}

	sw = i->sw;
	sh = i->sh;

	/*
	 * Bound to absolute size.
	 */
	if (i->cs) {
		ww = hh = i->cs;
	}
	else {
		if (i->aw)
			ww = i->aw;
		else
			ww = i->ow;

		if (i->ah)
			hh = i->ah;
		else
			hh = i->oh;
	}

	ewl_object_preferred_inner_w_set(EWL_OBJECT(i), sw * ww);
	ewl_object_preferred_inner_h_set(EWL_OBJECT(i), sh * hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_obscure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	emb = ewl_embed_widget_find(w);

	i = EWL_IMAGE(w);
	if (emb && i->image) {
		ewl_embed_object_cache(emb, i->image);
		i->image = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					  void *user_data __UNUSED__)
{
	Ewl_Image *i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);

	IF_FREE(i->path);
	IF_FREE(i->key);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_reparent_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Image *i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	if (!i->image)
		DRETURN(DLEVEL_STABLE);

	evas_object_layer_set(i->image, ewl_widget_layer_sum_get(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;
	int ww, hh;
	int dx = 0, dy = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	if (!i->image)
		DRETURN(DLEVEL_STABLE);

	emb = ewl_embed_widget_find(w);

	ww = CURRENT_W(w);
	hh = CURRENT_H(w);
	if (i->cs) {
		/*
		 * Limit to the constraining size
		 */
		if (ww > i->cs)
			ww = i->cs;
		if (hh > i->cs)
			hh = i->cs;

		/*
		 * Use images original size if smaller than constraint.
		 */
		if (ww > i->ow)
			ww = i->ow;
		if (hh > i->oh)
			hh = i->oh;
	}

	/*
	 * Fit the proportional scale.
	 */
	if (i->proportional) {
		double sw, sh;

		sw = (double)ww / (double)i->ow;
		sh = (double)hh / (double)i->oh;
		if (sw < sh) {
			hh = sw * i->oh;
		}
		else {
			ww = sh * i->ow;
		}
	}

	/*
	 * set the tile width and height if not set already
	 */
	if (!i->tile.set) {
		i->tile.x = i->tile.y = 0;
		i->tile.w = ww;
		i->tile.h = hh;
	}

	dx = (CURRENT_W(w) - ww) / 2;
	dy = (CURRENT_H(w) - hh) / 2;

	/*
	 * Move the image into place based on type.
	 */
	if (i->type != EWL_IMAGE_TYPE_EDJE)
		evas_object_image_fill_set(i->image, i->tile.x, i->tile.y,
					i->tile.w, i->tile.h);

	evas_object_move(i->image, CURRENT_X(w) + dx, CURRENT_Y(w) + dy);
	evas_object_resize(i->image, ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Determine the type of the file based on the filename.
 */
static Ewl_Image_Type
ewl_image_type_get(const char *i)
{
	int l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("i", i, -1);

	l = strlen(i);

	if ((l >= 8 && !(strncasecmp((char *) i + l - 8, ".bits.db", 8)))
	    || (l >= 4 && !(strncasecmp((char *) i + l - 4, ".eet", 4)))
	    || (l >= 4 && !(strncasecmp((char *) i + l - 4, ".edj", 4)))
	    || (l >= 4 && !(strncasecmp((char *) i + l - 4, ".eap", 5))))
		DRETURN_INT(EWL_IMAGE_TYPE_EDJE, DLEVEL_STABLE);

	DRETURN_INT(EWL_IMAGE_TYPE_NORMAL, DLEVEL_STABLE);
}

void
ewl_image_mouse_down_cb(Ewl_Widget *w, void *ev_data,
					void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;
	Ewl_Event_Mouse_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	emb = ewl_embed_widget_find(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EDJE)
		evas_event_feed_mouse_down(emb->evas, ev->button, 
				EVAS_BUTTON_NONE, 
				(unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff), 
				NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_mouse_up_cb(Ewl_Widget *w, void *ev_data,
					void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;
	Ewl_Event_Mouse_Up *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	emb = ewl_embed_widget_find(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EDJE && emb)
		evas_event_feed_mouse_up(emb->evas, ev->button, 
				EVAS_BUTTON_NONE, 
				(unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff), 
				NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_image_mouse_move_cb(Ewl_Widget *w, void *ev_data,
					void *user_data __UNUSED__)
{
	Ewl_Image *i;
	Ewl_Embed *emb;
	Ewl_Event_Mouse_Move *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	i = EWL_IMAGE(w);
	emb = ewl_embed_widget_find(w);
	ev = ev_data;

	if (i->type == EWL_IMAGE_TYPE_EDJE)
		evas_event_feed_mouse_move(emb->evas, ev->x, ev->y, 
				(unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff), 
				NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

