#ifdef EM_TYPEDEFS

typedef struct _Em_Gui Em_Gui;

#else
# ifndef EM_GUI_H
#  define EM_GUI_H

struct _Em_Gui 
{
   Em_Object em_obj_inherit;

   Evas_Object *win;
   Evas_Object *o_chansel;
   Evas_Object *o_chantxt;
   Evas_Object *o_entry;
};

EM_INTERN int em_gui_init(void);
EM_INTERN void em_gui_message_add(const char *text);
EM_INTERN int em_gui_shutdown(void);

# endif
#endif
