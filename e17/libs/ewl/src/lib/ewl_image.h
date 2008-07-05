/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_IMAGE_H
#define EWL_IMAGE_H

#include "ewl_view.h"
/**
 * @addtogroup Ewl_Image Ewl_Image: An Image Display Widget
 * Provides a widget for displaying evas loadable images, and edjes.
 *
 * @remarks Inherits from Ewl_Widget.
 * @if HAVE_IMAGES
 * @image html Ewl_Image_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /image/file
 * @themekey /image/group
 */

/**
 * @def EWL_IMAGE_TYPE
 * The type name for the Ewl_Image widget
 */
#define EWL_IMAGE_TYPE "image"

/**
 * @def EWL_IMAGE_IS(w)
 * Returns TRUE if the widget is an Ewl_Image, FALSE otherwise
 */
#define EWL_IMAGE_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_IMAGE_TYPE))

/**
 * The Ewl_Image widget
 */
typedef struct Ewl_Image Ewl_Image;

/**
 * @def EWL_IMAGE(image)
 * Typecase a pointer to an Ewl_Image widget
 */
#define EWL_IMAGE(image) ((Ewl_Image *) image)

/**
 * @brief Inherits from Ewl_Widget and extends to provide an image widget
 */
struct Ewl_Image
{
        Ewl_Widget widget;        /**< Inherit from Ewl_Widget */
        Ewl_Image_Type type;        /**< The type of the image */
        void *image;        /**< The evas object for the image */
        char *path;                /**< The path to the image */
        char *key;                /**< The key for the image */
        char proportional;        /**< Is the image displayed proportional */
        int ow,                 /**< Original image width */
            oh;                        /**< Original image height */
        double sw,                 /**< Scale width */
               sh;                /**< Scale height */
        int cs;                        /**< Constrain size */
        int aw,                 /**< Scale width */
           ah;                /**< Scale height */

        struct {
                int set;        /**< Tiling set? */
                int x,                 /**< Tile x start */
                    y,                 /**< Tile y start */
                    w,                 /**< Tile width */
                    h;                /**< Tile height */
        } tile;                        /**< Image tiling information */
};

/**
 * @def EWL_IMAGE_THUMBNAIL_TYPE
 * The type name for the Ewl_Image_Thumbnail widget
 */
#define EWL_IMAGE_THUMBNAIL_TYPE "image_thumbnail"

/**
 * @brief Inherits from Ewl_Image and extends to provide reference to original image.
 */
struct Ewl_Image_Thumbnail
{
        Ewl_Image image;        /**< Inherit from Ewl_Image */
        void *thumb; /**< Outstanding request to thumbnail image */
        Ewl_Thumbnail_Size size; /**< Size of the thumbnails>*/
        Ewl_Widget *orig;  /**< Reference to image used to create thumb */
};

/**
 * The Ewl_Image_Thumbnail widget
 */
typedef struct Ewl_Image_Thumbnail Ewl_Image_Thumbnail;

/**
 * @def EWL_IMAGE_THUMBNAIL(image)
 * Typecase a pointer to an Ewl_Image_Thumbnail widget
 */
#define EWL_IMAGE_THUMBNAIL(image) ((Ewl_Image_Thumbnail *) image)

Ewl_Widget              *ewl_image_new(void);
int                      ewl_image_init(Ewl_Image *i);
Ewl_View                *ewl_image_view_get(void);

void                     ewl_image_file_set(Ewl_Image *i, const char *im,
                                        const char *key);
void                     ewl_image_file_path_set(Ewl_Image *i, const char *im);
const char               *ewl_image_file_path_get(Ewl_Image *i);

void                     ewl_image_file_key_set(Ewl_Image *i, const char *key);
const char              *ewl_image_file_key_get(Ewl_Image *i);

void                     ewl_image_proportional_set(Ewl_Image *i, char p);
char                     ewl_image_proportional_get(Ewl_Image *i);
void                     ewl_image_scale_set(Ewl_Image *i, double wp,
                                        double hp);
void                     ewl_image_scale_get(Ewl_Image *i, double *wp,
                                        double *hp);
void                     ewl_image_size_set(Ewl_Image *i, int w, int h);
void                     ewl_image_size_get(Ewl_Image *i, int *w, int *h);
void                     ewl_image_tile_set(Ewl_Image *i, int x, int y, int w,
                                        int h);
void                     ewl_image_constrain_set(Ewl_Image *i,
                                        unsigned int size);
unsigned int             ewl_image_constrain_get(Ewl_Image *i);

void                     ewl_image_flip(Ewl_Image *img, Ewl_Orientation orient);
void                     ewl_image_rotate(Ewl_Image *i, Ewl_Rotate rotate);

Ewl_Widget              *ewl_image_thumbnail_get(Ewl_Image *i);

Ewl_Widget              *ewl_image_thumbnail_new(void);
int                      ewl_image_thumbnail_init(Ewl_Image_Thumbnail *image);
void                     ewl_image_thumbnail_size_set
                                        (Ewl_Image_Thumbnail *thumb,
                                        Ewl_Thumbnail_Size s);
Ewl_Thumbnail_Size       ewl_image_thumbnail_size_get
                                        (Ewl_Image_Thumbnail *thumb);
void                     ewl_image_thumbnail_request(Ewl_Image_Thumbnail *thumb,
                                        const char *path);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_image_cb_reveal(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_obscure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_mouse_down(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_mouse_up(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_mouse_move(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */

#endif
