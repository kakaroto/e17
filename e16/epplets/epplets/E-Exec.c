#include "epplet.h"

#define EPPLET_NAME 	"E-Exec"
#define EPPLET_VERSION 	"0.1"
#define EPPLET_INFO	"Exec a command given by the user"

Epplet_gadget       textbox = NULL;

static void cb_close(void *data);
static void run_contents(void *data);

static void
cb_close(void *data)
{
   Epplet_cleanup();
   exit(0);
   data = NULL;
}

static void
run_contents(void *data)
{
   Epplet_spawn_command(Epplet_textbox_contents(textbox));
   return;
   data = NULL;
}

int
main(int argc, char *argv[])
{
   Epplet_gadget       button = NULL;

   atexit(Epplet_cleanup);

   Epplet_Init(EPPLET_NAME, EPPLET_VERSION, EPPLET_INFO, 5, 2, argc, argv, 0);

   button = Epplet_create_button(NULL, NULL, 2, 2,
				 12, 12, "CLOSE", 0, NULL, cb_close, NULL);

   textbox =
      Epplet_create_textbox(NULL, NULL, 2, 15, 76, 16, 1, run_contents, NULL);

   Epplet_gadget_show(textbox);
   Epplet_gadget_show(button);

   Epplet_show();
   Epplet_Loop();
   return 0;
}
