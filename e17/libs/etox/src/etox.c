#include "Etox.h"

void etox_bit_update_geometry(Etox *e, Etox_Bit *abit) {
   double obj_x, obj_y, obj_w, obj_h, max_x, max_y, min_x, min_y;
   int i;
   
   abit->x = abit->y = abit->w = abit->h = 0;
   min_x = min_y = max_x = max_y = 0;
   if (e && abit && abit->evas_list && abit->font_style && (abit->font_style->num_bits>0) && (abit->num_evas > 0))
     {
	evas_get_geometry(e->evas, abit->evas_list[(abit->num_evas - 1)], &obj_x, &obj_y, &obj_w, &obj_h);
	for (i=0; i<abit->font_style->num_bits; i++) {
	   if (min_x > abit->font_style->bits[i].x)
	     min_x = abit->font_style->bits[i].x;
	   else if (max_x < abit->font_style->bits[i].x)
	     max_x = abit->font_style->bits[i].x;
	   if (min_y > abit->font_style->bits[i].y)
	     min_y = abit->font_style->bits[i].y;
	   else if (max_y < abit->font_style->bits[i].y)
	     max_y = abit->font_style->bits[i].y;
	}
	abit->x = obj_x;
	abit->y = obj_y;
	abit->w = obj_w + (max_x - min_x);
	abit->h = obj_h + (max_y - min_y);
     }
}

void etox_bit_set_face(Etox_Bit *bit, char *font, int font_size, E_Font_Style *style)
{

   if(bit->font)
     free(bit->font);
   bit->font = malloc((strlen(font) * sizeof(char)) + 1);
   strcpy(bit->font,font);
   bit->font_size = font_size;
   bit->font_style = style;
   style->in_use++;
}

char *etox_get_text(Etox *e)
{

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

void etox_set_color(Etox *e, E_Text_Color *cl)
{
   if (cl)
     e->color = *cl;
}

void etox_set_color_component(Etox *e, char *arg, E_Color component)
{
   if (!(strcmp(arg, "fg")))
     e->color.fg = component;
   else if (!(strcmp(arg, "ol")))
     e->color.ol = component;
   else if (!(strcmp(arg, "sh")))
     e->color.sh = component;
}

void etox_clean(Etox *e)
{

   /*
    * time to clean up any older crufty bits that could have been left around
    * from this particular Etox's previous incarnation.  This should do
    * everything but free the etox.  It is safe to call this multiple times.
    * This is also called from etox_free() - it is not necessary to call it
    * and then the etox_free() function.
    */

   int i,j;

   for(i=0;i<e->num_bits;i++)
     {
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
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

   //   if(e->text)
   //     free(e->text);
   //
   return;

}

int search_tokens(const char* text, const char** needles, int needles_count, char* *beg, char* *next)
{
   char *tmp=NULL;
   int token_length=0;
   int needle_num = -1;
   int i;

   if (*beg)
     free(*beg);
   if (text)
     {
	for (i=0; i < needles_count; i++)
	  {
	     char *sec;

	     sec = strstr(text,needles[i]);
	     token_length = strlen(needles[i]);
	     if (sec && ((!tmp) || ((tmp) && (sec<tmp))))
	       {
		  tmp = sec;
		  token_length = strlen(needles[i]);
		  needle_num = i;
	       }
	  }
	if (tmp)
	  {
	     *beg = malloc(sizeof(char) * (tmp - text +1));
	     strncpy(*beg, text, (tmp - text));
	     (*beg)[(tmp-text)] = '\0';
	     *next = tmp + token_length;
	  }
	else
	  {
	     *beg = strdup(text);
	     *next = NULL;
	  }
     }
   else
     {
	*beg = NULL;
	*next = NULL;
     }
   return needle_num;
}

void create_bit_objects(Etox_Bit *abit, Etox *e, E_Text_Color *text_color)
{
   double obj_x, obj_y, obj_w, obj_h;
   int i;

   for (i=0;i<abit->font_style->num_bits;i++)
     {
	Evas_Object *o;

	o = evas_add_text(e->evas,abit->font,abit->font_size,
			  abit->text);

	evas_move(e->evas,o,abit->x + abit->font_style->bits[i].x,
		  abit->y + abit->font_style->bits[i].y);

	//	evas_set_layer(e->evas, o, e->layer);

	abit->num_evas++;

	switch(abit->font_style->bits[i].type)
	  {
	     /* set the proper color and alpha mask */
	   case STYLE_TYPE_OUTLINE:
	     evas_set_color(e->evas,o,text_color->ol.r,
			    text_color->ol.g,text_color->ol.b,
			    abit->font_style->bits[i].alpha);
	     break;
	   case STYLE_TYPE_SHADOW:
	     evas_set_color(e->evas,o,text_color->sh.r,
			    text_color->sh.g,text_color->sh.b,
			    abit->font_style->bits[i].alpha);
	     break;
	   default:
	   case STYLE_TYPE_FOREGROUND:
	     evas_set_color(e->evas,o,text_color->fg.r,
			    text_color->fg.g,text_color->fg.b,
			    abit->font_style->bits[i].alpha);
	     break;
	  }

	if(!abit->evas_list)
	  {
	     abit->evas_list = malloc((sizeof(Evas_Object *) *
				       abit->num_evas) + 1);
	  }
	else
	  {
	     abit->evas_list = realloc(abit->evas_list,
				       (sizeof(Evas_Object *) * abit->num_evas)
				       + 1);
	  }

	abit->evas_list[i] = o;

	/*
	 * make sure that we're stacked appropriately
	 */

	if(i > 0)
	  {
	     evas_stack_above(e->evas,o,abit->evas_list[i-1]);
	  }
     }
   etox_bit_update_geometry(e, abit);
}

void update_bit_objects(Etox_Bit *abit, Etox *e)
{
   double obj_x, obj_y, obj_w, obj_h;
   int i;

   for (i=0;i<abit->font_style->num_bits;i++)
     evas_set_text(e->evas, abit->evas_list[i], abit->text);
   etox_bit_update_geometry(e, abit);
}

void etox_bit_move_relative(Etox *e, Etox_Bit *abit, double delta_x, double delta_y)
{
   int i;
   double obj_x, obj_y, obj_w, obj_h;
   abit->x += delta_x;
   abit->y += delta_y;
   
   for (i=0; i<abit->num_evas; i++)
     {
	evas_get_geometry(e->evas,abit->evas_list[i],&obj_x, &obj_y, &obj_w, &obj_h);
	obj_x += delta_x;
	obj_y += delta_y;
	evas_move(e->evas, abit->evas_list[i], obj_x, obj_y);
     }
}

double dump_line(Etox_Bit **abits, int bit_count, Etox *e, char align, char vertical_align, double beg_x, double cur_w)
{
   int i;
   double line_height, line_width, obj_x, obj_y, obj_w, obj_h, delta_x, delta_y, max_ascent, max_descent;

   /*
    * Copy  Etox bits to from the buffer to Evas
    * and place them accoring to height and
    * current alignments
    */

   line_height = 0;
   line_width = 0;
   
   /* Get line width and height */
   for (i=0; i<bit_count; i++) {
      if (abits[i]->h > line_height)
	{
	   evas_text_get_ascent_descent(e->evas, abits[i]->evas_list[(abits[i]->num_evas - 1)], &max_ascent, &max_descent);
	   line_height = abits[i]->h;
	}
      if (i == (bit_count-1))
	line_width = abits[i]->x + abits[i]->w;      
   }

   if (align == ALIGN_LEFT)
     delta_x = 0;
   else if (align == ALIGN_CENTER) 
     delta_x = (beg_x + cur_w - line_width) / 2;
   else if (align == ALIGN_RIGHT)
     delta_x = beg_x + (cur_w - line_width);

   /* Move to properly aligned postion */
   for (i=0; i<bit_count; i++) {

      if (vertical_align == ALIGN_TOP) 
	delta_y = 0;
      else if (vertical_align == ALIGN_CENTER) 
	delta_y = (line_height - abits[i]->h) / 2;
      else if (vertical_align == ALIGN_BOTTOM)
	{
	   double ascent, descent;
      
	   evas_text_get_ascent_descent(e->evas, abits[i]->evas_list[(abits[i]->num_evas - 1)], &ascent, &descent);
	   delta_y = (max_ascent - ascent);
	}
      etox_bit_move_relative(e, abits[i], delta_x, delta_y);
   }
   
   
   if (bit_count)
     {
	if(e->bit_list)
	  {
	     e->num_bits += bit_count;
	     e->bit_list = realloc(e->bit_list,
				   sizeof(Etox_Bit *) * (e->num_bits));
	  }
	else
	  {
	     e->num_bits += bit_count;
	     e->bit_list = malloc(sizeof(Etox_Bit *) *
				  (e->num_bits));
	  }

	for (i=0; i<bit_count; i++)
	  {
	     e->bit_list[e->num_bits - bit_count + i] = abits[i];
	  }
     }
   return line_height;
}

void find_available_size(Etox *e, double beg_x, double beg_y, double h, double padding, double *av_x, double *av_y, double *av_w)
{
   /* 
    * Find the next available rectangle of the given height 
    * that is not clipped off
    */
   
   double x1, x2, y1, y2;
   int i, j;
   char ok = 0;
   
   x1 = beg_x;
   x2 = e->w;
   y1 = beg_y;
   y2 = beg_y + h;
   
   if (beg_x > e->w) {
      /* It's the endof the line - begin a new one */
      
      x1 = 0;
      x2 = e->w;
      y1 = beg_y + h + padding;
      y2 = y2 + h;
   }
   while (!ok) {
      ok = 1;
      for(i=0; i<e->num_rects; i++)
	{
	   if ((y1 <= (e->rect_list[i]->y) + (e->rect_list[i]->h)) &&
	       ((y2) >= e->rect_list[i]->y ))
	     {
		if ((x1 >= e->rect_list[i]->x) &&
		    (x1 <= (e->rect_list[i]->x + e->rect_list[i]->w)))
		  {
		     /* The beginning is inside a rectangle - move it out of it */  
		
		     x1 = e->rect_list[i]->x + e->rect_list[i]->w;
		     if (x1 >= x2) 
		       {
			  /* No chance of a new line on this height - move down */
			  ok = 0;
			  x1 = 0;
			  x2 = e->w;
			  y1 = (beg_y + h + padding);
			  y2 = y1 + h;
		       }
		  }
		if ((x2 >= e->rect_list[i]->x) &&
		    (x1 <= (e->rect_list[i]->x)))
		  {
		     /* 
		      * The rectangle is inbetween the beginnig and
		      * the end of the line - shorten the line
		      */
		     x2 = e->rect_list[i]->x;
		     if (x2 <= x1)
		       {
			  /* No chance of a new line on this height - move down */
			  ok = 0;
			  x1 = 0;
			  x2 = e->w;
			  y1 = (beg_y + h + padding);
			  y2 = y1 + h;
			  
		       }
		  }
	     }
	}
   }
   *av_x = x1;
   *av_y = y1;
   *av_w = x2 - x1;
}

char etox_set_text(Etox *e, char *new_text)
{

   /*
    * This function does a few things:
    * 1) first and foremost, it sets the text string that is being parsed
    * 2) builds out the Etox internal data structure
    * 3) initializes any Evas objects that need to be created
    * 4) does the laundry
    *
    * returns TRUE on success, FALSE on failure.
    */

   /* Current font face */
   char *cur_font=NULL;
   int cur_font_size=0;
   E_Font_Style *cur_font_style = NULL;

   /* Placement variables */
   char align;
   double cur_x, cur_y, cur_w;
   double cur_line_width;
   double cur_padding;
   char cur_align, cur_vertical_align;
   double last_line_height;
   
   /* Current line bits */
   Etox_Bit **cur_line = NULL;
   int cur_line_bit_count;

   /* Current word */
   char *cur_word;

   /* Text position pointers */
   char *cur_beg;
   char *cur_end;

   /* Status variables */
   int first;
   char **needles;
   int cur_token, last_token;
   int ok;
   int thesame;

   E_Text_Color cur_text_color;

   if(!e)
     return 0;

   etox_clean(e);

   /*  Assume default font face - the etox one */
   cur_font = e->font;
   cur_font_size = e->font_size;
   cur_font_style = e->font_style;
   cur_align = e->align;
   cur_vertical_align = e->vertical_align;
   
   /* Prepare needles */
   needles = malloc (sizeof (char *) * 3);
   needles[0] = strdup(" ");
   needles[1] = strdup("\n");
   needles[2] = strdup("~");

   e->text = malloc((strlen(new_text) * sizeof(char)) + 1);
   e->text_len = strlen(new_text);
   strcpy(e->text,new_text);

   /*
    * set our initial location to be the top left corner of the Etox object
    */

   cur_x = 0;
   cur_y = 0;
   cur_w = e->w;
   cur_padding = e->padding;

   cur_line_bit_count = 0;
   /*
    * now we're going to loop through the text that they just set and build
    * our internal data structures as well as any Evas_Objects we will need
    * to draw this text to the screen
    */

   last_line_height = 0.0;
   first = 1;
   last_token = -1;
   cur_beg = NULL;
   cur_end = NULL;
   
   find_available_size(e, cur_x, cur_y, e->font_size, e->padding, &cur_x, &cur_y, &cur_w);
   cur_token = search_tokens(e->text, needles, 3, &cur_beg, &cur_end);
   while (cur_beg)
     {
	ok = 1;
	thesame = 1;

	/* We hava a next word */
	if (cur_line)
	  {
	     /* Existing line */
	     Etox_Bit *cur_bit;

	     cur_bit = cur_line[(cur_line_bit_count - 1)];
	     if ((cur_bit->font_style == cur_font_style) &&
		 (cur_bit->font_size == cur_font_size) &&
		 (!strcmp(cur_bit->font, cur_font)))
	       {
		  /* The same font face - adding text to the current bit */

		  Evas_Object *o;
		  double obj_x, obj_y, obj_w, obj_h;
		  char *new_line;
		  double line_end;		  
		  
		  new_line = malloc (strlen(cur_bit->text) + strlen(cur_beg) + 2);
		  strcpy(new_line, cur_bit->text);
		  if (last_token == 0)
		    strcat(new_line, " ");
		  strcat(new_line, cur_beg);

		  o = evas_add_text(e->evas,cur_font,cur_font_size,
				    new_line);

		  obj_w = evas_get_text_width(e->evas, o);
		  
		  line_end = cur_bit->x + obj_w;
		  
		  if (line_end > (cur_w + cur_line[0]->x))
		    {
		       /* this will not fit into a line - start a new one */
		       line_end = cur_w + cur_line[0]->x + 1;
		       
		       thesame = 0;
		       last_line_height = dump_line(cur_line, cur_line_bit_count, e, cur_align, cur_vertical_align, cur_line[0]->x, cur_w);
		       free(cur_line);
		       cur_line = NULL;
		       cur_line_bit_count = 0;
		       find_available_size(e, line_end, cur_y, 
					   last_line_height, e->padding, &cur_x, &cur_y, &cur_w);
		    }
		  else
		    {
		       /* Add a text to a current line */
		       
		       free(cur_bit->text);
		       cur_bit->text = strdup(new_line);
		       update_bit_objects(cur_bit, e);
		       
		       cur_x = cur_bit->x + cur_bit->w;	     	  
		    }
		  evas_del_object(e->evas, o);
	       }
	     else
	       {
		  thesame = 0;
	       }
	  }
	else
	  thesame = 0;
	if (!thesame && ((strcmp(cur_beg,"")) || (last_token == 0)))
	  {
	     /* New line or a new style - building a new bit */
	     double obj_x, obj_y, obj_w, obj_h;
	     
	     /* Create a new bit list */
	     if (cur_line) 
	       {
		  cur_line = realloc(cur_line,sizeof(Etox_Bit *) * (cur_line_bit_count + 1));
	       }
	     else 
	       {
		  cur_line = malloc(sizeof(Etox_Bit *) * (cur_line_bit_count + 1));
	       }
	     
	     /* Create the next bit */
	     cur_line[cur_line_bit_count] = Etox_Bit_new();
	     if ((last_token == 0) && (cur_line_bit_count > 0))
	       {
		  char *new_line;
		  
		  new_line = malloc(sizeof(char) * (strlen(cur_beg) + 2));
		  strcpy(new_line, " ");
		  strcat(new_line, cur_beg);
		  cur_line[cur_line_bit_count]->text = strdup(new_line);
		  free(new_line);
	       }
	     else
	       cur_line[cur_line_bit_count]->text = strdup(cur_beg);

	     cur_line[cur_line_bit_count]->font = strdup(cur_font);
	     cur_line[cur_line_bit_count]->font_style = cur_font_style;
	     cur_line[cur_line_bit_count]->font_style->in_use++;
	     cur_line[cur_line_bit_count]->font_size = cur_font_size;

	     /* Set the object in the proper place 0,0 */
	     cur_line[cur_line_bit_count]->x = cur_x;
	     cur_line[cur_line_bit_count]->y = cur_y;

	     /* Create the Evas objects */
	     create_bit_objects(cur_line[cur_line_bit_count], e, &cur_text_color);

	     cur_x = cur_line[cur_line_bit_count]->x + cur_line[cur_line_bit_count]->w;	     	  

	     cur_line_bit_count += 1;
	  }
	if (cur_token ==2)
	  {
	     char *s2, *s3;

	     ok = 0;

	     /*
	      * this text contains special directive, which we have
	      * to pay attention to (begins with a "~")
	      */

	     do
	       {
		  /* Search for a closing char */

		  s2 = strstr(cur_end,"~");
		  if (s2 && (s2 != (cur_end+1)))
		    {
		       s3 = strstr(cur_end,"=");
		       if (s3 && (s3 < s2))
			 {
			    /* It's a change directive */
			    char *s4;

			    /* Extract the value */
			    s4 = malloc(s2 - s3);
			    strncpy(s4,(s3+1),(s2 - s3 -1));
			    s4[(s2 - s3 - 1)] = '\0';

			    if (!strncmp(cur_end,"style",5))
			      {
				 /* It's a style directive */
				 /* Load a new style */
				 cur_font_style = E_load_font_style(s4);
			      }
			    else if (!strncmp(cur_end,"font",4))
			      {
				 /* Set a new current font */
				 if (cur_font && (cur_font != e->font))
				   free(cur_font);
				 cur_font = strdup(s4);
			      }
			    else if (!strncmp(cur_end,"size",4))
			      {
				 /* Set a new font size */
				 cur_font_size = atoi(s4);
			      }
			    else if (!strncmp(cur_end,"padding",7))
			      {
				 /* Set current padding value */
				 cur_padding = atoi(s4);
			      }
			    else if (!strncmp(cur_end,"color",5))
			      {
				 /* Set one of the colors */
				 char text_type[3];
				 E_Color color;

				 memset(text_type,0,3);
				 sscanf(s4,"%s %d %d %d",text_type,&color.r,
					&color.g,&color.b);
				 if(!strcmp(text_type,"sh"))
				   {
				      /* this is a shadow color */
				      cur_text_color.sh = color;
				   }
				 else if(!strcmp(text_type,"fg"))
				   {
				      /* this is a foreground color */
				      cur_text_color.fg = color;
				   }
				 else if(!strcmp(text_type,"ol"))
				   {
				      /* this is an outline color */
				      cur_text_color.ol = color;
				   }

			      }
			    else if (!strncmp(cur_end,"align",5))
			      {
				 /* Set the current alignment */
				 
				 if(!strcmp(s4,"right"))
				   {
				      cur_align = ALIGN_RIGHT;
				   }
				 else if(!strcmp(s4,"center"))
				   {
				      cur_align = ALIGN_CENTER;
				   }
				 else if(!strcmp(s4,"left"))
				   {
				      cur_align = ALIGN_LEFT;
				   }
			      }

			    else if (!strncmp(cur_end,"valign",6))
			      {
				 /* Set the current alignment */
				 
				 if(!strcmp(s4,"top"))
				   {
				      cur_vertical_align = ALIGN_TOP;
				   }
				 else if(!strcmp(s4,"center"))
				   {
				      cur_vertical_align = ALIGN_CENTER;
				   }
				 else if(!strcmp(s4,"bottom"))
				   {
				      cur_vertical_align = ALIGN_BOTTOM;
				   }
			      }
			    free(s4);
			 }
		       else
			 {
			    /* It's a "return to default" directive */
			    if (!strncmp(cur_end,"style",5))
			      {
				 cur_font_style = e->font_style;
			      }
			    else if (!strncmp(cur_end,"font",4))
			      {
				 if (cur_font && (cur_font != e->font))
				   free(cur_font);
				 cur_font = strdup(e->font);
			      }
			    else if (!strncmp(cur_end,"size",4))
			      {
				 cur_font_size = e->font_size;
			      }
			    else if (!strncmp(cur_end,"padding",7))
			      {
				 cur_padding = e->padding;
			      }
			    else if (!strncmp(cur_end,"colorfg",7))
			      {
				 cur_text_color.fg = e->color.fg;
			      }
			    else if (!strncmp(cur_end,"colorsh",7))
			      {
				 cur_text_color.sh = e->color.sh;
			      }
			    else if (!strncmp(cur_end,"colorol",7))
			      {
				 cur_text_color.ol = e->color.ol;
			      }
			    else if (!strncmp(cur_end,"align",5))
			      {
				 cur_align = e->align;
			      }
			    else if (!strncmp(cur_end,"valign",6))
			      {
				 cur_vertical_align = e->vertical_align;
			      }
			 }
		       last_token = cur_token;
		       cur_token = search_tokens((s2 + 1),needles,3, &cur_beg, &cur_end);
		    }
		  else
		    {
		       last_token = cur_token;
		       cur_token = search_tokens((s2 + 1),needles,3, &cur_beg, &cur_end);
		    }
	       }
	     while (cur_token==2 && !(strcmp(cur_beg,"")));
	  }
	if (ok)
	  {
	     last_token = cur_token;
	     cur_token = search_tokens(cur_end,needles,3, &cur_beg, &cur_end);
	  }
	if (!(cur_end || cur_beg) || (last_token == 1))
	  {
	     /* the end of the text or the forced end of line - flush the current line */
	     
	     if (cur_line)
	       {
		  last_line_height = dump_line(cur_line, cur_line_bit_count, e, cur_align, cur_vertical_align, cur_line[0]->x, cur_w);
		  free(cur_line);
		  cur_line = NULL;
		  cur_line_bit_count = 0;
	       }
	     find_available_size(e, e->w + 1, cur_y, 
				 last_line_height, e->padding, &cur_x, &cur_y, &cur_w);
	  }	
     }
}

char etox_set_layer(Etox *e, int layer)
{

   /* sets the layer on which all the Evas components sit */

   int i,j;

   if(!e)
     return 0;

   e->layer = layer;

   for(i=0;i<e->num_bits;i++)
     {
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     evas_set_layer(e->evas, e->bit_list[i]->evas_list[j], e->layer);
	  }
     }

   return 1;

}

int etox_get_layer(Etox *e)
{

	/*
	 * returns the layer on which all the Evas components sit
	 * or a negative -1 in case of failure
	 */

   if(!e)
     return -1;

   return e->layer;
}

char etox_show(Etox *e)
{

	/* calls evas_show on all active Evas components */

   int i,j;

   if(!e)
     return 0;

   for(i=0;i<e->num_bits;i++)
     {
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     evas_show(e->evas,e->bit_list[i]->evas_list[j]);
	  }
     }

   e->visible = 1;

   return 1;

}

char etox_hide(Etox *e)
{

	/* calls evas_hide on all active Evas components */

   int i,j;

   if(!e)
     return 0;

   for(i=0;i<e->num_bits;i++)
     {
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     evas_hide(e->evas, e->bit_list[i]->evas_list[j]);
	  }
     }

   e->visible = 0;

   return 1;

}

void etox_move(Etox *e, double x, double y)
{

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

   for(i=0;i<e->num_bits;i++)
     {
	e->bit_list[i]->x += delta_x;
	e->bit_list[i]->y += delta_y;
	for(j=0;j<e->bit_list[i]->num_evas;j++)
	  {
	     double old_y, old_x, w, h;

	     evas_get_geometry(e->evas,e->bit_list[i]->evas_list[j],&old_x,
			       &old_y,&w,&h);
	     evas_move(e->evas,e->bit_list[i]->evas_list[j],old_x + delta_x,
		       old_y + delta_y);
	  }
     }

   return;

}

void etox_resize(Etox *e, double w, double h)
{

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

char etox_set_font_style(Etox *e, E_Font_Style *font_style)
{

   /*
    * this function changes the default font_style, which will, cause a
    * refresh of the internal data
    */

   if(!font_style)
     return 0;

   e->font_style = font_style;

   //	etox_refresh(e);

   return 1;

}

void etox_refresh(Etox *e)
{

   /*
    * This is a bit kludgy, but will work for now, since it forces a complete
    * rebuilding of the setup inside of the Etox object
    */

   if (e->text)
     {
	char *tmp = NULL;

	tmp = malloc(e->text_len + 1);
	if(tmp)
	  strcpy(tmp,e->text);
	etox_set_text(e,tmp);
	if(tmp)
	  free(tmp);
     }

}

char etox_clip_rect_new(Etox *e, double x, double y, double w,double h)
{
   
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

   if(e->num_rects <=0)
     {
	e->rect_list = malloc(sizeof(E_Clip_Rect *) + 1);
	e->rect_list[0] = new_rect;
     }
   else
     {
	e->rect_list = realloc(e->rect_list,(sizeof(E_Clip_Rect *) *
					     e->num_rects + 1) + 1);
	e->rect_list[e->num_rects] = new_rect;
     }
   e->num_rects++;

   etox_refresh(e);

   return 1;

}

void etox_free(Etox *e)
{

	/*
	 * this function is pretty straight forward.  most of the internals have
	 * been moved into etox_clean, since it is used again in other places to
	 * reset internal bits
	 */

   etox_clean(e);
   free(e);

   return;
}

Etox_Bit *Etox_Bit_new()
{

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

Etox *Etox_new(char *name)
{

   /* initialize new Etox */

   Etox *e;

   e = malloc(sizeof(Etox));

   if(name)
     {
	e->name = malloc((strlen(name) * sizeof(char)) + 1);
	strcpy(e->name,name);
     }
   else
     {
	e->name = NULL;
     }

   e->text = NULL;
   e->text_len = 0;

   e->rect_list = NULL;
   e->num_rects = 0;
   e->rendered = 0;
   e->visible = 0;
   e->font=NULL;
   e->font_style = NULL;
   e->bit_list = NULL;
   e->num_bits = 0;
   e->evas = NULL;
   e->padding = 0;
   e->align = ALIGN_LEFT;
   e->vertical_align = ALIGN_BOTTOM;
   
   return e;
}

Etox *Etox_new_all(Evas *evas, char *name, double x, double y, double w, double h,
		   int layer, char *font, char *font_style, int font_size, E_Text_Color *cl,
		   double padding, int align, int vertical_align)
{
   Etox *e;

   e = malloc(sizeof(Etox));
   
   if (name)
     e->name = strdup(name);
   else
     e->name = NULL;
   e->text = NULL;
   e->text_len = 0;
   
   e->rect_list = NULL;
   e->num_rects = 0;
   e->rendered = 0;
   e->visible = 0;
   e->x = x;
   e->y = y;
   e->w = w;
   e->h = h;
   if (font)
     e->font = strdup(font);
   else
     e->font = NULL;
   if (font_style)
     e->font_style = E_load_font_style(font_style);
   else
     {
	e->font_style = malloc(sizeof(E_Font_Style));

	e->font_style->name = strdup("default");
	e->font_style->bits = malloc(sizeof(E_Style_Bit));
	
	e->font_style->bits[0].x = 0.0;
	e->font_style->bits[0].y = 0.0;
	e->font_style->bits[0].alpha = 255;
	e->font_style->bits[0].type = STYLE_TYPE_FOREGROUND;
	e->font_style->num_bits = 1;	
     }
   e->font_style->in_use++;
   e->bit_list = NULL;
   e->num_bits = 0;
   e->evas = evas;
   e->font_size = font_size;
   e->padding = padding;
   e->align = align;
   if (cl)
     e->color = *cl;
   else
     {
	e->color.fg.r = 255;
	e->color.fg.g = 255;
	e->color.fg.b = 255;
	e->color.ol.r = 0;
	e->color.ol.g = 0;
	e->color.ol.b = 0;
	e->color.sh.r = 0;
	e->color.sh.g = 0;
	e->color.sh.b = 0;
	
     }
   e->vertical_align = vertical_align;
}
