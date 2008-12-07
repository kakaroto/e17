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

#include <Ecore_Con.h>

static void _help(void);

static int wait_mode = 0;
static int wait_time = 10;
static const char *method = NULL;

int
main(int argc, char **argv)
{
   int i;
   char *file;
   char buf[4096];

   for (i=1; i < argc-1; ++i)
     {
        if (!strcmp(argv[i], "-h"))
             _help();
        else if (!strcmp(argv[i], "-wait"))
          {
             if (++i > argc) _help();
             wait_mode = 1;
             if (!(wait_time = (int)(atof(argv[i]))))
               {
                  printf("Invalid wait time\n");
                  return 1;
               }
          }
        else if (!strcmp(argv[i], "-ipc"))
          {
             if (i++ > argc) _help();
             method = strdup(argv[i]);
          }
     }

   file = getenv("EXQUISITE_IPC");
   if (!file || !file[0]) file = "/tmp/exquisite";
   
   if (!method) method = getenv("EXQUISITE_IPC_METHOD");
   
   // Assume last arg is command to send
   snprintf(buf, sizeof(buf), "%s\n", argv[argc-1]);
        
   if (!method || !method[0] || !strcmp(method, "fifo"))
     {
        int fd;

        fd = open(file, O_WRONLY);
        if ((wait_mode) && (fd < 0))
          {
             int left = wait_time;

             while (left > 0)
               {
                  left--;
                  sleep(1);
                  fd = open(file, O_WRONLY);
                  if (fd >= 0) break;
               }
          }
        if (fd < 0) 
          {
             printf("Cannot find fifo at %s.\n", file);
             return 1;
          }
        write(fd, buf, strlen(buf));
        close(fd);
     }
   else if (!strcmp(method, "socket") ||
            !strcmp(method, "abstract_socket"))
     {
        Ecore_Con_Server *sock = NULL;

        ecore_con_init();

        if (method[0] == 's')
          sock = ecore_con_server_connect(ECORE_CON_LOCAL_SYSTEM, file, 0,
                                          NULL);
        else
          sock = ecore_con_server_connect(ECORE_CON_LOCAL_ABSTRACT, file, 0,
                                          NULL);

        if ((wait_mode) && !sock)
        {
           int left = wait_time;

           while (left > 0)
             {
                left--;
                sleep(1);
                if (method[0] == 's')
                  sock = ecore_con_server_connect(ECORE_CON_LOCAL_SYSTEM,
                                                  file, 0, NULL);
                else
                  sock = ecore_con_server_connect(ECORE_CON_LOCAL_ABSTRACT,
                                                  file, 0, NULL);
                if (sock) break;
             }
        }

        if (!sock) 
          {
             printf("Could not connect to socket %s\n", file);
             return 1;
          }

        if (strlen(buf) != ecore_con_server_send(sock, buf, strlen(buf)))
          {
             printf("Command has been truncated.\n");
             return 1;
          }

        ecore_con_shutdown();
     }
   else
     {
       _help();
     }
   
   return 0;
}

static void
_help(void)
{
   printf("Usage:\n"
	  "  -h            This help\n"
	  "  -wait N       Wait up to N seconds for exquisite's socket to exist then exit\n"
          "  -ipc M        Use M (fifo [default], socket, abstract_socket) for ipc.\n"
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
          "EXQUISITE_IPC_METHOD can be used as an alternative to\n"
          "the -ipc option.  The same method names are used.\n"
	  );
          
   exit(0);
}
