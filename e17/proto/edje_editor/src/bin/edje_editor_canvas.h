#ifndef _EDJE_EDITOR_EVAS_H_
#define _EDJE_EDITOR_EVAS_H_

Evas_Object *EV_fakewin;               //The simple window implementation
Evas_Object *EV_movebox;               //The object used for moving fakewin
Evas_Object *EV_focus_handler;         //The yellow box around the selected part
Evas_Object *EV_rel1_handler;          //The red point
Evas_Object *EV_rel2_handler;          //The blue point
Evas_Object *EV_grad1_handler;         //The starting gradient point
Evas_Object *EV_grad2_handler;         //The ending gradient point
Evas_Object *EV_gradient_line;         //The line that conjunct start & end in gradient editing
Evas_Object *EV_move_handler;          //The central move arrows
Evas_Object *EV_rel1X_parent_handler;  //The 4 line that show the container for each side of the part
Evas_Object *EV_rel1Y_parent_handler;  //
Evas_Object *EV_rel2X_parent_handler;  //
Evas_Object *EV_rel2Y_parent_handler;  //
Evas_Object *EV_rel1X_align_guide;     //The 4 line (2 blu & 2 red) used for align objects on drag operations
Evas_Object *EV_rel1Y_align_guide;     //
Evas_Object *EV_rel2X_align_guide;     //
Evas_Object *EV_rel2Y_align_guide;     //

void canvas_prepare(void);
void canvas_redraw(void);

#endif
