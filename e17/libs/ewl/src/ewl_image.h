#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

/**
 * @defgroup Ewl_Image An Image Display Widget
 * Provides a widget for displaying evas loadable images, and edjes.
 *
 * @{
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
	void           *image;
	char           *path;
	char		proportional;
	int 		ow, oh;
	double		sw, sh;
};

Ewl_Widget     *ewl_image_new(char *i);
void            ewl_image_init(Ewl_Image * i, char *path);
void            ewl_image_set_file(Ewl_Image * i, char *im);
void            ewl_image_set_proportional(Ewl_Image * i, char p);
void		ewl_image_scale(Ewl_Image *i, double wp, double hp);
void		ewl_image_scale_to(Ewl_Image *i, int w, int h);

/**
 * @}
 */

#endif				/* __EWL_IMAGE_H__ */
