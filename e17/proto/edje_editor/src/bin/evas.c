#include <glib.h>
#include <Etk.h> 
#include <Edje.h>
#include "main.h"
#include "interface.h"  
#include "evas.h"
 


void on_Drag(void *data, Evas_Object *o, const char *emission, const char *source){
	Evas_Coord 	x,y;
	Evas_Coord 	parentx,parenty,parentw,parenth;
	Evas_Coord 	focusx,focusy,focusw,focush;
	int mouse_x, mouse_y;
	
	/* Resize fake win */
	if ((int)data == DRAG_MINIARROW){
		edje_object_part_geometry_get (EV_fakewin,"miniarrow", &x, &y, NULL, NULL);
		
		if (selected_group->min_x && x < selected_group->min_x) x = selected_group->min_x;
		if (selected_group->min_y && y < selected_group->min_y) y = selected_group->min_y;
		if (selected_group->max_x && x > selected_group->max_x) x = selected_group->max_x;
		if (selected_group->max_y && y > selected_group->max_y) y = selected_group->max_y;
		
		ev_resize_fake((int)x,(int)y);
	}
	/* Move Rel1 Point */
	if ((int)data == DRAG_REL1){
		evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
		evas_object_move(rel1_handler,mouse_x-2,mouse_y-2);
		
		//Get the position of the rel1 point 
		//edje_object_part_geometry_get (rel1_handler,"rel1", &x, &y, NULL, NULL);
		evas_object_geometry_get (rel1_handler, &x, &y, NULL, NULL);
		
		//Get the position of the focus handler
		evas_object_geometry_get (focus_handler, &focusx, &focusy, &focusw, &focush);
		
		//Get the position of the top parent handler
		evas_object_geometry_get (rel1X_parent_handler, &parentx, &parenty, &parentw, &parenth);
		
		//Calc the position of the rel1 point from the main interface
		//x+=focusx;
		//y+=focusy;
		
		//Calc the rel1_relative_x value
		selected_desc->rel1_relative_x = (float)(x - parentx)/(float)parentw;
		
		
		//Get the position of the left parent handler
		evas_object_geometry_get (rel1Y_parent_handler, &parentx, &parenty, &parentw, &parenth);
		
		//Calc the rel1_relative_y value
		selected_desc->rel1_relative_y = (float)(y - parenty)/(float)parenth;
		
		//printf("DRAG REL1 ( %d %d ) [top: %d %d %d %d] [focus: %d %d %d %d]\n",x,y,parentx,parenty,parentw,parenth,focusx,focusy,focusw,focush);
		//printf("REL1X: %.2f\n",selected_desc->rel1_relative_x);
		
		UpdatePositionFrame();
	}
	/* Move Rel2 Point */
	if ((int)data == DRAG_REL2){
		evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
		evas_object_move(rel2_handler,mouse_x-2,mouse_y-2);
		
		//Get the position of the rel2 point 
		evas_object_geometry_get (rel2_handler, &x, &y, NULL, NULL);
		
		//Get the position of the focus handler
		evas_object_geometry_get (focus_handler, &focusx, &focusy, &focusw, &focush);
		
		//Get the position of the bottom parent handler
		evas_object_geometry_get (rel2X_parent_handler, &parentx, &parenty, &parentw, &parenth);
		//Calc the rel1_relative_x value
		selected_desc->rel2_relative_x = (float)(x - parentx)/(float)parentw;
		
		
		//Get the position of the right parent handler
		evas_object_geometry_get (rel2Y_parent_handler, &parentx, &parenty, &parentw, &parenth);
		//Calc the rel2_relative_y value
		selected_desc->rel2_relative_y = (float)(y - parenty)/(float)parenth;
		
		printf("DRAG REL1 ( %d %d ) [top: %d %d %d %d] [focus: %d %d %d %d]\n",x,y,parentx,parenty,parentw,parenth,focusx,focusy,focusw,focush);
		printf("REL1X: %.2f\n",selected_desc->rel1_relative_x);
		
		UpdatePositionFrame();
	}
	
	ev_draw_all();
	
	
	//printf("Drag %s - ",source);
	//printf("Position %i, %i\n", x, y);
	//Evas_Coord mouse_x, mouse_y;
	//evas_pointer_canvas_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
	//printf("Mouse is at canvas position %f, %f\n", mouse_x, mouse_y);
}
void on_StartDrag(void *data, Evas_Object *o, const char *emission, const char *source){
	printf("Start Drag\n");
	//int mouse_x, mouse_y;
 
	//evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
	//printf("Mouse is at screen position %i, %i\n", mouse_x, mouse_y);
	
	if ((int)data == DRAG_REL1) edje_object_signal_callback_add(rel1_handler,"DRAG","rel1",on_Drag,(void*)DRAG_REL1);
	if ((int)data == DRAG_REL2) edje_object_signal_callback_add(rel2_handler,"DRAG","rel2",on_Drag,(void*)DRAG_REL2);
}
void on_EndDrag(void *data, Evas_Object *o, const char *emission, const char *source){
	printf("End Drag\n");
	if ((int)data == DRAG_REL1) edje_object_signal_callback_del(rel1_handler,"DRAG","rel1",on_Drag);
	if ((int)data == DRAG_REL2) edje_object_signal_callback_del(rel2_handler,"DRAG","rel2",on_Drag);
}

void prepare_canvas(void){
	//Create Focus Handler
	focus_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
        edje_object_file_set(focus_handler, EdjeFile->str, "FocusH");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), focus_handler);
	
	//Create Rel1 handler
	rel1_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
	edje_object_file_set(rel1_handler, EdjeFile->str, "Rel1H");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel1_handler);
	edje_object_signal_callback_add(rel1_handler,"START_DRAG","rel1",on_StartDrag,(void*)DRAG_REL1);
	edje_object_signal_callback_add(rel1_handler,"END_DRAG","rel1",on_EndDrag,(void*)DRAG_REL1);
	
	//Create Rel2 handler
	rel2_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
	edje_object_file_set(rel2_handler,EdjeFile->str, "Rel2H");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel2_handler);
	edje_object_signal_callback_add(rel2_handler,"START_DRAG","rel2",on_StartDrag,(void*)DRAG_REL2);
	edje_object_signal_callback_add(rel2_handler,"END_DRAG","rel2",on_EndDrag,(void*)DRAG_REL2);
	
	
	//Create Parent Handler
	rel1X_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
        edje_object_file_set(rel1X_parent_handler, EdjeFile->str, "Rel1X_ParentH");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel1X_parent_handler);
	
	rel1Y_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
        edje_object_file_set(rel1Y_parent_handler, EdjeFile->str, "Rel1Y_ParentH");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel1Y_parent_handler);
	
	rel2X_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
        edje_object_file_set(rel2X_parent_handler, EdjeFile->str, "Rel2X_ParentH");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel2X_parent_handler);
	
	rel2Y_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
        edje_object_file_set(rel2Y_parent_handler, EdjeFile->str, "Rel2Y_ParentH");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), rel2Y_parent_handler);
	

	// Create Fake win
	EV_fakewin = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(EV_fakewin, EdjeFile->str, "FakeWin");
	etk_canvas_object_add (ETK_CANVAS(ETK_canvas), EV_fakewin);
	//evas_object_show(EV_fakewin);
	
	edje_object_signal_callback_add(EV_fakewin,"DRAG","miniarrow",on_Drag,(void*)DRAG_MINIARROW);
	etk_canvas_object_move (ETK_CANVAS(ETK_canvas),EV_fakewin,20, 27);
	ev_resize_fake(200,200);
}


void ev_adjust_position(EDC_Description* desc, Evas_Object* ev_obj){
	int ParentX=0, ParentY=0, ParentW=0, ParentH=0;
	int P1X=0,P1Y=0,P2X=100,P2Y=100;
	EDC_Part *parent;
	
	if (ETK_canvas){
		//Calculate first point
		evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);	//Assume we take relativity from the main interface
		if (desc->rel1_to_x->len > 0){												//If rel1_to_x is found
			parent = GetPartByName(desc->rel1_to_x->str);							//Get the EDC_Part* named in the EDC
			ParentX = parent->realx;												//Set Parent X
			ParentW = parent->realw;												//Set Parent W
		}
		if (desc->rel1_to_y->len > 0){												//If rel1_to_x is found
			parent = GetPartByName(desc->rel1_to_y->str);							//Get the EDC_Part* named in the EDC
			ParentY = parent->realy;												//Set Parent Y
			ParentH = parent->realh;												//Set Parent H
		}
		
		P1X = ParentX + (ParentW * desc->rel1_relative_x) + desc->rel1_offset_x;			//Calculate relativity
		P1Y = ParentY + (ParentH * desc->rel1_relative_y) + desc->rel1_offset_y;			//
		
		//Calculate second point
		evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		if (desc->rel2_to_x->len > 0){
			parent = GetPartByName(desc->rel2_to_x->str);
			ParentX = parent->realx;
			ParentW = parent->realw;
		}
		if (desc->rel2_to_y->len > 0){
			parent = GetPartByName(desc->rel2_to_y->str);
			ParentY = parent->realy;
			ParentH = parent->realh;
		}
		P2X = ParentX + (ParentW * desc->rel2_relative_x) + desc->rel2_offset_x;
		P2Y = ParentY + (ParentH * desc->rel2_relative_y) + desc->rel2_offset_y;
		
		
		//Calculate real coord
		desc->part->realx = P1X;
		desc->part->realy = P1Y;
		
		desc->part->realw = P2X-P1X;
		desc->part->realh = P2Y-P1Y;
		
		//Reposition the object
		evas_object_move(ev_obj,desc->part->realx,desc->part->realy);
		evas_object_resize(ev_obj,desc->part->realw,desc->part->realh);
		if (desc->part->type == PART_TYPE_IMAGE){
			evas_object_image_fill_set(ev_obj,0,0,desc->part->realw,desc->part->realh); 
		}
		
	}
	
}
void ev_draw_part(EDC_Part* part){
	EDC_Description* descr=NULL;
	GString *str=g_string_new("");
	printf("***DRAW PART: %s\n",part->name->str);
	
	if (part->current_description)
		descr = part->current_description;
	else 
		descr = part->descriptions->data;
	
	if (descr){
		switch (part->type){
			case PART_TYPE_RECT:
				
				if (!part->ev_obj){	//Create RECT
					
					part->ev_obj = evas_object_rectangle_add  (etk_widget_toplevel_evas_get(ETK_canvas));   	
					etk_canvas_object_add (ETK_CANVAS(ETK_canvas), part->ev_obj);
					evas_object_color_set(part->ev_obj, descr->color_r, descr->color_g, descr->color_b, descr->color_a);
				
				}else{			//Modify RECT
					evas_object_color_set(part->ev_obj, descr->color_r, descr->color_g, descr->color_b, descr->color_a);
				}
			break;
			case PART_TYPE_IMAGE:
				if (!part->ev_obj){	//Create IMAGE
					part->ev_obj = evas_object_image_add  (etk_widget_toplevel_evas_get(ETK_canvas));
					etk_canvas_object_add (ETK_CANVAS(ETK_canvas), part->ev_obj);
				}
				g_string_printf(str,"%s/%s",EDCFileDir->str,descr->image_normal->str);
				evas_object_image_file_set(part->ev_obj, str->str, "");
				evas_object_color_set(part->ev_obj, descr->color_a, descr->color_a, descr->color_a, descr->color_a);
				evas_object_image_border_set(part->ev_obj,descr->image_border_left,descr->image_border_right,descr->image_border_top,descr->image_border_bottom);
				
			break;
			case PART_TYPE_TEXT:
				if (!part->ev_obj){	//Create TEXT
					part->ev_obj = evas_object_text_add  (etk_widget_toplevel_evas_get(ETK_canvas));
					etk_canvas_object_add (ETK_CANVAS(ETK_canvas), part->ev_obj);
				}
            evas_object_text_text_set(part->ev_obj,descr->text_text->str);
            g_string_printf(str,"%s/%s",EDCFileDir->str,descr->text_font->str);
            evas_object_text_font_set (part->ev_obj, str->str,descr->text_size);
            evas_object_color_set(part->ev_obj, descr->color_r, descr->color_g, descr->color_b, descr->color_a);
            evas_object_text_shadow_color_set(part->ev_obj, descr->color2_r, descr->color2_g, descr->color2_b, descr->color2_a);
            evas_object_text_outline_color_set(part->ev_obj, descr->color3_r, descr->color3_g, descr->color3_b, descr->color3_a);
            switch (part->effect){
               case FX_OUTLINE: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_OUTLINE); break;
               case FX_SOFT_OUTLINE: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_SOFT_OUTLINE); break;
               case FX_SHADOW: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_SHADOW); break;
               case FX_SOFT_SHADOW: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_SOFT_SHADOW); break;
               case FX_OUTLINE_SHADOW: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_OUTLINE_SHADOW); break;
               case FX_OUTLINE_SOFT_SHADOW: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW); break;
               default: evas_object_text_style_set(part->ev_obj, EVAS_TEXT_STYLE_PLAIN);break;
            }
			break;
		}
		
		if (part->ev_obj){
			ev_adjust_position(descr,part->ev_obj);
			evas_object_show(part->ev_obj);
		}
		
	}
	g_string_free(str,TRUE);
}
void ev_draw_focus(void){
	int ParentX,ParentY,ParentW,ParentH;
	EDC_Part	*parent;
	
	// If a part is selected draw the Focus Handler (only the yellow box)
	if (selected_part){
		evas_object_move (focus_handler, selected_part->realx-1, selected_part->realy-1);
		evas_object_resize(focus_handler,selected_part->realw+2,selected_part->realh+2);
		evas_object_raise (focus_handler);
		evas_object_show(focus_handler);
	}else{
		evas_object_hide(focus_handler);
	}
	
	// if a part description is selected draw also the parent handlers (the red and blue lines)
	if(selected_desc){
		//printf("Draw parent Handlers\n");
		
		//Draw rel1 & rel2 point
		evas_object_move (rel1_handler, selected_part->realx-2,selected_part->realy-2);
		evas_object_show(rel1_handler);
		evas_object_raise(rel1_handler);
		
		evas_object_move (rel2_handler, selected_part->realx+selected_part->realw-3,selected_part->realy+selected_part->realh-3);
		evas_object_show(rel2_handler);
		evas_object_raise(rel2_handler);
		
		
		//draw Rel1X_ParentH (top line)
		if (selected_desc->rel1_to_x->len > 0){
			//printf("REL1_TO_X: %s\n",selected_desc->rel1_to_x->str);
			parent = GetPartByName(selected_desc->rel1_to_x->str);
			evas_object_geometry_get (parent->ev_obj, &ParentX, &ParentY, &ParentW, &ParentH);
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel1X_parent_handler, ParentX,ParentY);
		evas_object_resize(rel1X_parent_handler,ParentW,2);
		
		//draw Rel1Y_ParentH (left line)
		if (selected_desc->rel1_to_y->len > 0){
			//printf("REL1_TO_Y: %s\n",selected_desc->rel1_to_y->str);
			parent = GetPartByName(selected_desc->rel1_to_y->str);
			evas_object_geometry_get (parent->ev_obj, &ParentX, &ParentY, &ParentW, &ParentH);
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel1Y_parent_handler, ParentX,ParentY);
		evas_object_resize(rel1Y_parent_handler,2,ParentH);
		
		//draw Rel2X_ParentH (bottom line)
		if (selected_desc->rel2_to_x->len > 0){
			//printf("REL2_TO_X: %s\n",selected_desc->rel2_to_x->str);
			parent = GetPartByName(selected_desc->rel2_to_x->str);
			evas_object_geometry_get (parent->ev_obj, &ParentX, &ParentY, &ParentW, &ParentH);
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel2X_parent_handler, ParentX,ParentY+ParentH-1);
		evas_object_resize(rel2X_parent_handler,ParentW,2);
		
		//draw Rel2Y_ParentH (right line)
		if (selected_desc->rel2_to_y->len > 0){
			//printf("REL2_TO_Y: %s\n",selected_desc->rel2_to_y->str);
			parent = GetPartByName(selected_desc->rel2_to_y->str);
			evas_object_geometry_get (parent->ev_obj, &ParentX, &ParentY, &ParentW, &ParentH);
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		
		evas_object_move (rel2Y_parent_handler, ParentX+ParentW-1,ParentY);
		evas_object_resize(rel2Y_parent_handler,2,ParentH);
		
		evas_object_raise (rel1X_parent_handler);
		evas_object_show(rel1X_parent_handler);
		evas_object_raise (rel1Y_parent_handler);
		evas_object_show(rel1Y_parent_handler);
		evas_object_raise (rel2X_parent_handler);
		evas_object_show(rel2X_parent_handler);
		evas_object_raise (rel2Y_parent_handler);
		evas_object_show(rel2Y_parent_handler);
		
		
		
		
	}else{
		edje_object_signal_emit(rel1_handler,"REL1_HIDE","cprog");
		edje_object_signal_emit(focus_handler,"REL2_HIDE","cprog");
		evas_object_hide(rel1X_parent_handler);
		evas_object_hide(rel1Y_parent_handler);
		evas_object_hide(rel2X_parent_handler);
		evas_object_hide(rel2Y_parent_handler);
		evas_object_hide(rel1_handler);
		evas_object_hide(rel2_handler);
	}
}
void ev_resize_fake(int w, int h){
	//printf("RESIZE: %d %d\n",w,h);
   if (!w) w=200;
   if (!h) h=200;
	evas_object_resize(EV_fakewin,w,h);
	edje_object_part_drag_value_set (EV_fakewin, "miniarrow", (double)w, (double)h);
}
void ev_draw_all(void){
	GList	*p;
	EDC_Part	*part;
   printf("DRAW ALL\n");
	if (selected_group){
		
		printf("***************DRAW GROUP: %s***********\n",selected_group->name->str);
	
		p = selected_group->parts;
		while (p){
			part = p->data;
			ev_draw_part(part);
			p = g_list_next(p);
		} 
		evas_object_raise (EV_fakewin);
		
		ev_draw_focus();
		
      edje_object_part_text_set (EV_fakewin, "title", selected_group->name->str);
      evas_object_show(EV_fakewin);
		//printf("***************END DRAW GROUP: %s***********\n",selected_group->name->str);
	}else{
      evas_object_hide(EV_fakewin);
   }
}
void ev_hide_group(EDC_Group* group){
	EDC_Part	*part;
	GList	*p;

	p = group->parts;
	while (p){
		part = p->data;
		evas_object_hide(part->ev_obj);
		p = g_list_next(p);
	} 
	evas_object_hide(focus_handler);

}

