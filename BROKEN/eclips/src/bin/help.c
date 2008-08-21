#include "help.h"
#include "eclipse.h"

void usage(void)
{
   
   fprintf(stderr, "Eclips Version " VERSION
	   "\nThe uber kewl E image viewer.\nHisham Mardam Bey <hisham@hisham.cc>"
	   "\n\nUsage:\n"
	   "\teclipse [OPTIONS] file(s)\n"
	   "\tOPTIONS:\n"
	   "\t\t-d numsecs   Delay to when refreshing webcam or changing\n"
	   "\t                     slideshow image. (default is 5)\n"
	   "\t\t-m s or w    Mode to use, s = slideshow and w = webcam\n"
	   "\t\t-t num       Set default transpareny (between 0 and 255)\n"
	   "\t\t-M num       Set menu transpareny (between 0 and 255)\n"
	   "\t\t-p           Toggle desktop only pseudo-transparency\n"
	   "\t\t-w           Toggle desktop and window pseudo-transparency\n"
	   "\t                     WARNING: very slow!! (currently broke)\n"	   
	   "\t\t-s           Toggle image shadows.\n"
	   "\t\t-R           Display images in random order.\n"	   
	   "\t\t-a num       Threshhold of alphabending when changing images\n"
	   "\t\t-g WxH       Specify window geometry (WIDTHxHEIGHT)\n"
	   "\t\t-T num       Time pause in seconds between each alpha change\n"
	   "\t                     when changing images (can de decimal)\n"
	   "\t\t-x           Borderless window.\n"
	   "\t\t-h           Show this help.\n");
   exit(1);
}
