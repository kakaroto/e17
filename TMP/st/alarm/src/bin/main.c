#include "main.h"

int
main(int argc, char **argv)
{
   Elm_Win *win;

   /* init Elementary (all Elementary calls begin with elm_ and all data
    * types, enums and macros will be Elm_ and ELM_ etc. */
   elm_init(argc, argv);

   win = elm_win_new(); /* create a window */
   win->title_set(win, "Alarm"); /* set the title */
   win->show(win); /* show it */
   
   elm_run(); /* and run the program now  and handle all events etc. */
   
   elm_shutdown(); /* clean up and shut down */
   return 0;
}
