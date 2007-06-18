#ifndef _EVOLVE_GUI_H
#define _EVOLVE_GUI_H

int evolve_gui_init();
void evolve_gui_mouse_down_select_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
void evolve_gui_mouse_down_new_cb(Etk_Object *object, Etk_Event_Mouse_Down *event, void *data);
void evolve_gui_prop_dialog_tables_set(Etk_Widget *g_table, Etk_Widget *p_table, Etk_Widget *c_table, Etk_Widget *s_table);

#endif
