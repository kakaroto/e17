/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_image.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#include <Evas.h>
#include <Edje.h>

#ifdef BUILD_EPSILON_SUPPORT
#include <Epsilon.h>
#include <Epsilon_Request.h>

static Ecore_Event_Handler *ewl_image_epsilon_handler = NULL;
static int ewl_image_thumbnail_cb_complete(void *data, int type, void *event);
static void ewl_image_thumbnail_cb_value_changed(Ewl_Widget *w, void *ev, void *data);
#endif


static Ewl_Image_Type  ewl_image_type_get(const char *i);
static void ewl_image_thumbnail_cb_destroy(Ewl_Widget *w, void *ev, void *data);

static void ewl_image_rotate_180(Ewl_Image *img);
static void ewl_image_rotate_90(Ewl_Image *img, int cc);

static Ewl_Widget *ewl_image_view_cb_header_fetch(void *data, unsigned int col);
static Ewl_Widget *ewl_image_view_cb_widget_fetch(void *data, unsigned int row,
                                                        unsigned int col);

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
 * @return Returns a view that can be used to display Ewl_Image widgets
 * @brief Creates and returns a view to be used by Ewl_Image widgets
 */
Ewl_View *
ewl_image_view_get(void)
{
        Ewl_View *view;

        DENTER_FUNCTION(DLEVEL_STABLE);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, ewl_image_view_cb_widget_fetch);
        ewl_view_header_fetch_set(view, ewl_image_view_cb_header_fetch);

        DRETURN_PTR(view, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_image_view_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                                unsigned int col __UNUSED__)
{
        Ewl_Widget *image;

        DENTER_FUNCTION(DLEVEL_STABLE);

        image = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(image), data);

        DRETURN_PTR(image, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_image_view_cb_header_fetch(void *data, unsigned int col __UNUSED__)
{
        Ewl_Widget *image;

        DENTER_FUNCTION(DLEVEL_STABLE);

        image = ewl_image_new();
        ewl_image_file_path_set(EWL_IMAGE(image), data);

        DRETURN_PTR(image, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR_RET(i, FALSE);

        w = EWL_WIDGET(i);

        if (!ewl_widget_init(w))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_IMAGE_TYPE);
        ewl_widget_inherit(w, EWL_IMAGE_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);

        /*
         * Append necessary callbacks.
         */
        ewl_callback_append(w, EWL_CALLBACK_REVEAL, ewl_image_cb_reveal,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_OBSCURE, ewl_image_cb_obscure,
                            NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, ewl_image_cb_destroy,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, ewl_image_cb_configure,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, ewl_image_cb_mouse_down,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, ewl_image_cb_mouse_up,
                            NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, ewl_image_cb_mouse_move,
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
        DCHECK_PARAM_PTR(i);
        DCHECK_PARAM_PTR(path);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR_RET(i, NULL);
        DCHECK_TYPE_RET(i, EWL_IMAGE_TYPE, NULL);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR_RET(i, NULL);
        DCHECK_TYPE_RET(i, EWL_IMAGE_TYPE, NULL);

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
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

        w = EWL_WIDGET(i);

        IF_FREE(i->path);
        IF_FREE(i->key);

        /*
         * Determine the type of image to be loaded.
         */
        if (im) {
                i->type = ewl_image_type_get(im);
                i->path = strdup(im);
                if (key) i->key = strdup(key);
        }
        else
                i->type = EWL_IMAGE_TYPE_NORMAL;

        /*
         * Load the new image if widget has been realized
         */
        if (REALIZED(w)) {
                ewl_widget_obscure(w);
                ewl_widget_reveal(w);
        }

        ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);

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
        int osize;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

        osize = i->cs;
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
        else {
                if (ewl_object_preferred_inner_w_get(EWL_OBJECT(i)) == osize)
                        ewl_object_preferred_inner_w_set(EWL_OBJECT(i), i->ow);
                if (ewl_object_preferred_inner_h_get(EWL_OBJECT(i)) == osize)
                        ewl_object_preferred_inner_h_set(EWL_OBJECT(i), i->oh);
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
        DCHECK_PARAM_PTR_RET(i, 0);
        DCHECK_TYPE_RET(i, EWL_IMAGE_TYPE, 0);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR_RET(i, FALSE);
        DCHECK_TYPE_RET(i, EWL_IMAGE_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

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
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

        i->tile.set = 1;
        i->tile.x = x;
        i->tile.y = y;
        i->tile.w = w;
        i->tile.h = h;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param img: The image to flip
 * @param orient: The orientation to flip
 * @return Returns no value
 * @brief Flips the given image in the given direction
 */
void
ewl_image_flip(Ewl_Image *img, Ewl_Orientation orient)
{
        int ix, iy, ox, oy, mx, my, i, j, w, h;
        int *ia, *ib, *oa, *ob, s;
        unsigned int *in;
        unsigned int tmp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(img);
        DCHECK_TYPE(img, EWL_IMAGE_TYPE);

        evas_object_image_size_get(img->image, &w, &h);
        in = evas_object_image_data_get(img->image, TRUE);

        if (orient == EWL_ORIENTATION_VERTICAL)
        {
                mx = w;
                my = h / 2;
                ia = &iy;
                ib = &ix;
                oa = &oy;
                ob = &ox;
                s = h;
        }
        else
        {
                mx = w / 2;
                my = h;
                ia = &ix;
                ib = &iy;
                oa = &ox;
                ob = &oy;
                s = w;
        }

        for (iy = 0; iy < my; iy++)
        {
                for (ix = 0; ix < mx; ix++)
                {
                        *oa = s - 1 - *ia;
                        *ob = *ib;

                        i = iy * w + ix;
                        j = oy * w + ox;

                        tmp = in[j];
                        in[j] = in[i];
                        in[i] = tmp;
                }
        }

        evas_object_image_data_set(img->image, in);
        evas_object_image_data_update_add(img->image, 0, 0, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param i: The image to rotate
 * @param rotate: The amount to rotate the image
 * @return Returns no value
 * @brief Rotates the given image by the given @a rotate value
 */
void
ewl_image_rotate(Ewl_Image *i, Ewl_Rotate rotate)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(i);
        DCHECK_TYPE(i, EWL_IMAGE_TYPE);

        if (rotate == EWL_ROTATE_180)
                ewl_image_rotate_180(i);

        else if ((rotate == EWL_ROTATE_CW_90) ||
                        (rotate == EWL_ROTATE_CC_270))
                ewl_image_rotate_90(i, FALSE);

        else
                ewl_image_rotate_90(i, TRUE);


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
        DCHECK_PARAM_PTR_RET(i, NULL);
        DCHECK_TYPE_RET(i, EWL_IMAGE_TYPE, NULL);

#ifdef BUILD_EPSILON_SUPPORT
        if (i->path && (i->type == EWL_IMAGE_TYPE_NORMAL)) {
                thumb = ewl_image_thumbnail_new();
                if (thumb) {
                        ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(thumb),
                                        (char *)ewl_image_file_path_get(i));
                        EWL_IMAGE_THUMBNAIL(thumb)->orig = EWL_WIDGET(i);
                        ewl_callback_append(EWL_WIDGET(i),
                                        EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_image_thumbnail_cb_value_changed,
                                        thumb);
                }
        }
#endif

        DRETURN_PTR(thumb, DLEVEL_STABLE);
}

#ifdef BUILD_EPSILON_SUPPORT
static void
ewl_image_thumbnail_cb_value_changed(Ewl_Widget *w, void *ev __UNUSED__
                                        , void *data)
{
        Ewl_Widget *thumb;
        Ewl_Widget *image;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);

        thumb = data;
        image = w;

        ewl_image_thumbnail_request(EWL_IMAGE_THUMBNAIL(thumb),
                        ewl_image_file_path_get(EWL_IMAGE(image)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}
#endif

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
        DCHECK_PARAM_PTR_RET(image, FALSE);

        if (!ewl_image_init(EWL_IMAGE(image)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(image),
                                        EWL_IMAGE_THUMBNAIL_TYPE);
        ewl_widget_inherit(EWL_WIDGET(image), EWL_IMAGE_THUMBNAIL_TYPE);

        ewl_callback_prepend(EWL_WIDGET(image), EWL_CALLBACK_DESTROY,
                            ewl_image_thumbnail_cb_destroy, NULL);
        image->size = EWL_THUMBNAIL_SIZE_NORMAL;

#ifdef BUILD_EPSILON_SUPPORT
        if (!ewl_image_epsilon_handler) {
                epsilon_request_init();
                ewl_image_epsilon_handler =
                        ecore_event_handler_add(EPSILON_EVENT_DONE,
                                        ewl_image_thumbnail_cb_complete,
                                        NULL);
        }
#endif

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param thumb: The image thumbnail to set the size on
 * @param s: The Ewl_Thumbnail_Size to set for the image
 * @return Returns no value.
 * @brief This will set the size of the thumbnails
 */
void
ewl_image_thumbnail_size_set(Ewl_Image_Thumbnail *thumb, Ewl_Thumbnail_Size s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(thumb);
        DCHECK_TYPE(thumb, EWL_IMAGE_THUMBNAIL_TYPE);

        thumb->size = s;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param thumb: The image thumbnail to get the size from
 * @return Returns the size of the thumbnail.
 * @brief This will return the current size of thumbnails
 */
Ewl_Thumbnail_Size
ewl_image_thumbnail_size_get(Ewl_Image_Thumbnail *thumb)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(thumb, EWL_THUMBNAIL_SIZE_NORMAL);

        DRETURN_INT(thumb->size, DLEVEL_STABLE);
}

/**
 * @param thumb: The thumbnail to request
 * @param path: The path to the image
 * @return Returns no value.
 * @brief This will request the generation of a thumbnail for the image
 * given by @a path.
 */
void
ewl_image_thumbnail_request(Ewl_Image_Thumbnail *thumb, const char *path)
{
#ifdef BUILD_EPSILON_SUPPORT
        Ewl_Thumbnail_Size size;
#endif
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(thumb);
        DCHECK_TYPE(thumb, EWL_IMAGE_THUMBNAIL_TYPE);
        DCHECK_PARAM_PTR(path);

#ifdef BUILD_EPSILON_SUPPORT
        if (thumb->size == EWL_THUMBNAIL_SIZE_NORMAL)
                size = EPSILON_THUMB_NORMAL;
        else size = EPSILON_THUMB_LARGE;

        thumb->thumb = epsilon_request_add(path, size, thumb);
#else
        thumb->thumb = NULL;
#endif

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_image_thumbnail_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Image_Thumbnail *thumb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_THUMBNAIL_TYPE);

        thumb = EWL_IMAGE_THUMBNAIL(w);
        if (thumb->thumb) {
#ifdef BUILD_EPSILON_SUPPORT
                epsilon_request_del(thumb->thumb);
#endif
                thumb->thumb = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

#ifdef BUILD_EPSILON_SUPPORT
static int
ewl_image_thumbnail_cb_complete(void *data __UNUSED__, int type __UNUSED__,
                                                                void *event)
{
        Ewl_Image_Thumbnail *thumb;
        Epsilon_Request *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(event, FALSE);

        ev = event;
        thumb = ev->data;

        if (thumb)
        {
                if (ev->dest)
                {
                        ewl_image_file_path_set(EWL_IMAGE(thumb), ev->dest);
                        ewl_callback_call(EWL_WIDGET(thumb),
                                        EWL_CALLBACK_VALUE_CHANGED);
                }

                thumb->thumb = NULL;
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}
#endif

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_image_cb_reveal(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Image *i;
        Ewl_Embed *emb;
        int ww, hh;
        double sw, sh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);
        emb = ewl_embed_widget_find(w);

        /*
         * Load the image based on the type.
         */
        if (i->type == EWL_IMAGE_TYPE_EDJE) {
                if (!i->image)
                        i->image = ewl_embed_object_request(emb, "edje");
                if (!i->image)
                        i->image = edje_object_add(emb->canvas);
                if (!i->image)
                        DRETURN(DLEVEL_STABLE);

                if (i->path)
                        edje_object_file_set(i->image, i->path, i->key);
                edje_object_size_min_get(i->image, &i->ow, &i->oh);
        } else {
                if (!i->image)
                        i->image = ewl_embed_object_request(emb, EWL_IMAGE_TYPE);
                if (!i->image)
                        i->image = evas_object_image_add(emb->canvas);
                if (!i->image)
                        DRETURN(DLEVEL_STABLE);

                if (i->path)
                        evas_object_image_file_set(i->image, i->path, i->key);
                evas_object_image_size_get(i->image, &i->ow, &i->oh);
        }

        evas_object_smart_member_add(i->image, w->smart_object);
        if (w->fx_clip_box)
                evas_object_stack_below(i->image, w->fx_clip_box);

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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The obscure callback
 */
void
ewl_image_cb_obscure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Image *i;
        Ewl_Embed *emb;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        emb = ewl_embed_widget_find(w);

        i = EWL_IMAGE(w);
        if (emb && i->image) {
                evas_object_image_file_set(i->image, NULL, NULL);
                ewl_embed_object_cache(emb, i->image);
                i->image = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_image_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                          void *user_data __UNUSED__)
{
        Ewl_Image *i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);

        IF_FREE(i->path);
        IF_FREE(i->key);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The reparent callback
 */
void
ewl_image_cb_reparent(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Image *i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);
        if (!i->image)
                DRETURN(DLEVEL_STABLE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_image_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        Ewl_Image *i;
        int ww, hh;
        int dx = 0, dy = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        i = EWL_IMAGE(w);
        if (!i->image)
                DRETURN(DLEVEL_STABLE);

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
        DCHECK_PARAM_PTR_RET(i, -1);

        l = strlen(i);

        if ((l >= 8 && !(strncasecmp(i + l - 8, ".bits.db", 8)))
            || (l >= 4 && !(strncasecmp(i + l - 4, ".eet", 4)))
            || (l >= 4 && !(strncasecmp(i + l - 4, ".edj", 4)))
            || (l >= 4 && !(strncasecmp(i + l - 4, ".eap", 4))))
                DRETURN_INT(EWL_IMAGE_TYPE_EDJE, DLEVEL_STABLE);

        DRETURN_INT(EWL_IMAGE_TYPE_NORMAL, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Down data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_image_cb_mouse_down(Ewl_Widget *w, void *ev_data,
                                        void *user_data __UNUSED__)
{
        Ewl_Image *i;
        Ewl_Embed *emb;
        Ewl_Event_Mouse_Down *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);
        emb = ewl_embed_widget_find(w);
        ev = ev_data;

        if (i->type == EWL_IMAGE_TYPE_EDJE)
                evas_event_feed_mouse_down(emb->canvas, ev->button,
                                EVAS_BUTTON_NONE,
                                (unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff),
                                NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Up data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_image_cb_mouse_up(Ewl_Widget *w, void *ev_data,
                                        void *user_data __UNUSED__)
{
        Ewl_Image *i;
        Ewl_Embed *emb;
        Ewl_Event_Mouse_Up *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);
        emb = ewl_embed_widget_find(w);
        ev = ev_data;

        if (i->type == EWL_IMAGE_TYPE_EDJE && emb)
                evas_event_feed_mouse_up(emb->canvas, ev->button,
                                EVAS_BUTTON_NONE,
                                (unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff),
                                NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: The Ewl_Event_Mouse_Move data
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_image_cb_mouse_move(Ewl_Widget *w, void *ev_data,
                                        void *user_data __UNUSED__)
{
        Ewl_Image *i;
        Ewl_Embed *emb;
        Ewl_Event_Mouse *ev;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_IMAGE_TYPE);

        i = EWL_IMAGE(w);
        emb = ewl_embed_widget_find(w);
        ev = ev_data;

        if (i->type == EWL_IMAGE_TYPE_EDJE)
                evas_event_feed_mouse_move(emb->canvas, ev->x, ev->y,
                                (unsigned int)((unsigned long long)(ecore_time_get() * 1000.0) & 0xffffffff),
                                NULL);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_image_rotate_90(Ewl_Image *img, int cc)
{
        int i, j, w = 0, h = 0, ix, iy, ox, oy, os, ow, oh;
        int *ia, *ib, *oa, *ob;
        unsigned int *in, *out;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(img);
        DCHECK_TYPE(img, EWL_IMAGE_TYPE);

        evas_object_image_size_get(img->image, &w, &h);
        in = evas_object_image_data_get(img->image, FALSE);

        out = malloc(w * h * sizeof(unsigned int));

        ow = h;
        oh = w;

        /* these pointers pull the invarient conditional out of the loop */
        if (cc)
        {
                oa = &oy;
                ob = &ox;
                ia = &iy;
                ib = &ix;
                os = oh;
        }
        else
        {
                oa = &ox;
                ob = &oy;
                ia = &ix;
                ib = &iy;
                os = ow;
        }

        for (i = 0; i < (w * h); i++)
        {
                ix = i % w;
                iy = i / w;

                /* rotate */
                *oa = os - 1 - *ib;
                *ob = *ia;

                /* convert back to array index */
                j = oy * ow + ox;

                out[j] = in[i];
        }

        img->ow = ow;
        img->oh = oh;

        evas_object_image_size_set(img->image, ow, oh);
        evas_object_image_data_set(img->image, out);
        evas_object_image_data_update_add(img->image, 0, 0, ow, oh);

        ewl_object_preferred_inner_size_set(EWL_OBJECT(img), ow, oh);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_image_rotate_180(Ewl_Image *img)
{
        int ix, iy, ox, oy, i, j, size, w, h;
        unsigned int *in, tmp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(img);
        DCHECK_TYPE(img, EWL_IMAGE_TYPE);

        evas_object_image_size_get(img->image, &w, &h);
        in = evas_object_image_data_get(img->image, TRUE);

        size = w * h / 2;
        for (i = 0; i < size; i++)
        {
                ix = i % w;
                iy = i / w;
                ox = w - 1 - ix;
                oy = h - 1 - iy;
                j = oy * w + ox;

                tmp = in[j];
                in[j] = in[i];
                in[i] = tmp;
        }

        evas_object_image_data_set(img->image, in);
        evas_object_image_data_update_add(img->image, 0, 0, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

