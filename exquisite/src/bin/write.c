#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static void _help(void);

static int wait_mode = 0;
static double wait_time = 10.0;

int
main(int argc, char **argv)
{
   int fd;
   char *fifo;
   
   if ((argc == 3) && (!strcmp(argv[1], "-wait")))
     {
	wait_mode = 1;
	wait_time = atof(argv[2]);
     }
   if (argc != 2)
     {
	_help();
	return 0;
     }
   fifo = getenv("EXQUISITE_IPC");
   if (!fifo) fifo = "/tmp/exquisite-fifo";
   fd = open(fifo, O_WRONLY);
   if ((wait_mode) && (fd < 0))
     {
        int left = wait_time;
        
        while (left > 0)
          {
             left--;
             sleep(1);
             fd = open(fifo, O_WRONLY);
             if (fd >= 0) break;
          }
     }
   if (fd < 0)
     {
	_help();
	return 0;
     }
   write(fd, argv[1], strlen(argv[1]));
   write(fd, "\n", 1);
   close(fd);
   return 0;
}

static void
_help(void)
{
   printf("Usage:\n"
	  "  -h            This help\n"
	  "  -wait N       Wait up to N seconds for exquisite's socket to exist then exit\n"
	  "  QUIT          Tell splash to exit immediately\n"
	  "  PROGRESS N    Indicate boot progress is at N percent\n"
	  "  MSG X         Display string message X\n"
	  "  TITLE X       Diplsay title string X\n"
	  "  END           Shut down splash gracefully and exit when done\n"
	  "  TICK          Send hearbeat tick to splash\n"
	  "  PULSATE       Set exquisite into pulsate mode\n"
          "  TEXT X        Add a line of text to the text box\n"
          "  TEXT-URGENT X Add a line of text even in quiet mode\n"
          "  STATUS  X     Set a general status for the last line of text\n"
          "  SUCCESS X     Set a success status for the last line of text\n"
          "  FAILURE X     Set a failure status for the last line of text\n"
          "  CLEAR         Clear all text and hide text box\n"
          "  TIMEOUT N     Exquisite will timeout in N seconds if no commands recv'd\n"
	  "\n"
	  "Notes:\n"
	  "\n"
	  "You can also set the EXQUISITE_IPC environment variable\n"
	  "to give a fill path to any file to be used as the base\n"
	  "filename of the IPC socket to be used between exquisite\n"
	  "and exquisitie-write for communication.\n"
	  );
}
