#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <Edje.h>
#include <Etk.h>

#include "parse.h"
#include "evas.h"
#include "interface.h"
#include "main.h"

/* Object creation and destrucion */
EDC_Description*	EDC_Description_new(EDC_Part *parent_part, char* state, float state_index){
   EDC_Description*	description;

   description = g_new(EDC_Description,1);

   description->state = g_string_new(state);
   description->state_index = state_index;

   description->visible = 1;
   description->align_h = description->align_v = 0.0;
   description->min_w = description->min_h = 0;
   description->max_w = description->max_h = 0;
   description->step_h = description->step_v = 0;
   description->aspect_min = description->aspect_max = 0.0;
   description->inherit=g_string_new("");
   description->inherit_index=0.0;

   description->color_r = description->color_g = description->color_b = description->color_a = 255;
   description->color2_r = description->color2_g = description->color2_b = description->color2_a = 255;
   description->color3_r = description->color3_g = description->color3_b = description->color3_a = 255;

   description->rel1_relative_x = description->rel1_relative_y = 0.1;
   description->rel1_offset_x = description->rel1_offset_y = 0.1;
   description->rel1_to = g_string_new("");
   description->rel1_to_x = g_string_new("");
   description->rel1_to_y = g_string_new("");

   description->rel2_relative_x = description->rel2_relative_y = 0.5;
   description->rel2_offset_x = description->rel2_offset_y = 0.5;
   description->rel2_to = g_string_new("");
   description->rel2_to_x = g_string_new("");
   description->rel2_to_y = g_string_new("");

   description->image_normal = g_string_new("");
   description->image_tween = NULL;

   description->image_border_left = description->image_border_right = description->image_border_top = description->image_border_bottom = 0;

   description->text_text = g_string_new("Insert your text");
   description->text_font = g_string_new("");
   description->text_size = 25;
   description->text_fit_h = description->text_fit_v = 0;
   description->text_min_h = description->text_min_v = 0;
   description->text_align_h = description->text_align_v = 0;

   description->tree_row = NULL;

   description->part = parent_part;
   AddDescToTree(description);
   return description;
}

void 			EDC_Description_clear(EDC_Description *desc,int skip_parent_modify){
   printf("		Clear Description: %s\n",desc->state->str);

   //Remove the row from the main tree
   etk_tree2_row_delete(desc->tree_row);

   //Remove from the parent descriptions list
   if (!skip_parent_modify)
     desc->part->descriptions = g_list_remove (desc->part->descriptions,desc);

   //TODO Remove all the named link to the description (inherit)

   //Free GStrings
   g_string_free(desc->state,TRUE);
   g_string_free(desc->inherit,TRUE);
   g_string_free(desc->rel1_to,TRUE);
   g_string_free(desc->rel1_to_x,TRUE);
   g_string_free(desc->rel1_to_y,TRUE);
   g_string_free(desc->rel2_to,TRUE);
   g_string_free(desc->rel2_to_x,TRUE);
   g_string_free(desc->rel2_to_y,TRUE);
   g_string_free(desc->image_normal,TRUE);

   while (desc->image_tween){
      g_string_free(desc->image_tween->data,TRUE);
      desc->image_tween = g_list_next(desc->image_tween);
   }
   g_list_free(desc->image_tween);
   desc->image_tween = NULL;

   g_string_free(desc->text_text,TRUE);
   g_string_free(desc->text_font,TRUE);

   //Free the object
   g_free(desc);
}
EDC_Group*		EDC_Group_new(char* name,int min_x, int min_y, int max_x, int max_y){
   EDC_Group*	group;

   group = g_new(EDC_Group,1);

   group->name=g_string_new(name);
   group->min_x = min_x;
   group->min_y = min_y;
   group->max_x = max_x;
   group->max_y = max_y;
   group->parts = NULL;

   EDC_Group_list = g_list_append(EDC_Group_list,group);

   //printf("**NEW GROUP  %s\n\n",group->name->str);
   AddGroupToTree(group);
   return group;
}

void 			EDC_Group_clear(EDC_Group *group,int skip_parent_modify){
   GList *p;		//Parts Pointer
   EDC_Part *part;
   printf("Clear Group: %s\n",group->name->str);

   //Clear all parts in group
   p = group->parts;
   while (p){
      part = p->data;
      //printf("	part: %s\n",part->name->str);
      EDC_Part_clear(part,TRUE);
      p = g_list_next(p);
   }
   g_list_free(group->parts);
   
   //If this is the selected group then  NULL it
   if (selected_group == group) selected_group = NULL;

   //Remove the row from the main tree
   etk_tree2_row_delete (group->tree_row);

   if (!skip_parent_modify){
      EDC_Group_list = g_list_remove(EDC_Group_list,group);
   }

   //Clear GStrings
   //g_string_free(group->complete_tag,TRUE);
   g_string_free(group->name,TRUE);

   //Free the object
   g_free(group);

}
EDC_Part*		EDC_Part_new(EDC_Group *parent_group,char* name, int type){
   EDC_Part*	part;
   //printf("New Part: %s\n",name);
   part = g_new(EDC_Part,1);
   part->name = g_string_new(name);
   part->type = type;
   part->descriptions = NULL;
   part->current_description = NULL;
   part->ev_obj = NULL;
   part->clip_to = g_string_new("");
   part->color_class = g_string_new("");
   part->text_class = g_string_new("");
   part->group = parent_group;
   part->realx = part->realy = part->realw = part->realh = 10;
   part->mouse_events = part->repeat_events = 0;
   parent_group->parts = g_list_append(parent_group->parts,part);
   AddPartToTree(part);
   return part;
}
void 			EDC_Part_clear(EDC_Part *part, int skip_parent_modify){
   GList *p;
   GList *dp;
   EDC_Description *desc;

   printf("	Clear Part: %s\n",part->name->str);

   //Clear all the descriptions and the descriptions list
   p = part->descriptions;
   while (p){
      EDC_Description_clear(p->data,skip_parent_modify);

      p = g_list_next(p);
   }
   g_list_free(part->descriptions);

   //g_string_free(part->clip_to,TRUE);

   //Remove the row from the main tree
   etk_tree2_row_delete (part->tree_row);

   //Remove the evas object
   etk_canvas_object_remove(ETK_CANVAS(ETK_canvas),part->ev_obj);
   evas_object_del(part->ev_obj);

   if (!skip_parent_modify){
      //Remove from the parent parts list
      part->group->parts = g_list_remove(part->group->parts,part);

      //Remove all the named link to the object
      p = part->group->parts;
      while (p){
	 dp = ((EDC_Part*)(p->data))->descriptions;
	 if (0 == strcmp(part->name->str,((EDC_Part*)(p->data))->clip_to->str)) g_string_truncate(((EDC_Part*)(p->data))->clip_to,0);	//TODO controllare questa riga se funziona ...     //clear clip_to in all the parts that clip to the part to remove
	 while (dp){
	    desc = dp->data;
	    if (0 == strcmp(part->name->str,desc->rel1_to->str)) g_string_truncate(desc->rel1_to,0);
	    if (0 == strcmp(part->name->str,desc->rel1_to_x->str)) g_string_truncate(desc->rel1_to_x,0);
	    if (0 == strcmp(part->name->str,desc->rel1_to_y->str)) g_string_truncate(desc->rel1_to_y,0);
	    if (0 == strcmp(part->name->str,desc->rel2_to->str)) g_string_truncate(desc->rel2_to,0);
	    if (0 == strcmp(part->name->str,desc->rel2_to_x->str)) g_string_truncate(desc->rel2_to_x,0);
	    if (0 == strcmp(part->name->str,desc->rel2_to_y->str)) g_string_truncate(desc->rel2_to_y,0);
	    dp = g_list_next(dp);
	 }
	 p = g_list_next(p);
      }
   }

   //Clear GStrings
   g_string_free(part->name,TRUE);
   g_string_free(part->clip_to,TRUE);
   g_string_free(part->color_class,TRUE);
   g_string_free(part->text_class,TRUE);

   //Free the object
   g_free(part);

}

/* Various function */
void CreateBlankEDC(void){
   printf("Create Blank EDC\n");

   EDC_Group_list = NULL;
   selected_group = NULL;
   selected_part = NULL;
   selected_desc = NULL;

}
void ClearAll(void){
   GList *p;			//Groups Pointer
   EDC_Group *group;

   printf("Clear All\n");

   p = EDC_Group_list;
   while (p){
      group = p->data;
      EDC_Group_clear(group, TRUE);

      p = g_list_next(p);
   }
   g_list_free(EDC_Group_list);
   EDC_Group_list = NULL;

   selected_group = NULL;
   selected_part = NULL;
   selected_desc = NULL;
}
EDC_Part* GetPartByName(char* part_name){
   EDC_Part	*part = NULL;
   GList*	p_list;
   //printf("GetPartByName: %s\n",part_name);

   p_list = selected_group->parts;

   while(p_list){
      part = p_list->data;
      if (strcmp(part_name,part->name->str) == 0) return part;
      //printf("%s, ",part->name->str);
      p_list = g_list_next(p_list);
   }
   return NULL;
}
void PrintDebugInformation(int show_groups_list){
   EDC_Group *group;
   EDC_Part *part;
   EDC_Description *desc;
   GList *gp,*pp,*dp;

   printf("\n\n********************* D E B U G ***************************\n");
   printf("Current edje_editor.edj: %s\n",EdjeFile->str);
   printf("Current EDCFile: %s\n",EDCFile->str);
   printf("Current EDCFileDir: %s\n",EDCFileDir->str);
   printf("Total group in EDC: %d\n",g_list_length(EDC_Group_list));
   printf("Selected Group:	 ");if (selected_group) printf("%s\n", selected_group->name->str);else printf("none\n");
   printf("Selected Part:	 ");if (selected_part) printf("%s\n", selected_part->name->str);else printf("none\n");
   printf("Selected Desc:	 ");if (selected_desc) printf("%s %.2f\n\n", selected_desc->state->str,selected_desc->state_index);else printf("none\n\n");

   if (show_groups_list){
      gp = EDC_Group_list;
      while(gp){
	 group = gp->data;
	 printf("Group: %s (%d parts)\n",group->name->str,g_list_length(group->parts));

	 pp = group->parts;
	 while(pp){
	    part = pp->data;
	    printf("	part: %s (%d descs) [parent: %s]\n",part->name->str,g_list_length(part->descriptions),part->group->name->str);

	    dp = part->descriptions;
	    while(dp){
	       desc = dp->data;
	       printf("		state: %s %.2f [parent: %s]\n",desc->state->str,desc->state_index,desc->part->name->str);
	       dp = g_list_next(dp);
	    }
	    pp = g_list_next(pp);
	 }

	 gp = g_list_next(gp);
      }
   }

   printf("*********************** E N D *****************************\n\n");
}
int OpenEDC(const gchar* EDCfile){
   gchar*	EDC;
   gsize	lenght;
   char buf[4096];
   char tmpn[4096];
   int fd,ret;
	
   /*
	 * Run the input through the C pre-processor.
	 *
	 * On some BSD based systems (MacOS, OpenBSD), the default cpp
	 * in the path is a wrapper script that chokes on the -o option.
	 * If the preprocessor is invoked via gcc -E, it will treat
	 * file_in as a linker file. The safest route seems to be to
	 * run cpp with the output as the second non-option argument.
	 * 
	 * Redirecting the output is required for MacOS 10.3, and works fine
	 * on other systems.
	 */
   //create tmp file
   strcpy(tmpn, "/tmp/edje_editor-tmp-XXXXXX");
   fd = mkstemp(tmpn);
   if (!fd) return 0;
   close(fd);
   
   
   //Run the input through the C pre-processor.  //TODO check on other systems
	snprintf(buf, sizeof(buf), "cat %s | cpp -I%s > %s",EDCfile,g_path_get_dirname(EDCfile), tmpn);
	ret = system(buf);
	if (ret < 0){
	     snprintf(buf, sizeof(buf), "gcc -E -o %s %s", tmpn, EDCfile);
	     ret = system(buf);
	  }
	if (ret != EXIT_SUCCESS) return 0;
   printf("Preprocessor command: %s\n",buf);

	
   //Read preprocessed file
   if (!g_file_get_contents (tmpn,&EDC,&lenght, NULL)){
      printf("Error reading edc file: %s\n",EDCfile);
      return FALSE;
   }
   unlink(tmpn);
   
   if (!ParseEDC(EDC)) return FALSE;

   EDCFile = g_string_new(EDCfile);
   EDCFileDir = g_string_new(g_path_get_dirname(EDCFile->str));

   UpdateWindowTitle();
   UpdateFontComboBox();
   UpdateImageComboBox();

   return TRUE;
}
void SaveEDC(char *save_as){
   EDC_Group *group;
   EDC_Part *part;
   EDC_Description *desc;
   GList *gp,*pp,*dp,*tp,*ip;
   GList *IM_List=NULL; //list of char*
   GList *FO_List = NULL; 	//font list of char
   GString *EDC = g_string_new("");
   GString *EDCImages = g_string_new("");
   GString *EDCFonts = g_string_new("");

   if (save_as){
      g_string_printf(EDCFile,"%s",save_as);
      g_string_printf(EDCFileDir,"%s",g_path_get_dirname(save_as));
      UpdateFontComboBox();
      UpdateImageComboBox();
   }

   if (!EDCFile->len > 0){
      printf("SaveEDC: %s\n",EDCFile->str);
      ShowFilechooser(FILECHOOSER_SAVE_AS);
      return;
   }
	/* this function add a string to a list of strings only if it's not already in */
   GList* add_str_to_list(char* str , GList* list){
      ip = list;
      while (ip){
         if (0 == strcmp(ip->data,str)) return list;
         ip = g_list_next(ip);
      }
      list = g_list_append(list,str);
      return list;
   }

   EDC = g_string_append(EDC,"collections{\n");

	/* TODO Controllare tutti i dati in memoria prima di scrivere
		-2 parti con lo stesso nome
	*/
	/* COLLECTIONS */
   gp = EDC_Group_list;
   while(gp){
      group = gp->data;
      //Write Group information
      EDC = g_string_append(EDC,IN1"group{\n");
      g_string_append_printf(EDC,IN2"name, \"%s\";\n",group->name->str);
      if (group->min_x || group->min_y) g_string_append_printf(EDC,IN2"min, %d %d;\n",group->min_x,group->min_y);
      if (group->max_x || group->max_y) g_string_append_printf(EDC,IN2"max, %d %d;\n",group->max_x,group->max_y);

      EDC = g_string_append(EDC,IN2"parts{\n");
      pp = group->parts;
      while(pp){
	 part = pp->data;
	 //Write Part information
	 EDC = g_string_append(EDC,IN3"part{\n");
	 g_string_append_printf(EDC,IN4"name, \"%s\";\n",part->name->str);
	 if (part->type == PART_TYPE_RECT) EDC = g_string_append(EDC,IN4"type, RECT;\n");
	 if (part->type == PART_TYPE_IMAGE) EDC = g_string_append(EDC,IN4"type, IMAGE;\n");
	 if (part->type == PART_TYPE_TEXT) EDC = g_string_append(EDC,IN4"type, TEXT;\n");
	 if (part->type == NONE) EDC = g_string_append(EDC,IN4"type, NONE;\n");
	 if (part->effect == FX_OUTLINE) EDC = g_string_append(EDC,IN4"effect, OUTLINE;\n");
	 if (part->effect == FX_SOFT_OUTLINE) EDC = g_string_append(EDC,IN4"effect, SOFT_OUTLINE;\n");
	 if (part->effect == FX_SHADOW) EDC = g_string_append(EDC,IN4"effect, SHADOW;\n");
	 if (part->effect == FX_SOFT_SHADOW) EDC = g_string_append(EDC,IN4"effect, SOFT_SHADOW;\n");
	 if (part->effect == FX_OUTLINE_SHADOW) EDC = g_string_append(EDC,IN4"effect, OUTLINE_SHADOW;\n");
	 if (part->effect == FX_OUTLINE_SOFT_SHADOW) EDC = g_string_append(EDC,IN4"effect, OUTLINE_SOFT_SHADOW;\n");
	 g_string_append_printf(EDC,IN4"mouse_events, %d;\n",part->mouse_events);//TODO controllare il falore di default di mouse_events e scrivere la riga solo in caso contrario (DA CONTROLLARE ANCHE IN LETTURA DEL FILE)
	 g_string_append_printf(EDC,IN4"repeat_events, %d;\n",part->repeat_events);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	 if (part->clip_to->len > 0) g_string_append_printf(EDC,IN4"clip_to, \"%s\";\n",part->clip_to->str);

	 dp = part->descriptions;
	 while(dp){
	    desc = dp->data;
	    //Write description
	    EDC = g_string_append(EDC,IN4"description{\n");
	    g_string_append_printf(EDC,IN5"state, \"%s\" %.3f;\n",desc->state->str,desc->state_index);
	    g_string_append_printf(EDC,IN5"visible, %d;\n",desc->visible);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	    g_string_append_printf(EDC,IN5"align, %.3f %.3f;\n",desc->align_h, desc->align_v);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	    if (desc->min_w || desc->min_h) g_string_append_printf(EDC,IN5"min, %d %d;\n",desc->min_w, desc->min_h);
	    if (desc->max_w || desc->max_h) g_string_append_printf(EDC,IN5"max, %d %d;\n",desc->max_w, desc->max_h);
	    if (desc->step_h > 0 || desc->step_v > 0) g_string_append_printf(EDC,IN5"step, %d %d;\n",desc->step_h, desc->step_v);
	    if (desc->aspect_min > 0 || desc->aspect_max > 0) g_string_append_printf(EDC,IN5"aspect, %.3f %.3f;\n",desc->aspect_min, desc->aspect_max);
	    g_string_append_printf(EDC,IN5"color, %d %d %d %d;\n",desc->color_r, desc->color_g, desc->color_b, desc->color_a);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	    if (desc->inherit->len > 0) g_string_append_printf(EDC,IN5"inherit, \"%s\" %.3f;\n",desc->inherit->str, desc->inherit_index);
	    //rel1
	    g_string_append_printf(EDC,IN5"rel1 {relative, %.3f %.3f; ",desc->rel1_relative_x, desc->rel1_relative_y);//TODO se non metto rel1 e rel 2 cosa fa edje?? cosa devo fare io??
	    if (desc->rel1_offset_x != 0 || desc->rel1_offset_y != 0) g_string_append_printf(EDC,"offset, %d %d; ",desc->rel1_offset_x, desc->rel1_offset_y);
	    if (desc->rel1_to_x->len > 0) g_string_append_printf(EDC,"to_x, \"%s\";",desc->rel1_to_x->str);
	    if (desc->rel1_to_y->len > 0) g_string_append_printf(EDC,"to_y, \"%s\";",desc->rel1_to_y->str);
	    EDC = g_string_append(EDC,"}\n");//close rel1
	    //rel2
	    g_string_append_printf(EDC,IN5"rel2 {relative, %.3f %.3f; ",desc->rel2_relative_x, desc->rel2_relative_y);
	    if (desc->rel2_offset_x != 0 || desc->rel2_offset_y != 0) g_string_append_printf(EDC,"offset, %d %d; ",desc->rel2_offset_x, desc->rel2_offset_y);
	    if (desc->rel2_to_x->len > 0) g_string_append_printf(EDC,"to_x, \"%s\";",desc->rel2_to_x->str);
	    if (desc->rel2_to_y->len > 0) g_string_append_printf(EDC,"to_y, \"%s\";",desc->rel2_to_y->str);
	    EDC = g_string_append(EDC,"}\n");//close rel2
	    //image
	    if (desc->image_normal->len > 0 || desc->image_tween){
	       EDC = g_string_append(EDC,IN5"image{\n");
	       if (desc->image_normal->len > 0){
		  g_string_append_printf(EDC,IN6"normal, \"%s\";\n",desc->image_normal->str);
		  IM_List = add_str_to_list(desc->image_normal->str,IM_List);
	       }
	       if (desc->image_border_left != 0 || desc->image_border_right != 0 || desc->image_border_top != 0 || desc->image_border_bottom !=0)
		 g_string_append_printf(EDC,IN6"border, %d %d %d %d;\n",desc->image_border_left,desc->image_border_right,desc->image_border_top,desc->image_border_bottom);
	       if (desc->image_tween){
		  tp = desc->image_tween;
		  while (tp){
		     g_string_append_printf(EDC,IN6"tween, \"%s\";\n",((GString*)(tp->data))->str);
		     IM_List = add_str_to_list(((GString*)(tp->data))->str,IM_List);
		     tp = g_list_next(tp);
		  }
	       }
	       EDC = g_string_append(EDC,IN5"}\n");//close image
	    }
	    //TODO fill
	    //text
	    if (part->type == PART_TYPE_TEXT){
	       g_string_append_printf(EDC,IN5"color3, %d %d %d %d;\n",desc->color2_r, desc->color2_g, desc->color2_b, desc->color2_a);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	       g_string_append_printf(EDC,IN5"color2, %d %d %d %d;\n",desc->color3_r, desc->color3_g, desc->color3_b, desc->color3_a);//TODO controllare il falore di default di repeat_events e scrivere la riga solo in caso contrario
	       EDC = g_string_append(EDC,IN5"text{\n");
	       if (desc->text_text->len > 0) g_string_append_printf(EDC,IN6"text, \"%s\";\n",desc->text_text->str);
	       if (desc->text_font->len > 0){
		  g_string_append_printf(EDC,IN6"font, \"%s\";\n",desc->text_font->str);
		  FO_List = add_str_to_list(desc->text_font->str,FO_List);
	       }
	       if (desc->text_size > 0) g_string_append_printf(EDC,IN6"size, %d;\n",desc->text_size);
	       if (desc->text_fit_h > 0 || desc->text_fit_v > 0) g_string_append_printf(EDC,IN6"fit, %d %d;\n",desc->text_fit_h,desc->text_fit_v);
	       if (desc->text_min_h != 0.5 || desc->text_min_v != 0.5) g_string_append_printf(EDC,IN6"min, %d %d;\n",desc->text_min_h,desc->text_min_v);
	       if (desc->text_align_h > 0 || desc->text_align_v > 0) g_string_append_printf(EDC,IN6"align, %.3f %.3f;\n",desc->text_align_h,desc->text_align_v);

	       EDC = g_string_append(EDC,IN5"}\n");
	    } 

	    EDC = g_string_append(EDC,IN4"}\n"); //close description
	    dp = g_list_next(dp);
	 }
	 EDC = g_string_append(EDC,IN3"}\n");	//close part
	 pp = g_list_next(pp);
      }
      EDC = g_string_append(EDC,IN2"}\n");		//close parts
      EDC = g_string_append(EDC,IN1"}\n");		//close group
      gp = g_list_next(gp);
   }
   EDC = g_string_append(EDC,"}\n");			//close collections

	/* FONTS */
   ip = FO_List;
   EDCFonts = g_string_append(EDCFonts,"fonts{\n");
   while (ip){
      g_string_append_printf(EDCFonts,IN1"font, \"%s\" \"%s\";\n",(gchar*)ip->data,(gchar*)ip->data);
      ip = g_list_next(ip);
   }
   EDCFonts = g_string_append(EDCFonts,"}\n");	//close fonts
   //put the image tag at the beginning of the EDC
   EDC = g_string_prepend(EDC,EDCFonts->str);

	/* IMAGES */
   ip = IM_List;
   EDCImages = g_string_append(EDCImages,"images{\n");
   while (ip){
      g_string_append_printf(EDCImages,IN1"image, \"%s\" LOSSY 90;\n",(gchar*)ip->data);
      ip = g_list_next(ip);
   }
   EDCImages = g_string_append(EDCImages,"}\n");	//close images
   //put the image tag at the beginning of the EDC
   EDC = g_string_prepend(EDC,EDCImages->str);

   //TODO check the return value
   g_file_set_contents(EDCFile->str,EDC->str,EDC->len,NULL);

   UpdateWindowTitle();
}
void PlayEDC(void){
   GString *cc;	//Compiler Command
   Etk_Textblock_Iter *iter;
   FILE *pipe;
   char buf[4096];

	/* Compiler Command */
   cc = g_string_new("");
   g_string_printf(cc,"cd \"%s\" && edje_cc -v \"%s\" 2>&1",EDCFileDir->str,EDCFile->str);
   printf("cc: %s\n",cc->str);

   //TODO remove edj file if exist

   etk_textblock_clear(ETK_TEXT_VIEW(UI_PlayTextView)->textblock);
   etk_widget_show(UI_PlayDialog);

   if (!(pipe = popen(cc->str, "r"))){
      printf("Error executing edje_cc\n");
      return;
   }

   while (fgets(buf, 4096, pipe) != NULL){
      iter = etk_text_view_cursor_get (ETK_TEXT_VIEW(UI_PlayTextView));
      etk_textblock_insert (ETK_TEXT_VIEW(UI_PlayTextView)->textblock, iter, buf, strlen(buf));
   }

   pclose(pipe);

   g_string_free(cc,TRUE);
}
void RenameDescription(EDC_Description* desc, char* new_name, float index){
   if ((strcmp("default",desc->state->str)) || desc->state_index){
      if (new_name) g_string_printf(desc->state,"%s",new_name);
      desc->state_index = index;
   }
   else{
      ShowAlert("You can't rename default 0.0");
   }
}
void RenamePart(EDC_Part* part, char* new_name){
   GList	*pp, *dp;
   EDC_Description *desc;
   printf("Rename Part: %s -> %s\n",part->name->str, new_name);

   //Update all the named links to the part (rel1_to_x, rel1_to_y, rel2_to_x, rel2_to_y)
   pp = part->group->parts;
   while (pp){
      dp = ((EDC_Part*)(pp->data))->descriptions;
      while (dp){
	 desc = dp->data;
	 if (0 == strcmp(desc->rel1_to_x->str,part->name->str)) g_string_printf(desc->rel1_to_x,"%s",new_name);
	 if (0 == strcmp(desc->rel1_to_y->str,part->name->str)) g_string_printf(desc->rel1_to_y,"%s",new_name);
	 if (0 == strcmp(desc->rel2_to_x->str,part->name->str)) g_string_printf(desc->rel2_to_x,"%s",new_name);
	 if (0 == strcmp(desc->rel2_to_y->str,part->name->str)) g_string_printf(desc->rel2_to_y,"%s",new_name);
	 dp = g_list_next(dp);
      }
      pp = g_list_next(pp);
   }

   //Change the name of the part
   g_string_printf(part->name,"%s",new_name);

   RecreateRelComboBoxes();

}
void FileCopy(char* source, char*dest){
   int c;
   GString	*str=g_string_new("");
   FILE *IPFile,*OPFile;

   //printf("COPY FROM: %s TO: %s/%s\n",source,dest,g_path_get_basename (source));
   g_string_printf(str,"%s/%s",dest,g_path_get_basename (source));

   // Open the file - no error checking done TODO error checking
   IPFile = fopen(source,"r");
   OPFile = fopen(str->str,"w");

   // Read one character at a time, checking for the End of File.
   while ((c = fgetc(IPFile)) != EOF){
      fputc(c, OPFile);
   }

   //close files
   fclose(IPFile);
   fclose(OPFile);
   g_string_free(str,TRUE);
}
/* main */
int main(int argc, char **argv){
   //Init Globals
   ETK_canvas = NULL;
   EDCFile = g_string_new("");
   EDCFileDir = g_string_new("");
   selected_group = NULL;
   selected_part = NULL;
   selected_desc = NULL;
   EDC_Group_list =NULL;

   //Init ETK
   if (!etk_init(&argc, &argv)){
      printf("Could not init etk. Exiting...\n");
      return 1;
   }

   EdjeFile = g_string_new(PACKAGE_DATA_DIR"/edje_editor.edj");

   //Create the etk window with all his widget
   create_main_window();

   //Create the evas objects needed by the canvas (fakewin, handlers)
   prepare_canvas();

   //Create a new black EDC in memory (with one group, one part and one description
   CreateBlankEDC();

   //Start main loop
   etk_main();
   etk_shutdown();

   return 0;
}

