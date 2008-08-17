#include "e_mod_main.h"


static E_Module *emu_module = NULL;
static const char *_emu_module_edje = NULL;

/* The emu commands. */
static char *_commands[] = {
#define EMU_MENU	0
   "emu",                       // Menu
   "menu",
#define EMU_DROPZONE	1
   "dropbear",                  // DND drop zone
   "dropzone",
#define EMU_ICON	2
   "mug",                       // Icon
   "icon",
#define EMU_DIALOG	3
   "jumpspider",                // Basic dialogs
   "dialog",
#define EMU_TEXT	4
   "redback",                   // Text
   "text",
#define EMU_GRAPH	5
   "taipan",                    // Graph
   "graph",
   "end",                       // End of command
   ""
};

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);

/* Parsers. */
static void _emu_parse_command(Emu_Face *emu_face, int command, char *name, int start, int end);
static void _emu_parse_dropzone(Emu_Face *emu_face, char *name, int start, int end);
static void _emu_parse_icon(Emu_Face *emu_face, char *name, int start, int end);
static void _emu_parse_dialog(Emu_Face *emu_face, char *name, int start, int end);
static void _emu_parse_text(Emu_Face *emu_face, char *name, int start, int end);
static void _emu_parse_graph(Emu_Face *emu_face, char *name, int start, int end);
static void _emu_parse_menu(Emu_Face *emu_face, char *name, int start, int end);

/* Support functions. */
//static void _emu_add_face_menu(E_Gadget_Face *face, E_Menu *menu);

/* Ecore_Exe callback functions. */
static int _emu_cb_exe_add(void *data, int type, void *ev);
static int _emu_cb_exe_del(void *data, int type, void *ev);
static int _emu_cb_exe_data(void *data, int type, void *ev);

/* Menu callback functions. */
static void _emu_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _emu_menu_cb_post_deactivate(void *data, E_Menu *m);

static void _emu_menu_cb_action(void *data, E_Menu *m, E_Menu_Item *mi);
static Evas_Bool _emu_menus_hash_cb_free(const Evas_Hash *hash, const char *key, void *data, void *fdata);

//static void _emu_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);

/**
 * @defgroup Emu_Module_Basic_Group Emu module API interface
 *
 * Structures and functions that deal with the module API.
 * These things must exist, otherwise the module is considered to be invalid.
 */

/**
 * Really basic information about your module.
 *
 * @ingroup Emu_Module_Basic_Group
 */
E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,        /* Minimal API version this module expects. */
   "Emu_gadcon"                        /* Title of this module, or NULL to just use the modules name. */
};

static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
     "emu",
     {
	_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
     },
   E_GADCON_CLIENT_STYLE_PLAIN
};

/**
 * Setup basic module.
 *
 * It is called when the module is enabled.
 *
 * @param   m a pointer to your E_Module structure.
 * @return  This becomes m->data, return NULL if this falied, and you won't get enabled.
 * @ingroup Emu_Module_Basic_Group
 */
EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   emu_module = m;
   snprintf(buf, sizeof(buf), "%s/e-module-emu.edj", e_module_dir_get(m));
   _emu_module_edje = evas_stringshare_add(buf);
   /* Set up module's message catalogue */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   e_gadcon_provider_register(&_gadcon_class);

   return m;
}

/**
 * Save module config data.
 *
 * It is called just before a module is disabled, and whenever all modules configs are saved.
 *
 * @param   m a pointer to your E_Module structure.
 * @return  1 for success, 0 for failure.
 * @ingroup Emu_Module_Basic_Group
 */
EAPI int
e_modapi_save(E_Module *m)
{
   return 1;
}

/**
 * Tear down basic module.
 *
 * It is called when the module is disabled.
 * If a module is enabled when it is unloaded, it gets disabled first.
 * m->data = NULL; is done for you after this returns.
 *
 * @param   m a pointer to your E_Module structure.
 * @return  1 for success, 0 for failure.
 * @ingroup Emu_Module_Basic_Group
 */
EAPI int
e_modapi_shutdown(E_Module *m)
{
   emu_module = NULL;
   e_gadcon_provider_unregister(&_gadcon_class);

// FIXME: we really want to do this at unload time.
//   if (_emu_module_edje)   evas_stringshare_del(_emu_module_edje);

   return 1;
}

/**
 * @defgroup Emu_Module_Gadget_Group Emu module E_Gadget interface
 *
 * Structures and functions that deal with the E_Gadget API.
 */

/**
 * Module specific face initialisation.
 *
 * It is called when a new face is created.
 * Currently E_Gadget creates one when you call e_gadget_new(),
 * but doesn't let you create more faces.  I'm hoping to get
 * changed.
 *
 * @param   data the pointer you passed to e_gadget_new().
 * @param   face a pointer to your E_Gadget_Face.
 * @ingroup Emu_Module_Gadget_Group
 */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   E_Gadcon_Client *gcc = NULL;
   Emu_Face *emu_face;
   
   emu_face = E_NEW(Emu_Face, 1);

   if (emu_face)
     {
        Evas_Object *o;
        char buf[4096];

        o = edje_object_add(gc->evas);
        if ((!e_theme_edje_object_set(o, "base/theme/modules/emu", "modules/emu/main")) /*&& (_emu_module_edje)*/)
           edje_object_file_set(o, _emu_module_edje, "modules/emu/main");
        edje_object_signal_emit(o, "passive", "");

        gcc = e_gadcon_client_new(gc, name, id, style, o);
        gcc->data = emu_face;
        emu_face->gcc = gcc;
        emu_face->o_button = o;
        emu_face->menus = NULL;

        emu_face->name = evas_stringshare_add("Emu tester");
        snprintf(buf, sizeof(buf), "%s/bin/emu_client", e_module_dir_get(emu_module));
        emu_face->command = evas_stringshare_add(buf);
        if (emu_face->command)
          {
             emu_face->del = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _emu_cb_exe_del, emu_face);
             emu_face->add = ecore_event_handler_add(ECORE_EXE_EVENT_ADD, _emu_cb_exe_add, emu_face);
             emu_face->read = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _emu_cb_exe_data, emu_face);
             emu_face->exe =
                ecore_exe_pipe_run(emu_face->command, ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_WRITE | ECORE_EXE_PIPE_READ_LINE_BUFFERED
                                   /*| ECORE_EXE_RESPAWN */ , emu_face);
             if (!emu_face->exe)
                e_module_dialog_show(NULL, D_("Enlightenment Emu Module - error"), D_("There is no emu."));
          }

        e_gadcon_client_util_menu_attach(gcc);
        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _emu_face_cb_mouse_down, emu_face);
     }
   
   return gcc;
}


/**
 * Module specific face right click menu creation.
 *
 * It is called when a face is created.
 *
 * @param   data the pointer you passed to e_gadget_new().
 * @param   face a pointer to your E_Gadget_Face.
 * @ingroup Emu_Module_Gadget_Group
 */
//static void
//_emu_face_menu_init(void *data, E_Gadget_Face *face)
//{
//   _emu_add_face_menu(face, face->menu);
//}

/**
 * Module specific face freeing.
 *
 * It is called when a face is freed.
 *
 * @param   data the pointer you passed to e_gadget_new().
 * @param   face a pointer to your E_Gadget_Face.
 * @ingroup Emu_Module_Gadget_Group
 */
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Emu_Face *emu_face;

   emu_face = gcc->data;
   if (emu_face)
     {
        evas_hash_foreach(emu_face->menus, _emu_menus_hash_cb_free, NULL);
        evas_hash_free(emu_face->menus);
        emu_face->menus = NULL;

        if (emu_face->read)
           ecore_event_handler_del(emu_face->read);
        if (emu_face->add)
           ecore_event_handler_del(emu_face->add);
        if (emu_face->del)
           ecore_event_handler_del(emu_face->del);

        if (emu_face->exe)
           ecore_exe_terminate(emu_face->exe);
        if (emu_face->command)
           evas_stringshare_del(emu_face->command);

        evas_object_del(emu_face->o_button);
        E_FREE(emu_face);
     }
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void)
{
   return D_("Emu");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-emu.edj", e_module_dir_get(emu_module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}


/**
 * @defgroup Emu_Module_Parser_Group Emu module command parsers
 *
 * Structures and functions that deal with parsing commands
 */

/**
 * The top level parser.
 *
 * This is the entry point for the parsers, it calls all the others.
 * On the other hand, it assumes that the command structure for a
 * single command has been plucked out and passed to it.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_command(Emu_Face *emu_face, int command, char *name, int start, int end)
{
   switch (command)
     {
     case EMU_MENU:
        _emu_parse_menu(emu_face, name, start, end);
        break;                  // Menu
     case EMU_DROPZONE:
        _emu_parse_dropzone(emu_face, name, start, end);
        break;                  // DND drop zone
     case EMU_ICON:
        _emu_parse_icon(emu_face, name, start, end);
        break;                  // Icon
     case EMU_DIALOG:
        _emu_parse_dialog(emu_face, name, start, end);
        break;                  // Basic dialogs
     case EMU_TEXT:
        _emu_parse_text(emu_face, name, start, end);
        break;                  // Text
     case EMU_GRAPH:
        _emu_parse_graph(emu_face, name, start, end);
        break;                  // Graph
     }
}

/**
 * The dropzone command parser.
 *
 * This parses dropzone a single command.
 * Currently a stub that prints the command.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_dropzone(Emu_Face *emu_face, char *name, int start, int end)
{
   int i;

   for (i = start; i <= end; i++)
     {
/* This is just for testing purposes. */
        printf("EMU CLIENT DROPZONE - %d ", emu_face->lines[i].size);
        printf("%s\n", emu_face->lines[i].line);
     }
}

/**
 * The icon command parser.
 *
 * This parses icon a single command.
 * Currently a stub that prints the command.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_icon(Emu_Face *emu_face, char *name, int start, int end)
{
   int i;

   for (i = start; i <= end; i++)
     {
/* This is just for testing purposes. */
        printf("EMU CLIENT ICON - %d ", emu_face->lines[i].size);
        printf("%s\n", emu_face->lines[i].line);
     }
}

/**
 * The dialog command parser.
 *
 * This parses dialog a single command.
 * Currently a stub that prints the command.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_dialog(Emu_Face *emu_face, char *name, int start, int end)
{
   int i;

   for (i = start; i <= end; i++)
     {
/* This is just for testing purposes. */
        printf("EMU CLIENT DIALOG - %d ", emu_face->lines[i].size);
        printf("%s\n", emu_face->lines[i].line);
     }
}

/**
 * The text command parser.
 *
 * This parses text a single command.
 * Currently a stub that prints the command.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_text(Emu_Face *emu_face, char *name, int start, int end)
{
   int i;

   for (i = start; i <= end; i++)
     {
/* This is just for testing purposes. */
        printf("EMU CLIENT TEXT - %d ", emu_face->lines[i].size);
        printf("%s\n", emu_face->lines[i].line);
     }
}

/**
 * The graph command parser.
 *
 * This parses graph a single command.
 * Currently a stub that prints the command.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_graph(Emu_Face *emu_face, char *name, int start, int end)
{
   int i;

   for (i = start; i <= end; i++)
     {
/* This is just for testing purposes. */
        printf("EMU CLIENT GRAPH - %d ", emu_face->lines[i].size);
        printf("%s\n", emu_face->lines[i].line);
     }
}

/**
 * The menu command parser.
 *
 * This parses menu a single command.
 *
 * This assumes that the emu_face->lines[].line pointers point into
 * a contiguous area of ram.
 *
 * @param   emu_face the face, it includes the input lines that the command is a part of.
 * @param   command the index of the command.
 * @param   name any name that was included with this instance of the command.
 * @param   start index of the start line of the command.
 * @param   end index of the end line of the command.
 * @ingroup Emu_Module_Parser_Group
 */
static void
_emu_parse_menu(Emu_Face *emu_face, char *name, int start, int end)
{
   char *category = NULL;
   int length;

   /* Calculate the length of the menu data. */
   length = (emu_face->lines[end].line + emu_face->lines[end].size) - emu_face->lines[start].line;

   if (length > 0)
     {
        Easy_Menu *menu;

        if (name == NULL)       /* Default sub menu item text is the name of the face. */
           name = (char *)emu_face->name;
        else
          {                     /* The category is after the name. */
             category = name;
             while ((*category != '|') && (*category != '\0'))
                category++;
             if (*category == '|')
                *category++ = '\0';
             else
                category = NULL;
          }

        /* Turn the command data into a menu. */
        menu = easy_menu_add_menus(name, category, emu_face->lines[start].line, length, _emu_menu_cb_action, emu_face->exe);
        if (menu)
          {
             Easy_Menu *old_menu;

             /* Associate this menu with it's category. Only one menu per category. */
             old_menu = evas_hash_find(emu_face->menus, menu->category);
             if (old_menu)
               {                /* Clean up the old one. */
                  emu_face->menus = evas_hash_del(emu_face->menus, menu->category, old_menu);
                  emu_face->menus = evas_hash_del(emu_face->menus, NULL, old_menu);     /* Just to be on the safe side. */
                  e_object_del(E_OBJECT(old_menu->menu->menu));
               }
             /* evas_hash_direct_add is used because we allocate the key ourselves and don't deallocate it until after removing it. */
             emu_face->menus = evas_hash_direct_add(emu_face->menus, menu->category, menu);
          }
     }
}

/**
 * @defgroup Emu_Module_Support_Group Emu module support stuff
 *
 * Structures and functions for general support.
 */

/**
 * Add the config menu for a face.
 *
 * It is called to add a faces config menu to a menu.
 *
 * @param   face the pointer to your face.
 * @param   menu the pointer to your menu.
 * @ingroup Emu_Module_Support_Group
 */
//static void
//_emu_add_face_menu(E_Gadget_Face *face, E_Menu *menu)
//{
//   E_Menu_Item *mi;
//
//   mi = e_menu_item_new(menu);
//   e_menu_item_label_set(mi, D_("Configuration"));
//   e_menu_item_callback_set(mi, _emu_cb_menu_configure, face);
//
//   mi = e_menu_item_new(menu);
//   e_menu_item_separator_set(mi, 1);
//
//   mi = e_menu_item_new(menu);
//   e_menu_item_label_set(mi, D_("Add row"));
//   mi = e_menu_item_new(menu);
//   e_menu_item_label_set(mi, D_("Remove row"));
//   mi = e_menu_item_new(menu);
//   e_menu_item_label_set(mi, D_("Add column"));
//   mi = e_menu_item_new(menu);
//   e_menu_item_label_set(mi, D_("Remove column"));
//
//   if (0)
//      e_menu_item_toggle_set(mi, 1);
//}

//static void
//_emu_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
//{
//   E_Gadget_Face *face;
//   Emu_Face *emu_face;
//
//   face = data;
//   if (!face)
//      return;
//   emu_face = face->data;
//   _config_emu_module(face->con, emu_face->emu);
//}

/**
 * @defgroup Emu_Module_Exe_Group Emu module Ecore_Exe interface
 *
 * Structures and functions that deal with the Ecore_Exe API.
 * For the callbacks, we are using two different methods to 
 * check that this is an event we are interested in.  It is 
 * debatable which is better, but as this is teaching code,
 * both are given to provide examples.
 */

/**
 * Handle starting exe.
 *
 * It is called when the exe starts.
 *
 * @param   data the pointer you passed to ecore_event_handler_add().
 * @param   type the type you passed to ecore_event_handler_add().
 * @param   ev a pointer to the relevant event structure.
 * @return  1 to continue processing this event, 0 to not process this event any further.
 * @ingroup Emu_Module_Exe_Group
 */
static int
_emu_cb_exe_add(void *data, int type, void *ev)
{
   return 1;
}

/**
 * Handle exiting exe.
 *
 * It is called when the exe's exit.
 * The exe itself is free'd after this returns.
 *
 * @param   data the pointer you passed to ecore_event_handler_add().
 * @param   type the type you passed to ecore_event_handler_add().
 * @param   ev a pointer to the relevant event structure.
 * @return  1 to continue processing this event, 0 to not process this event any further.
 * @ingroup Emu_Module_Exe_Group
 */
static int
_emu_cb_exe_del(void *data, int type, void *ev)
{
   Ecore_Exe_Event_Del *event;
   Emu_Face *emu_face;

   emu_face = data;
   event = ev;
   if ((emu_face->exe == event->exe) && (ecore_exe_data_get(event->exe) == emu_face))
     {  /* This is the event we are interested in. */
        emu_face->exe = NULL;

        printf("EMU CLIENT DEL - \n");
        return 0;
     }

   return 1;
}

/**
 * Process received data.
 *
 * It is called when there is data from exe's.
 *
 * @param   data the pointer you passed to ecore_event_handler_add().
 * @param   type the type you passed to ecore_event_handler_add().
 * @param   ev a pointer to the relevant event structure.
 * @return  1 to continue processing this event, 0 to not process this event any further.
 * @ingroup Emu_Module_Exe_Group
 */
static int
_emu_cb_exe_data(void *data, int type, void *ev)
{
   Ecore_Exe_Event_Data *event;
   Emu_Face *emu_face;

   event = ev;
   emu_face = data;
   if ((emu_face->exe == event->exe) && (ecore_exe_data_get(event->exe) == emu_face))
     {                          /* This is the event we are interested in. */
        char *data;

        /* Copy new data to the end of the old data. */
        emu_face->data = realloc(emu_face->data, emu_face->size + event->size);
        data = emu_face->data;
        memcpy(&data[emu_face->size], event->data, event->size);
        emu_face->size += event->size;

        if (event->lines)
          {
             int old_size = 0;
             int new_size;

             /* Find out how many lines there are and make room for the new lines. */
             if (emu_face->lines)
                for (old_size = 0; emu_face->lines[old_size].line != NULL; old_size++)
                   ;
             for (new_size = 0; event->lines[new_size].line != NULL; new_size++)
                ;
             emu_face->lines = realloc(emu_face->lines, (old_size + new_size + 1) * sizeof(Ecore_Exe_Event_Data_Line));
             if (emu_face->lines)
               {
                  int i;
                  int looking = TRUE;
                  char *name = NULL;

                  /* Copy the new sizes to the end. */
                  for (i = 0; event->lines[i].line != NULL; i++)
                    {
                       emu_face->lines[old_size].size = event->lines[i].size;
                       emu_face->lines[old_size].line = NULL;
                       old_size++;
                    }
                  old_size = 0;
                  new_size = 0;
                  /* Scan through all the lines. */
                  for (i = 0; new_size < emu_face->size; i++)
                    {
                       int j;

                       /* Create new line pointer. */
                       emu_face->lines[i].line = &data[new_size];
                       new_size += emu_face->lines[i].size + 1;
                       /* Look for commands. */
                       if (looking)
                         {
                            for (j = 0; _commands[j][0] != '\0'; j++)
                              {
                                 if (strncasecmp(emu_face->lines[i].line, _commands[j], strlen(_commands[j])) == 0)
                                   {    /* Found the beginning of a command. */
                                      char *s;

                                      s = &(emu_face->lines[i].line[strlen(_commands[j])]);
                                      if ((s[0] == '\0') || isspace(s[0]))
                                        {       /* Double check that it wasn't part of a word. */
                                           if (isspace(s[0]))
                                              name = &s[1];
                                           old_size = i;
                                           looking = FALSE;
                                           break;
                                        }
                                   }
                              }
                         }
                       else
                         {
                            if (strcasecmp(emu_face->lines[i].line, "end") == 0)
                              { /* Found a command. */
                                 if ((i - old_size) > 1)        /* Ignore this if there is no data. */
                                    _emu_parse_command(emu_face, j / 2, name, old_size + 1, i - 1);
                                 looking = TRUE;
                                 name = NULL;
                                 old_size = i;
                              }
                         }
                    }

                  if (old_size) /* FIXME: A circular buffer might be a better choice. */
                    {           /* Valid emu commands were found and processed, remove them. */
                       old_size++;
                       new_size = 0;
                       /* Remave old data. */
                       for (i = 0; i < old_size; i++)
                          new_size += emu_face->lines[i].size + 1;
                       memmove(emu_face->data, &data[new_size], emu_face->size - new_size);
                       emu_face->size -= new_size;
                       /* Adjust lines accordingly. */
                       new_size = 0;
                       for (i = old_size; new_size < emu_face->size; i++)
                         {
                            emu_face->lines[i - old_size].line = &data[new_size];
                            emu_face->lines[i - old_size].size = emu_face->lines[i].size;
                            new_size += emu_face->lines[i].size + 1;
                         }
                       emu_face->lines[i - old_size].line = NULL;
                    }
               }
          }
        else
          {
             /* This is just for testing purposes, and should never happen. */
             data = event->data;
             data[event->size - 1] = '\0';
             printf("EMU CLIENT DATA - %s\n", data);
          }

        return 0;
     }

   return 1;
}

/**
 * @defgroup Emu_Module_Menu_Group Emu module menu interface
 *
 * Structures and functions that deal with the e_menu API.
 */

/**
 * Handle clicking on the face.
 *
 * While this callback is not a menu callback, currently all face clicks
 * that do anything popup a menu.  This will change as more emu commands
 * are implemented
 *
 * @param   data the pointer you passed to evas_object_event_callback_add().
 * @param   e not used.
 * @param   obj not used.
 * @param   event_info pointer to the event.
 * @ingroup Emu_Module_Menu_Group
 */
static void
_emu_face_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Emu_Face *emu_face;
   Evas_Event_Mouse_Down *ev;

   emu_face = data;
   ev = event_info;
//   if ((ev->button == 3) && emu_face->face->menu)
//     {                          /* Right click configuration menu. */
//        e_menu_activate_mouse(emu_face->face->menu,
//                              e_zone_current_get(emu_face->face->con), ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_AUTO,
//                              ev->timestamp);
//        e_util_container_fake_mouse_up_all_later(emu_face->face->con);
//     }
   /*else*/ if (ev->button == 1)
     {                          /* Left click default menu. */
        Easy_Menu *menu;
	Evas_Coord x, y, w, h;
	int cx, cy, cw, ch;
	
	evas_object_geometry_get(emu_face->o_button, &x, &y, &w, &h); 
	e_gadcon_canvas_zone_geometry_get(emu_face->gcc->gadcon, &cx, &cy, &cw, &ch);
	x += cx;
	y += cy;

        /* Find the default menu, if there is one. */
        menu = evas_hash_find(emu_face->menus, "");
        if (menu && menu->valid)
          {
	     int dir;
	     
             e_menu_post_deactivate_callback_set(menu->menu->menu, _emu_menu_cb_post_deactivate, emu_face);
	     dir = E_MENU_POP_DIRECTION_AUTO;
	     switch (emu_face->gcc->gadcon->orient)
	       {
		case E_GADCON_ORIENT_TOP:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_BOTTOM:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_LEFT:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_RIGHT:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_CORNER_TL:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_CORNER_TR:
		  dir = E_MENU_POP_DIRECTION_DOWN;
		  break;
		case E_GADCON_ORIENT_CORNER_BL:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_CORNER_BR:
		  dir = E_MENU_POP_DIRECTION_UP;
		  break;
		case E_GADCON_ORIENT_CORNER_LT:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_CORNER_RT:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_CORNER_LB:
		  dir = E_MENU_POP_DIRECTION_RIGHT;
		  break;
		case E_GADCON_ORIENT_CORNER_RB:
		  dir = E_MENU_POP_DIRECTION_LEFT;
		  break;
		case E_GADCON_ORIENT_FLOAT:
		case E_GADCON_ORIENT_HORIZ:
		case E_GADCON_ORIENT_VERT:
		default:
		  dir = E_MENU_POP_DIRECTION_AUTO;
		  break;
	       }
//             e_menu_activate_mouse(menu->menu->menu, e_zone_current_get(emu_face->face->con), ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
	     e_menu_activate_mouse(menu->menu->menu, e_util_zone_current_get(e_manager_current_get()), x, y, w, h, dir, ev->timestamp);
//             e_util_container_fake_mouse_up_all_later(emu_face->face->con);
	     edje_object_signal_emit(emu_face->o_button, "active", "");
	     evas_event_feed_mouse_up(emu_face->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);
          }
     }
}

/**
 * Handle deactivation of the face menu.
 *
 * @param   data the pointer you passed to e_menu_post_deactivate_callback_set().
 * @param   m the face menu.
 * @ingroup Emu_Module_Menu_Group
 */
static void
_emu_menu_cb_post_deactivate(void *data, E_Menu *m)
{
   Emu_Face *emu_face;

   emu_face = data;
   edje_object_signal_emit(emu_face->o_button, "passive", "");
}

/**
 * Handle menu item activation.
 *
 * @param   data the pointer you passed to e_menu_item_callback_set().
 * @param   m the menu.
 * @param   mi the menu item.
 * @ingroup Emu_Module_Menu_Group
 */
static void
_emu_menu_cb_action(void *data, E_Menu *m, E_Menu_Item *mi)
{
   struct _Menu_Item_Data *item;

   item = data;
   if (item->data)
     {
        Ecore_Exe *exe;
        char *action;

        exe = item->data;
        action = strdup(item->action);
        if ((action) && (item->easy_menu->category_data))
          {
             /* Check the category, if its border or fileman, do some % subs on action. */
             if ((item->easy_menu->category) && (strncmp(item->easy_menu->category, "border", 6) == 0))
               {
                  E_Border *bd;

                  bd = item->easy_menu->category_data;
                  if (strcmp(action, "Properties") == 0)
                    {
                       E_FREE(action);
                       border_props_dialog(m->zone->container, bd);
                    }
                  else
                    {
                       char *buffer = NULL, *c;
                       int length, i = 0;

                       /* Do some % subs on action. */
                       length = (strlen(action) * 2) + 1;
                       buffer = realloc(buffer, length);
                       for (c = action; (*c) != '\0'; c++)
                         {
                            if ((*c) == '%')
                              {
                                 char *sub;

                                 c++;
                                 sub = border_props_substitute(bd, (*c));       // Fucked if I know why the compiler bitchs about this.
                                 if (sub)
                                   {
                                      int len;

                                      len = strlen(sub);
                                      if ((i + len + 1) > length)
                                        {
                                           length *= 2;
                                           length += len + 1;
                                           buffer = realloc(buffer, length);
                                        }
                                      memcpy(&buffer[i], sub, len + 1);
                                      i += len;
                                      continue;
                                   }
                                 if ((*c) == '\0')
                                    break;
                              }
                            if (i > length)
                              {
                                 length *= 2;
                                 buffer = realloc(buffer, length);
                              }
                            buffer[i++] = (*c);
                         }
                       buffer[i++] = '\0';
                       printf("\n_emu_menu_cb_action() -> \n%s\nbecame\n%s\n\n", action, buffer);
                       E_FREE(action);
                       action = buffer;
                    }
               }
/* FIXME: I ws just about to go to bed when I noticed this was broken.  I'll fix it later.
             else if ((item->easy_menu->category) && (strncmp(item->easy_menu->category, "fileman/action", 14) == 0))
               {
                  E_Fm_Icon *icon;

                  icon = item->easy_menu->category_data;
               }
             else if ((item->easy_menu->category) && (strncmp(item->easy_menu->category, "fileman", 7) == 0))
               {
                  E_Fm_Smart_Data *sd;

                  sd = item->easy_menu->category_data;
               }
*/
          }
        if ((action) && (action[0] != '\0'))
          {
             ecore_exe_send(exe, action, strlen(action));
             ecore_exe_send(exe, "\n", 1);
          }
        E_FREE(action);
     }
}

/**
 * Handle menu freeing from an Evas_Hash.
 *
 * Just a thin wrapper to cater for evas_hash_foreach().
 * It's only used during freeing of a face for mass menu destruction.
 *
 * @param   hash the Evas_Hash that this menu is in.
 * @param   key the key to the menu in the hash.
 * @param   data a pointer to the data stored in the hash.
 * @param   fdata unused.
 * @ingroup Emu_Module_Menu_Group
 */
static Evas_Bool
_emu_menus_hash_cb_free(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Easy_Menu *menu;

   menu = data;
   if ((menu) && (menu->menu) && (menu->menu->menu))
      e_object_del(E_OBJECT(menu->menu->menu));
   return 1;
}

void
_emu_cb_config_updated(void *data)
{
   /* Call Any Needed Funcs To Let Module Handle Config Changes */
//   _ibar_bar_cb_follower(data);
//   _ibar_bar_cb_width_auto(data);
//   _ibar_bar_cb_iconsize_change(data);
}
