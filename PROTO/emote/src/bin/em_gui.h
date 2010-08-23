#ifdef EM_TYPEDEFS

typedef struct _Em_Gui Em_Gui;

#else
# ifndef EM_GUI_H
#  define EM_GUI_H

struct _Em_Gui 
{
   Em_Object em_obj_inherit;

   Evas_Object *win, *o_tb;
   Evas_Object *o_proto;
};

EM_INTERN int em_gui_init(void);
EM_INTERN int em_gui_shutdown(void);

# endif
#endif
