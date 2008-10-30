#include <Elementary.h>

// FIXME: things that could be added:
// * set alarm text message
// * select alarm ring sound file

// config - simple text file in $HOME/.alarm-config. save and load
typedef struct 
{
   Evas_Bool on; // FIXME: Evas_Bool
   Evas_Bool repeat; // FIXME: Evas_Bool
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
	   (int)alm.on, (int)alm.repeat, alm.hours, alm.minutes, alm.job);
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
	while (fgets(buf, sizeof(buf), f)) alm.job = atoi(buf);
	fclose(f);
	unlink("/tmp/alarm-waker-out");
     }
}

// if user changed the time in the clock-settings (editable) then record
static void
on_clock_changed(void *data, Evas_Object *obj, void *event_info)
{
   elm_clock_time_get(obj, &(alm.hours), &(alm.minutes), NULL);
}

// press ok to save time and set up alarm
static void
on_button_activate(void *data, Evas_Object *obj, void *event_info)
{
   clear_alarm();
   if (alm.on) set_alarm();
   save_alarm();
   elm_exit();
}

// generic callback - delete any window (close button/remove) and it just exits
static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   /* called when my_win_main is requested to be deleted */
   elm_exit(); /* exit the program's main loop that runs in elm_run() */
}

// alarm main window - setup
static void
create_main_win(void)
{
   Evas_Object *win, *bg, *frame, *box, *subbox, *toggle, *cloc, *button;
   
   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Set Alarm");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0); // expand h/v 1/1 (for win this also fills)
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, 1.0, 1.0); // expand h/v 1/1 (for win this also fills)
   elm_win_resize_object_add(win, box);
   
   frame = elm_frame_add(win);
   evas_object_size_hint_weight_set(frame, 1.0, 0.0); // expand h/v 1/0
   evas_object_size_hint_align_set(frame, -1.0, 0.0); // fill h/v 1/0
   elm_frame_label_set(frame, "Alarm");
   
   subbox = elm_box_add(win);
   elm_frame_content_set(frame, subbox);

   toggle = elm_toggle_add(win);
   elm_toggle_label_set(toggle, "Enabled");
   elm_toggle_states_labels_set(toggle, "ON", "OFF");
   elm_toggle_state_pointer_set(toggle, &(alm.on));   
   evas_object_size_hint_weight_set(toggle, 1.0, 0.0); // expand h/v 1/0
   evas_object_size_hint_align_set(toggle, -1.0, 0.0); // fill h/v 1/0
   elm_box_pack_end(subbox, toggle);
   evas_object_show(toggle);
   
   cloc = elm_clock_add(win);
   elm_clock_show_am_pm_set(cloc, 1);
   elm_clock_edit_set(cloc, 1);
   elm_clock_time_set(cloc, alm.hours, alm.minutes, 0);
   evas_object_smart_callback_add(cloc, "changed", on_clock_changed, NULL);
   elm_box_pack_end(subbox, cloc);
   evas_object_show(cloc);
   
   toggle = elm_toggle_add(win);
   elm_toggle_label_set(toggle, "Repeat");
   elm_toggle_states_labels_set(toggle, "ON", "OFF");
   elm_toggle_state_pointer_set(toggle, &(alm.repeat));   
   evas_object_size_hint_weight_set(toggle, 1.0, 0.0); // expand h/v 1/0
   evas_object_size_hint_align_set(toggle, -1.0, 0.0); // fill h/v 1/0
   elm_box_pack_end(subbox, toggle);
   evas_object_show(toggle);
   
   evas_object_show(subbox);
   
   elm_box_pack_end(box, frame);
   evas_object_show(frame);

   cloc = elm_clock_add(win);
   elm_clock_show_am_pm_set(cloc, 1);
   elm_clock_show_seconds_set(cloc, 1);
   elm_box_pack_end(box, cloc);
   evas_object_show(cloc);
   
   button = elm_button_add(win);
   elm_button_label_set(button, "OK");
   evas_object_smart_callback_add(button, "clicked", on_button_activate, NULL);
   evas_object_size_hint_weight_set(button, 1.0, 1.0); // expand h/v 1/0
   evas_object_size_hint_align_set(button, -1.0, 1.0); // fill h/v 1/0, align h/v ?/1.0
   elm_box_pack_end(box, button);
   evas_object_show(button);
   
   evas_object_show(box);
   evas_object_show(win);
}

// for playing the sound - quick hack to just run aplay and loop/ this could 
// definitely become a toolkit call at some point?
static Ecore_Exe *audio_exe = NULL;
static Ecore_Event_Handler *child_exit_handler = NULL;
static Evas_Object *alarm_win = NULL;

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
on_set_button_activate(void *data, Evas_Object *obj, void *event_info)
{
   if (!alarm_win) return;
   evas_object_del(alarm_win);
   alarm_win = NULL;
   alarm_sound_end();
   clear_alarm();
   alm.on = 1;
   save_alarm();
   create_main_win();
}

static void
on_alarm_button_activate(void *data, Evas_Object *obj, void *event_info)
{
   alarm_sound_end();
   elm_exit();
}

static void
create_alarm_win(void)
{
   Evas_Object *win, *bg, *frame, *box, *subbox, *toggle, *cloc, *button, *label;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Alarm");
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0); // expand h/v 1/1 (for win this also fills)
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   
   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, 1.0, 1.0); // expand h/v 1/1 (for win this also fills)
   elm_win_resize_object_add(win, box);
   
   frame = elm_frame_add(win);
   evas_object_size_hint_weight_set(frame, 1.0, 0.0); // expand h/v 1/0
   evas_object_size_hint_align_set(frame, -1.0, 0.0); // fill h/v 1/0
   elm_frame_label_set(frame, "State");
   
   subbox = elm_box_add(win);
   elm_frame_content_set(frame, subbox);

   label = elm_label_add(win);
   elm_label_label_set(label,
		       "<br>"
		       "<b>Alarm Message</b><br>"
		       "<br>");
   elm_box_pack_end(subbox, label);
   evas_object_show(label);

   evas_object_show(subbox);
   
   elm_box_pack_end(box, frame);
   evas_object_show(frame);
   
   frame = elm_frame_add(win);
   evas_object_size_hint_weight_set(frame, 1.0, 0.0); // expand h/v 1/0
   evas_object_size_hint_align_set(frame, -1.0, 0.0); // fill h/v 1/0
   elm_frame_label_set(frame, "Current Time");
   
   subbox = elm_box_add(win);
   elm_frame_content_set(frame, subbox);

   cloc = elm_clock_add(win);
   elm_clock_show_am_pm_set(cloc, 1);
   elm_clock_show_seconds_set(cloc, 1);
   elm_box_pack_end(subbox, cloc);
   evas_object_show(cloc);
   
   evas_object_show(subbox);
   
   elm_box_pack_end(box, frame);
   evas_object_show(frame);
   
   button = elm_button_add(win);
   elm_button_label_set(button, "Set new alarm");
   evas_object_smart_callback_add(button, "clicked", on_set_button_activate, NULL);
   evas_object_size_hint_weight_set(button, 1.0, 1.0); // expand h/v 1/0
   evas_object_size_hint_align_set(button, -1.0, 0.0); // fill h/v 1/0, align h/v ?/1.0
   elm_box_pack_end(box, button);
   evas_object_show(button);
   
   button = elm_button_add(win);
   elm_button_label_set(button, "OK");
   evas_object_smart_callback_add(button, "clicked", on_button_activate, NULL);
   evas_object_size_hint_weight_set(button, 1.0, 1.0); // expand h/v 1/0
   evas_object_size_hint_align_set(button, -1.0, 1.0); // fill h/v 1/0, align h/v ?/1.0
   elm_box_pack_end(box, button);
   evas_object_show(button);
   
   evas_object_show(box);
   evas_object_show(win);
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
