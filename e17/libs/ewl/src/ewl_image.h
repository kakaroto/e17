
#ifndef __EWL_IMAGE_H__
#define __EWL_IMAGE_H__

typedef enum _ewl_image_type Ewl_Image_Type;

enum _ewl_image_type
{
	EWL_IMAGE_TYPE_NORMAL,
	EWL_IMAGE_TYPE_EBITS
};

typedef struct _ewl_image Ewl_Image;

#define EWL_IMAGE(image) ((Ewl_Image *) image)

struct _ewl_image
{
	Ewl_Widget widget;
	Ewl_Image_Type type;
	void *image;
	char *path;
	struct
	{
		int r, g, b, a;
	}
	color;
	struct
	{
		int t, b, l, r;
	}
	border;
};

Ewl_Widget *ewl_image_load(const char *i);

void ewl_image_set_layer(Ewl_Widget * w, int l);
int ewl_image_get_layer(Ewl_Widget * w);
void ewl_image_lower(Ewl_Widget * w);
void ewl_image_raise(Ewl_Widget * w);

void ewl_image_move(Ewl_Widget * w, double x, double y);
void ewl_image_get_position(Ewl_Widget * w, double *x, double *y);

void ewl_image_resize(Ewl_Widget * w, double w2, double h);
void ewl_image_get_size(Ewl_Widget * w, double *w2, double *h);

void ewl_image_set_geometry(Ewl_Widget * w,
			    double x, double y, double w2, double h);
void ewl_image_get_geometry(Ewl_Widget * w,
			    double *x, double *y, double *w2, double *h);

void ewl_image_set_color(Ewl_Widget * w, int r, int g, int b, int a);
void ewl_image_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a);

void ewl_image_set_border(Ewl_Widget * w, int t, int b, int l, int r);
void ewl_image_get_border(Ewl_Widget * w, int *t, int *b, int *l, int *r);

#endif /* __EWL_IMAGE_H__ */
