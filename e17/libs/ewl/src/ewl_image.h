
#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

typedef enum _ewl_image_type Ewl_Image_Type;

enum _ewl_image_type {
	EWL_IMAGE_TYPE_NORMAL,
	EWL_IMAGE_TYPE_EDJE
};

typedef struct _ewl_image Ewl_Image;

#define EWL_IMAGE(image) ((Ewl_Image *) image)

struct _ewl_image {
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

#endif				/* __EWL_IMAGE_H__ */
