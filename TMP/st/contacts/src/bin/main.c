#include <Elementary.h>


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
   Elm_Frame *frame;
   Elm_Box *box, *subbox;
   Elm_Toggle *toggle;
   Elm_Clock *cloc;
   Elm_Button *button;
   Elm_Pad *pad;

   win = elm_win_new();
   win->name_set(win, "main");
   win->title_set(win, "Contacts");
   win->autodel = 0;
   win->cb_add(win, ELM_CB_DEL_REQ, on_win_del_req, NULL);

   bg = elm_bg_new(win);
   bg->show(bg);
   
   box = elm_box_new(win);
   box->expand_x = 1;
   box->expand_y = 1;
   elm_widget_sizing_update(box);
   box->show(box);
   
   frame = elm_frame_new(win);
   frame->text_set(frame, "Contacts");
   frame->expand_y = 0;
   box->pack_end(box, frame);
   elm_widget_sizing_update(frame);
   frame->show(frame);
   
   subbox = elm_box_new(win);
   subbox->expand_x = 0;
   subbox->expand_y = 0;
   frame->child_add(frame, subbox);
   elm_widget_sizing_update(subbox);
   subbox->show(subbox);

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
