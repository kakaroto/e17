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
	 * from this particular Etox's previous incarnation.  This should do
	 * everything but free the etox.  It is safe to call this multiple times.
	 * This is also called from etox_free() - it is not necessary to call it 
	 * and then the etox_free() function.
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
	 * 
	 * returns TRUE on success, FALSE on failure.
	 *
	 */

	char *s=NULL;

	char *font=NULL;
	char first=0;
	char last=0;
	int font_size=0;
	char align;
	E_Font_Style *font_style = NULL;
	char ok;
	char changed_last_round;
	char *cur_text = NULL;
	double padding=-1;

	double obj_x, obj_y, obj_w, obj_h;

	struct {
		E_Color fg;
		E_Color ol;
		E_Color sh;
	} text_color;

	double cur_x, cur_y;

	if(!e)
		return 0;

	etox_clean(e);

	e->text = malloc((strlen(new_text) * sizeof(char)) + 1);
	strcpy(e->text,new_text);

	s = strtok(new_text,"\n");

	/* 
	 * set our initial location to be the top left corner of the Etox object
	 */

	cur_x = 0;
	cur_y = 0;

	/* 
	 * now we're going to loop through the text that they just set and build
	 * our internal data structures as well as any Evas_Objects we will need
	 * to draw this text to the screen
	 */

	first = 1;
	changed_last_round = 1;
	last = 1;
	while((first == 1) || (s = strtok(NULL,"\n")) || (last = 1)) {

		if(!s) {
			last = 0;
		}

		/*
		 * FIXME: this is a pretty big mess.  Need to abstract most of this code
		 * and clean it up considerably before it will become readable again.
		 */

		if(s) {
			ok = 1;
			if(s[0] == '.') {
				if(s[1] != '.') {

					/*
					 * this line is contains special operatives that we have
					 * to pay attention to (begins with a ".")
					 */

					char *s2, *s3;

					s2 = malloc((strlen(s) * sizeof(char)) +1);
					if(s2) {
						char *s4=NULL;
						*s4 = s[1];
						strcpy(s2, s4);
						s3 = strtok(s2,"=");
						if(!strcmp(s3,"style")) {
							/* change font styles */
							s3 = strtok(NULL,"=");
							if(s3) {
								font_style = E_load_font_style(s3);
							}
						} else if(!strcmp(s3,"size")) {
							/* change font size */
							s3 = strtok(NULL,"=");
							if(s3) {
								font_size = atoi(s3);
							}
						} else if(!strcmp(s3,"font")) {
							/* change the font */
							s3 = strtok(NULL,"=");
							if(s3) {
								if(font)
									free(font);
								font = malloc((strlen(s3) * sizeof(char)) + 1);
								strcpy(font,s3);
							}
						} else if(!strcmp(s3,"padding")) {
							/*
							 * change the current amount of padding between
							 * lines 
							 */
							s3 = strtok(NULL,"=");
							padding = atoi(s3);
						} else if(!strcmp(s3,"color")) {
							/*
							 * change one of the three active colors
							 * (shadow, foreground, background)
							 */
							s3 = strtok(NULL,"=");
							if(s3) {
								char text_type[3];
								E_Color color;
								
								memset(text_type,0,3);
								sscanf(s3,"%s %d %d %d",text_type,&color.r,
										&color.g,&color.b);
								if(!strcmp(text_type,"sh")) {
									/* this is a shadow color */
									text_color.sh = color;
								} else if(!strcmp(text_type,"fg")) {
									/* this is a foreground color */
									text_color.fg = color;
								} else if(!strcmp(text_type,"ol")) {
									/* this is an outline color */
									text_color.ol = color;
								}
							}
						} else if(!strcmp(s3,"align")) {

							/* 
							 * let's set up the text alignment.
							 *
							 * FIXME: this code currently does nothing useful,
							 * as everything is always left-aligned until I
							 * this code gets more abstract.  This will break
							 * on right->left font orientation also.  :(
							 */

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
					 * on to the next segment, so increment the pointer
					 */

					s++;
				}
			} /* done testing to see if it's a special line (".") */

			if(ok || last) {
				/* this line didn't contain a special line delimiter */

				if(changed_last_round || last) {
					Etox_Bit *bit;

					if(cur_text) {
						/* 
						 * dump the last bit of text into the now full
						 * previous member
						 */
						
						char s3[4096];
						char s4[4096];
						int i;

						bit = e->bit_list[e->num_bits - 1];
						bit->text = malloc((strlen(cur_text)  * sizeof(char))+ 1);
						strcpy(bit->text,cur_text);

						/* 
						 * Ditch the old text that we've already used up...
						 * You have to do this early, as we may reset cur_text
						 * shortly on down in this function
						 */

						free(cur_text);
						cur_text = NULL;

						ok=0;
						while(!ok) {

							/*
							 * this segment should loop through and keep
							 * knocking off segments of text until it
							 * finds one that will fit in the spot we've
							 * got laid out for it
							 */

							Evas_Object *o;
							char clipped=0;

							o = evas_add_text(e->evas,bit->font,bit->font_size,
									bit->text);

							evas_get_geometry(e->evas,o,&obj_x,&obj_y,
									&obj_w,&obj_h);

							for(i=0;i<e->num_rects;i++) {

								/*
								 * make sure we're not hitting the interior of
								 * a clipped area
								 * NOTE: This is very primitive, revisit later
								 */

								if(cur_x >= e->rect_list[i]->x) {
									if(cur_x <= (e->rect_list[i]->x +
											   	e->rect_list[i]->w)) {
										if(cur_y <= (e->rect_list[i]->y +
												   	e->rect_list[i]->h)) {
											if(cur_y >= e->rect_list[i]->y) {
												clipped = 1;
												cur_x = e->rect_list[i]->x +1;
												if(cur_x < e->w){
													cur_x = 0;
													if(padding >= 0) {
														cur_y += padding;
													} else {
														cur_y += cur_y +
														   	(obj_h + 
															 (obj_h / 2));
													}
												}
											}
										}
									}
								}
							}

							if((obj_w + cur_x) < e->w) {
								if((cur_y + obj_h) < e->h) {
									ok=1;
								}
							}

							if(!ok && !clipped) {
								char s[4096];
								char *s2;

								memset(s,0,4096);
								memset(s3,0,4096);
								memset(s4,0,4096);

								strcpy(s,bit->text);

								s2 = strtok(s," ");
								strcpy(s4,s2);
								strcpy(s4, s2);
								while((s2 = strtok(NULL," "))) {
									strcpy(s3,s2);
									strcat(s4,s2);
								}
							}

							if(s4[0]) {
								if(!strcmp(s4,s3)) {
									/*
									 * apparently this text will not fit on
									 * the current line...  we'll try putting
									 * it on a new line next
									 *
									 * FIXME: needs to break a long word into
									 * multiple lines on a character boundary
									 * should things go awry (should also pay
									 * attention to clip rects)
									 */

									if(cur_x == 0) {
										ok=0;
										while(!ok) {
											char *s2;
										}
									} else {
										cur_x = 0;
										if(padding >= 0) {
											cur_y += padding;
										} else {
											cur_y += cur_y + (obj_h + 
													(obj_h / 2));
										}
									}

								} else {
									/*
									 * now we've got a new segment of text,
									 * regardless of whether we make it out of
									 * this loop or not, we can't tell at this
									 * point, so we'll assign it anyways.
									 */

									if(bit->text)
										free(bit->text);

									bit->text = malloc((strlen(s4) * 
												sizeof(char)) + 1);
									strcpy(bit->text,s4);
									if(cur_text) {
										cur_text = realloc(cur_text,
												(strlen(cur_text) * 
												 sizeof(char)) 
												+ (strlen(s4) * 
													sizeof(char)) + 2);
										strcat(cur_text," ");
									} else {
										cur_text = malloc((strlen(s4) * 
													sizeof(char)) +1);
										strcpy(cur_text,"");
									}
									strcat(cur_text,s4);
								}
							}

							/*
							 * kill the object since we know we have
							 * to redo it next loop
							 */
							evas_del_object(e->evas,o);

						}

						/*
						 * now we have our final text segment that belongs on
						 * this line.  We're going to loop through the proper
						 * font style for this text segment and make create
						 * all the proper Evas_Objects for it
						 */

						for (i=0;i<bit->font_style->num_bits;i++){

							Evas_Object *o;

							o = evas_add_text(e->evas,bit->font,bit->font_size,
									bit->text);

							evas_move(e->evas,o,bit->x + font_style->bits[i].x,
									bit->y + font_style->bits[i].y);

							bit->num_evas++;

							switch(bit->font_style->bits[i].type) {
								/* set the proper color and alpha mask */
								case STYLE_TYPE_OUTLINE:
									evas_set_color(e->evas,o,text_color.ol.r,
											text_color.ol.g,text_color.ol.b,
											bit->font_style->bits[i].alpha);
									break;
								case STYLE_TYPE_SHADOW:
									evas_set_color(e->evas,o,text_color.sh.r,
											text_color.sh.g,text_color.sh.b,
											bit->font_style->bits[i].alpha);
									break;
								default:
								case STYLE_TYPE_FOREGROUND:
									evas_set_color(e->evas,o,text_color.fg.r,
											text_color.fg.g,text_color.fg.b,
											bit->font_style->bits[i].alpha);
									break;
							}

							bit->num_evas++;
							if(bit->evas_list) {
								bit->evas_list = malloc((sizeof(Evas_Object *) *
											bit->num_evas) + 1);
							} else {
								bit->evas_list = realloc(bit->evas_list,
										(sizeof(Evas_Object *) * bit->num_evas)
										+ 1);
							}

							bit->evas_list[i] = o;

							/*
							 * make sure that we're stacked appropriately
							 */

							if(i > 0) {
								evas_stack_above(e->evas,o,bit->evas_list[i - 1]);
							}
						}
					}

					cur_x = cur_x + obj_w;

					if(!last) {
						/*
						 * we'll only build a new Etox_Bit if we're going to
						 * use it next time
						 */

						bit = Etox_Bit_new();
						bit->font = malloc((strlen(font) * sizeof(char)) + 1);
						strcpy(bit->font,font);
						bit->font_size = font_size;

						/*
						 * this isn't very clean, but we have to incrememnt the
						 * "in_use" flag ourselves since there isn't a cleaner way
						 * to do it...  otherwise our font_style could
						 * accidentally get hosed before when something else frees
						 * it on the way out.
						 */

						bit->font_style = font_style;
						font_style->in_use++;

						/* 
						 * now that we've set the bit up, tack it at the end of
						 * our Etox
						 */

						if(e->bit_list) {
							e->bit_list = realloc(e->bit_list,
									sizeof(Etox_Bit *) * ++(e->num_bits));
						} else {
							e->bit_list = malloc(sizeof(Etox_Bit *) * 
									++(e->num_bits));
						}

						e->bit_list[e->num_bits - 1] = bit;

					}

				} else {
					/* we'll store this text for a bit later. */

					if(cur_text) {
						cur_text = realloc(cur_text,
								(strlen(cur_text) * sizeof(char))
								+ (strlen(s) * sizeof(char)) + 2);
						strcat(cur_text," ");
					} else {
						cur_text = malloc((strlen(s) * sizeof(char)) + 1);
						strcpy(cur_text,"");
					}
					strcat(cur_text,s);
				}
				changed_last_round=0;
			}
		}
		first = 0;
	}

	/*
	 * somewhat kludgy to have to remove one of our pointers to this
	 * font_style, but it very well could have ended up not getting used...
	 * it will not free the font_style if it ever got used in the text
	 * initialization - though it will release our in_use handle on it.
	 */

	if(font_style)
		E_Font_Style_free(font_style);

	if(font)
		free(font);

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

	/* 
	 * returns the layer on which all the Evas components sit
	 * or a negative -1 in case of failure
	 */

	if(!e)
		return -1;

	return e->layer;
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

void etox_move(Etox *e, double x, double y) {

	/* this will move all the evas components in the etox */

	double delta_x, delta_y;
	int i,j;

	if(!e)
		return;

	delta_x = x - e->x;
	delta_y = y - e->y;

	e->x = x;
	e->y = y;

	/* 
	 * now we need to rotate through the list of evas inside of each bit and
	 * move each of the evas components
	 */

	for(i=0;i<e->num_bits;i++) {
		e->bit_list[i]->x += delta_x;
		e->bit_list[i]->y += delta_y;
		for(j=0;j<e->bit_list[i]->num_evas;j++) {
			double old_y, old_x, w, h;

			evas_get_geometry(e->evas,e->bit_list[i]->evas_list[j],&old_x,
					&old_y,&w,&h);
			evas_move(e->evas,e->bit_list[i]->evas_list[j],old_x + delta_x,
					old_y + delta_y);
		}
	}

	return;

}

void etox_resize(Etox *e, double w, double h) {

	/* 
	 * this will resize the container that the etox components are rendered
	 * in.  Should force an etox_refresh
	 */

	if(!e)
		return;

	e->w = w;
	e->h = h;

	etox_refresh(e);

	return;

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

}

char etox_clip_rect_new(Etox *e, double x, double y, double w,double h) {

	/*
	 * This function adds a new clip rect to the Etox.
	 * Need to add a function to remove a cliprect from an Etox, as well as
	 * query for current clip rects.
	 *
	 * Clip Rectangles are specified against the topleft corner of the etox
	 * passed in.
	 *
	 * returns TRUE on success, FALSE on failure.
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

	etox_refresh(e);

	return 1;

}

void etox_free(Etox *e) {

	/*
	 * this function is pretty straight forward.  most of the internals have
	 * been moved into etox_clean, since it is used again in other places to
	 * reset internal bits
	 */

	etox_clean(e);
	free(e);

	return;
}

Etox_Bit *Etox_Bit_new() {

	/* initialiazation of new Etox_Bit data structures */

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

	/* initialize new Etox */

	Etox *e;

	e = malloc(sizeof(Etox));

	if(name) {
		e->name = malloc((strlen(name) * sizeof(char)) + 1);
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
