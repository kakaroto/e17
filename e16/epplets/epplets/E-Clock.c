#include "epplet.h"
Epplet_gadget label;
int           mode = 0;

static void cb_timer(void *data);
static void cb_close(void *data);

static void
cb_timer(void *data)
{
   char                tm[64];
   struct tm           tim;
   struct tm          *tim2;
   time_t              t2;
   static char         colon = 0;

   t2 = time(NULL);
   tim2 = localtime(&t2);
   if (tim2)
     {
	memcpy(&tim, tim2, sizeof(struct tm));
	if (colon)
	  {
	     strftime(tm, 63, "%l:%M:%S %p %a %e %b", &tim);
	     colon = 0;
	  }
	else
	  {
	     strftime(tm, 63, "%l %M %S %p %a %e %b", &tim);
	     colon = 1;
	  }
	Epplet_change_label(label, tm);
     }      
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");   
   data = NULL;
}

static void
cb_close(void *data)
{
   Epplet_unremember();
   Esync();
   exit(0);
}

int
main(int argc, char **argv)
{
   Epplet_Init("E-Clock", "0.1", "Enlightenment Clock Epplet",
	       10, 1, argc, argv, 0);
   Epplet_timer(cb_timer, NULL, 0.5, "TIMER");
   Epplet_gadget_show(Epplet_create_button(NULL, NULL, 
					   2, 2, 0, 0, "CLOSE", 0, NULL, 
					   cb_close, NULL));
   Epplet_gadget_show(label = Epplet_create_label(16, 2, "Time", 2));
   Epplet_show();
   Epplet_Loop();
   return 0;
}
