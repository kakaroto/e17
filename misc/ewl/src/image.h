#ifndef _EWL_IMAGE_CLASS_H_
#define _EWL_IMAGE_CLASS_H_ 1

#include "includes.h"
#include "debug.h"
#include "util.h"
#include "ll.h"
#include "layout.h"
#include "imlib.h"

typedef struct _EwlImage   EwlImage;
typedef struct _EwlImLayer EwlImLayer;

struct _EwlImage	{
	EwlLL           ll;
	char            *name;
	char            *path; /* reference with fam later */
	EwlRect         *rect;
	EwlRRect        *rrect;

	EwlBool         visible;
	EwlBool         tile;

	Imlib_Image      im;
};

struct _EwlImLayer	{
	EwlLL           ll;
	char            *name;
	EwlRect         *rect;
	EwlRRect        *rrect;
	char             visible, render_alpha;
	EwlImage        *images;
};


EwlImage      *ewl_image_new();
EwlImage      *ewl_image_new_with_values(char *name, char *path,
                                         int *w, int *h, char visible);
void           ewl_image_init(EwlImage *im);
EwlImage      *ewl_image_dup(EwlImage *im);
void           ewl_image_free(EwlImage *im);

EwlImage      *ewl_image_load(char *path);
EwlBool        ewl_image_save(EwlImage *eim, char *path);

void           ewl_image_set_image(EwlImage *eim, Imlib_Image iim);
void           ewl_image_set_image_by_path(EwlImage *im, char *path);
Imlib_Image    ewl_image_get_image(EwlImage *im);
char          *ewl_image_get_image_path(EwlImage *im);

void           ewl_image_show(EwlImage *im);
void           ewl_image_hide(EwlImage *im);
void           ewl_image_set_name(EwlImage *im, char *name);
char          *ewl_image_get_name(EwlImage *im);
void           ewl_image_set_rect(EwlImage *im, EwlRect *rect);
EwlRect       *ewl_image_get_rect(EwlImage *im);
void           ewl_image_set_rrect(EwlImage *im, EwlRRect *rrect);
EwlRRect      *ewl_image_get_rrect(EwlImage *im);
void           ewl_image_set_visibility(EwlImage *im, char visibility);
char           ewl_image_get_visibility(EwlImage *im);


EwlImLayer    *ewl_imlayer_new();
EwlImLayer    *ewl_imlayer_new_with_values(char *name, int *w, int *h,
                                           char visible, char render_alpha);
void           ewl_imlayer_init(EwlImLayer *l);
EwlImLayer    *ewl_imlayer_dup(EwlImLayer *l);
void           ewl_imlayer_free(EwlImLayer *l);

void           ewl_imlayer_image_insert(EwlImLayer *l, EwlImage *im);
void           ewl_imlayer_image_remove(EwlImLayer *l, EwlImage *im);
void           ewl_imlayer_image_push(EwlImLayer *l, EwlImage *im);
EwlImage      *ewl_imlayer_image_pop(EwlImLayer *l);

void           ewl_imlayer_show(EwlImLayer *im);
void           ewl_imlayer_hide(EwlImLayer *im);
void           ewl_imlayer_set_name(EwlImLayer *l, char *name);
char          *ewl_imlayer_get_name(EwlImLayer *l);
void           ewl_imlayer_set_rect(EwlImLayer *l, EwlRect *rect);
EwlRect       *ewl_imlayer_get_rect(EwlImLayer *l);
void           ewl_imlayer_set_rrect(EwlImLayer *l, EwlRRect *rrect);
EwlRRect      *ewl_imlayer_get_rrect(EwlImLayer *l);
void           ewl_imlayer_set_visibility(EwlImLayer *l, char visibility);
char           ewl_imlayer_get_visibility(EwlImLayer *l);
void           ewl_imlayer_set_render_alpha(EwlImLayer *l, char ra);
char           ewl_imlayer_get_render_alpha(EwlImLayer *l);

#endif /* _EWL_IMAGE_CLASS_H_ */
