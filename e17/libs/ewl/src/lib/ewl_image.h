#ifndef EWL_IMAGE_H
#define EWL_IMAGE_H

#ifdef BUILD_EPSILON_SUPPORT
#include <Epsilon.h>
#include <Epsilon_Request.h>
#endif

/**
 * @addtogroup Ewl_Image Ewl_Image: An Image Display Widget
 * Provides a widget for displaying evas loadable images, and edjes.
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

enum Ewl_Image_Type
{
	EWL_IMAGE_TYPE_NORMAL, /**< Standard image type */
	EWL_IMAGE_TYPE_EDJE /**< Edje image type */
};
typedef enum Ewl_Image_Type Ewl_Image_Type;

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
 * Inherits from Ewl_Widget and extends to provide an image widget
 */
struct Ewl_Image
{
	Ewl_Widget      widget;		/**< Inherit from Ewl_Widget */
	Ewl_Image_Type  type;		/**< The type of the image */
	Evas_Object    *image;		/**< The evas object for the image */
	char           *path;		/**< The path to the image */
	char           *key;		/**< The key for the image */
	char		proportional;	/**< Is the image displayed proportional */
	int 		ow, 		/**< Original image width */
			oh;		/**< Original image height */
	double		sw, 		/**< Scale width */
			sh;		/**< Scale height */
	int		cs;		/**< Constrain size */
	Evas_Coord	aw, 		/**< Scale width */
			ah;		/**< Scale height */

	struct {
		int set;		/**< Tiling set? */
		int x, 			/**< Tile x start */
		    y, 			/**< Tile y start */
		    w, 			/**< Tile width */
		    h;			/**< Tile height */
	} tile;				/**< Image tiling information */
};

/**
 * @def EWL_IMAGE_THUMBNAIL_TYPE
 * The type name for the Ewl_Image_Thumbnail widget
 */
#define EWL_IMAGE_THUMBNAIL_TYPE "image_thumbnail"

/**
 * Inherits from Ewl_Image and extends to provide reference to original image.
 */
struct Ewl_Image_Thumbnail
{
	Ewl_Image        image;	/**< Inherit from Ewl_Image */
#ifdef BUILD_EPSILON_SUPPORT
	Epsilon_Request *thumb; /**< Outstanding request to thumbnail image */
#else
	void            *thumb; /**< Not supported :( */
#endif
	Ewl_Widget      *orig;  /**< Reference to image used to create thumb */
};

/**
 * The Ewl_Image_Thumbnail widget
 */
typedef struct Ewl_Image_Thumbnail Ewl_Image_Thumbnail;

/**
 * @def EWL_IMAGE_Thumbnail(image)
 * Typecase a pointer to an Ewl_Image_Thumbnail widget
 */
#define EWL_IMAGE_THUMBNAIL(image) ((Ewl_Image_Thumbnail *) image)

Ewl_Widget	*ewl_image_new(void);
int		 ewl_image_init(Ewl_Image *i);

void		 ewl_image_file_set(Ewl_Image *i, const char *im, 
						const char *key);
void		 ewl_image_file_path_set(Ewl_Image *i, const char *im);
const char	*ewl_image_file_path_get(Ewl_Image *i);

void		 ewl_image_file_key_set(Ewl_Image *i, const char *key);
const char 	*ewl_image_file_key_get(Ewl_Image *i);

void		 ewl_image_proportional_set(Ewl_Image *i, char p);
char	 	 ewl_image_proportional_get(Ewl_Image *i);
void		 ewl_image_scale_set(Ewl_Image *i, double wp, double hp);
void		 ewl_image_scale_get(Ewl_Image *i, double *wp, double *hp);
void		 ewl_image_size_set(Ewl_Image *i, int w, int h);
void		 ewl_image_size_get(Ewl_Image *i, int *w, int *h);
void		 ewl_image_tile_set(Ewl_Image *i, int x, int y, 
						int w, int h);
void 		 ewl_image_constrain_set(Ewl_Image *i, unsigned int size);
unsigned int	 ewl_image_constrain_get(Ewl_Image *i);

Ewl_Widget      *ewl_image_thumbnail_get(Ewl_Image *i);

Ewl_Widget      *ewl_image_thumbnail_new(void);
int              ewl_image_thumbnail_init(Ewl_Image_Thumbnail *image);
void             ewl_image_thumbnail_request(Ewl_Image_Thumbnail *thumb,
					     char *path);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_image_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_obscure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_image_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data );

/**
 * @}
 */

#endif
