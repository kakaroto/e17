#include <Elementary.h>

/* This is a test app (that is also functional) to test the api and serve
 * as an example
 */

static void
on_win_resize(void *data, Elm_Win *win, Elm_Cb_Type type, void *info)
{
   /* window is resized */
   printf("resize to: %ix%i\n", win->w, win->h);
}

static void
on_win_del_req(void *data, Elm_Win *win, Elm_Cb_Type type, void *info)
{
   /* because autodel is on - after this callback the window will be deleted */
   /* but to be explicit - exit the app when window is closed */
   elm_exit();
}

int
main(int argc, char **argv)
{
   Elm_Win *win;
   Elm_Bg *bg;
   Elm_Scroller *scroller;
   Elm_Bg *bg2;

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
   win->cb_add(win, ELM_CB_DEL_REQ, on_win_del_req, NULL);
   /* add a callback that gets called when the window is resized */
   win->cb_add(win, ELM_CB_RESIZE, on_win_resize, NULL);

   /* our window needs a baground, so ask for one - it will be set with a
    * default bg */
   bg = elm_bg_new(win);
#if 0   
   /* this is a test - shows how to have your own custom wallpaper in
    * your app - don't use this unless you have a very good reason! there
    * is a default and all apps look nicer sharing the default, but if
    * you insist... */
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf), "%s/images/sample_01.jpg", PACKAGE_DATA_DIR);
   bg->file_set(bg, buf, NULL); /* set the bg - the NULL is for special
				 * files that contain multiple images
				 * inside 1 file. not normally used but
				 * might be if you have archive files with
				 * multiple images in them */
#endif   
   bg->show(bg); /* show the bg */

   scroller = elm_scroller_new(win);
   scroller->show(scroller);
   
   bg2 = elm_bg_new(win);
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf), "%s/images/sample_01.jpg", PACKAGE_DATA_DIR);
   bg2->file_set(bg2, buf, NULL); /* set the bg - the NULL is for special
				   * files that contain multiple images
				   * inside 1 file. not normally used but
				   * might be if you have archive files with
				   * multiple images in them */
   bg2->geom_set(bg2, bg2->x, bg2->y, 500, 500);
   scroller->child_add(scroller, bg2);
   bg2->show(bg2);
   
   win->show(win); /* show the window */
   
   elm_run(); /* and run the program now  and handle all events etc. */
   
   elm_shutdown(); /* clean up and shut down */
   return 0; 
}
