#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

/**
 * @defgroup Ewl_Image Image: An Image Display Widget
 * Provides a widget for displaying evas loadable images, and edjes.
 *
 * @{
 */

/**
 * @themekey /image/file
 * @themekey /image/group
 */

typedef enum Ewl_Image_Type Ewl_Image_Type;

enum Ewl_Image_Type
{
	EWL_IMAGE_TYPE_NORMAL, /**< Standard image type */
	EWL_IMAGE_TYPE_EDJE /**< Edje image type */
};

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
};

Ewl_Widget     *ewl_image_new(char *i, char *k);
void            ewl_image_init(Ewl_Image * i, char *path, char *key);
void            ewl_image_set_file(Ewl_Image * i, char *im, char *key);
void            ewl_image_set_proportional(Ewl_Image * i, char p);
void		ewl_image_scale(Ewl_Image *i, double wp, double hp);
void		ewl_image_scale_to(Ewl_Image *i, int w, int h);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_image_realize_cb(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            ewl_image_unrealize_cb(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            ewl_image_configure_cb(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            ewl_image_mouse_down_cb(Ewl_Widget * w, void *ev_data,
				        void *user_data);
void            ewl_image_mouse_up_cb(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            ewl_image_mouse_move_cb(Ewl_Widget * w, void *ev_data,
					void *user_data);

/**
 * @}
 */

#endif				/* __EWL_IMAGE_H__ */
