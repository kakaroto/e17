#include <Elementary.h>

// FIXME: things that could be added:
// * set alarm text message
// * select alarm ring sound file

// config - simple text file in $HOME/.alarm-config. save and load
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
   FILE *f;
   char *home;
   char buf[4096];
   int on, hours, repeat, minutes,job, count;
   
   home = getenv("HOME");
   if (!home) home = "/";
   snprintf(buf, sizeof(buf), "%s/.alarm-config", home);
   f = fopen(buf, "r");
   if (!f) return;
   if (fscanf(f, "%i %i %i %i %i\n", 
	      &on, &repeat, &hours, &minutes, &job) == 5)
     {
	alm.on = on;
	alm.repeat = repeat;
	alm.hours = hours;
	alm.minutes = minutes;
	alm.job = job;
     }
   fclose(f);
}

static void
save_alarm(void)
{
   FILE *f;
   char *home;
   char buf[4096];
   
   home = getenv("HOME");
   if (!home) home = "/";
   snprintf(buf, sizeof(buf), "%s/.alarm-config", home);
   f = fopen(buf, "w");
   if (!f) return;
   fprintf(f, "%i %i %i %i %i\n",
	   alm.on, alm.repeat, alm.hours, alm.minutes, alm.job);
   fclose(f);
}

static void
clear_alarm(void)
{
   char buf[4096];
   
   if (alm.job == 0) return;
   snprintf(buf, sizeof(buf), "waker del %i", alm.job);
   system(buf);
   alm.job = 0;
}

static void
set_alarm(void)
{
   FILE *f;
   char buf[1024];

   snprintf(buf, sizeof(buf), 
	    "echo 'alarm -activate' | waker add %i:%02i:00 1 X > /tmp/alarm-waker-out", 
	    alm.hours, alm.minutes);
   system(buf);
   f = fopen("/tmp/alarm-waker-out", "r");
   if (f)
     {
	unlink("/tmp/alarm-waker-out");
	while (fgets(buf, sizeof(buf), f))
	  {
	     if (!strncmp(buf, "job ", 4)) alm.job = atoi(buf + 4);
	  }
	fclose(f);
     }
}

// generic callback - delete any window (close button/remove) and it just exits
static void
on_win_del_req(void *data, Elm_Win *win, Elm_Cb_Type type, void *info)
{
   elm_exit();
}

// if user changed the time in the clock-settings (editable) then record
static void
on_clock_changed(void *data, Elm_Clock *cloc, Elm_Cb_Type type, void *info)
{
   alm.hours = cloc->hrs;
   alm.minutes = cloc->min;
}

// press ok to save time and set up alarm
static void
on_button_activate(void *data, Elm_Button *bt, Elm_Cb_Type type, void *info)
{
   clear_alarm();
   if (alm.on) set_alarm();
   save_alarm();
   elm_exit();
}

// alarm main window - setup
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
   win->title_set(win, "Set Alarm");
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
   cloc->hrs =  alm.hours;
   cloc->min = alm.minutes;
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

// for playing the sound - quick hack to just run aplay and loop/ this could 
// definitely become a toolkit call at some point?
static Ecore_Exe *audio_exe = NULL;
static Ecore_Event_Handler *child_exit_handler = NULL;
static Elm_Win *alarm_win = NULL;

static void alarm_sound(void);
static void alarm_sound_end(void);

static int
child_exit(void *data, int type, Ecore_Exe_Event_Del *event)
{
   if ((event->exe) && (event->exe == audio_exe)) alarm_sound();
   return 1;
}

static void
alarm_sound(void)
{
   char buf[4096];

   if (!child_exit_handler)
     child_exit_handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, 
						  child_exit, NULL);
   snprintf(buf, sizeof(buf), "aplay %s/sounds/phone.wav", PACKAGE_DATA_DIR);
   audio_exe = ecore_exe_run(buf, NULL);
}

static void
alarm_sound_end(void)
{
   if (audio_exe)
     {
	ecore_exe_interrupt(audio_exe);
	ecore_exe_free(audio_exe);
	audio_exe = NULL;
     }
}

static void
on_set_button_activate(void *data, Elm_Button *bt, Elm_Cb_Type type, void *info)
{
   if (alarm_win) alarm_win->del(alarm_win);
   alarm_win = NULL;
   alarm_sound_end();
   clear_alarm();
   alm.on = 1;
   save_alarm();
   create_main_win();
}

static void
on_alarm_button_activate(void *data, Elm_Button *bt, Elm_Cb_Type type, void *info)
{
   alarm_sound_end();
   elm_exit();
}

static void
create_alarm_win(void)
{
   Elm_Win *win;
   Elm_Bg *bg;
   Elm_Frame *frame;
   Elm_Box *box, *subbox;
   Elm_Toggle *toggle;
   Elm_Clock *cloc;
   Elm_Button *button;
   Elm_Pad *pad;
   Elm_Label *label;
   
   win = elm_win_new();
   win->name_set(win, "alarm");
   win->title_set(win, "Alarm");
   win->autodel = 0;
   win->cb_add(win, ELM_CB_DEL_REQ, on_win_del_req, NULL);
   
   alarm_win = win;

   bg = elm_bg_new(win);
   bg->show(bg);
   
   box = elm_box_new(win);
   box->expand_x = 1;
   box->expand_y = 1;
   elm_widget_sizing_update(box);
   box->show(box);
   
   frame = elm_frame_new(win);
   frame->text_set(frame, "Message");
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

   label = elm_label_new(win);
   label->text_set(label, 
		   "<br>"
		   "Alarm Message<br>"
		   "<br>");
   label->fill_x = 0;
   label->expand_x = 0;
   label->expand_y = 0;
   label->fill_y = 0;
   subbox->pack_end(subbox, label);
   elm_widget_sizing_update(label);
   label->show(label);

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
   cloc->seconds = 1;
   cloc->time_update(cloc);
   cloc->cb_add(cloc, ELM_CB_CHANGED, on_clock_changed, NULL);
   subbox->pack_end(subbox, cloc);
   elm_widget_sizing_update(cloc);
   elm_widget_sizing_update(cloc);
   cloc->show(cloc);
   
   button = elm_button_new(win);
   button->text_set(button, "Set new alarm");
   button->cb_add(button, ELM_CB_ACTIVATED, on_set_button_activate, NULL);
   elm_widget_sizing_update(button);
   button->show(button);
   
   pad = elm_pad_new(win);
   pad->fill_x = 1;
   pad->fill_y = 0;
   pad->expand_x = 1;
   pad->expand_y = 0;
   pad->align_x = 0.5;
   pad->align_y = 0.0;
   box->pack_end(box, pad);
   box->layout_update(box);
   pad->child_add(pad, button);
   elm_widget_sizing_update(pad);
   pad->show(pad);
   
   elm_widget_sizing_update(button);
   elm_widget_sizing_update(box);
   
   button = elm_button_new(win);
   button->text_set(button, "OK");
   button->cb_add(button, ELM_CB_ACTIVATED, on_alarm_button_activate, NULL);
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
     }
   load_alarm();
   if (activate)
     {
	clear_alarm();
	alm.job = 0;
	if (alm.on)
	  {
	     if (alm.repeat) set_alarm();
	     else alm.on = 0;
	  }
	save_alarm();
	create_alarm_win();
	alarm_sound();
     }
   else
     create_main_win();
   elm_run();
   elm_shutdown();
   return 0; 
}
