#include <Elementary.h>

// press ok to save time and set up alarm
static void
on_button_activate(void *data, Elm_Button *bt, Elm_Cb_Type type, void *info)
{
}


// generic callback - delete any window (close button/remove) and it just exits
static void
on_win_del_req(void *data, Elm_Win *win, Elm_Cb_Type type, void *info)
{
   elm_exit();
}

// contacts main window - setup
static void
create_main_win(void)
{
   Elm_Win *win;
   Elm_Bg *bg;
   Elm_Contactlist *contactlist;
   
   win = elm_win_new();
   win->name_set(win, "main");
   win->title_set(win, "Contacts");
   win->autodel = 0;
   win->cb_add(win, ELM_CB_DEL_REQ, on_win_del_req, NULL);

   bg = elm_bg_new(win);
   bg->show(bg);

   contactlist = elm_contactlist_new(win);
   
   // settings in clist - like editable, etc. etc.
   
   contactlist->show(contactlist);

   win->size_req(win, NULL, 320, 400);
   
   win->show(win);
}

int
main(int argc, char **argv)
{
   int i;
   
   elm_init(argc, argv);
   for (i = 1; i < argc; i++)
     {
	// parse args
     }
   create_main_win();
   elm_run();
   elm_shutdown();
   return 0; 
}
