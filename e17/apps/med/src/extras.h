

#ifndef __EXTRAS_H__
#define __EXTRAS_H__ 1

typedef enum {
  MED_SUBMENU,
  MED_MENU_ITEM
} MED_ENUM;



#ifdef __cplusplus
extern "C" {
#endif

#include "menu.h"
#include "entry.h"

  void pop_menu(int,int);

  void med_entry_text( const char *text, const char *exe, const char *icon, const char *script );

  void med_setup_entries(Evas e);

  void med_display( MED_ENUM type, E_Menu_Item *mi);

  void med_commit_entry_changes(E_Entry *_entry);

  void med_delete_mi_at(E_Menu_Item *mi);

  void med_insert_mi_at(E_Menu_Item *mi, med_tool_type tt);

  void med_move_focus( E_Entry *entry, Ev_Key_Down *e);


#ifdef __cplusplus
}
#endif
#endif __EXTRAS_H__

//eof
