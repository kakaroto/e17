#include "image.h"


EwlImage      *ewl_image_new()
{
	EwlImage *im = NULL;
	FUNC_BGN("ewl_image_new");
	im = ewl_image_new_with_values("","",0,0,0);
	if (!im)	{
		ewl_debug("ewl_image_new", EWL_NULL_ERROR, "im");
	}
	FUNC_END("ewl_image_new");
	return im;
}

EwlImage      *ewl_image_new_with_values(char *name, char *path,
                                         int *w, int *h, char visible)
{
	EwlImage *im = malloc(sizeof(EwlImage));
	FUNC_BGN("ewl_image_new_with_values");
	if (!im)	{
		ewl_debug("ewl_image_new_with_values", EWL_NULL_ERROR, "im");
	} else {
		im->ll.data = NULL;
		im->ll.next = NULL;
		im->name = e_string_dup(name);
		im->path = e_string_dup(path);
		im->rect = ewl_rect_new_with_values(0, 0, w, h);
		im->rrect = ewl_rrect_new();
		im->visible = visible;
		im->im = ewl_imlib_load_image(path);
	}
	FUNC_END("ewl_image_new_with_values");
	return im;
}

void           ewl_image_init(EwlImage *im)
{
	FUNC_BGN("ewl_image_init");
	if (!im)	{
		ewl_debug("ewl_image_init", EWL_NULL_ERROR, "im");
	} else {
		/* nothing here yet */
	}
	FUNC_END("ewl_image_init");
	return;
}

EwlImage      *ewl_image_dup(EwlImage *sim)
{
	EwlImage *dim = NULL;
	FUNC_BGN("ewl_image_dup");
	if (!sim)	{
		ewl_debug("ewl_image_dup", EWL_NULL_ERROR, "sim");
	} else {
		dim = malloc(sizeof(EwlImage));
		if (!dim)	{
			ewl_debug("ewl_image_dup", EWL_NULL_ERROR, "dim");
		} else {
			dim->ll.data = NULL;
			dim->ll.next = NULL;
			dim->name = e_string_dup(sim->name);
			dim->path = e_string_dup(sim->path);
			dim->rect = ewl_rect_dup(sim->rect);
			dim->rrect = ewl_rrect_dup(sim->rrect);
			dim->visible = sim->visible;
			dim->im = ewl_imlib_clone_image(sim->im);
		}
	}
	FUNC_END("ewl_image_dup");
	return dim;
}

void           ewl_image_free(EwlImage *im)
{
	FUNC_BGN("ewl_image_free");
	if (!im)	{
		ewl_debug("ewl_image_free", EWL_NULL_ERROR, "im");
	} else {
		if (im->name)
			free(im->name);
		if (im->path)
			free(im->path);
		if (im->rect)
			ewl_rect_free(im->rect);
		if (im->rrect)
			ewl_rrect_free(im->rrect);
		if (im->im)
			ewl_imlib_free_image(im->im);
		free(im);
		im = 0;
	}
	FUNC_END("ewl_image_free");
	return;
}


EwlImage      *ewl_image_load(char *path)
{
	EwlImage    *eim = NULL;
	Imlib_Image  iim = NULL;
	EwlRect     *r   = NULL;
	int          w=0, h=0;
	FUNC_BGN("ewl_image_load");
	if (!path)	{
		ewl_debug("ewl_image_load", EWL_NULL_ERROR, "path");
	} else {
		iim = ewl_imlib_load_image(path);
		if (!iim)	{
			ewl_debug("ewl_image_load", EWL_NULL_ERROR, "iim");
		} else {
			w = ewl_imlib_image_get_width(iim);
			h = ewl_imlib_image_get_height(iim);
			eim = ewl_image_new();
			r   = ewl_rect_new_with_values(NULL, NULL, &w, &h);
			if (!eim)	{
				ewl_debug("ewl_image_load", EWL_NULL_ERROR, "eim");
			} else if (!r) {
				ewl_imlib_free_image(iim);
				ewl_image_free(eim);
				ewl_debug("ewl_image_load", EWL_NULL_ERROR, "r");
			} else {
				ewl_image_set_image(eim,iim);
				ewl_image_set_rect(eim,r);
				if (eim->path)	{
					free(eim->path);
					eim->path = 0;
				}
				eim->path = e_string_dup(path);
			}
		}
	}
	FUNC_END("ewl_image_load");
	return eim;
}

EwlBool        ewl_image_save(EwlImage *eim, char *path)
{
	/*Imlib_Image tim = NULL;*/
	EwlBool     r = FALSE;
	FUNC_BGN("ewl_image_save");
	if (!eim)	{
		ewl_debug("ewl_image_save", EWL_NULL_ERROR, "eim");
	} else if (!path)	{
		ewl_debug("ewl_image_save", EWL_NULL_ERROR, "path");
	} else {
		/* SHOULD BE APPLYING THE CHANGED WIDTH AND HEIGHT HERE,
		   BUT WE'LL ADD THAT LATER */
		ewl_imlib_save_image(eim->im,path);
	}
	FUNC_END("ewl_image_save");
	return r;
}


void           ewl_image_set_image(EwlImage *eim, Imlib_Image iim)
{
	FUNC_BGN("ewl_image_set_image");
	if (!eim)	{
		ewl_debug("ewl_image_set_image", EWL_NULL_ERROR, "eim");
	} else {
		ewl_imlib_free_image(eim->im);
		eim->im = iim;
	}
	FUNC_END("ewl_image_set_image");
	return;
}

void           ewl_image_set_image_by_path(EwlImage *im, char *path)
{
	FUNC_BGN("ewl_image_set_image_by_path");
	if (!im)	{
		ewl_debug("ewl_image_set_image_by_path", EWL_NULL_ERROR, "im");
	} else if (!path)	{
		ewl_debug("ewl_image_set_image_by_path", EWL_NULL_ERROR, "path");
	} else {
		ewl_imlib_free_image(im->im);
		im->im = ewl_imlib_load_image(path);;
	}
	FUNC_END("ewl_image_set_image_by_path");
	return;
}

Imlib_Image    ewl_image_get_image(EwlImage *im)
{
	Imlib_Image iim = NULL;
	FUNC_BGN("ewl_image_get_image");
	if (!im)	{
		ewl_debug("ewl_image_get_image", EWL_NULL_ERROR, "im");
	} else {
		iim = im->im;
	}
	FUNC_END("ewl_image_get_image");
	return iim;
}

char          *ewl_image_get_image_path(EwlImage *im)
{
	char *path = NULL;
	FUNC_BGN("ewl_image_get_image_path");
	if (!im)	{
		ewl_debug("ewl_image_get_image_path", EWL_NULL_ERROR, "im");
	} else {
		path = e_string_dup(im->path);
		if (!path)	{
			ewl_debug("ewl_image_get_image_path", EWL_NULL_ERROR, "path");
		}
	}
	FUNC_END("ewl_image_get_image_path");
	return path;
	
}


void           ewl_image_show(EwlImage *im)
{
	FUNC_BGN("ewl_image_show");
	if (!im)	{
		ewl_debug("ewl_image_show", EWL_NULL_ERROR, "im");
	} else {
		ewl_image_set_visibility(im,1);
	}
	FUNC_END("ewl_image_show");
	return;
}

void           ewl_image_hide(EwlImage *im)
{
	FUNC_BGN("ewl_image_hide");
	if (!im)	{
		ewl_debug("ewl_image_hide", EWL_NULL_ERROR, "im");
	} else {
		ewl_image_set_visibility(im,0);
	}
	FUNC_END("ewl_image_hide");
	return;
}

void           ewl_image_set_name(EwlImage *im, char *name)
{
	FUNC_BGN("ewl_image_set_name");
	if (!im)	{
		ewl_debug("ewl_image_set_name", EWL_NULL_ERROR, "im");
	} else if (!name) {
		ewl_debug("ewl_image_set_name", EWL_NULL_ERROR, "name");
	} else {
		im->name = e_string_dup(name);
	}
	FUNC_END("ewl_image_set_name");
	return;
}

char          *ewl_image_get_name(EwlImage *im)
{
	char *temp = NULL;
	FUNC_BGN("ewl_image_get_name");
	if (!im)	{
		ewl_debug("ewl_image_get_name", EWL_NULL_ERROR, "im");
	} else {
		temp = e_string_dup(im->name);
		if (!temp)	{
			ewl_debug("ewl_image_get_name", EWL_NULL_ERROR, "temp");
		}
	}
	FUNC_END("ewl_image_get_name");
	return temp;
}

void           ewl_image_set_rect(EwlImage *im, EwlRect *rect)
{
	FUNC_BGN("ewl_image_set_rect");
	if (!im)	{
		ewl_debug("ewl_image_set_rect", EWL_NULL_ERROR, "im");
	} else {
		ewl_rect_free(im->rect);
		im->rect = ewl_rect_dup(rect);
	}
	FUNC_END("ewl_image_set_rect");
	return;
}

EwlRect       *ewl_image_get_rect(EwlImage *im)
{
	EwlRect *rect = NULL;
	FUNC_BGN("ewl_image_get_rect");
	if (!im)	{
		ewl_debug("ewl_image_get_rect", EWL_NULL_ERROR, "im");
	} else {
		rect = ewl_rect_dup(im->rect);
	}
	FUNC_END("ewl_image_get_rect");
	return rect;
}

void           ewl_image_set_rrect(EwlImage *im, EwlRRect *rrect)
{
	FUNC_BGN("ewl_image_set_rrect");
	if (!im)	{
		ewl_debug("ewl_image_set_rrect", EWL_NULL_ERROR, "im");
	} else {
		ewl_rrect_free(im->rrect);
		im->rrect = ewl_rrect_dup(rrect);
	}
	FUNC_END("ewl_image_set_rrect");
	return;
}

EwlRRect      *ewl_image_get_rrect(EwlImage *im)
{
	EwlRRect *rrect = NULL;
	FUNC_BGN("ewl_image_get_rrect");
	if (!im)	{
		ewl_debug("ewl_image_get_rrect", EWL_NULL_ERROR, "im");
	} else {
		rrect = ewl_rrect_dup(im->rrect);
	}
	FUNC_END("ewl_image_get_rrect");
	return rrect;
}

void           ewl_image_set_visibility(EwlImage *im, char visibility)
{
	FUNC_BGN("ewl_image_set_visibility");
	if (!im)	{
		ewl_debug("ewl_image_set_visibility", EWL_NULL_ERROR, "im");
	} else {
		im->visible = visibility;
	}
	FUNC_END("ewl_image_set_visibility");
	return;
}

char           ewl_image_get_visibility(EwlImage *im)
{
	char v = 0;
	FUNC_BGN("ewl_image_get_visibility");
	if (!im)	{
		ewl_debug("ewl_image_get_visibility", EWL_NULL_ERROR, "im");
	} else {
		v = im->visible;
	}
	FUNC_END("ewl_image_get_visibility");
	return v;
}



EwlImLayer    *ewl_imlayer_new()
{
	EwlImLayer *l = NULL;
	FUNC_BGN("ewl_imlayer_new");
	l = ewl_imlayer_new_with_values("",0,0,0,0);
	if (!l)	{
		ewl_debug("ewl_imlayer_new", EWL_NULL_ERROR, "l");
	}
	FUNC_END("ewl_imlayer_new");
	return l;
}

EwlImLayer    *ewl_imlayer_new_with_values(char *name, int *w, int *h,
                                           char visible, char render_alpha)
{
	EwlImLayer *l = malloc(sizeof(EwlImLayer));
	FUNC_BGN("ewl_imlayer_new_with_values");
	if (!l)	{
		ewl_debug("ewl_imlayer_new_with_values", EWL_NULL_ERROR, "l");
	} else {
		l->ll.data = NULL;
		l->ll.next = NULL;
		l->name = e_string_dup(name);
		l->rect = ewl_rect_new_with_values(0,0,w,h);
		l->rrect = ewl_rrect_new_with_values(0,0,0,0,0,0,0,0);
		l->visible = visible;
		l->render_alpha = render_alpha;
		l->images = NULL;
	}
	FUNC_END("ewl_imlayer_new_with_values");
	return l;
}

void           ewl_imlayer_init(EwlImLayer *l)
{
	FUNC_BGN("ewl_imlayer_init");
	if (!l)	{
		ewl_debug("ewl_imlayer_init", EWL_NULL_ERROR, "l");
	} else {
		/* nothing here yet */
	}
	FUNC_END("ewl_imlayer_init");
	return;
}

static char _cb_ewl_imlayer_dup_images(EwlLL *node, EwlData *data)
{
	EwlImage *sim = (EwlImage*) node;
	EwlLL    *dim = (EwlLL*) data;
	dim = ewl_ll_insert(dim, (EwlLL*)ewl_image_dup(sim));
	return 1;
}

EwlImLayer    *ewl_imlayer_dup(EwlImLayer *l)
{
	EwlImLayer *dl = malloc(sizeof(EwlImLayer));
	FUNC_BGN("ewl_imlayer_dup");
	if (!dl)	{
		ewl_debug("ewl_imlayer_dup", EWL_NULL_ERROR, "dl");
	} else {
		dl->name = e_string_dup(l->name);
		dl->rect = ewl_rect_dup(l->rect);
		dl->rrect = ewl_rrect_dup(l->rrect);
		dl->visible = l->visible;
		dl->render_alpha = l->render_alpha;
		l->images = (EwlImage*) ewl_ll_foreach((EwlLL*)l->images,
		                                       _cb_ewl_imlayer_dup_images,
		                                       (EwlData*) dl->images);
	}
	FUNC_END("ewl_imlayer_dup");
	return dl;
}

static char _cb_ewl_imlayer_free_images(EwlLL *node, EwlData *data)
{
	ewl_image_free((EwlImage*) node);
	return 1;
}

void           ewl_imlayer_free(EwlImLayer *l)
{
	FUNC_BGN("ewl_imlayer_free");
	if (!l)	{
		ewl_debug("ewl_imlayer_free", EWL_NULL_ERROR, "l");
	} else {
		if (l->name)
			free(l->name);
		if (l->rect)
			ewl_rect_free(l->rect);
		if (l->rrect)
			ewl_rrect_free(l->rrect);
		if (l->images)
			l->images = (EwlImage*) ewl_ll_foreach((EwlLL*)l->images,
		                                           _cb_ewl_imlayer_free_images,
		                                           (EwlData*) NULL);
		free(l);
		l = 0;
	}
	FUNC_END("ewl_imlayer_free");
	return;
}


void           ewl_imlayer_image_insert(EwlImLayer *l, EwlImage *im)
{
	FUNC_BGN("ewl_imlayer_image_insert");
	if (!l)	{
		ewl_debug("ewl_imlayer_image_insert", EWL_NULL_ERROR, "l");
	} else if (!im) {
		ewl_debug("ewl_imlayer_image_insert", EWL_NULL_ERROR, "im");
	} else {
		l->images = (EwlImage*) ewl_ll_insert((EwlLL*) l->images,
		                                      (EwlLL*) im);
	}
	FUNC_END("ewl_imlayer_image_insert");
	return;
}

void           ewl_imlayer_image_remove(EwlImLayer *l, EwlImage *im)
{
	FUNC_BGN("ewl_imlayer_image_remove");
	if (!l)	{
		ewl_debug("ewl_imlayer_image_remove", EWL_NULL_ERROR, "l");
	} else if (!im) {
		ewl_debug("ewl_imlayer_image_remove", EWL_NULL_ERROR, "im");
	} else {
		l->images = (EwlImage*) ewl_ll_remove((EwlLL*) l->images,
		                                      (EwlLL*) im);
	}
	FUNC_END("ewl_imlayer_image_remove");
	return;
}

void           ewl_imlayer_image_push(EwlImLayer *l, EwlImage *im)
{
	FUNC_BGN("ewl_imlayer_image_push");
	if (!l)	{
		ewl_debug("ewl_imlayer_image_push", EWL_NULL_ERROR, "l");
	} else if (!im) {
		ewl_debug("ewl_imlayer_image_push", EWL_NULL_ERROR, "im");
	} else {
		l->images = (EwlImage*) ewl_ll_push((EwlLL*) l->images,
		                                    (EwlLL*) im);
	}
	FUNC_END("ewl_imlayer_image_push");
	return;
}

EwlImage      *ewl_imlayer_image_pop(EwlImLayer *l)
{
	EwlImage *im = NULL;
	FUNC_BGN("ewl_imlayer_image_pop");
	if (!l)	{
		ewl_debug("ewl_imlayer_image_pop", EWL_NULL_ERROR, "l");
	} else {
		im = (EwlImage*) ewl_ll_pop((EwlLL*) l->images);
		if (!im)	{
			ewl_debug("ewl_imlayer_image_pop", EWL_NULL_ERROR, "im");
		}
	}
	FUNC_END("ewl_imlayer_image_pop");
	return im;
}


void           ewl_imlayer_show(EwlImLayer *im)
{
	FUNC_BGN("ewl_imlayer_show");
	if (!im)	{
		ewl_debug("ewl_imlayer_show", EWL_NULL_ERROR, "im");
	} else {
		ewl_imlayer_set_visibility(im,1);
	}
	FUNC_END("ewl_imlayer_show");
	return;
}

void           ewl_imlayer_hide(EwlImLayer *im)
{
	FUNC_BGN("ewl_imlayer_hide");
	if (!im)	{
		ewl_debug("ewl_imlayer_hide", EWL_NULL_ERROR, "im");
	} else {
		ewl_imlayer_set_visibility(im,0);
	}
	FUNC_END("ewl_imlayer_hide");
	return;
}

void           ewl_imlayer_set_name(EwlImLayer *im, char *name)
{
	FUNC_BGN("ewl_imlayer_set_name");
	if (!im)	{
		ewl_debug("ewl_imlayer_set_name", EWL_NULL_ERROR, "im");
	} else if (!name) {
		ewl_debug("ewl_imlayer_set_name", EWL_NULL_ERROR, "name");
	} else {
		im->name = e_string_dup(name);
	}
	FUNC_END("ewl_imlayer_set_name");
	return;
}

char          *ewl_imlayer_get_name(EwlImLayer *im)
{
	char *temp = NULL;
	FUNC_BGN("ewl_imlayer_get_name");
	if (!im)	{
		ewl_debug("ewl_imlayer_get_name", EWL_NULL_ERROR, "im");
	} else {
		temp = e_string_dup(im->name);
		if (!temp)	{
			ewl_debug("ewl_imlayer_get_name", EWL_NULL_ERROR, "temp");
		}
	}
	FUNC_END("ewl_imlayer_get_name");
	return temp;
}

void           ewl_imlayer_set_rect(EwlImLayer *im, EwlRect *rect)
{
	FUNC_BGN("ewl_imlayer_set_rect");
	if (!im)	{
		ewl_debug("ewl_imlayer_set_rect", EWL_NULL_ERROR, "im");
	} else {
		ewl_rect_free(im->rect);
		im->rect = ewl_rect_dup(rect);
	}
	FUNC_END("ewl_imlayer_set_rect");
	return;
}

EwlRect       *ewl_imlayer_get_rect(EwlImLayer *im)
{
	EwlRect *rect = NULL;
	FUNC_BGN("ewl_imlayer_get_rect");
	if (!im)	{
		ewl_debug("ewl_imlayer_get_rect", EWL_NULL_ERROR, "im");
	} else {
		rect = ewl_rect_dup(im->rect);
	}
	FUNC_END("ewl_imlayer_get_rect");
	return rect;
}

void           ewl_imlayer_set_rrect(EwlImLayer *im, EwlRRect *rrect)
{
	FUNC_BGN("ewl_imlayer_set_rrect");
	if (!im)	{
		ewl_debug("ewl_imlayer_set_rrect", EWL_NULL_ERROR, "im");
	} else {
		ewl_rrect_free(im->rrect);
		im->rrect = ewl_rrect_dup(rrect);
	}
	FUNC_END("ewl_imlayer_set_rrect");
	return;
}

EwlRRect      *ewl_imlayer_get_rrect(EwlImLayer *im)
{
	EwlRRect *rrect = NULL;
	FUNC_BGN("ewl_imlayer_get_rrect");
	if (!im)	{
		ewl_debug("ewl_imlayer_get_rrect", EWL_NULL_ERROR, "im");
	} else {
		rrect = ewl_rrect_dup(im->rrect);
	}
	FUNC_END("ewl_imlayer_get_rrect");
	return rrect;
}

void           ewl_imlayer_set_visibility(EwlImLayer *im, char visibility)
{
	FUNC_BGN("ewl_imlayer_set_visibility");
	if (!im)	{
		ewl_debug("ewl_imlayer_set_visibility", EWL_NULL_ERROR, "im");
	} else {
		im->visible = visibility;
	}
	FUNC_END("ewl_imlayer_set_visibility");
	return;
}

char           ewl_imlayer_get_visibility(EwlImLayer *im)
{
	char v = 0;
	FUNC_BGN("ewl_imlayer_get_visibility");
	if (!im)	{
		ewl_debug("ewl_imlayer_get_visibility", EWL_NULL_ERROR, "im");
	} else {
		v = im->visible;
	}
	FUNC_END("ewl_imlayer_get_visibility");
	return v;
}

void           ewl_imlayer_set_render_alpha(EwlImLayer *im, char ra)
{
	FUNC_BGN("ewl_imlayer_set_render_alpha");
	if (!im)	{
		ewl_debug("ewl_imlayer_set_render_alpha", EWL_NULL_ERROR, "im");
	} else {
		im->render_alpha = ra;
	}
	FUNC_END("ewl_imlayer_set_render_alpha");
	return;
}

char           ewl_imlayer_get_render_alpha(EwlImLayer *im)
{
	char v = 0;
	FUNC_BGN("ewl_imlayer_get_render_alpha");
	if (!im)	{
		ewl_debug("ewl_imlayer_get_render_alpha", EWL_NULL_ERROR, "im");
	} else {
		v = im->render_alpha;
	}
	FUNC_END("ewl_imlayer_get_render_alpha");
	return v;
}

