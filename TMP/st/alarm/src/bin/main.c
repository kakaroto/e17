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
   Elm_Label *label;

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

#if 0   
   scroller = elm_scroller_new(win);
   scroller->show(scroller);
#endif
   
#if 0   
   bg2 = elm_bg_new(win); /* create a test bg for scrolling */
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
#endif
   
   label = elm_label_new(win);
//   scroller->child_add(scroller, label);
   label->text_set(label, 
		   "Hello world<br>"
		   "<br>"
		   "I am a label. I come here to temonstrate how to put<br>"
		   "text into a label, with newlines, even markup to test<br>"
		   "things like <b>bold text</b> where markup can be custom<br>"
		   "and extensible, defined by the theme's textbloxk style<br>"
		   "for the label.<br>"
		   "<br>"
		   "Note that the markup is html-like and things like newline<br>"
		   "chars and tab chars like stdout text are not valid text<br>"
		   "markup mechanisms. Use markup tags instead.<br>"
		   "<br>"
		   "Now we shall borrow some text from another test program<br>"
		   "(Expedite) to put some more tests in here<br>"
		   "<br>"
		   "<style=outline color=#fff outline_color=#000>Styled</> "
		   "<style=shadow shadow_color=#fff8>text</> "
		   "<style=soft_shadow shadow_color=#0002>should</> "
		   "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >go here</> "
		   "<style=far_shadow shadow_color=#0005>as it is</> "
		   "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>within</> "
		   "<style=outline_soft_shadow color=#fff outline_color=#8228 shadow_color=#0002>right tags</> "
		   "<style=far_soft_shadow color=#fff shadow_color=#0002>to make it align to the</> "
		   "<underline=on underline_color=#00f>right hand</> "
		   "<backing=on backing_color=#fff8>side </><backing_color=#ff08>of</><backing_color=#0f08> </>"
		   "<strikethrough=on strikethrough_color=#f0f8>the textblock</>."
		   "<br>"
		   "<underline=double underline_color=#f00 underline2_color=#00f>now we need</> "
		   "to test some <color=#f00 font_size=8>C</><color=#0f0 font_size=10>O</>"
		   "<color=#00f font_size=12>L</><color=#fff font_size=14>O</>"
		   "<color=#ff0 font_size=16>R</><color=#0ff font_size=18> Bla Rai</>"
		   "<color=#f0f font_size=20> Stuff</>."
		   "<br>"
		   "<style=outline color=#fff outline_color=#000>Round about the cauldron go;</> "
		   "In the poison'd entrails throw.<br>"
		   "<style=shadow shadow_color=#fff8>Toad, that under cold stone</> "
		   "Days and nights has thirty-one<br>"
		   "<style=soft_shadow shadow_color=#0002>Swelter'd venom sleeping got,</> "
		   "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >Boil thou first i' the charmed pot.</><br>"
		   "Double, double toil and trouble; "
		   "Fire burn, and cauldron bubble.<br>"
		   "<style=far_shadow shadow_color=#0005>Fillet of a fenny snake,</> "
		   "In the cauldron boil and bake;<br>"
		   "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>Eye of newt and toe of frog,</> "
		   "<underline=on underline_color=#00f>Wool of bat and tongue of dog,</><br>"
		   "<backing=on backing_color=#fff8>Adder's fork and blind-worm's sting,</> "
		   "<underline=double underline_color=#f00 underline2_color=#00f>Lizard's leg and owlet's wing,</><br>"
		   "<color=#808 font_size=20>For a charm of powerful trouble, "
		   "Like a hell-broth boil and bubble.<br>"
		   "Double, double toil and trouble;</> "
		   "Fire burn and cauldron bubble.<br>"
		   "Scale of dragon, tooth of wolf, "
		   "Witches' mummy, maw and gulf<br>"
		   "Of the ravin'd salt-sea shark, "
		   "Root of hemlock digg'd i' the dark,<br>"
		   "Liver of blaspheming Jew, "
		   "Gall of goat, and slips of yew<br>"
		   "Silver'd in the moon's eclipse, "
		   "Nose of Turk and Tartar's lips,<br>"
		   "Finger of birth-strangled babe "
		   "Ditch-deliver'd by a drab,<br>"
		   "Make the gruel thick and slab: "
		   "Add thereto a tiger's chaudron,<br>"
		   "For the ingredients of our cauldron. "
		   "Double, double toil and trouble;<br>"
		   "Fire burn and cauldron bubble. "
		   "Cool it with a baboon's blood,<br>"
		   "Then the charm is firm and good.<br>"
		   "<br>"
		   "Heizölrückstoßabdämpfung fløde pingüino kilómetros cœur déçu l'âme<br>"
		   "plutôt naïve Louÿs rêva crapaüter Íosa Úrmhac Óighe pór Éava Ádhaim<br>"
		   );
   label->show(label);
   label->expand = 1;
   elm_widget_sizing_update(label);
   
   win->show(win); /* show the window */
   
   elm_run(); /* and run the program now  and handle all events etc. */
   
   elm_shutdown(); /* clean up and shut down */
   return 0; 
}
