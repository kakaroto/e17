#include <Elementary.h>

// FIXME: we need a "run this at time X daemon"

typedef struct 
{
   int on;
   int repeat;
   int hours;
   int minutes;
   /** dynamic as a result of setting the alarm job **/
   int job;
} Alarm;

static Alarm alm =
{
     0,  // on
     1,  // repeat
     7,  // hours
     30, // minutes
     //
     0 // job # - 0 == no job
};

static void
load_alarm(void)
{
   // FIXME: load config for alarm (time, repeat, on and job #)
}

static void
save_alarm(void)
{
   // FIXME: save alarm config (time, repeat, on and job #)
}

static void
clear_alarm(void)
{
   // FIXME: delete alarm (atrm alm.job)
   if (alm.job == 0) return;
}

static void
set_alarm(void)
{
   // FIXME: set alarm to go off
   FILE *f;
   char buf[1024];

   if (alm.repeat)
     snprintf(buf, sizeof(buf), 
	      "echo 'alarm -activate -repeat' | at -q a %i:%02i",
	      alm.hours, alm.minutes);
   else     
     snprintf(buf, sizeof(buf), 
	      "echo 'alarm -activate' | at -q a %i:%02i", 
	      alm.hours, alm.minutes);
   // doesn't work... as we can't read back job #. shoudl we use at?
   f = popen(buf, "r");
   if (f)
     {
	while (fgets(buf, sizeof(buf), f))
	  {
	     printf("line %s\n", buf);
	     if (!strncmp(buf, "job ", 4)) alm.job = atoi(buf + 4);
	  }
	pclose(f);
     }
   printf("job set for job # %i\n", alm.job);
}

static void
on_win_del_req(void *data, Elm_Win *win, Elm_Cb_Type type, void *info)
{
   elm_exit();
}

static void
on_clock_changed(void *data, Elm_Clock *cloc, Elm_Cb_Type type, void *info)
{
   alm.hours = cloc->hrs;
   alm.minutes = cloc->min;
}

static void
on_button_activate(void *data, Elm_Button *bt, Elm_Cb_Type type, void *info)
{
   
   // FIXME: actually set at job (or delete it)
   printf("ALARM:\n"
	  " on = %i\n"
	  " repeat = %i\n"
	  " hours = %i\n"
	  " minutes = %i\n",
	  alm.on, alm.repeat, alm.hours, alm.minutes);
   if (!alm.on)
     {
	clear_alarm();
	save_alarm();
	return;
     }
   clear_alarm();
   set_alarm();
   save_alarm();
}

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
   win->title_set(win, "Alarm");
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
   frame->text_set(frame, "State");
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

   toggle = elm_toggle_new(win);
   toggle->text_set(toggle, "Alarm");
   toggle->states_text_set(toggle, "ON", "OFF");
   toggle->state_ptr = &(alm.on);
   toggle->layout_update(toggle);
   toggle->expand_y = 0;
   toggle->fill_y = 0;
   subbox->pack_end(subbox, toggle);
   elm_widget_sizing_update(toggle);
   toggle->show(toggle);

   frame = elm_frame_new(win);
   frame->text_set(frame, "Time");
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

   cloc = elm_clock_new(win);
   cloc->expand_x = 0;
   cloc->fill_x = 0;
   cloc->edit = 1;
   cloc->seconds = 0;
   cloc->hrs =  7;
   cloc->min = 30;
   cloc->sec =  0;
   cloc->time_update(cloc);
   cloc->cb_add(cloc, ELM_CB_CHANGED, on_clock_changed, NULL);
   subbox->pack_end(subbox, cloc);
   elm_widget_sizing_update(cloc);
   elm_widget_sizing_update(cloc);
   cloc->show(cloc);
   
   frame = elm_frame_new(win);
   frame->text_set(frame, "Options");
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

   toggle = elm_toggle_new(win);
   toggle->text_set(toggle, "Repeat");
   toggle->states_text_set(toggle, "ON", "OFF");
   toggle->state_ptr = &(alm.repeat);
   toggle->layout_update(toggle);
   toggle->expand_y = 0;
   toggle->fill_y = 0;
   subbox->pack_end(subbox, toggle);
   elm_widget_sizing_update(toggle);
   toggle->show(toggle);
   
   button = elm_button_new(win);
   button->text_set(button, "OK");
   button->cb_add(button, ELM_CB_ACTIVATED, on_button_activate, NULL);
   elm_widget_sizing_update(button);
   button->show(button);
   
   pad = elm_pad_new(win);
   pad->fill_x = 1;
   pad->fill_y = 0;
   pad->expand_x = 1;
   pad->expand_y = 1;
   pad->align_x = 0.5;
   pad->align_y = 1.0;
   box->pack_end(box, pad);
   box->layout_update(box);
   pad->child_add(pad, button);
   elm_widget_sizing_update(pad);
   pad->show(pad);
   
   elm_widget_sizing_update(button);
   elm_widget_sizing_update(box);
   
   win->show(win);
}

int
main(int argc, char **argv)
{
   int i;
   int activate = 0;
   int repeat = 0;
   
   elm_init(argc, argv);
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-activate")) activate = 1;
	else if (!strcmp(argv[i], "-repeat")) repeat = 1;
     }
   load_alarm();
   if (activate)
     {
	// FIXME: show different window with:
	// * alarm label
	// * current time 
	// * snooze button
	// * ok button (closes and sets for repeat, if repeat set)
	// * play sound
	// * set timer for cancel sound, and repeat
	// * 
     }
   else
     create_main_win();
   elm_run();
   elm_shutdown();
   return 0; 
}
