#include "Etox.h"

char *etox_get_text(Etox *e) {

	if(e)
		if(e->text)
			return(e->text);

	return(NULL);

}

char etox_set_text(Etox *e, char *new_text) {

	if(!e)
		return 0;

	if(e->text)
		free(e->text);

	e->text = malloc(strlen(new_text) + 1);
	strcpy(e->text,new_text);
	return 1;


}

char etox_set_font_style(E_Font_Style *font_style) {

	if(!font_style)
		return 0;

	return 1;
}

char etox_clip_rect_new(Etox *e, int x, int y, int w,int h) {

	E_Clip_Rect *new_rect;

	if(!e)
		return 0;
	if(w<=0)
		return 0;
	if(h<=0)
		return 0;

	new_rect = malloc(sizeof(E_Clip_Rect));

	new_rect->w = w;
	new_rect->h = h;
	new_rect->x = x;
	new_rect->y = y;

	if(e->num_rects <=0) {
		e->rect_list = malloc(sizeof(E_Clip_Rect *) + 1);
		e->rect_list[0] = new_rect;
	} else {
		e->rect_list = realloc(e->rect_list,(sizeof(E_Clip_Rect *) * 
					e->num_rects + 1) + 1);
		e->rect_list[e->num_rects] = new_rect;
	}
	e->num_rects++;

	return 1;

}

Etox *etox_new(char *name) {

	Etox *e;

	e = malloc(sizeof(Etox));

	if(name) {
		e->name = malloc(strlen(name) + 1);
		strcpy(e->name,name);
	} else {
		e->name = NULL;
	}

	e->text = NULL;
	e->text_len = 0;

	e->rect_list = NULL;
	e->num_rects = 0;

	return e;

}
