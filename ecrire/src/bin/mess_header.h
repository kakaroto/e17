#ifndef _MESS_HEADER_H
#define _MESS_HEADER_H

#include <Evas.h>

struct _Ecrire_Entry {
     Evas_Object *entry;
     Evas_Object *win;
     const char *filename;
     int unsaved;
     Elm_Object_Item *copy_item, *cut_item, *save_item, *paste_item, *undo_item, *redo_item;

     /* Undo stack */
     Eina_List *undo_stack;
     Eina_List *undo_stack_ptr;
     Eina_List *last_saved_stack_ptr;
     Eina_Bool undo_stack_can_merge;
};

typedef struct _Ecrire_Entry Ecrire_Entry;

char *_load_plain(const char *file);
char *_load_file(const char *file);
Eina_Bool _save_markup_utf8(const char *file, const char *text);
Eina_Bool _save_plain_utf8(const char *file, const char *text);

void editor_font_choose(Ecrire_Entry *ent, const char *font, int size);
void editor_save(Ecrire_Entry *ent);

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(x) gettext(x)
#else
# define _(x) (x)
#endif

#endif
