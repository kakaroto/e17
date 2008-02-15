#include "config.h"
#include <Etk.h>
#include <Edje.h>
#include "main.h"
#include "interface.h"
#include "evas.h"
#include <Edje_Edit.h>

#define MAIN_LEFT 20
#define MAIN_TOP 27

void
on_Drag(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //printf("ON DRAG\n");
   Evas_Coord parentx,parenty,parentw,parenth;
   Evas_Coord x,y;
   int mouse_x, mouse_y;

   // MoveBox
   if ((int)data == DRAG_MOVEBOX)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin),&x, &y);
      ev_move_fake(x-30,y+8);//TODO
   }

   // Resize fake win
   if ((int)data == DRAG_MINIARROW)
   {
      edje_object_part_geometry_get(EV_fakewin,"miniarrow", &x, &y, NULL, NULL);
      if (x < 0) x = 0;
      if (y < 0) y = 0;
      ev_resize_fake((int)x,(int)y);
   }
   // Move Rel1 Point
   if ((int)data == DRAG_REL1)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin),
                                 &mouse_x, &mouse_y);

      //Calc the rel1_relative_x value
      evas_object_geometry_get(rel1X_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel1_relative_x_set(edje_o, Cur.part->string, 
                  Cur.state->string, (float)(mouse_x - parentx)/(float)parentw);
    
      //Calc the rel1_relative_y value
      evas_object_geometry_get(rel1Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);

      edje_edit_state_rel1_relative_y_set(edje_o, Cur.part->string,
                  Cur.state->string,(float)(mouse_y - parenty)/(float)parenth);

      UpdatePositionFrame();
   }
   // Move Rel2 Point
   if ((int)data == DRAG_REL2)
   {
      evas_pointer_output_xy_get(ecore_evas_get(UI_ecore_MainWin), 
                                 &mouse_x, &mouse_y);

      //Calc the rel2_relative_x value
      evas_object_geometry_get(rel2X_parent_handler,
                                &parentx, &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_x_set(edje_o, Cur.part->string,
                  Cur.state->string, (float)(mouse_x - parentx)/(float)parentw);

      //Calc the rel2_relative_y value
      evas_object_geometry_get(rel2Y_parent_handler, &parentx,
                                &parenty, &parentw, &parenth);
      edje_edit_state_rel2_relative_y_set(edje_o, Cur.part->string,
                  Cur.state->string, (float)(mouse_y - parenty)/(float)parenth);

      UpdatePositionFrame();
   }
   ev_redraw();

   //printf("Drag %s - ",source);
   //printf("Position %i, %i\n", x, y);
}

void
on_StartDrag(void *data, Evas_Object *o, const char *em, const char *src)
{
   //printf("Start Drag\n");

   if ((int)data == DRAG_REL1) 
      edje_object_signal_callback_add(rel1_handler,"DRAG","rel1",
                                      on_Drag,(void*)DRAG_REL1);
   if ((int)data == DRAG_REL2) 
      edje_object_signal_callback_add(rel2_handler,"DRAG","rel2",
                                      on_Drag,(void*)DRAG_REL2);
   /* THIS COMMENT DISABLE THE FAKEWIN MOVE ABILITY (don't like it) */
   if ((int)data == DRAG_MOVEBOX)
       evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_MOVE,
                                        on_Drag, (void*)DRAG_MOVEBOX);
   
}

void
on_EndDrag(void *data, Evas_Object *o, const char *emission, const char *source)
{
   //printf("End Drag\n");
   if ((int)data == DRAG_REL1)
      edje_object_signal_callback_del(rel1_handler,"DRAG","rel1",on_Drag);
   if ((int)data == DRAG_REL2)
      edje_object_signal_callback_del(rel2_handler,"DRAG","rel2",on_Drag);
   if ((int)data == DRAG_MOVEBOX)
      evas_object_event_callback_del(EV_movebox, EVAS_CALLBACK_MOUSE_MOVE, on_Drag);
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
   focus_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(focus_handler, EdjeFile, "FocusH");

   //Create Rel1 handler
   rel1_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(rel1_handler, EdjeFile, "Rel1H");
   edje_object_signal_callback_add(rel1_handler,"START_DRAG","rel1",
                                   on_StartDrag,(void*)DRAG_REL1);
   edje_object_signal_callback_add(rel1_handler,"END_DRAG","rel1",
                                   on_EndDrag,(void*)DRAG_REL1);

   //Create Rel2 handler
   rel2_handler = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(rel2_handler,EdjeFile, "Rel2H");
   edje_object_signal_callback_add(rel2_handler,"START_DRAG","rel2",
                                   on_StartDrag,(void*)DRAG_REL2);
   edje_object_signal_callback_add(rel2_handler,"END_DRAG","rel2",
                                   on_EndDrag,(void*)DRAG_REL2);

   //Create Parent Handlers
   rel1X_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(rel1X_parent_handler, 255, 0, 0, 200);
   
   rel1Y_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(rel1Y_parent_handler, 255, 0, 0, 200);
    
   rel2X_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(rel2X_parent_handler, 0, 0, 255, 200);

   rel2Y_parent_handler = evas_object_line_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(rel2Y_parent_handler, 0, 0, 255, 200);

   //Create engrave canvas
   engrave_canvas = engrave_canvas_new(ecore_evas_get(UI_ecore_MainWin));

   // Create Fake win
   EV_fakewin = edje_object_add(ecore_evas_get(UI_ecore_MainWin));
   edje_object_file_set(EV_fakewin, EdjeFile, "FakeWin");
   edje_object_signal_callback_add(EV_fakewin,"DRAG","miniarrow",
                                   on_Drag, (void*)DRAG_MINIARROW);
   edje_object_signal_callback_add(EV_fakewin,"END_DRAG","miniarrow",
                                   on_EndDrag, (void*)DRAG_MINIARROW);

   // Create MoveBox
   EV_movebox = evas_object_rectangle_add(ecore_evas_get(UI_ecore_MainWin));
   evas_object_color_set(EV_movebox, 0, 0, 0, 0);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_DOWN,
                                  on_StartDrag, (void*)DRAG_MOVEBOX);
   evas_object_event_callback_add(EV_movebox, EVAS_CALLBACK_MOUSE_UP,
                                  on_EndDrag, (void*)DRAG_MOVEBOX);

   //Place Fakewin 
   ev_move_fake(TREE_WIDTH+15,75);
   ev_resize_fake(200,200);
}

void
ev_draw_focus(void)
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
       
      evas_object_move(focus_handler,
            px + wx - o1x - 2, 
            py + wy - o1y - 2);

      evas_object_resize(focus_handler,
            pw + o1x - o2x + 2, 
            ph + o1y - o2y + 2);
      evas_object_raise(focus_handler);
      evas_object_show(focus_handler);
   }else
      evas_object_hide(focus_handler);

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
      evas_object_move(rel1_handler,
                       fx + px - o1x - 2,
                       fy + py - o1y - 2);
      evas_object_show(rel1_handler);
      evas_object_raise(rel1_handler);

      evas_object_move(rel2_handler,
                       fx + px + pw - o2x - 4,
                       fy + py + ph - o2y - 4);
      evas_object_show(rel2_handler);
      evas_object_raise(rel2_handler);
       
      //draw Rel1X_ParentH (top line)
      if((rel = edje_edit_state_rel1_to_x_get(edje_o, Cur.part->string, Cur.state->string)))
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
      evas_object_line_xy_set(rel1X_parent_handler, ParentX, ParentY, 
                                                    ParentX+ParentW, ParentY+1);
      
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
      evas_object_line_xy_set(rel1Y_parent_handler, ParentX, ParentY,
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
      evas_object_line_xy_set(rel2X_parent_handler, ParentX, ParentY+ParentH,
                                          ParentX+ParentW+1, ParentY+ParentH+1);

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
      evas_object_line_xy_set(rel2Y_parent_handler, ParentX+ParentW, ParentY,
                                            ParentX+ParentW, ParentY+ParentH);

      evas_object_raise(rel1X_parent_handler);
      evas_object_show(rel1X_parent_handler);
      evas_object_raise(rel1Y_parent_handler);
      evas_object_show(rel1Y_parent_handler);
      evas_object_raise(rel2X_parent_handler);
      evas_object_show(rel2X_parent_handler);
      evas_object_raise(rel2Y_parent_handler);
      evas_object_show(rel2Y_parent_handler);
      evas_object_raise(focus_handler);
      evas_object_raise(rel1_handler);
      evas_object_raise(rel2_handler);
      
   }else{ //none selected, hide all
      edje_object_signal_emit(rel1_handler,"REL1_HIDE","edje_editor");
      edje_object_signal_emit(focus_handler,"REL2_HIDE","edje_editor");
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
   evas_object_resize(EV_movebox, w, 10);
   edje_object_part_drag_value_set(EV_fakewin, "miniarrow",
                                   (double)w, (double)h);
   char buf[100];
   snprintf(buf,100,"Current size: %d x %d",w,h);
   etk_label_set(ETK_LABEL(UI_CurrentGroupSizeLabel), buf);
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
   //printf("PART: %s\n", Cur.part->string);
   if (etk_string_length_get(Cur.part))
   {
      //Get the geometry of ETK_canvas
      //evas_object_geometry_get(EV_canvas_bg,&x,&y,&w,&h);

      //Get the geometry of fakewin
      evas_object_geometry_get(EV_fakewin,&x,&y,&w,&h);

      //place engrave canvas
      evas_object_move(edje_o, x, y);
      evas_object_resize(edje_o, w+1, h);

      evas_object_show(EV_fakewin);
      evas_object_show(EV_movebox);
      evas_object_raise(EV_fakewin);
      evas_object_raise(EV_movebox);
     // evas_object_raise(focus_handler);
      
      ev_draw_focus();

   }else
   {
      evas_object_hide(engrave_canvas);
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
