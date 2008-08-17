#include "main.h"

int main(int argc, char **argv)
{

 theme_read(argc, argv);

 /* 
 ** Most inits only return 1 for now, but there's
 ** nothing wrong with beeing prepared.. ;)
 */

 if (!core_init())
   {
    printf("Can't init core, exitting..\n");
    game_exit(0);
   }

 if (!gfx_init(win_title, win_w, win_h, font_path))
   {
    printf("Can't init gfx, exitting..\n");
    game_exit(0);
   }

 if (!game_init())
   {
    printf("Can't init game, exitting..\n");
    game_exit(0);
   }

 game_main();

 return 0;
}
