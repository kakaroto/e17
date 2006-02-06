#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

/**
 * @file ewl_image.h
 * @defgroup Ewl_Image Image: An Image Display Widget
 * Provides a widget for displaying evas loadable images, and edjes.
 *
 * @{
 */

/**
 * @themekey /image/file
 * @themekey /image/group
 */

#define EWL_IMAGE_TYPE "image"

enum Ewl_Image_Type
{
	EWL_IMAGE_TYPE_NORMAL, /**< Standard image type */
	EWL_IMAGE_TYPE_EDJE /**< Edje image type */
};

typedef enum Ewl_Image_Type Ewl_Image_Type;

typedef struct Ewl_Image Ewl_Image;

#define EWL_IMAGE(image) ((Ewl_Image *) image)

/**
 * @struct Ewl_Image
 *
 */
struct Ewl_Image
{
	Ewl_Widget      widget;
	Ewl_Image_Type  type;
	Evas_Object    *image;
	char           *path;
	char           *key;
	char		proportional;
	int 		ow, oh;
	double		sw, sh;
	int		cs;
	Evas_Coord	aw, ah;

	struct {
		int set;
		int x, y, w, h;
	} tile;
};

Ewl_Widget	*ewl_image_new(void);
int		 ewl_image_init(Ewl_Image *i);

void		 ewl_image_file_set(Ewl_Image *i, const char *im, 
						const char *key);
void		 ewl_image_file_path_set(Ewl_Image *i, const char *im);
const char	*ewl_image_file_path_get(Ewl_Image *i);

void		 ewl_image_file_key_set(Ewl_Image *i, const char *key);
const char 	*ewl_image_file_key_get(Ewl_Image *i);

void		 ewl_image_proportional_set(Ewl_Image *i, char p);
void		 ewl_image_scale(Ewl_Image *i, double wp, double hp);
void		 ewl_image_scale_to(Ewl_Image *i, int w, int h);
void		 ewl_image_tile_set(Ewl_Image *i, int x, int y, 
						int w, int h);
void 		 ewl_image_constrain_set(Ewl_Image *i, unsigned int size);
unsigned int	 ewl_image_constrain_get(Ewl_Image *i);

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

#endif				/* __EWL_IMAGE_H__ */
