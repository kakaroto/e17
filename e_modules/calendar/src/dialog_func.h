#ifdef E_TYPEDEFS
#else
#ifndef DIALOG_FUNC_H
#define DIALOG_FUNC_H
#include "e_mod_main.h"
EAPI void  font_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  color_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  config_dialog(void *data, E_Menu *m, E_Menu_Item *mi);

EAPI void  add_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  add_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  remove_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  remove_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  view_event_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  view_todo_dialog(void *data, E_Menu *m, E_Menu_Item *mi);

EAPI void  file_select_bk_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  file_select_ti_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  file_select_cf_dialog(void *data, E_Menu *m, E_Menu_Item *mi);
EAPI void  e_file_cf_select_cb(E_File_Dialog *dia, char *file, void *data);
EAPI void  e_file_ti_select_cb(E_File_Dialog *dia, char *file, void *data);
EAPI void  e_file_bk_select_cb(E_File_Dialog *dia, char *file, void *data);
#endif
#endif




