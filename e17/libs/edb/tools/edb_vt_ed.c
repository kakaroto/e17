#include "../src/Edb.h"
#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void the_end(int sig);

static void the_end(int sig)
{
   endwin();
   /* do your non-curses wrapup here */
   exit(0);
}

int
main(int argc, char **argv)
{
   signal(SIGINT, the_end);
   
   initscr();
   keypad(stdscr, TRUE);
   nonl();
   cbreak();
   noecho();
   if (has_colors())
     {
	start_color();
	
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
	init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
     }
   for (;;)
     {
	int c;
	
	c = getch();
     }
   the_end(0);
   return 0;
}
