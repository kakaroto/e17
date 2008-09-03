#include <Elementary.h>

/* This is a test app (that is also functional) to test the api and serve
 * as an example
 */

static void
on_win_resize(void *data, Elm_Win *win, Elm_Callback_Type type, void *info)
{
   /* window is resized */
   printf("resize to: %ix%i\n", win->w, win->h);
}

static void
on_win_del_req(void *data, Elm_Win *win, Elm_Callback_Type type, void *info)
{
   /* because autodel is on - after this callback the window will be deleted */
   /* but to be explicit - exit the app when window is closed */
   elm_exit();
}

int
main(int argc, char **argv)
{
   Elm_Win *win;

   /* init Elementary (all Elementary calls begin with elm_ and all data
    * types, enums and macros will be Elm_ and ELM_ etc.) */
   elm_init(argc, argv);

   win = elm_win_new(); /* create a window */
   win->name_set(win, "main"); /* set the window name - used by window 
				* manager. make it uniqie for windows within 
				* this application */
   win->title_set(win, "Alarm"); /* set the title */
   win->autodel = 0; /* dont auto delete the window if someone closes it.
		      * this means the del+req handler has to delete it. by
		      * default it is on */
   /* add a callback that is called when the user tries to close the window */
   win->callback_add(win, ELM_CALLBACK_DEL_REQ, on_win_del_req, NULL);
   /* add a callback that gets called when the window is resized */
   win->callback_add(win, ELM_CALLBACK_RESIZE, on_win_resize, NULL);
   
   win->show(win); /* show the window */
   
   elm_run(); /* and run the program now  and handle all events etc. */
   
   elm_shutdown(); /* clean up and shut down */
   return 0; 
}
