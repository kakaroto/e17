#include "Etox.h"

char *etox_get_text(Etox *e) {

	/*
	 * this function is pretty simple, it merely returns the string that is
	 * being used for the text display/formatting - maybe a seperate option
	 * later to return just the text without any of the formatting.
	 */

	if(e)
		if(e->text)
			return(e->text);

	return(NULL);

}

void etox_clean(Etox *e) {

	/*
	 * time to clean up any older crufty bits that could have been left around
	 * from before - after all since they're changing the text in here we're
	 * going to have to rebuild the thing, right?
	 */

	int i,j;

	for(i=0;i<e->num_bits;i++) {
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			evas_free(e->bit_list[i]->evas_list[j]);
		}
		if(e->bit_list[i]->evas_list)
			free(e->bit_list[i]->evas_list);
		if(e->bit_list[i]->evas_list)
			free(e->bit_list[i]->evas_list);
		if(e->bit_list[i]->font_style)
			E_Font_Style_free(e->bit_list[i]->font_style);
		if(e->bit_list[i]->font)
			free(e->bit_list[i]->font);
	}
	if(e->bit_list)
		free(e->bit_list);

	e->bit_list = NULL;
	e->num_bits = 0;

	if(e->text)
		free(e->text);

	return;

}

char etox_set_text(Etox *e, char *new_text) {

	/*
	 * This function does a few things:
	 * 1) first and foremost, it sets the text string that is being parsed
	 * 2) builds out the Etox internal data structure
	 * 3) initializes any Evas objects that need to be created
	 * 4) does the laundry
	 */

	char *s=NULL;

	char *font=NULL;
	char first=0;
	int font_size;
	char align;
	E_Font_Style *font_style = NULL;
	char ok;
	char changed_last_round;
	char *cur_text = NULL;

	if(!e)
		return 0;

	etox_clean(e);

	e->text = malloc(strlen(new_text) + 1);
	strcpy(e->text,new_text);

	s = strtok(new_text,"\n");

	/* 
	 * now we're going to loop through the text that they just set and build
	 * our internal data structures as well as any Evas_Objects we will need
	 * to draw this text to the screen
	 */

	first = 1;
	changed_last_round = 1;
	while((first == 1) || (s = strtok(NULL,"\n"))) {
		if(s) {
			ok = 1;
			if(s[0] == '.') {
				if(s[1] != '.') {

					/*
					 * this line is contains special operatives that we have
					 * to pay attention to (begins with a .)
					 */

					char *s2, *s3;

					s2 = malloc(strlen(s));
					if(s2) {
						char *s4=NULL;
						*s4 = s[1];
						strcpy(s2, s4);
						s3 = strtok(s2,"=");
						if(!strcmp(s3,"style")) {
							s3 = strtok(NULL,"=");
							if(s3) {
								font_style = E_load_font_style(s3);
							}
						} else if(!strcmp(s3,"size")) {
							s3 = strtok(NULL,"=");
							if(s3) {
								font_size = atoi(s3);
							}
						} else if(!strcmp(s3,"font")) {
							s3 = strtok(NULL,"=");
							if(s3) {
								if(font)
									free(font);
								font = malloc(strlen(s3) + 1);
								strcpy(font,s3);
							}
						} else if(!strcmp(s3,"align")) {
							s3 = strtok(NULL,"=");
							if(!strcmp(s3,"right")) {
								align = ALIGN_RIGHT;
							} else if(!strcmp(s3,"center")) {
								align = ALIGN_CENTER;
							} else if(!strcmp(s3,"left")) {
								align = ALIGN_LEFT;
							}
						}
						changed_last_round = 1;
						ok = 0;
						free(s2);
					}
				} else {
					/*
					 * this line has 2 dots at the beginning which means that
					 * we have to get rid of the first dot and then pass it
					 * on to the next segment
					 */

					s++;
				}
			} /* done testing to see if it's a special line (.) */

			if(ok) {
				/* this line didn't contain a special line delimiter */

				if(changed_last_round) {
					Etox_Bit *bit;

					if(cur_text) {
						/* 
						 * dump the last bit of text into the now full
						 * previous member
						 */
						
					}
					if(e->num_bits == 0) {
						e->bit_list = malloc(sizeof(Etox_Bit *) * (++(e->num_bits)));
					} else {
						e->bit_list = realloc(e->bit_list,sizeof(Etox_Bit *) * (++(e->num_bits)));
					}

					bit = Etox_Bit_new();
					bit->font = malloc(strlen(font) + 1);
					strcpy(bit->font,font);
					bit->font_style = font_style;
					font_style->in_use++;

					free(cur_text);
					cur_text = NULL;
				} else {
					/* we'll store this text for a bit later. */

					if(cur_text) {
						cur_text = realloc(cur_text, strlen(cur_text) + strlen(s) + 1);
					} else {
						cur_text = malloc(strlen(s) + 1);
						strcpy(cur_text,"");
					}
					strcat(cur_text,s);
				}
				changed_last_round=0;
			}
		}
		first = 0;
	}

	if(font_style)
		E_Font_Style_free(font_style);
	if(font)
		free(font);

	/*  Not sure if you should automatically rerender if it's visible and
	 *  rendered........ 
	if(e->rendered)
		etox_render(e);
	 */

	return 1;

}

char etox_set_layer(Etox *e, int layer) {

	/* sets the layer on which all the Evas components sit */

	int i,j;

	if(!e)
		return 0;

	e->layer = layer;

	for(i=0;i<e->num_bits;i++) {
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			evas_set_layer(e->evas, e->bit_list[i]->evas_list[j], e->layer);
		}
	}

	return 1;

}

int etox_get_layer(Etox *e) {

	/* returns the layer on which all the Evas components sit */

	if(!e)
		return -1;

	return e->layer;
}

char etox_render(Etox *e) {

	/* calls evas_rendr on all active Evas components */

	int i,j;

	if(!e)
		return 0;

	for(i=0;i<e->num_bits;i++) {
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			evas_render(e->bit_list[i]->evas_list[j]);
		}
	}

	e->rendered = 1;

	return 1;
}

char etox_show(Etox *e) {

	/* calls evas_show on all active Evas components */

	int i,j;

	if(!e)
		return 0;


	for(i=0;i<e->num_bits;i++) {
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			evas_show(e->evas,e->bit_list[i]->evas_list[j]);
		}
	}

	e->visible = 1;

	return 1;

}

char etox_hide(Etox *e) {

	/* calls evas_hide on all active Evas components */

	int i,j;

	if(!e)
		return 0;

	for(i=0;i<e->num_bits;i++) {
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			evas_hide(e->evas, e->bit_list[i]->evas_list[j]);
		}
	}

	e->visible = 0;

	return 1;

}

char etox_set_font_style(Etox *e, E_Font_Style *font_style) {

	/* 
	 * this function changes the default font_style, which will, cause a
	 * refresh of the internal data
	 */

	if(!font_style)
		return 0;

	e->font_style = font_style;

	etox_refresh(e);

	return 1;

}

void etox_refresh(Etox *e) {

	/*
	 * This is a bit kludgy, but will work for now, since it forces a complete
	 * rebuilding of the setup inside of the Etox object
	 */

	char *tmp = NULL;

	tmp = malloc(e->text_len + 1);
	if(tmp)
		strcpy(tmp,e->text);
	etox_set_text(e,tmp);
	if(tmp)
		free(tmp);
	etox_render(e);

}

char etox_clip_rect_new(Etox *e, int x, int y, int w,int h) {

	/*
	 * This function adds a new clip rect to the Etox.
	 * Need to add a function to remove a cliprect from an Etox, as well as
	 * query for current clip rects
	 */

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

	if(e->rendered)
		etox_refresh(e);

	return 1;

}

void etox_free(Etox *e) {

	/*
	 * this function is pretty straight forward.  most of the internals have
	 * been moved into etox_clean, since I use it again in another place to
	 * reset internal bits
	 */

	etox_clean(e);
	free(e);

	return;
}

Etox_Bit *Etox_Bit_new() {

	Etox_Bit *bit;

	bit = malloc(sizeof(Etox_Bit));
	bit->text = NULL;
	bit->x = 0;
	bit->y = 0;
	bit->w = 0;
	bit->h = 0;
	bit->font = NULL;
	bit->evas_list = NULL;
	bit->num_evas = 0;
	bit->font_style = NULL;
	bit->font_size = 0;

	return bit;

}

Etox *Etox_new(char *name) {

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
	e->rendered = 0;

	return e;

}
