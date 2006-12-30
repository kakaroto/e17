#include <string.h>
#include <glib.h>
#include <stdio.h>
#include <Etk.h>
#include "main.h"
#include "interface.h"
#include "parse.h"

/* Parser Helper */
gchar* SearchMatchingBra(gchar* s){
   int i=0;
   int opened=0,closed=0;
   int len=strlen(s);

   //printf("BRA: %d\n",len);
   while (i < len){
      if (s[i] == '{'){
	 opened++;
	 //printf("%c",s[i]);
      }
      else if (s[i] == '}'){
	 closed++;
	 //printf("%d",opened-closed);
      }
      if (opened-closed <= 0) return s+i;
      i++;
   }
   printf("Parsing error: Can't find closed brackets ' } '\n");
   return NULL;
}
GString* GetString(gchar *s){
   gchar *start=0,*end=0;

   if (!(start = strchr(s,'"'))){
      printf(_("Parsing error: Can't find string start character ' \" '\n"));
      return NULL;
   }
   start++;
   if (!(end = strchr(start,'"'))){
      printf(_("Parsing error: Can't find string terminating character ' \"'\n"));
      return FALSE;
   }

   return g_string_new_len(start,end-start);
}

int GetEnum(gchar *s){
   gchar *start=0,*end=0;

   start = s;
   while (!(g_ascii_isupper(start[0]))) start++;	//TODO overflow if not found?
   end = start;
   while ((g_ascii_isupper(end[0])) || (end[0] == '_')) end++;

   if (g_strstr_len(start,end-start,"RECT")) return PART_TYPE_RECT;
   if (g_strstr_len(start,end-start,"IMAGE")) return PART_TYPE_IMAGE;
   if (g_strstr_len(start,end-start,"TEXT")) return PART_TYPE_TEXT;
   if (g_strstr_len(start,end-start,"NONE")) return NONE;
   if (g_strstr_len(start,end-start,"PLAIN")) return FX_PLAIN;
   if (g_strstr_len(start,end-start,"OUTLINE")) return FX_OUTLINE;
   if (g_strstr_len(start,end-start,"SOFT_OUTLINE")) return FX_SOFT_OUTLINE;
   if (g_strstr_len(start,end-start,"OUTLINE_SOFT_SHADOW")) return FX_OUTLINE_SOFT_SHADOW;
   if (g_strstr_len(start,end-start,"SOFT_SHADOW")) return FX_SOFT_SHADOW;
   if (g_strstr_len(start,end-start,"OUTLINE_SHADOW")) return FX_OUTLINE_SHADOW;
   if (g_strstr_len(start,end-start,"SHADOW")) return FX_SHADOW;

   return 0;
   // OLD METHOD
   //start = strstr
   //printf("GET UPPER: %s\n",s);
   //while (!(g_ascii_isupper(s[0]))) s++;
   //sscanf(s,"%s;*",&buf);
   //printf("GET UPPER: %d\n",end-start);

   //return g_string_new_len(start,end-start);
}
gchar* SearchToken(gchar *s,gchar* token, int len){
   if (len){
      if (!(s =g_strstr_len(s,len,token))) return NULL;
   }
   else{
      if (!(s = strstr(s,token))) return NULL;
   }

   s += strlen(token);
   return s;
}
/* Parser Functions */
int ParseImages(gchar* edc){
/* 	gchar* start;
	gchar* end;
	GString* im_tag;
	GString* im_name;
	gint	im_comp_type;
	gint	im_comp=0;

	while ((start = strstr(edc, "image,"))){
		// Get the image: tag
		end = strchr(start,';');
		im_tag = g_string_new_len(start,end-start);
		edc = end;
		//printf("TAG: %s\n",im_tag->str);

		// Get image name
		start = strchr(im_tag->str,'"')+1;
		end = strchr(start,'"');
		im_name = g_string_new_len(start,end-start);

		// Get image compression
		start = 0;
		start = strstr(im_tag->str,"RAW");
		if (start){
			im_comp_type = COMP_RAW;
		}else{
			start = 0;
			start = strstr(im_tag->str,"COMP");
			if (start){
				im_comp_type = COMP_COMP;
			}else{
				start = 0;
				start = strstr(im_tag->str,"LOSSY");
				if (start){
					im_comp_type = COMP_LOSSY;
					start = start+6;
					im_comp = atoi(start);
				}else{
					printf("Parser error: Can't find image compression type.\n");
					return FALSE;
				}

			}
		}
		EDC_Image_new(im_name->str,im_comp_type,im_comp);
	} */
   return TRUE;
}
int GetGroupsTag(gchar* edc){
   EDC_Group *group;
   gchar* start=NULL;
   gchar* end=NULL;
   GString* tmp;

   //printf("Get Parts:\n%s",edc);

   //Search collections section and check brackets integrity
   if (!(start = strstr(edc,"collections{"))){
      printf(_("Parsing error: Can't find 'collections' section\n"));
      return FALSE;
   }
   start = start+11;	//Skip collections word
   end = SearchMatchingBra(start);

   //search all groups
   while((start = strstr(start,"group{"))){
      start = start+5; //Skip group word
      end = SearchMatchingBra(start);
      tmp = g_string_new_len(start+1,(end-start)-2);
      group = EDC_Group_new("new",0,0,0,0);
      g_string_printf(group->complete_tag,"%s",tmp->str);

      start = end; 	//redo with next group
   }
   return TRUE;
}

EDC_Part* ParsePart(GString *part_tag, EDC_Group *group){
   gchar* start=NULL;
   gchar* end=NULL;
   GString *pinfo;
   GString *descr;
   EDC_Part* part=EDC_Part_new(group,"",NONE);

   //printf("GET PART: %s\n",part_tag->str);

	/* Read part information (name,ecc..) */
   start = part_tag->str;
   end = strstr(start,"description{");
   pinfo = g_string_new_len(start,end-start);

   //Search name in information
   if (!(start = SearchToken(pinfo->str,"name,",0))){
      printf(_("Parsing error: Can't find part name\n"));
      return FALSE;
   }
   part->name = GetString(start);

   //Set Group
   //part->group = group;

   //Search type in information
   start = SearchToken(pinfo->str,"type,",0);
   part->type = GetEnum(start);

   //Search effect in information
   if ((start = SearchToken(pinfo->str,"effect,",0)))
     part->effect = GetEnum(start);
   else
     part->effect = NONE;

   //Search mouse_events in information
   if ((start = SearchToken(pinfo->str,"mouse_events,",0)))
     sscanf(start,"%d",&part->mouse_events);
   else
     part->mouse_events = 0;

   //Search repeat_events in information
   if ((start = SearchToken(pinfo->str,"repeat_events,",0)))
     sscanf(start,"%d",&part->repeat_events);
   else
     part->repeat_events = 0;

   //Search clip_to in information
   if ((start = SearchToken(pinfo->str,"clip_to,",0)))
     part->clip_to = GetString(start);
   else
     part->clip_to = g_string_new("");

   //Search color_class in information
   if ((start = SearchToken(pinfo->str,"color_class,",0)))
     part->color_class = GetString(start);
   else
     part->color_class = g_string_new("");

   //Search text_class in information
   if ((start = SearchToken(pinfo->str,"text_class,",0)))
     part->text_class = GetString(start);
   else
     part->text_class = g_string_new("");

	/* printf("INFO: %s\n",pinfo->str);
	printf("NAME: %s\n",part->name->str);
	printf("TYPE: %d\n",part->type);
	printf("MOUSE_EVENTS: %d\n",part->mouse_events);
	printf("REPEAT_EVENTS: %d\n",part->repeat_events);
	printf("EFFECT: %d\n",part->effect);
	printf("CLIP_TO: %s\n",part->clip_to->str);
	printf("COLOR_CLASS: %s\n",part->color_class->str);
	printf("TEXT_CLASS: %s\n",part->text_class->str);  */

	/* Get all description */
   start = part_tag->str;
   while ((start = SearchToken(start,"description{",0))){
      end = SearchMatchingBra(start-1);
      descr = g_string_new_len(start,end-start);

      part->descriptions = g_list_append(part->descriptions,ParseDescription(descr,part));

      g_string_free(descr,TRUE);
   }
   //Set first description selected
   part->current_description = part->descriptions->data;

   AddPartToTree(part);

   return part;
}
EDC_Description* ParseDescription(GString *description_tag, EDC_Part* part){
   gchar* start=NULL;
   gchar* rel=NULL;
   gchar* image=NULL;
   gchar* text=NULL;
   int len;
   gchar* end=NULL;

   EDC_Description* Desc;

   //printf("\nDESCR: %s\n",description_tag->str);

   //Search state tag
   if (!(start = SearchToken(description_tag->str,"state,",0))){
      printf(_("Parsing error: Can't find group name\n"));
      return FALSE;
   }
   Desc = EDC_Description_new(part,"",0);
   Desc->state = GetString(start);
   start = strchr(start,'"')+1;	//Skip string
   start = strchr(start,'"')+1;
   sscanf(start,"%f",&Desc->state_index);	//Get state index

   //Search visible tag
   if ((start = SearchToken(description_tag->str,"visible,",0)))
     sscanf(start,"%d",&Desc->visible);

   //Search align tag
   if ((start = SearchToken(description_tag->str,"align,",0)))
     sscanf(start,"%f %f",&Desc->align_h, &Desc->align_v);

   //Search min tag
   if ((start = SearchToken(description_tag->str,"min,",0)))
     sscanf(start,"%d %d",&Desc->min_w, &Desc->min_h);

   //Search max tag
   if ((start = SearchToken(description_tag->str,"max,",0)))
     sscanf(start,"%d %d",&Desc->max_w, &Desc->max_h);

   //Search step tag
   if ((start = SearchToken(description_tag->str,"step,",0)))
     sscanf(start,"%d %d",&Desc->step_h, &Desc->step_v);

   //Search aspect tag
   if ((start = SearchToken(description_tag->str,"aspect,",0)))
     sscanf(start,"%f %f",&Desc->aspect_min, &Desc->aspect_max);

   //Search color tag
   if ((start = SearchToken(description_tag->str,"color,",0)))
     sscanf(start,"%d %d %d %d",&Desc->color_r, &Desc->color_g, &Desc->color_b, &Desc->color_a);

   //Search color2 tag
   if ((start = SearchToken(description_tag->str,"color2,",0)))
     sscanf(start,"%d %d %d %d",&Desc->color2_r, &Desc->color2_g, &Desc->color2_b, &Desc->color2_a);

   //Search color3 tag
   if ((start = SearchToken(description_tag->str,"color3,",0)))
     sscanf(start,"%d %d %d %d",&Desc->color3_r, &Desc->color3_g, &Desc->color3_b, &Desc->color3_a);

   //Search inherit tag
   if ((start = SearchToken(description_tag->str,"inherit,",0))){
      Desc->inherit = GetString(start);
      start = strchr(start,'"')+1;	//Skip string
      start = strchr(start,'"')+1;
      sscanf(start,"%f",&Desc->inherit_index);	//Get state index
   }

   //Search rel1 tag
   if ((rel = SearchToken(description_tag->str,"rel1{",0))){
      end = SearchMatchingBra(rel-1);
      len = end - rel;
      //Search rel1 relative tag
      if ((start = SearchToken(rel,"relative,",len))){
	 sscanf(start,"%f %f",&Desc->rel1_relative_x,&Desc->rel1_relative_y);
      }
      //Search rel1 offset tag
      if ((start = SearchToken(rel,"offset,",len)))
	sscanf(start,"%d %d",&Desc->rel1_offset_x,&Desc->rel1_offset_y);
      //Search rel1_to tag
      if ((start = SearchToken(rel,"to,",len)))
	Desc->rel1_to = GetString(start);
      //Search rel1_to_x tag
      if ((start = SearchToken(rel,"to_x,",len)))
	Desc->rel1_to_x = GetString(start);
      //Search rel1_to_y tag
      if ((start = SearchToken(rel,"to_y,",len)))
	Desc->rel1_to_y = GetString(start);
   }
   //Search rel2 tag
   if ((rel = SearchToken(description_tag->str,"rel2{",0))){
      end = SearchMatchingBra(rel-1);
      len = end - rel;
      //Search rel2 relative tag
      if ((start = SearchToken(rel,"relative,",len)))
	sscanf(start,"%f %f",&Desc->rel2_relative_x,&Desc->rel2_relative_y);
      //Search rel2 offset tag
      if ((start = SearchToken(rel,"offset,",len)))
	sscanf(start,"%d %d",&Desc->rel2_offset_x,&Desc->rel2_offset_y);
      //Search rel2_to tag
      if ((start = SearchToken(rel,"to,",len)))
	Desc->rel2_to = GetString(start);
      //Search rel2_to_x tag
      if ((start = SearchToken(rel,"to_x,",len)))
	Desc->rel2_to_x = GetString(start);
      //Search rel2_to_y tag
      if ((start = SearchToken(rel,"to_y,",len)))
	Desc->rel2_to_y = GetString(start);
   }
   //Search image tag
   if ((image = SearchToken(description_tag->str,"image{",0))){
      end = SearchMatchingBra(image-1);
      len = end - image;
      //Search image_normal tag
      if ((start = SearchToken(image,"normal,",len)))
	Desc->image_normal = GetString(start);
      //Search image_tween tags
      start = image;
      while ((start = SearchToken(start,"tween,",len))){
	 Desc->image_tween = g_list_append(Desc->image_tween,GetString(start));
      }

      //Search image border tag
      if ((start = SearchToken(image,"border,",len))){
	 sscanf(start,"%d %d %d %d",&Desc->image_border_left,&Desc->image_border_right,&Desc->image_border_top,&Desc->image_border_bottom);
      }

   }
   //TODO Search fill tag

   //Search text tag
   if ((text = SearchToken(description_tag->str,"text{",0))){
      end = SearchMatchingBra(text-1);
      len = end - text;
      //Search text_text tag
      if ((start = SearchToken(text,"text,",len)))
	Desc->text_text = GetString(start);
      //Search text_font tag
      if ((start = SearchToken(text,"font,",len)))
	Desc->text_font = GetString(start);
      //Search text_size tag
      if ((start = SearchToken(text,"size,",len)))
	sscanf(start,"%d",&Desc->text_size);
      //Search text_fit tag
      if ((start = SearchToken(text,"fit,",len)))
	sscanf(start,"%d %d",&Desc->text_fit_h,&Desc->text_fit_v);
      //Search text_min tag
      if ((start = SearchToken(text,"min,",len)))
	sscanf(start,"%d %d",&Desc->text_min_h,&Desc->text_min_v);
      //Search text_align tag
      if ((start = SearchToken(text,"align,",len)))
	sscanf(start,"%f %f",&Desc->text_align_h,&Desc->text_align_v);
   }

	/* printf("************************\n");
	printf("State: %s - %f\n",Desc->state->str,Desc->state_index);
	printf("Visible: %d\n",Desc->visible);
	printf("Align: %.2f - %.2f\n",Desc->align_h,Desc->align_v);
	printf("Min: %d - %d\n",Desc->min_x,Desc->min_y);
	printf("Max: %d - %d\n",Desc->max_x,Desc->max_y);
	printf("Step: %d - %d\n",Desc->step_x,Desc->step_y);
	printf("Aspect: %.2f - %.2f\n",Desc->aspect_min,Desc->aspect_max);
	printf("Color: %d - %d - %d - %d\n",Desc->color_r,Desc->color_g,Desc->color_b,Desc->color_a);
	printf("Color2: %d - %d - %d - %d\n",Desc->color2_r,Desc->color2_g,Desc->color2_b,Desc->color2_a);
	printf("Color3: %d - %d - %d - %d\n",Desc->color3_r,Desc->color3_g,Desc->color3_b,Desc->color3_a);
	printf("Inherit: %s - %.2f\n",Desc->inherit->str,Desc->inherit_index);
	printf("Rel1 relative: %.2f - %.2f offset: %d - %d\n",Desc->rel1_relative_x,Desc->rel1_relative_y,Desc->rel1_offset_x,Desc->rel1_offset_y);
	printf("Rel1 to: %s to_x: %s to_y: %s\n",Desc->rel1_to->str,Desc->rel1_to_x->str,Desc->rel1_to_y->str);
	printf("Rel2 relative: %.2f - %.2f offset: %d - %d\n",Desc->rel2_relative_x,Desc->rel2_relative_y,Desc->rel2_offset_x,Desc->rel2_offset_y);
	printf("Rel2 to: %s to_x: %s to_y: %s\n",Desc->rel2_to->str,Desc->rel2_to_x->str,Desc->rel2_to_y->str);
	printf("Image normal: %s\n",Desc->image_normal->str);
	printf("Image tweens: %s\n",Desc->image_tween->str);
	printf("Image border: %d - %d - %d - %d\n",Desc->image_border_left,Desc->image_border_right,Desc->image_border_top,Desc->image_border_bottom);
	printf("Text text: %s\n",Desc->text_text->str);
	printf("Text font: %s - %d\n",Desc->text_font->str,Desc->text_size);
	printf("Text fit: %d - %d\n",Desc->text_fit_h,Desc->text_fit_v);
	printf("Text min: %d - %d\n",Desc->text_min_h,Desc->text_min_v);
	printf("Text align: %.2f - %.2f\n",Desc->text_align_h,Desc->text_align_v);  */

   return Desc;
}
int ParsePartsFromGroup(EDC_Group* group){
   gchar* start=NULL;
   gchar* end=NULL;
   GString* ginfo;
   GString* part;

   //printf("GET PARTS: %s\n\n",group->complete_tag->str);

	/* Read group information (name,min,max) */
   start = group->complete_tag->str;
   end = strstr(start,"parts{");
   ginfo = g_string_new_len(start,end-start);
   //printf("GROUP INFO: %s\n",ginfo->str);

   //Search name in information
   if (!(start = SearchToken(ginfo->str,"name,",0))){
      printf(_("Parsing error: Can't find group name\n"));
      return FALSE;
   }
   group->name = GetString(start);

   //Search min in information
   if ((start = SearchToken(ginfo->str,"min,",0)))
     sscanf(start,"%d %d",&group->min_x,&group->min_y);
   //Search max in information
   if ((start = SearchToken(ginfo->str,"max,",0)))
     sscanf(start,"%d %d",&group->max_x,&group->max_y);

   //printf("NAME: %s\nMIN: %d - %d\nMAX: %d - %d\n",group->name->str,group->min_x,group->min_y,group->max_x,group->max_y);

	/* Read all parts*/
   start = SearchToken(group->complete_tag->str,"parts{",0);

   part = g_string_new("");
   while ((start = SearchToken(start,"part{",0))){
      end = SearchMatchingBra(start-1);
      part = g_string_truncate(part,0);
      part = g_string_append_len(part,start,end-start);
      //group->parts = g_list_append(group->parts,ParsePart(part,group));
      ParsePart(part,group);

      start = end;
   }
   g_string_free(part,TRUE);

   return TRUE;
}
