#include "epplet.h"

#define EPPLET_NAME 	"E-Exec"
#define EPPLET_VERSION 	"0.4"
#define EPPLET_INFO	"Exec a command given by the user"

#define MAX_HIST_LEN	15

Epplet_gadget       textbox = NULL;
Epplet_gadget       history_popup = NULL;
char               *command_history[MAX_HIST_LEN];
int                 current_command = 0;
int                 num_commands = 0;

static void         cb_close(void *data);
static void         run_contents(void *data);
static void         change_textbox(void *data);

static void
cb_close(void *data)
{
   Epplet_cleanup();
   Epplet_unremember();
   exit(0);
   data = NULL;
}

static void
run_contents(void *data)
{
   char               *command = Epplet_textbox_contents(textbox);
   int                 i;

   if (command && strlen(command))
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

	Epplet_add_popup_entry(history_popup, command, NULL, change_textbox,
			       strdup(command));
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

int
main(int argc, char *argv[])
{
   atexit(Epplet_cleanup);

   Epplet_Init(EPPLET_NAME, EPPLET_VERSION, EPPLET_INFO, 5, 3, argc, argv, 0);

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 2, 2,
					   12, 12, "CLOSE", 0, NULL, cb_close,
					   NULL));

   Epplet_gadget_show(Epplet_create_label(-12, 2, "E-Exec", 2));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 2, 16,
					   12, 12, "PREVIOUS", 0, NULL,
					   hist_last, NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 16, 16,
					   12, 12, "NEXT", 0, NULL, hist_next,
					   NULL));

   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 60, 16,
					   12, 12, "PLAY", 0, NULL,
					   run_contents, NULL));

   history_popup = Epplet_create_popup();
   Epplet_add_popup_entry(history_popup, "-NeverMind-", NULL, NULL, NULL);

   Epplet_gadget_show(Epplet_create_popupbutton(NULL, NULL, 30, 16,
						12, 12, "ARROW_UP",
						history_popup));

   textbox =
      Epplet_create_textbox(NULL, NULL, 2, 32, 76, 14, 2, run_contents, NULL);

   Epplet_gadget_show(textbox);

   Epplet_show();
   Epplet_Loop();
   return 0;
}
