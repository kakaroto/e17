#include "config.h"
#include <Etk.h>
#include <Edje.h>
#include "main.h"
#include "interface.h"
#include "evas.h"

#define MAIN_LEFT 20
#define MAIN_TOP 27

void
on_Drag(void *data, Evas_Object *o, const char *emission, const char *source)
{
	//printf("ON DRAG\n");
   Evas_Coord 	x,y;
	Evas_Coord 	parentx,parenty,parentw,parenth;
	Evas_Coord 	focusx,focusy,focusw,focush;
	int mouse_x, mouse_y;

	// Resize fake win
	if ((int)data == DRAG_MINIARROW)
   {
		edje_object_part_geometry_get (EV_fakewin,"miniarrow", &x, &y, NULL, NULL);

		/* if (Cur.eg->min.w && x < Cur.eg->min.w) x = Cur.eg->min.w;
		if (Cur.eg->min.h && y < Cur.eg->min.h) y = Cur.eg->min.h;
		if (Cur.eg->max.w && x > Cur.eg->max.w) x = Cur.eg->max.w;
		if (Cur.eg->max.h && y > Cur.eg->max.h) y = Cur.eg->max.h;
		 */

		ev_resize_fake((int)x,(int)y);
	}
	// Move Rel1 Point
	if ((int)data == DRAG_REL1)
   {
		evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas),
            &mouse_x, &mouse_y);
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
		Cur.eps->rel1.relative.x = (float)(x - parentx)/(float)parentw;


		//Get the position of the left parent handler
		evas_object_geometry_get (rel1Y_parent_handler, &parentx, &parenty, &parentw, &parenth);

		//Calc the rel1_relative_y value
		Cur.eps->rel1.relative.y = (float)(y - parenty)/(float)parenth;


		UpdatePositionFrame();
	}
	// Move Rel2 Point
	if ((int)data == DRAG_REL2)
   {
		evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
		evas_object_move(rel2_handler,mouse_x-2,mouse_y-2);

		//Get the position of the rel2 point
		evas_object_geometry_get (rel2_handler, &x, &y, NULL, NULL);

		//Get the position of the focus handler
		evas_object_geometry_get (focus_handler, &focusx, &focusy, &focusw, &focush);

		//Get the position of the bottom parent handler
		evas_object_geometry_get (rel2X_parent_handler, &parentx, &parenty, &parentw, &parenth);
		//Calc the rel1_relative_x value
		Cur.eps->rel2.relative.x = (float)(x - parentx)/(float)parentw;


		//Get the position of the right parent handler
		evas_object_geometry_get (rel2Y_parent_handler, &parentx, &parenty, &parentw, &parenth);
		//Calc the rel2_relative_y value
		Cur.eps->rel2.relative.y = (float)(y - parenty)/(float)parenth;


		UpdatePositionFrame();
	}

	ev_redraw();


	//printf("Drag %s - ",source);
	//printf("Position %i, %i\n", x, y);
	//Evas_Coord mouse_x, mouse_y;
	//evas_pointer_canvas_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
	//printf("Mouse is at canvas position %f, %f\n", mouse_x, mouse_y);
}

void
on_StartDrag(void *data, Evas_Object *o, const char *emission, const char *source)
{
   printf("Start Drag\n");
	//int mouse_x, mouse_y;

//	evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas), &mouse_x, &mouse_y);
//	printf("Mouse is at screen position %i, %i\n", mouse_x, mouse_y);

	if ((int)data == DRAG_REL1) edje_object_signal_callback_add(rel1_handler,"DRAG","rel1",on_Drag,(void*)DRAG_REL1);
	if ((int)data == DRAG_REL2) edje_object_signal_callback_add(rel2_handler,"DRAG","rel2",on_Drag,(void*)DRAG_REL2);
}
void
on_EndDrag(void *data, Evas_Object *o, const char *emission, const char *source)
{
 	printf("End Drag\n");
	if ((int)data == DRAG_REL1) edje_object_signal_callback_del(rel1_handler,"DRAG","rel1",on_Drag);
	if ((int)data == DRAG_REL2) edje_object_signal_callback_del(rel2_handler,"DRAG","rel2",on_Drag);
}

void
prepare_canvas(void)
{
	//Create Focus Handler
	focus_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(focus_handler, EdjeFile, "FocusH");

	//Create Rel1 handler
	rel1_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
	edje_object_file_set(rel1_handler, EdjeFile, "Rel1H");
	edje_object_signal_callback_add(rel1_handler,"START_DRAG","rel1",on_StartDrag,(void*)DRAG_REL1);
	edje_object_signal_callback_add(rel1_handler,"END_DRAG","rel1",on_EndDrag,(void*)DRAG_REL1);

	//Create Rel2 handler
	rel2_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
	edje_object_file_set(rel2_handler,EdjeFile, "Rel2H");
	edje_object_signal_callback_add(rel2_handler,"START_DRAG","rel2",on_StartDrag,(void*)DRAG_REL2);
	edje_object_signal_callback_add(rel2_handler,"END_DRAG","rel2",on_EndDrag,(void*)DRAG_REL2);


	//Create Parent Handlers
	rel1X_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(rel1X_parent_handler, EdjeFile, "Rel1X_ParentH");

	rel1Y_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(rel1Y_parent_handler, EdjeFile, "Rel1Y_ParentH");

	rel2X_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(rel2X_parent_handler, EdjeFile, "Rel2X_ParentH");

	rel2Y_parent_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(rel2Y_parent_handler, EdjeFile, "Rel2Y_ParentH");


	// Create Fake win
	EV_fakewin = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(EV_fakewin, EdjeFile, "FakeWin");

   edje_object_signal_callback_add(EV_fakewin,"DRAG","miniarrow",on_Drag,(void*)DRAG_MINIARROW);

	//evas_object_move (EV_fakewin,200+MAIN_LEFT, 200+MAIN_TOP);
	ev_resize_fake(200,200);


   //Create engrave canvas
   ecanvas = engrave_canvas_new (etk_widget_toplevel_evas_get(ETK_canvas));
   //etk_canvas_object_add (ETK_CANVAS(ETK_canvas), ecanvas);
   //etk_canvas_object_move (ETK_CANVAS(ETK_canvas),ecanvas,MAIN_LEFT, MAIN_TOP);
}
void
ev_adjust_position(Engrave_Part_State* state, Evas_Object* object)
{
 /* 	int ParentX = 0, ParentY = 0, ParentW = 0, ParentH = 0;
	int P1X = 0, P1Y = 0, P2X = 100, P2Y = 100;
	Engrave_Part *part = state->parent;
   Engrave_Group *group = part->parent;
   Engrave_Part *relto_part = NULL;

	if (ETK_canvas)
   {
		// Calculate first point
      //Assume we take relativity from the main interface
		evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);


      if (state->rel1.to_x)   //If rel1_to_x is found
      {
			relto_part = engrave_group_part_by_name_find(group, state->rel1.to_x);
         if (relto_part)
         {
            ParentX = relto_part->pos.x;
            ParentW = relto_part->pos.w;
         }
		}
 		if (state->rel1.to_y)    //If rel1_to_y is found
      {
			relto_part = engrave_group_part_by_name_find(group, state->rel1.to_y);
			if (relto_part)
         {
            ParentY = relto_part->pos.y;
            ParentH = relto_part->pos.h;
         }
		}
      //Calculate relativity
		P1X = ParentX + (ParentW * state->rel1.relative.x) + state->rel1.offset.x;
		P1Y = ParentY + (ParentH * state->rel1.relative.y) + state->rel1.offset.y;

		//Calculate second point
		evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		if (state->rel2.to_x){
			relto_part = engrave_group_part_by_name_find(group, state->rel2.to_x);
			if (relto_part)
         {
            ParentX = relto_part->pos.x;
            ParentW = relto_part->pos.w;
         }
		}
		if (state->rel2.to_y){
			relto_part = engrave_group_part_by_name_find(group, state->rel2.to_y);
			if (relto_part)
         {
            ParentY = relto_part->pos.y;
            ParentH = relto_part->pos.h;
         }
		}
		P2X = ParentX + (ParentW * state->rel2.relative.x) + state->rel2.offset.x;
		P2Y = ParentY + (ParentH * state->rel2.relative.y) + state->rel2.offset.y;


		//Calculate real coord
		part->pos.x = P1X;
		part->pos.y = P1Y;

		part->pos.w = P2X-P1X;
		part->pos.h = P2Y-P1Y;

		//Reposition the object
		evas_object_move(object,part->pos.x,part->pos.y);
		evas_object_resize(object,part->pos.w,part->pos.h);

      if (part->type == ENGRAVE_PART_TYPE_IMAGE){
			evas_object_image_fill_set(part->object,0,0,part->pos.w,part->pos.h);
		}
	} */
}
void
ev_draw_part(Engrave_Part* part)
{
 /* 	Engrave_Part_State *state = NULL;
	Evas *e = etk_widget_toplevel_evas_get(ETK_canvas);
   char buf[4096];
   //printf("***DRAW PART: %s\n",part->name);

	if (part->current_state)
		state = part->current_state;
	else
		state = part->states->data;

	if (state)
   {
		switch (part->type)
      {
			 case ENGRAVE_PART_TYPE_RECT:
				if (!part->object)	//Create RECT
				{
               part->object = evas_object_rectangle_add(e);
					etk_canvas_object_add (ETK_CANVAS(ETK_canvas), part->object);
				}
            //Set the color
            evas_object_color_set(part->object,
               state->color.r, state->color.g,state->color.b, state->color.a);
            break;
			 case ENGRAVE_PART_TYPE_IMAGE:
				if (!part->object)	//Create IMAGE
				{
               printf("** CREATE ** %s\n",part->name);
               part->object = evas_object_image_add(e);
					etk_canvas_object_add (ETK_CANVAS(ETK_canvas), part->object);
				}
				snprintf(buf,4096,"%s/%s",OpenFile->im_dir,state->image.normal->name);
            //printf("IMAGE: %s\n",buf);
            //Set the image file
            evas_object_image_file_set(part->object, buf, "");
            //Set the alpha
            evas_object_color_set(part->object,
               state->color.a, state->color.a, state->color.a, state->color.a);
				//Set the border
            evas_object_image_border_set(part->object,state->image.border.l,
               state->image.border.r,state->image.border.t,state->image.border.b);

            break;
		/*	case PART_TYPE_TEXT:
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
			break; */
	/*	}

		if (part->object)
      {
			ev_adjust_position(state,part->object);
			evas_object_show(part->object);
		}

	} */
}
void
ev_draw_focus(void)
{
	int ParentX,ParentY,ParentW,ParentH;
	int fx,fy,fw,fh;  //Fakewin geometry
	Engrave_Part *relto_part;

   //printf("DRAW FOCUS\n");

   if (Cur.ep && !Cur.ep->current_state)
      Cur.ep->current_state = Cur.ep->states->data;

	// If a part is selected draw the Focus Handler (only the yellow box)
	if (Cur.ep && Cur.ep->current_state)
   {
		etk_canvas_object_move(ETK_CANVAS(ETK_canvas),focus_handler,
            Cur.ep->current_state->pos.x + MAIN_LEFT - Cur.ep->current_state->rel1.offset.x - 1,
            Cur.ep->current_state->pos.y + MAIN_TOP - Cur.ep->current_state->rel1.offset.y - 1);

      evas_object_resize(focus_handler,
            Cur.ep->current_state->pos.w + Cur.ep->current_state->rel1.offset.x - Cur.ep->current_state->rel2.offset.x + 1,
            Cur.ep->current_state->pos.h + Cur.ep->current_state->rel1.offset.y - Cur.ep->current_state->rel2.offset.y + 1);
		evas_object_raise (focus_handler);
		evas_object_show(focus_handler);
	}else
   {
		evas_object_hide(focus_handler);
	}

	// if a part description is selected draw also the parent handlers (the red and blue lines)
	if(Cur.eps && EV_fakewin)
   {
	 	//printf("Draw parent Handlers\n");

      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&fx,&fy,&fw,&fh);

      //printf("FW geom: %d %d %d %d\n",fx,fy,fw,fh);
      //printf("pos.x: %d \n",Cur.eps->pos.w);
		//Draw rel1 & rel2 point
		evas_object_move (rel1_handler,
            fx + Cur.eps->pos.x - Cur.eps->rel1.offset.x - 2,
            fy + Cur.eps->pos.y - Cur.eps->rel1.offset.y - 2);
		evas_object_show(rel1_handler);
      
		evas_object_raise(rel1_handler);

		evas_object_move (rel2_handler,
            fx + Cur.eps->pos.x + Cur.eps->pos.w - Cur.eps->rel2.offset.x - 2,
            fy + Cur.eps->pos.y + Cur.eps->pos.h - Cur.eps->rel2.offset.y - 2);
		evas_object_show(rel2_handler);
		evas_object_raise(rel2_handler);


		//draw Rel1X_ParentH (top line)
		if (engrave_part_state_rel1_to_x_get(Cur.eps)){
			relto_part = engrave_group_part_by_name_find(Cur.eg,
                        engrave_part_state_rel1_to_x_get(Cur.eps));
         if (relto_part->current_state)
         {
            ParentX = fx + relto_part->current_state->pos.x;
            ParentY = fy + relto_part->current_state->pos.y;
            ParentW = relto_part->current_state->pos.w;
            ParentH = relto_part->current_state->pos.h;
         }
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel1X_parent_handler, ParentX,ParentY);
		evas_object_resize(rel1X_parent_handler,ParentW,2);

 	 	//draw Rel1Y_ParentH (left line)
		if (engrave_part_state_rel1_to_y_get(Cur.eps)){
			relto_part = engrave_group_part_by_name_find(Cur.eg,
                        engrave_part_state_rel1_to_y_get(Cur.eps));
			if (relto_part->current_state)
         {
            ParentX = fx + relto_part->current_state->pos.x;
            ParentY = fy + relto_part->current_state->pos.y;
            ParentW = relto_part->current_state->pos.w;
            ParentH = relto_part->current_state->pos.h;
         }
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel1Y_parent_handler, ParentX,ParentY);
		evas_object_resize(rel1Y_parent_handler,2,ParentH);

		//draw Rel2X_ParentH (bottom line)
		if (engrave_part_state_rel2_to_x_get(Cur.eps)){
			relto_part = engrave_group_part_by_name_find(Cur.eg,
                        engrave_part_state_rel2_to_x_get(Cur.eps));
			if (relto_part->current_state)
         {
            ParentX = fx + relto_part->current_state->pos.x;
            ParentY = fy + relto_part->current_state->pos.y;
            ParentW = relto_part->current_state->pos.w;
            ParentH = relto_part->current_state->pos.h;
         }
		}else{//Get FakeWin Geom
			evas_object_geometry_get (EV_fakewin, &ParentX, &ParentY, &ParentW, &ParentH);
		}
		evas_object_move (rel2X_parent_handler, ParentX,ParentY+ParentH-1);
		evas_object_resize(rel2X_parent_handler,ParentW,2);

		//draw Rel2Y_ParentH (right line)
		if (engrave_part_state_rel2_to_y_get(Cur.eps)){
			relto_part = engrave_group_part_by_name_find(Cur.eg,
                        engrave_part_state_rel2_to_y_get(Cur.eps));
			if (relto_part->current_state)
         {
            ParentX = fx + relto_part->current_state->pos.x;
            ParentY = fy + relto_part->current_state->pos.y;
            ParentW = relto_part->current_state->pos.w;
            ParentH = relto_part->current_state->pos.h;
         }
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
void
ev_resize_fake(int w, int h)
{
	//printf("RESIZE: %d %d\n",w,h);
   if (!w) w = 200;
   if (!h) h = 200;
	evas_object_resize(EV_fakewin,w,h);
	edje_object_part_drag_value_set (EV_fakewin, "miniarrow", (double)w, (double)h);
}
void
ev_redraw(void)
{
   int x, y, w, h;

   //printf("DRAW ALL\n");

   if (Cur.eg)
   {
      //Get the geometry of ETK_canvas
      evas_object_geometry_get(EV_canvas_bg,&x,&y,&w,&h);

      //Place Fakewin
      evas_object_move(EV_fakewin, x + 20, y + 27);

      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&x,&y,&w,&h);

      //place engrave canvas
      evas_object_move(ecanvas, x, y);
      evas_object_resize(ecanvas,w+1,h);  //This make engrave_canvas redraw




      evas_object_show(ecanvas);
      evas_object_show(EV_fakewin);
      evas_object_raise(EV_fakewin);
      evas_object_raise(focus_handler);
      ev_draw_focus();

   }else
   {
      evas_object_hide(ecanvas);
      evas_object_hide(EV_fakewin);
   }
}
void
ev_hide_group(Engrave_Group* group)
{
/*    Engrave_Part *part;
	Evas_List *l;

   for (l = group->parts; l; l = l->next)
   {
		part = l->data;
		evas_object_hide(part->object);
	}
	evas_object_hide(focus_handler); */
}

