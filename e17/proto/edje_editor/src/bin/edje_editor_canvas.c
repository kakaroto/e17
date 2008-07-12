#include <Etk.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "main.h"


/***   Locals   ***/
static Evas_Coord dx, dy, dx2, dy2;

static void _canvas_draw_focus(void);
static void _canvas_resize_fake(int w, int h);
static void _canvas_move_fake(int x, int y);

/***   Callbacks   ***/
void
_canvas_drag_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //printf("ON DRAG\n");
   Evas_Coord parentx,parenty,parentw,parenth;
   Evas_Coord x,y;
   Evas_Coord mouse_x, mouse_y;

   // MoveBox
   if ((int)data == DRAG_MOVEBOX)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin),
                                 &mouse_x, &mouse_y);
      _canvas_move_fake(mouse_x - dx, mouse_y + 16 - dy);
   }

   // Resize fake win
   if ((int)data == DRAG_MINIARROW)
   {
      edje_object_part_geometry_get(EV_fakewin,"miniarrow", &x, &y, NULL, NULL);
      if (x < 0) x = 0;
      if (y < 0) y = 0;
      _canvas_resize_fake((int)x,(int)y);
   }
   
   // Move Rel1 Point
   if ((int)data == DRAG_REL1)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin),
                                 &mouse_x, &mouse_y);

      //Calc the rel1_relative_x value
      evas_object_geometry_get(EV_rel1X_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel1_relative_x_set(edje_o, Cur.part->string, 
                  Cur.state->string, (float)(mouse_x - parentx)/(float)parentw);
      //Calc the rel1_relative_y value
      evas_object_geometry_get(EV_rel1Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);

      edje_edit_state_rel1_relative_y_set(edje_o, Cur.part->string,
                  Cur.state->string,(float)(mouse_y - parenty)/(float)parenth);
      
      position_frame_update();
   }
   // Move Rel2 Point
   if ((int)data == DRAG_REL2)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), 
                                 &mouse_x, &mouse_y);

      //Calc the rel2_relative_x value
      evas_object_geometry_get(EV_rel2X_parent_handler,
                                &parentx, &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_x_set(edje_o, Cur.part->string,
                  Cur.state->string, (float)(mouse_x - parentx)/(float)parentw);
      //Calc the rel2_relative_y value
      evas_object_geometry_get(EV_rel2Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_y_set(edje_o, Cur.part->string,
                  Cur.state->string, (float)(mouse_y - parenty)/(float)parenth);

      position_frame_update();
   }
   // Move Move Handler
   if ((int)data == DRAG_MOVE)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), 
                                 &mouse_x, &mouse_y);
      //Calc the rel1_relative_x value
      evas_object_geometry_get(EV_rel1X_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel1_relative_x_set(edje_o,
                        Cur.part->string, Cur.state->string,
                        (float)(mouse_x - parentx - dx)/(float)parentw);
      //Calc the rel1_relative_y value
      evas_object_geometry_get(EV_rel1Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel1_relative_y_set(edje_o,
                        Cur.part->string, Cur.state->string,
                        (float)(mouse_y - parenty - dy)/(float)parenth);
      //Calc the rel2_relative_x value
      evas_object_geometry_get(EV_rel2X_parent_handler,
                                &parentx, &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_x_set(edje_o,
                        Cur.part->string, Cur.state->string,
                        (float)(mouse_x - parentx - dx + dx2)/(float)parentw);
      //Calc the rel2_relative_y value
      evas_object_geometry_get(EV_rel2Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_y_set(edje_o,
                        Cur.part->string, Cur.state->string,
                        (float)(mouse_y - parenty - dy + dy2)/(float)parenth);

      position_frame_update();
   }
   // Move First Gradient Point Handler
   if ((int)data == DRAG_GRAD_1 || (int)data == DRAG_GRAD_2 )
   {
      char *type;
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), 
                                 &mouse_x, &mouse_y);
      evas_object_geometry_get(EV_focus_handler, &parentx, &parenty,
                               &parentw, &parenth);
      type = edje_edit_state_gradient_type_get(edje_o, Cur.part->string,
                                               Cur.state->string);
      //printf("GELLA %d %d %d %d [%f] [m %d %d]\n", fhx, fhy, fhw, fhh, (float)(mouse_x - fhx)/(float)fhx, mouse_x, mouse_y);
      if ((int)data == DRAG_GRAD_1)
      {
         if (edje_edit_state_gradient_use_fill_get(edje_o, Cur.part->string,
                                                   Cur.state->string))
         {//Update fill origin
            edje_edit_state_fill_origin_relative_x_set(edje_o, Cur.part->string,
             Cur.state->string, (float)(mouse_x - parentx - dx)/(float)parentw);
            edje_edit_state_fill_origin_relative_y_set(edje_o, Cur.part->string,
             Cur.state->string, (float)(mouse_y - parenty - dy)/(float)parenth);
         }
         else
         {//Update gradient rel
            edje_edit_state_gradient_rel1_relative_x_set(edje_o, Cur.part->string,
             Cur.state->string, (float)(mouse_x - parentx - dx)/(float)parentw);
            edje_edit_state_gradient_rel1_relative_y_set(edje_o, Cur.part->string,
             Cur.state->string, (float)(mouse_y - parenty - dy)/(float)parenth);
         }
      }
      else
      {
         if (edje_edit_state_gradient_use_fill_get(edje_o, Cur.part->string,
                                                   Cur.state->string))
         {//Update fill origin
            edje_edit_state_fill_size_relative_x_set(edje_o, Cur.part->string,
               Cur.state->string, (float)(mouse_x - parentx - dx)/(float)parentw);
            edje_edit_state_fill_size_relative_y_set(edje_o, Cur.part->string,
               Cur.state->string, (float)(mouse_y - parenty - dy)/(float)parenth);
         }
         else
         {//Update gradient rel
            edje_edit_state_gradient_rel2_relative_x_set(edje_o, Cur.part->string,
               Cur.state->string, (float)(mouse_x - parentx - dx)/(float)parentw);
            edje_edit_state_gradient_rel2_relative_y_set(edje_o, Cur.part->string,
               Cur.state->string, (float)(mouse_y - parenty - dy)/(float)parenth);
         }
         
      }
      gradient_frame_update();
      fill_frame_update();
      edje_edit_string_free(type);
   }
   
   canvas_redraw();

   //printf("Drag %s - ",source);
   //printf("Position %i, %i\n", x, y);
}

void
_canvas_drag_start_cb(void *data, Evas_Object *o, const char *em, const char *src)
{
   Evas_Coord x, y, mx, my;
   printf("Start Drag\n");

   if ((int)data == DRAG_REL1)
   {
      evas_object_event_callback_add(EV_rel1_handler, EVAS_CALLBACK_MOUSE_MOVE,
                                    _canvas_drag_cb,(void*)DRAG_REL1);
      evas_object_show(EV_rel1X_align_guide);
      evas_object_raise(EV_rel1X_align_guide);
      evas_object_show(EV_rel1Y_align_guide);
      evas_object_raise(EV_rel1Y_align_guide);
   }
   if ((int)data == DRAG_REL2)
   {
      evas_object_event_callback_add(EV_rel2_handler, EVAS_CALLBACK_MOUSE_MOVE,
                                    _canvas_drag_cb,(void*)DRAG_REL2);
      evas_object_show(EV_rel2X_align_guide);
      evas_object_raise(EV_rel2X_align_guide);
      evas_object_show(EV_rel2Y_align_guide);
      evas_object_raise(EV_rel2Y_align_guide);
   }
   if ((int)data == DRAG_MOVE)
   {
      evas_object_event_callback_add(EV_move_handler, EVAS_CALLBACK_MOUSE_MOVE,
                                    _canvas_drag_cb,(void*)DRAG_MOVE);
      evas_object_geometry_get(EV_rel1_handler, &x, &y, NULL, NULL);
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), &mx, &my);
      dx = mx - x - 3;
      dy = my - y - 3;
      edje_object_part_geometry_get(edje_o, Cur.part->string, NULL, NULL,
                                    &dx2, &dy2);
      
      evas_object_show(EV_rel1X_align_guide);
      evas_object_raise(EV_rel1X_align_guide);
      evas_object_show(EV_rel1Y_align_guide);
      evas_object_raise(EV_rel1Y_align_guide);
      evas_object_show(EV_rel2X_align_guide);
      evas_object_raise(EV_rel2X_align_guide);
      evas_object_show(EV_rel2Y_align_guide);
      evas_object_raise(EV_rel2Y_align_guide);
   }
   
   if ((int)data == DRAG_MOVEBOX)
   {
      evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb, (void*)DRAG_MOVEBOX);
      evas_object_geometry_get(EV_movebox, &x, &y, NULL, NULL);
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), &mx, &my);
      dx = mx - x;
      dy = my - y;
   }
   
   if ((int)data == DRAG_GRAD_1)
   {
      evas_object_event_callback_add(EV_grad1_handler, EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb, (void*)DRAG_GRAD_1);
      evas_object_geometry_get(EV_grad1_handler, &x, &y, NULL, NULL);
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), &mx, &my);
      dx = mx - x;
      dy = my - y;
   }
   if ((int)data == DRAG_GRAD_2)
   {
      evas_object_event_callback_add(EV_grad2_handler, EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb, data);
      evas_object_geometry_get(EV_grad2_handler, &x, &y, NULL, NULL);
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), &mx, &my);
      dx = mx - x;
      dy = my - y;
   }
   canvas_redraw();
}

void
_canvas_drag_end_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
   //printf("End Drag\n");
   if ((int)data == DRAG_REL1)
   {
      evas_object_event_callback_del(EV_rel1_handler,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
      evas_object_hide(EV_rel1X_align_guide);
      evas_object_hide(EV_rel1Y_align_guide);
      evas_object_line_xy_set(EV_rel1X_align_guide, 0, 0, 0, 0);
      evas_object_line_xy_set(EV_rel1Y_align_guide, 0, 0, 0, 0);
   }
   if ((int)data == DRAG_REL2)
   {
      evas_object_event_callback_del(EV_rel2_handler,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
      evas_object_hide(EV_rel2X_align_guide);
      evas_object_hide(EV_rel2Y_align_guide);
      evas_object_line_xy_set(EV_rel2X_align_guide, 0, 0, 0, 0);
      evas_object_line_xy_set(EV_rel2Y_align_guide, 0, 0, 0, 0);
   }
   if ((int)data == DRAG_MOVE)
   {
      evas_object_event_callback_del(EV_move_handler,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
      evas_object_hide(EV_rel1X_align_guide);
      evas_object_hide(EV_rel1Y_align_guide);
      evas_object_line_xy_set(EV_rel1X_align_guide, 0, 0, 0, 0);
      evas_object_line_xy_set(EV_rel1Y_align_guide, 0, 0, 0, 0);
      evas_object_hide(EV_rel2X_align_guide);
      evas_object_hide(EV_rel2Y_align_guide);
      evas_object_line_xy_set(EV_rel2X_align_guide, 0, 0, 0, 0);
      evas_object_line_xy_set(EV_rel2Y_align_guide, 0, 0, 0, 0);
   }
   if ((int)data == DRAG_MOVEBOX)
      evas_object_event_callback_del(EV_movebox,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
   if ((int)data == DRAG_GRAD_1)
      evas_object_event_callback_del(EV_grad1_handler,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
   if ((int)data == DRAG_GRAD_2)
      evas_object_event_callback_del(EV_grad2_handler,EVAS_CALLBACK_MOUSE_MOVE,
                                     _canvas_drag_cb);
   if ((int)data == DRAG_MINIARROW)
   {
      int fw,fh;  //Fakewin geometry
      evas_object_geometry_get(EV_fakewin,NULL,NULL,&fw,&fh);
      _canvas_resize_fake(fw,fh);
   }
}


/***   Internals   ***/
static void
_canvas_draw_focus(void)
{
   int ParentX,ParentY,ParentW,ParentH;
   int fx,fy,fw,fh;  //Fakewin geometry

   //printf("DRAW FOCUS\n");

   int o1x, o1y, o2x, o2y;
   const char *state;
   
   state = edje_edit_part_selected_state_get(edje_o, Cur.part->string);
   o1x = edje_edit_state_rel1_offset_x_get(edje_o, Cur.part->string, state);
   o1y = edje_edit_state_rel1_offset_y_get(edje_o, Cur.part->string, state);
   o2x = edje_edit_state_rel2_offset_x_get(edje_o, Cur.part->string, state);
   o2y = edje_edit_state_rel2_offset_y_get(edje_o, Cur.part->string, state);
   edje_edit_string_free(state);
   
   // If a part is selected draw the Focus Handler (only the yellow box)
   if (etk_string_length_get(Cur.part))//&& Cur.ep->current_state)
   {
      int wx, wy;
      int px,py,pw,ph;
       
      evas_object_geometry_get(EV_fakewin,&wx,&wy,NULL,NULL);
      
      edje_object_part_geometry_get(edje_o, Cur.part->string, &px, &py, &pw, &ph);
      
      //printf("CURRENT STATE: %s   -   %d\n",state,o1x);
       
      evas_object_move(EV_focus_handler,
            px + wx - o1x - 2, 
            py + wy - o1y - 2);

      evas_object_resize(EV_focus_handler,
            pw + o1x - o2x + 2, 
            ph + o1y - o2y + 2);
      
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_SWALLOW)
         edje_object_signal_emit(EV_focus_handler, "SwallShow", "edje_editor");
      else
         edje_object_signal_emit(EV_focus_handler, "SwallHide", "edje_editor");
      
      evas_object_raise(EV_focus_handler);
      evas_object_show(EV_focus_handler);
   }else
      evas_object_hide(EV_focus_handler);

   // if a part description is selected draw also 
   // the parent handlers (the red and blue lines)
   if (etk_string_length_get(Cur.state) && etk_string_length_get(Cur.part))
   {
      int px,py,pw,ph;
      int rx,ry,rw,rh;
      const char *rel;
      //printf("Draw parent Handlers\n");

      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&fx,&fy,&fw,&fh);
      edje_object_part_geometry_get(edje_o, Cur.part->string, &px, &py, &pw, &ph);
     // printf("FW geom: %d %d %d %d\n",fx,fy,fw,fh);
     // printf("PA geom: %d %d %d %d\n",px,py,pw,ph);
       
      //Draw rel1 & rel2 point
      evas_object_move(EV_rel1_handler,
                       fx + px - o1x - 2,
                       fy + py - o1y - 2);
      evas_object_show(EV_rel1_handler);
      evas_object_raise(EV_rel1_handler);

      evas_object_move(EV_rel2_handler,
                       fx + px + pw - o2x - 4,
                       fy + py + ph - o2y - 4);
      evas_object_show(EV_rel2_handler);
      evas_object_raise(EV_rel2_handler);
      
      //Draw move_handler
      evas_object_move(EV_move_handler,
                       fx + px + pw/2 - 6,
                       fy + py + ph/2 - 6);
      evas_object_show(EV_move_handler);
      evas_object_raise(EV_move_handler);

      
      //draw Rel1X_ParentH (top line)
      rel = edje_edit_state_rel1_to_x_get(edje_o, Cur.part->string,
                                          Cur.state->string);
      if(rel)
      {
         edje_object_part_geometry_get(edje_o, rel, &rx, &ry, &rw, &rh);
         ParentX = fx + rx;
         ParentY = fy + ry;
         ParentW = rw;
         ParentH = rh;
         edje_edit_string_free(rel);
      }
      else//Get FakeWin Geom
         evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY,
                                              &ParentW, &ParentH);
      evas_object_line_xy_set(EV_rel1X_parent_handler, ParentX, ParentY, 
                                                    ParentX+ParentW, ParentY);
      
      //draw Rel1Y_ParentH (left line)
      if((rel = edje_edit_state_rel1_to_y_get(edje_o, Cur.part->string, Cur.state->string)))
      {
         edje_object_part_geometry_get(edje_o, rel, &rx, &ry, &rw, &rh);
         ParentX = fx + rx;
         ParentY = fy + ry;
         ParentW = rw;
         ParentH = rh;
         edje_edit_string_free(rel);
      }
      else//Get FakeWin Geom
         evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY,
                                              &ParentW, &ParentH);
      evas_object_line_xy_set(EV_rel1Y_parent_handler, ParentX, ParentY,
                                                    ParentX, ParentY+ParentH);

      //draw Rel2X_ParentH (bottom line)
      if((rel = edje_edit_state_rel2_to_x_get(edje_o, Cur.part->string, Cur.state->string)))
      {
         edje_object_part_geometry_get(edje_o, rel, &rx, &ry, &rw, &rh);
         ParentX = fx + rx;
         ParentY = fy + ry;
         ParentW = rw;
         ParentH = rh;
         edje_edit_string_free(rel);
      }
      else//Get FakeWin Geom
         evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY,
                                              &ParentW, &ParentH);
      evas_object_line_xy_set(EV_rel2X_parent_handler, ParentX, ParentY+ParentH,
                                          ParentX+ParentW, ParentY+ParentH);

      //draw Rel2Y_ParentH (right line)
      if((rel = edje_edit_state_rel2_to_y_get(edje_o, Cur.part->string, Cur.state->string)))
      {
         edje_object_part_geometry_get(edje_o, rel, &rx, &ry, &rw, &rh);
         ParentX = fx + rx;
         ParentY = fy + ry;
         ParentW = rw;
         ParentH = rh;
         edje_edit_string_free(rel);
      }
      else//Get FakeWin Geom
         evas_object_geometry_get(EV_fakewin, &ParentX, &ParentY,
                                              &ParentW, &ParentH);
      evas_object_line_xy_set(EV_rel2Y_parent_handler, ParentX+ParentW, ParentY,
                                            ParentX+ParentW, ParentY+ParentH);

      
      //Place the align guides
      evas_object_geometry_get(EV_rel1_handler, &px, &py, NULL, NULL);
      evas_object_line_xy_set(EV_rel1X_align_guide, px + 2, 0, px + 2, 2000); //TODO FIX 2000
      evas_object_line_xy_set(EV_rel1Y_align_guide, 0, py + 2, 2000, py + 2); //TODO FIX 2000
      evas_object_geometry_get(EV_rel2_handler, &px, &py, NULL, NULL);
      evas_object_line_xy_set(EV_rel2X_align_guide, px + 2, 0, px + 2, 2000); //TODO FIX 2000
      evas_object_line_xy_set(EV_rel2Y_align_guide, 0, py + 2, 2000, py + 2); //TODO FIX 2000
      
      //Place the gradient handler
      if (edje_edit_part_type_get(edje_o, Cur.part->string) == EDJE_PART_TYPE_GRADIENT)
      {
         double gx, gy, gox, goy;
         const char *type;
         int p1x,p1y,p2x,p2y;
         edje_object_part_geometry_get(edje_o, Cur.part->string, &ParentX, &ParentY, &ParentW, &ParentH);
         type = edje_edit_state_gradient_type_get(edje_o, Cur.part->string,
                                                  Cur.state->string);
         
         //First point
         if (edje_edit_state_gradient_use_fill_get(edje_o, Cur.part->string,
                                                   Cur.state->string))
         {
            gx = edje_edit_state_fill_origin_relative_x_get(edje_o, Cur.part->string, Cur.state->string);
            gy = edje_edit_state_fill_origin_relative_y_get(edje_o, Cur.part->string, Cur.state->string);
            gox = edje_edit_state_fill_origin_offset_x_get(edje_o, Cur.part->string, Cur.state->string);
            goy = edje_edit_state_fill_origin_offset_y_get(edje_o, Cur.part->string, Cur.state->string);
         }
         else
         {
            gx = edje_edit_state_gradient_rel1_relative_x_get(edje_o, Cur.part->string, Cur.state->string);
            gy = edje_edit_state_gradient_rel1_relative_y_get(edje_o, Cur.part->string, Cur.state->string);
            gox = edje_edit_state_gradient_rel1_offset_x_get(edje_o, Cur.part->string, Cur.state->string);
            goy = edje_edit_state_gradient_rel1_offset_y_get(edje_o, Cur.part->string, Cur.state->string);
         }
         p1x = ParentX + fx + (pw * gx) + gox;
         p1y = ParentY + fy + (ph * gy) + goy;
         evas_object_move(EV_grad1_handler, p1x, p1y);
         
         //Second point
         if (edje_edit_state_gradient_use_fill_get(edje_o, Cur.part->string,
                                                   Cur.state->string))
         {
            gx = edje_edit_state_fill_size_relative_x_get(edje_o, Cur.part->string, Cur.state->string);
            gy = edje_edit_state_fill_size_relative_y_get(edje_o, Cur.part->string, Cur.state->string);
            gox = edje_edit_state_fill_size_offset_x_get(edje_o, Cur.part->string, Cur.state->string);
            goy = edje_edit_state_fill_size_offset_y_get(edje_o, Cur.part->string, Cur.state->string);
         }
         else
         {
            gx = edje_edit_state_gradient_rel2_relative_x_get(edje_o, Cur.part->string, Cur.state->string);
            gy = edje_edit_state_gradient_rel2_relative_y_get(edje_o, Cur.part->string, Cur.state->string);
            gox = edje_edit_state_gradient_rel2_offset_x_get(edje_o, Cur.part->string, Cur.state->string);
            goy = edje_edit_state_gradient_rel2_offset_y_get(edje_o, Cur.part->string, Cur.state->string);
         }
         p2x = ParentX + fx + (pw * gx) + gox;
         p2y = ParentY + fy + (ph * gy) + goy;
         evas_object_move(EV_grad2_handler, p2x, p2y);
         
         //Line
         evas_object_line_xy_set(EV_gradient_line, p1x, p1y, p2x, p2y);
         
         
         evas_object_show(EV_gradient_line);
         evas_object_raise(EV_gradient_line);
         evas_object_show(EV_grad1_handler);
         evas_object_raise(EV_grad1_handler);
         evas_object_show(EV_grad2_handler);
         evas_object_raise(EV_grad2_handler);
         
         edje_edit_string_free(type);

      }
      else
      {
         evas_object_hide(EV_gradient_line);
         evas_object_hide(EV_grad1_handler);
         evas_object_hide(EV_grad2_handler);
      }
      
      evas_object_raise(EV_rel1X_parent_handler);
      evas_object_show(EV_rel1X_parent_handler);
      evas_object_raise(EV_rel1Y_parent_handler);
      evas_object_show(EV_rel1Y_parent_handler);
      evas_object_raise(EV_rel2X_parent_handler);
      evas_object_show(EV_rel2X_parent_handler);
      evas_object_raise(EV_rel2Y_parent_handler);
      evas_object_show(EV_rel2Y_parent_handler);
      evas_object_raise(EV_focus_handler);
      evas_object_raise(EV_rel1_handler);
      evas_object_raise(EV_rel2_handler);
      evas_object_raise(EV_move_handler);
      
   }else{ //none selected, hide all
      edje_object_signal_emit(EV_rel1_handler,"REL1_HIDE","edje_editor");
      edje_object_signal_emit(EV_focus_handler,"REL2_HIDE","edje_editor");
      evas_object_hide(EV_rel1X_parent_handler);
      evas_object_hide(EV_rel1Y_parent_handler);
      evas_object_hide(EV_rel2X_parent_handler);
      evas_object_hide(EV_rel2Y_parent_handler);
      evas_object_hide(EV_rel1_handler);
      evas_object_hide(EV_rel2_handler);
      evas_object_hide(EV_move_handler);
      evas_object_hide(EV_gradient_line);
      evas_object_hide(EV_grad1_handler);
      evas_object_hide(EV_grad2_handler);
   }
}

static void
_canvas_resize_fake(int w, int h)
{
   int max_w = 0;
   int max_h = 0;
   int min_w = 0;
   int min_h = 0;

   edje_object_size_min_get(edje_o, &min_w, &min_h);
   edje_object_size_max_get(edje_o, &max_w, &max_h);
   
   if (max_w > 0 && w > max_w) w = max_w;
   if (max_h > 0 && h > max_h) h = max_h;

   if (min_w > 0 && w < min_w) w = min_w;
   if (min_h > 0 && h < min_h) h = min_h;

   if (w < 0) w = 100;
   if (h < 0) h = 100;

   evas_object_resize(EV_fakewin, w, h);
   evas_object_resize(EV_movebox, w+8, 14);
   edje_object_part_drag_value_set(EV_fakewin, "miniarrow",
                                   (double)w, (double)h);
   char buf[100];
   snprintf(buf,100,"Current size: %d x %d",w,h);
   etk_label_set(ETK_LABEL(UI_CurrentGroupSizeLabel), buf);
}

static void
_canvas_move_fake(int x, int y)
{
   //printf("MOVEBOX: %d %d\n",x,y);
   evas_object_move(EV_fakewin, x, y);
   evas_object_move(EV_movebox, x-4, y-16);
}


/***   Implementation   ***/
void
canvas_prepare(void)
{
   //Create Focus Handler
   EV_focus_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_focus_handler, EdjeFile, "FocusH");

   //Create Rel1 handler
   EV_rel1_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_rel1_handler, EdjeFile, "Rel1H");
   edje_object_signal_callback_add(EV_rel1_handler, "START_DRAG", "rel1",
                                   _canvas_drag_start_cb, (void*)DRAG_REL1);
   edje_object_signal_callback_add(EV_rel1_handler, "END_DRAG", "rel1",
                                   _canvas_drag_end_cb, (void*)DRAG_REL1);

   //Create Rel2 handler
   EV_rel2_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_rel2_handler, EdjeFile, "Rel2H");
   edje_object_signal_callback_add(EV_rel2_handler, "START_DRAG", "rel2",
                                   _canvas_drag_start_cb, (void*)DRAG_REL2);
   edje_object_signal_callback_add(EV_rel2_handler, "END_DRAG", "rel2",
                                   _canvas_drag_end_cb, (void*)DRAG_REL2);
   
   //Create Move handler
   EV_move_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_move_handler, EdjeFile, "MoveH");
   edje_object_signal_callback_add(EV_move_handler, "START_DRAG", "MoveH",
                                   _canvas_drag_start_cb, (void*)DRAG_MOVE);
   edje_object_signal_callback_add(EV_move_handler, "END_DRAG", "MoveH",
                                   _canvas_drag_end_cb, (void*)DRAG_MOVE);

   //Create two Gradient handlers
   EV_grad1_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_grad1_handler, EdjeFile, "GradH");
   edje_object_signal_callback_add(EV_grad1_handler, "START_DRAG", "GradH",
                                   _canvas_drag_start_cb, (void*)DRAG_GRAD_1);
   edje_object_signal_callback_add(EV_grad1_handler, "END_DRAG", "GradH",
                                   _canvas_drag_end_cb, (void*)DRAG_GRAD_1);
   
   EV_grad2_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_grad2_handler, EdjeFile, "GradH");
   edje_object_signal_callback_add(EV_grad2_handler, "START_DRAG", "GradH",
                                   _canvas_drag_start_cb, (void*)DRAG_GRAD_2);
   edje_object_signal_callback_add(EV_grad2_handler, "END_DRAG", "GradH",
                                   _canvas_drag_end_cb, (void*)DRAG_GRAD_2);
   //Create Gradient line
   EV_gradient_line = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_gradient_line, 42, 42, 42, 200);
   
   //Create Parent Handlers
   EV_rel1X_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel1X_parent_handler, 255, 0, 0, 200);
   
   EV_rel1Y_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel1Y_parent_handler, 255, 0, 0, 200);
    
   EV_rel2X_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel2X_parent_handler, 0, 0, 255, 200);

   EV_rel2Y_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel2Y_parent_handler, 0, 0, 255, 200);

   //Create Align Guides
   EV_rel1X_align_guide = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel1X_align_guide, 180, 0, 0, 180);
   EV_rel1Y_align_guide = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel1Y_align_guide, 180, 0, 0, 180);

   EV_rel2X_align_guide = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel2X_align_guide, 0, 0, 180, 180);
   EV_rel2Y_align_guide = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_rel2Y_align_guide, 0, 0, 180, 180);

   // Create Fake win
   EV_fakewin = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_fakewin, EdjeFile, "FakeWin");
   edje_object_signal_callback_add(EV_fakewin,"DRAG","miniarrow",
                                   _canvas_drag_cb, (void*)DRAG_MINIARROW);
   edje_object_signal_callback_add(EV_fakewin,"END_DRAG","miniarrow",
                                   _canvas_drag_end_cb, (void*)DRAG_MINIARROW);

   // Create MoveBox
   EV_movebox = evas_object_rectangle_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_movebox, 0, 0, 0, 0);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_DOWN,
                                  _canvas_drag_start_cb, (void*)DRAG_MOVEBOX);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_UP,
                                  _canvas_drag_end_cb, (void*)DRAG_MOVEBOX);
   
   //Place Fakewin 
   _canvas_move_fake(TREE_WIDTH + 15, 75);
   _canvas_resize_fake(200, 200);
}

void
canvas_redraw(void)
{
   int x, y, w, h;

   //printf("DRAW ALL\n");
   if (etk_string_length_get(Cur.group))
   {
      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&x,&y,&w,&h);
      
      //Ensure we respect min e max group size
      _canvas_resize_fake(w, h);
      evas_object_geometry_get(EV_fakewin,&x,&y,&w,&h);
      
      //place edje editing object
      evas_object_move(edje_o, x, y);
      evas_object_resize(edje_o, w+1, h);

      evas_object_show(EV_fakewin);
      evas_object_show(EV_movebox);
      evas_object_raise(EV_fakewin);
      evas_object_raise(EV_movebox);
     // evas_object_raise(focus_handler);
      
      if (etk_string_length_get(Cur.part))
      {
         _canvas_draw_focus();
         return;
      }
      
   }else
   {
      evas_object_hide(EV_fakewin);
      evas_object_hide(EV_movebox);
   }
   evas_object_hide(EV_rel1X_parent_handler);
   evas_object_hide(EV_rel1Y_parent_handler);
   evas_object_hide(EV_rel2X_parent_handler);
   evas_object_hide(EV_rel2Y_parent_handler);
   evas_object_hide(EV_rel1_handler);
   evas_object_hide(EV_rel2_handler);
   evas_object_hide(EV_move_handler);
   evas_object_hide(EV_focus_handler);
   evas_object_hide(EV_gradient_line);
   evas_object_hide(EV_grad1_handler);
   evas_object_hide(EV_grad2_handler);
}
