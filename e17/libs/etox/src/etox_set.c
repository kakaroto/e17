#include "Etox_private.h"
#include "Etox.h"
#include <string.h>

void
etox_set_evas(Etox e, Evas evas)
{
  if (!e) return;
  e->evas = evas;

  _etox_refresh(e);
}

void
etox_set_name(Etox e, char *name)
{
  if (!e) return;
  if (name)
    e->name = strdup(name);
  else 
    e->name = NULL;
}

void
etox_set_font(Etox e, char *font, int size)
{
  if (!e) return;
  if (font)
    e->font = strdup(font);
  else
    e->font = NULL;
  e->font_size = size;
  
  _etox_refresh(e);
}

void
etox_set_style(Etox e, Etox_Style style)
{
  if (!e) return;
  if (e->font_style)
    etox_style_free(e->font_style);
  e->font_style = style;
  e->font_style->in_use++;

  _etox_refresh(e);
}

void
etox_set_align(Etox e, Etox_Align h_align, Etox_Align v_align)
{
  if (!e) return;
  e->align = h_align;
  e->vertical_align = v_align;

  _etox_refresh(e);
}

void
etox_set_layer(Etox e, int layer)
{
  int i, j;

  if (!e) return;
  e->layer = layer;

  for (i = 0; i < e->num_bits; i++)
    for (j = 0; j < e->bit_list[i]->num_evas; j++)
      evas_set_layer(e->evas, e->bit_list[i]->evas_list[j], e->layer);
}

void
etox_set_padding(Etox e, double padding)
{
  if (!e) 
    return;
  e->padding = padding;

  _etox_refresh(e);
}

void
etox_set_text(Etox e, char *new_text)
{
  /* Current font face */
  char *cur_font = NULL;   
  int cur_font_size=0;
  Etox_Style cur_font_style = NULL;

  /* Placement variables */
  Etox_Align align;
  double cur_x, cur_y, cur_w;
  double cur_line_width;     
  double cur_padding;
  Etox_Align cur_align, cur_vertical_align;
  double last_line_height;

  /* Current line bits */
  Etox_Bit *cur_line = NULL;
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

  Etox_Color cur_text_color;

  if (!e)
    return;

  _etox_clean(e);

  /*  Assume default font face - the etox one */
  cur_font = e->font;               
  cur_font_size = e->font_size;
  cur_font_style = e->font_style;
  cur_align = e->align;
  cur_vertical_align = e->vertical_align;
  
  /* Prepare needles */
  needles = malloc(sizeof(char *) * 3);
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
  cur_text_color = e->color;

  cur_line_bit_count = 0;
  /*
   * now we're going to loop through the text that they just set word
   * by word and build our internal data structures as well as any   
   * Evas_Objects we will need to draw this text to the screen
   */

  last_line_height = 0.0;
  first = 1;
  last_token = -1;
  cur_beg = NULL;
  cur_end = NULL;
  
  _etox_available_size(e, cur_x, cur_y, e->font_size, e->padding,
		       &cur_x, &cur_y, &cur_w);
  cur_token = _etox_search_tokens(e->text, needles, 3, &cur_beg, &cur_end);
  while (cur_beg)
    {            
      ok = 1;
      thesame = 1;
      
      /* We hava a next word */
      if (cur_line)
	{
	  /* Existing line */
	  Etox_Bit cur_bit;

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
                                  
	      new_line = malloc(strlen(cur_bit->text) + strlen(cur_beg) + 2);
	      strcpy(new_line, cur_bit->text);
	      if (last_token == 0)            
		strcat(new_line, " ");
	      strcat(new_line, cur_beg);
                                            
	      o = evas_add_text(e->evas,cur_font,cur_font_size,
				new_line);
                                              
	      obj_w = evas_get_text_width(e->evas, o);
                                                          
	      line_end = cur_bit->x + obj_w;          
                                                
	      evas_del_object(e->evas, o);  
	      if (line_end > (cur_w + cur_line[0]->x))
		{                                     
		  /* this will not fit into a line - start a new one */
		  line_end = cur_w + cur_line[0]->x + 1;
                                                             
		  thesame = 0;                          
		  last_line_height = _etox_bit_dump_line(cur_line, 
							 cur_line_bit_count, 
							 e, cur_align, 
							 cur_vertical_align, 
							 cur_line[0]->x, 
							 cur_w);
		  free(cur_line);
		  cur_line = NULL;
		  cur_line_bit_count = 0;
		  _etox_available_size(e, line_end, cur_y,
				       last_line_height, e->padding,
				       &cur_x, &cur_y, &cur_w);
		}
	      else
		{
		  /* Add a text to a current line */
                                                         
		  free(cur_bit->text);              
		  cur_bit->text = strdup(new_line);
		  _etox_bit_update_objects(cur_bit, e);  
		  
		  cur_x = cur_bit->x + cur_bit->w;
		}
	    }
	  else
	    thesame = 0;
	}     
      else
	thesame = 0;
      if (!thesame && ((strcmp(cur_beg,"")) || (last_token == 0)))
	{
	  /* New line or a new font face - building a new bit */
	  double obj_x, obj_y, obj_w, obj_h;

	  /* Create a new bit list */
	  if (cur_line)
	    cur_line = realloc(cur_line,sizeof(Etox_Bit) * 
			       (cur_line_bit_count + 1));
	  else
	    cur_line = malloc(sizeof(Etox_Bit) * (cur_line_bit_count + 1));
                
	  /* Create the next bit */
	  cur_line[cur_line_bit_count] = _etox_bit_new();
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

	  /* Set the object in the proper place */
	  cur_line[cur_line_bit_count]->x = cur_x;
	  cur_line[cur_line_bit_count]->y = cur_y;

	  /* Create the Evas objects */
	  _etox_bit_create_objects(cur_line[cur_line_bit_count], e, 
				   cur_text_color);
             
	  cur_x = cur_line[cur_line_bit_count]->x + 
	    cur_line[cur_line_bit_count]->w;
               
	  cur_line_bit_count += 1;
	}
      if (cur_token == 2)
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
	       if ((cur_end[0] == '~'))
		 {
		    /* New line or a new font face - building a new bit */
		    double obj_x, obj_y, obj_w, obj_h;
		    
		    /* Create a new bit list */
		    if (cur_line)
		      cur_line = realloc(cur_line,sizeof(Etox_Bit) * 
					      (cur_line_bit_count + 1));
		    else
		      cur_line = malloc(sizeof(Etox_Bit) * (cur_line_bit_count + 1));
		    
		    /* Create the next bit */
		    cur_line[cur_line_bit_count] = _etox_bit_new();
		    if ((last_token == 0) && (cur_line_bit_count > 0))
		      {
			 char *new_line;
			 
			 new_line = malloc(sizeof(char) * (strlen(needles[2]) + 1));
			 strcpy(new_line, needles[2]);
			 cur_line[cur_line_bit_count]->text = strdup(new_line);
			      free(new_line);                                       
		      }
		    else
		      cur_line[cur_line_bit_count]->text = strdup(needles[2]);
		    
		    cur_line[cur_line_bit_count]->font = strdup(cur_font);
		    cur_line[cur_line_bit_count]->font_style = cur_font_style;
		    cur_line[cur_line_bit_count]->font_style->in_use++;
		    cur_line[cur_line_bit_count]->font_size = cur_font_size;
			 
		    /* Set the object in the proper place */
		    cur_line[cur_line_bit_count]->x = cur_x;
		    cur_line[cur_line_bit_count]->y = cur_y;
		    
		    /* Create the Evas objects */
		    _etox_bit_create_objects(cur_line[cur_line_bit_count], e, 
					     cur_text_color);
		    
		    cur_x = cur_line[cur_line_bit_count]->x + 
		      cur_line[cur_line_bit_count]->w;
		    
		    cur_line_bit_count += 1;
		 }
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
			  cur_font_style = etox_style_new(s4);
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
			  struct _Etox_Color_Bit color;
                                               
			  memset(text_type,0,3);
			  sscanf(s4,"%s %d %d %d %d", &text_type,
				 &color.r, &color.g, &color.b, &color.a);
			  if(!strcmp(text_type,"sh"))
			    {                        
			      /* this is a shadow color */
			      etox_color_set_member(cur_text_color, "sh", 
						    color.r, color.g, 
						    color.b, color.a);
			    }
			  else if(!strcmp(text_type,"fg"))
			    {                             
			      /* this is a foreground color */
			      etox_color_set_member(cur_text_color, "fg",  
						    color.r, color.g,  
						    color.b, color.a);
			    }                            
			  else if(!strcmp(text_type,"ol"))
			    {                             
			      /* this is an outline color */
			      etox_color_set_member(cur_text_color, "ol",  
						    color.r, color.g,  
						    color.b, color.a);
			    }
			}
		      else if (!strncmp(cur_end,"align",5))
			{                                  
			  /* Set the current alignment */
			  
			  if(!strcmp(s4,"right"))                            
			    cur_align = ETOX_ALIGN_RIGHT;
			  else if(!strcmp(s4,"center"))
			    cur_align = ETOX_ALIGN_CENTER;               
			  else if(!strcmp(s4,"left"))
			    cur_align = ETOX_ALIGN_LEFT;               
			}
		      else if (!strncmp(cur_end,"valign",6))
			{                                  
			  /* Set the current alignment */
                                                                
			  if(!strcmp(s4,"top"))                       
			    cur_vertical_align = ETOX_ALIGN_TOP;
			  else if(!strcmp(s4,"center"))
			    cur_vertical_align = ETOX_ALIGN_CENTER;
			  else if(!strcmp(s4,"bottom"))  
			    cur_vertical_align = ETOX_ALIGN_BOTTOM;
			}
		      free(s4);                                
		    }
		  else
		    {
		      Etox_Color_Bit cb;

		      /* It's a "return to default" directive */
		      if (!strncmp(cur_end,"style",5))          
			cur_font_style = e->font_style;
		      else if (!strncmp(cur_end,"font",4))
			{
			  if (cur_font && (cur_font != e->font))
			    free(cur_font);
			  cur_font = strdup(e->font);
			}                             
		      else if (!strncmp(cur_end,"size",4))
			cur_font_size = e->font_size;
		      else if (!strncmp(cur_end,"padding",7))
			cur_padding = e->padding;
		      else if (!strncmp(cur_end,"colorfg",7))
			{  
			  cb = _etox_color_get_bit(e->color, "fg");
			  etox_color_set_member(cur_text_color, "fg",
						cb->r, cb->g,
						cb->b, cb->a);
			}                                  
		      else if (!strncmp(cur_end,"colorsh",7))
			{                                    
			  cb = _etox_color_get_bit(e->color, "sh");    
			  etox_color_set_member(cur_text_color, "sh",
						cb->r, cb->g,
						cb->b, cb->a);
			}                                  
		      else if (!strncmp(cur_end,"colorol",7))
			{ 
			  cb = _etox_color_get_bit(e->color, "ol");    
			  etox_color_set_member(cur_text_color, "ol",
						cb->r, cb->g,
						cb->b, cb->a);
			}                                  
		      else if (!strncmp(cur_end,"align",5))
			cur_align = e->align;
		      else if (!strncmp(cur_end,"valign",6))
			cur_vertical_align = e->vertical_align;
		    }
		  last_token = cur_token;
		  cur_token = _etox_search_tokens((s2 + 1),needles,3, 
						  &cur_beg, &cur_end);
		}
	      else
		{
		  last_token = cur_token;
		  cur_token = _etox_search_tokens((s2 + 1),needles,3, 
						  &cur_beg, &cur_end);
		}
	    }
	  while (cur_token == 2 && !(strcmp(cur_beg, "")));
	}
      if (ok)
	{    
	  last_token = cur_token;
	  cur_token = _etox_search_tokens(cur_end,needles,3, &cur_beg, 
					  &cur_end);
	}
      if (!(cur_end || cur_beg) || (last_token == 1))
	{
	  /* the end of the text or the forced end of line - flush the 
	   * buffer (current line) 
	   */
	  if (cur_line)
	    {
	      last_line_height = _etox_bit_dump_line(cur_line, 
						     cur_line_bit_count, e,
						     cur_align, 
						     cur_vertical_align, 
						     cur_line[0]->x, cur_w);
	      free(cur_line);
	      cur_line = NULL;
	      cur_line_bit_count = 0; 
	    }
	  _etox_available_size(e, e->w + 1, cur_y, 
			       last_line_height, e->padding, 
			       &cur_x, &cur_y, &cur_w);
	}
    }
}


void
etox_set_color(Etox e, Etox_Color color)
{
  if (!e) return;

  e->color = color;

  _etox_refresh(e);
}

void
etox_set_alpha(Etox e, int alpha)
{
  int i, j;

  if (!e) return;
  e->alpha_mod = alpha;
  
  for (i = 0; i < e->num_bits; i++)                 
    for (j = 0; j < e->bit_list[i]->num_evas; j++)
      {
	int r, g, b, a;

	evas_get_color(e->evas, e->bit_list[i]->evas_list[j], &r, &g, &b, &a);
	a = e->bit_list[i]->font_style->bits[j].alpha * alpha / 255;
	evas_set_color(e->evas, e->bit_list[i]->evas_list[j], r, g, b, a);
      }
}

void
etox_set_clip(Etox e, Evas_Object clip)
{
  int i, j;

  if (!e) return;
  if (!clip) return;

  e->clip = clip;

  for (i = 0; i < e->num_bits; i++)
    for (j = 0; j < e->bit_list[i]->num_evas; j++)
      evas_set_clip(e->evas, e->bit_list[i]->evas_list[j], e->clip);
}

void
etox_unset_clip(Etox e)
{
  int i, j;

  if (!e) return;
  e->clip = NULL;

  for (i = 0; i < e->num_bits; i++)
    for (j = 0; j < e->bit_list[i]->num_evas; j++)
      evas_unset_clip(e->evas, e->bit_list[i]->evas_list[j]);
}
