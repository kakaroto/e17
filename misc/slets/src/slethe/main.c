#include "main.h"

int main(int argc, char **argv)
{

 if (argc != 2)
   {
    printf("Usage:  %s <theme-file>\n", argv[0]);
    return 0;
   }

 theme_read(argv[1]);

 if (!gfx_init(win_title, win_w, win_h, font_path))
   {
    printf("Can't init gfx, exitting..\n");
    view_exit(0);
   }

 if (!view_init())
   {
    printf("Can't init view, exitting..\n");
    view_exit(0);
   }

 printf("%s can only show a theme in a window, for now..\n", argv[0]);

 view_main();

 return 0;
}
