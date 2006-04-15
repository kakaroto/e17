#include "epplet.h"

#define EPPLET_NAME 	"E-Exec"
#define EPPLET_VERSION 	"0.5"
#define EPPLET_INFO	"Exec a command given by the user"

#define MAX_HIST_LEN	50

#define TRUE 1
#define FALSE 0

Epplet_gadget       textbox = NULL;
Epplet_gadget       history_popup = NULL;
char               *command_history[MAX_HIST_LEN];
int                 current_command = 0;
int                 num_commands = 0;
int                 save_history = TRUE, delete_history = FALSE, auto_run =
   FALSE;
Window              config_win = None;

static void         cb_close(void *data);
static void         run_contents(void *data);
static void         change_textbox(void *data);
static void         hist_last(void *data);
static void         hist_next(void *data);
static void         empty_popup(void);
static void         fill_popup(void);
static void         exec_popup(void *data);
static void         cb_ok(void *data);
static void         cb_cancel(void *data);
static void         cb_configure(void *data);

static void
cb_close(void *data)
{
   Epplet_unremember();
   exit(0);
   data = NULL;
}

static void
run_contents(void *data)
{
   char               *command = Epplet_textbox_contents(textbox);
   int                 i;
   int                 command_exists;

   if (!Epplet_popup_entry_get_data(history_popup, 0))
      Epplet_remove_popup_entry(history_popup, 0);

   if (command && strlen(command))
     {
	command_exists = FALSE;
	for (i = 0; i < num_commands; i++)
	  {
	     if (!strcmp(command_history[i], command))
	       {
		  command_exists = TRUE;
	       }
	  }
	if (!command_exists)
	  {
	     if (num_commands < MAX_HIST_LEN)
		command_history[num_commands++] = strdup(command);
	     else
	       {
		  free(command_history[0]);
		  Epplet_remove_popup_entry(history_popup, 1);

		  for (i = 0; i < MAX_HIST_LEN; i++)
		     command_history[i] = command_history[i + 1];

		  command_history[MAX_HIST_LEN - 1] = strdup(command);
	       }

	     current_command = num_commands;

	     Epplet_add_popup_entry(history_popup, command, NULL, exec_popup,
				    strdup(command));
	  }

	if (save_history)
	   Epplet_modify_multi_config("Command", command_history, num_commands);

	Epplet_spawn_command(command);
	Epplet_reset_textbox(textbox);
     }
   return;
   data = NULL;
}

static void
change_textbox(void *data)
{
   char               *s = (char *)data;

   Epplet_change_textbox(textbox, s);
}

static void
exec_popup(void *data)
{
   char               *command = (char *)data;

   if (auto_run)
     {
	Epplet_spawn_command(command);
     }
   else
     {
	change_textbox(command);
     }
   return;
   data = NULL;
}

static void
hist_last(void *data)
{
   if (current_command == 0)
     {
	current_command = num_commands;
	Epplet_reset_textbox(textbox);
     }
   else
     {
	Epplet_change_textbox(textbox, command_history[--current_command]);
     }

   return;
   data = NULL;
}

static void
hist_next(void *data)
{
   if (current_command == num_commands - 1)
     {
	current_command++;
	Epplet_reset_textbox(textbox);
	return;
     }
   else if (current_command >= num_commands)
     {
	current_command = 0;
     }
   else
     {
	current_command++;
     }
   Epplet_change_textbox(textbox, command_history[current_command]);

   return;
   data = NULL;
}

static void
empty_popup(void)
{
   int                 i, num_popup_entries;

   num_popup_entries = Epplet_popup_entry_num(history_popup);
   for (i = num_popup_entries; i > 0; i--)
     {
	Epplet_remove_popup_entry(history_popup, i - 1);
     }

   for (i = 0; i < num_commands; i++)
     {
	free(command_history[i]);
	command_history[i] = NULL;
     }
   num_commands = 0;

   if (save_history)
      Epplet_modify_multi_config("Command", command_history, num_commands);

   Epplet_add_popup_entry(history_popup, "-Empty-", NULL, NULL, NULL);
   return;
}

static void
fill_popup(void)
{
   int                 num_results, i;
   char              **results;

   results = Epplet_query_multi_config("Command", &num_results);
   if (results)
     {
	for (i = 0; i < num_results; i++)
	  {
	     Epplet_add_popup_entry(history_popup, results[i], NULL, exec_popup,
				    strdup(results[i]));
	     command_history[i] = strdup(results[i]);
	     num_commands = i + 1;
	  }
     }
   else
      Epplet_add_popup_entry(history_popup, "-Empty-", NULL, NULL, NULL);

   free(results);
   return;
}

static void
cb_ok(void *data)
{
   char                buff[2];

   Epplet_window_destroy(config_win);
   config_win = None;

   if (delete_history)
      empty_popup();

   sprintf(buff, "%d", save_history);
   Epplet_modify_config("Save_History", buff);
   sprintf(buff, "%d", auto_run);
   Epplet_modify_config("Autorun", buff);

   return;
   data = NULL;
}

static void
cb_cancel(void *data)
{
   Epplet_window_destroy(config_win);
   config_win = None;
   return;
   data = NULL;
}

static void
cb_configure(void *data)
{
   if (config_win)
      return;

   delete_history = FALSE;
   config_win =
      Epplet_create_window_config(220, 76, "E-Exec Configuration", cb_ok, NULL,
				  NULL, NULL, cb_cancel, NULL);
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 6, 6, 12, 12, &save_history, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label(22, 6, "Save command history?", 2));
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 6, 22, 12, 12, &auto_run, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label
		      (22, 22, "Autorun popup selection?", 2));
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 6, 38, 12, 12, &delete_history, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label
		      (22, 38, "Empty command popup now?", 2));
   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

int
main(int argc, char *argv[])
{
   atexit(Epplet_cleanup);

   if ((argc > 1) && (!strcmp("--proper-size", argv[1])))
     {
	Epplet_Init(EPPLET_NAME, EPPLET_VERSION, EPPLET_INFO, 6, 3, argc, argv,
		    0);

	Epplet_gadget_show(Epplet_create_label(-10, 2, "E-Exec", 2));

	textbox =
	   Epplet_create_textbox(NULL, NULL, 2, 32, 92, 14, 2, run_contents,
				 NULL);
     }
   else
     {
	Epplet_Init(EPPLET_NAME, EPPLET_VERSION, EPPLET_INFO, 5, 3, argc, argv,
		    0);
	Epplet_gadget_show(Epplet_create_label(-10, 2, "E-Exec", 2));

	textbox =
	   Epplet_create_textbox(NULL, NULL, 2, 32, 76, 14, 2, run_contents,
				 NULL);
     }

   Epplet_load_config();

   save_history = atoi(Epplet_query_config_def("Save_History", "1"));
   auto_run = atoi(Epplet_query_config_def("Autorun", "0"));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 2, 2,
					   12, 12, "CLOSE", 0, NULL,
					   cb_close, NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 16, 2,
					   12, 12, "CONFIGURE", 0, NULL,
					   cb_configure, NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 2, 16,
					   12, 12, "PREVIOUS", 0, NULL,
					   hist_last, NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 16, 16,
					   12, 12, "NEXT", 0, NULL,
					   hist_next, NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 60, 16,
					   12, 12, "PLAY", 0, NULL,
					   run_contents, NULL));

   history_popup = Epplet_create_popup();
   fill_popup();

   Epplet_gadget_show(Epplet_create_popupbutton(NULL, NULL, 30, 16,
						12, 12, "ARROW_UP",
						history_popup));

   Epplet_gadget_show(textbox);

   Epplet_show();
   Epplet_Loop();
   return 0;
}
