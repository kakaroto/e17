
#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

typedef enum _ewl_image_type Ewl_Image_Type;

enum _ewl_image_type {
	EWL_IMAGE_TYPE_NORMAL,
	EWL_IMAGE_TYPE_EBITS
};

typedef struct _ewl_image Ewl_Image;

#define EWL_IMAGE(image) ((Ewl_Image *) image)

struct _ewl_image {
	Ewl_Widget      widget;
	Ewl_Image_Type  type;
	void           *image;
	char           *path;
};

Ewl_Widget     *ewl_image_load(const char *i);
void            ewl_image_init(Ewl_Image * i);
void            ewl_image_set_file(Ewl_Image * i, const char *im);

#endif				/* __EWL_IMAGE_H__ */
