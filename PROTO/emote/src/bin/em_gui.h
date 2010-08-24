#ifdef EM_TYPEDEFS

typedef struct _Em_Gui Em_Gui;

#else
# ifndef EM_GUI_H
#  define EM_GUI_H

struct _Em_Gui 
{
   Em_Object em_obj_inherit;

   Eina_Hash *chantxt;
   Evas_Object *win, *o_tb;
   Evas_Object *o_chansel;
   Evas_Object *o_chantxt;
   Evas_Object *o_entry;
};

EM_INTERN int em_gui_init(void);
EM_INTERN void em_gui_server_add(const char *server);
EM_INTERN void em_gui_channel_add(const char *server, const char *channel);
EM_INTERN void em_gui_message_add(const char *server, const char *channel, const char *text);
EM_INTERN int em_gui_shutdown(void);

# endif
#endif
