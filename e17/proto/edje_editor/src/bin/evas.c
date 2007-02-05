#include "config.h"
#include <Etk.h>
#include <Edje.h>
#include "main.h"
#include "interface.h"
#include "evas.h"

#define MAIN_LEFT 20
#define MAIN_TOP 27

void
on_Drag(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //printf("ON DRAG\n");
   Evas_Coord x,y;
   Evas_Coord parentx,parenty,parentw,parenth;
   Evas_Coord focusx,focusy,focusw,focush;
   int mouse_x, mouse_y;

   // MoveBox
   if ((int)data == DRAG_MOVEBOX)
   {
      evas_pointer_output_xy_get(etk_widget_toplevel_evas_get(ETK_canvas),&x, &y);
      //evas_object_move(rel1_handler,mouse_x-2,mouse_y-2);
      ev_move_fake(x-30,y+8);//TODO
   }

   // Resize fake win
   if ((int)data == DRAG_MINIARROW)
   {
      edje_object_part_geometry_get (EV_fakewin,"miniarrow", &x, &y, NULL, NULL);
      if (x < 0) x = 0;
      if (y < 0) y = 0;
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
   //printf("Start Drag\n");

   if ((int)data == DRAG_REL1) 
      edje_object_signal_callback_add(rel1_handler,"DRAG","rel1",on_Drag,(void*)DRAG_REL1);
   if ((int)data == DRAG_REL2) 
      edje_object_signal_callback_add(rel2_handler,"DRAG","rel2",on_Drag,(void*)DRAG_REL2);
  if ((int)data == DRAG_MOVEBOX) 
       evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_MOVE, on_Drag, (void*)DRAG_MOVEBOX);
}

void
on_EndDrag(void *data, Evas_Object *o, const char *emission, const char *source)
{
   //printf("End Drag\n");
   if ((int)data == DRAG_REL1) edje_object_signal_callback_del(rel1_handler,"DRAG","rel1",on_Drag);
   if ((int)data == DRAG_REL2) edje_object_signal_callback_del(rel2_handler,"DRAG","rel2",on_Drag);
   if ((int)data == DRAG_MOVEBOX) evas_object_event_callback_del(EV_movebox, EVAS_CALLBACK_MOUSE_MOVE, on_Drag);
   if ((int)data == DRAG_MINIARROW)
   {
      int fw,fh;  //Fakewin geometry
      evas_object_geometry_get(EV_fakewin,NULL,NULL,&fw,&fh);
      ev_resize_fake(fw,fh);
   }
}

void
prepare_canvas(void)
{
   //Create Focus Handler
   focus_handler = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(focus_handler, EdjeFile, "FocusH");
   etk_canvas_object_add (ETK_CANVAS(ETK_canvas), focus_handler);

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

   //Create engrave canvas
   ecanvas = engrave_canvas_new (etk_widget_toplevel_evas_get(ETK_canvas));
   etk_canvas_object_add (ETK_CANVAS(ETK_canvas), ecanvas);

   // Create Fake win
   EV_fakewin = edje_object_add(etk_widget_toplevel_evas_get(ETK_canvas));
   edje_object_file_set(EV_fakewin, EdjeFile, "FakeWin");
   etk_canvas_object_add(ETK_CANVAS(ETK_canvas),EV_fakewin);
   edje_object_signal_callback_add(EV_fakewin,"DRAG","miniarrow",on_Drag,(void*)DRAG_MINIARROW);
   edje_object_signal_callback_add(EV_fakewin,"END_DRAG","miniarrow",on_EndDrag,(void*)DRAG_MINIARROW);

   // Create MoveBox
   EV_movebox = evas_object_rectangle_add(etk_widget_toplevel_evas_get(ETK_canvas));
   etk_canvas_object_add (ETK_CANVAS(ETK_canvas), EV_movebox);
   evas_object_color_set(EV_movebox, 0, 0, 0, 0);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_DOWN, on_StartDrag, (void*)DRAG_MOVEBOX);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_UP, on_EndDrag, (void*)DRAG_MOVEBOX);

   //Place Fakewin 
   ev_move_fake(20,27);

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
      int x, y;
      etk_canvas_object_geometry_get(ETK_CANVAS(ETK_canvas),EV_fakewin,&x,&y,NULL,NULL);

      etk_canvas_object_move(ETK_CANVAS(ETK_canvas),focus_handler,
            Cur.ep->pos.x + x - Cur.ep->current_state->rel1.offset.x - 2,
            Cur.ep->pos.y + y - Cur.ep->current_state->rel1.offset.y - 2);

      evas_object_resize(focus_handler,
            Cur.ep->pos.w + Cur.ep->current_state->rel1.offset.x - Cur.ep->current_state->rel2.offset.x + 2,
            Cur.ep->pos.h + Cur.ep->current_state->rel1.offset.y - Cur.ep->current_state->rel2.offset.y + 2);
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
            fx + Cur.ep->pos.x - Cur.eps->rel1.offset.x - 2,
            fy + Cur.ep->pos.y - Cur.eps->rel1.offset.y - 2);
      evas_object_show(rel1_handler);

      evas_object_raise(rel1_handler);

      evas_object_move (rel2_handler,
            fx + Cur.ep->pos.x + Cur.ep->pos.w - Cur.eps->rel2.offset.x - 2,
            fy + Cur.ep->pos.y + Cur.ep->pos.h - Cur.eps->rel2.offset.y - 2);
      evas_object_show(rel2_handler);
      evas_object_raise(rel2_handler);


      //draw Rel1X_ParentH (top line)
      if (engrave_part_state_rel1_to_x_get(Cur.eps)){
         relto_part = engrave_group_part_by_name_find(Cur.eg,
                        engrave_part_state_rel1_to_x_get(Cur.eps));
         if (relto_part->current_state)
         {
            ParentX = fx + relto_part->pos.x;
            ParentY = fy + relto_part->pos.y;
            ParentW = relto_part->pos.w;
            ParentH = relto_part->pos.h;
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
            ParentX = fx + relto_part->pos.x;
            ParentY = fy + relto_part->pos.y;
            ParentW = relto_part->pos.w;
            ParentH = relto_part->pos.h;
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
            ParentX = fx + relto_part->pos.x;
            ParentY = fy + relto_part->pos.y;
            ParentW = relto_part->pos.w;
            ParentH = relto_part->pos.h;
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
            ParentX = fx + relto_part->pos.x;
            ParentY = fy + relto_part->pos.y;
            ParentW = relto_part->pos.w;
            ParentH = relto_part->pos.h;
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
      evas_object_raise (focus_handler);

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
   int max_w=0, max_h=0;
   int min_w=0, min_h=0;

   engrave_group_max_size_get(Cur.eg, &max_w, &max_h);
   engrave_group_min_size_get(Cur.eg, &min_w, &min_h);

   if (max_w > 0 && w > max_w) w = max_w;
   if (max_h > 0 && h > max_h) h = max_h;

   if (min_w > 0 && w < min_w) w = min_w;
   if (min_h > 0 && h < min_h) h = min_h;

   if (w < 0) w = 100;
   if (h < 0) h = 100;

   //printf("RESIZE: %d %d./ed  \n",w,h);
   evas_object_resize(EV_fakewin, w, h);
   evas_object_resize(EV_movebox, w, 10);
   edje_object_part_drag_value_set (EV_fakewin, "miniarrow", (double)w, (double)h);
}

void
ev_move_fake(int x, int y)
{
   //printf("MOVEBOX: %d %d\n",x,y);
   evas_object_move(EV_fakewin, x, y);
   evas_object_move(EV_movebox, x, y-14);
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

      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&x,&y,&w,&h);

      //place engrave canvas
      evas_object_move(ecanvas, x, y);
      evas_object_resize(ecanvas,w+1,h);  //This make engrave_canvas redraw

      evas_object_show(ecanvas);
      evas_object_show(EV_fakewin);
      evas_object_show(EV_movebox);
      evas_object_raise(EV_fakewin);
      evas_object_raise(EV_movebox);
      evas_object_raise(focus_handler);
      ev_draw_focus();

   }else
   {
      evas_object_hide(ecanvas);
      evas_object_hide(EV_fakewin);
      evas_object_hide(EV_movebox);
      evas_object_hide(rel1X_parent_handler);
      evas_object_hide(rel1Y_parent_handler);
      evas_object_hide(rel2X_parent_handler);
      evas_object_hide(rel2Y_parent_handler);
      evas_object_hide(rel1_handler);
      evas_object_hide(rel2_handler);
   }
}


