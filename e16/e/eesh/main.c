/*
 Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies of the Software, its documentation and marketing & publicity
 materials, and acknowledgment shall be given in the documentation, materials
 and software packages that this Software was used.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "E.h"
#include <sys/time.h>
#ifdef __EMX__
#include <io.h>			/* for select() in EMX */
#endif
#include <sys/types.h>
#include <unistd.h>
#if USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

extern char         waitonly;

#if !USE_READLINE
static char         buf[10240];
static int          stdin_state;
#endif
static Client      *e;

static void
restore_stdin_state(void)
{
#if USE_READLINE
   rl_callback_handler_remove();
#else
   fcntl(0, F_SETFL, stdin_state);
#endif
}

static void
process_line(char *line)
{
   if (line == NULL)
      exit(0);
   if (*line == '\0')
      return;

   CommsSend(e, line);
   XSync(disp, False);
#if USE_READLINE
   add_history(line);
#endif
}

int
main(int argc, char **argv)
{
   XEvent              ev;
   Client             *me;
   int                 i, j;
   fd_set              fd;
   char               *command;

   waitonly = 0;
   lists.next = NULL;
   display_name = NULL;
   command = NULL;

   for (i = 0; i < argc; i++)
     {
	if (!strcmp(argv[i], "-e"))
	  {
	     if (i != (argc - 1))
	       {
		  command = argv[++i];
	       }
	  }
	else if (!strcmp(argv[i], "-ewait"))
	  {
	     waitonly = 1;
	     if (i != (argc - 1))
		command = argv[++i];
	  }
	else if (!strcmp(argv[i], "-display"))
	  {
	     if (i != (argc - 1))
		display_name = duplicate(argv[++i]);
	  }
	else if ((!strcmp(argv[i], "-h")) ||
		 (!strcmp(argv[i], "--h")) ||
		 (!strcmp(argv[i], "-help")) || (!strcmp(argv[i], "--help")))
	  {
	     printf("%s [ -e \"Command to Send to Enlightenment then exit\"]\n"
		    "     [ -ewait \"Command to Send to E then wait for a reply then exit\"]\n",
		    argv[0]);
	     printf("Use \"%s\" by itself to enter the \"interactive mode\"\n"
		    "Ctrl-D will exit interactive mode (EOF)\n"
		    "use \"help\" from inside interactive mode for further "
		    "assistance\n", argv[0]);
	     exit(0);
	  }
     }

   SetupX();
   CommsSetup();
   CommsFindCommsWindow();
   XSelectInput(disp, comms_win, StructureNotifyMask);
   XSelectInput(disp, root.win, PropertyChangeMask);
   e = MakeClient(comms_win);
   AddItem(e, "E", e->win, LIST_TYPE_CLIENT);
   me = MakeClient(my_win);
   AddItem(me, "ME", me->win, LIST_TYPE_CLIENT);
   CommsSend(e, "set clientname eesh");
   CommsSend(e, "set version 0.1");
   CommsSend(e, "set author The Rasterman");
   CommsSend(e, "set email raster@rasterman.com");
   CommsSend(e, "set web http://www.enlightenment.org");
/*  CommsSend(e, "set address NONE"); */
   CommsSend(e, "set info Enlightenment IPC Shell - talk to E direct");
/*  CommsSend(e, "set pixmap 0"); */

   if (command)
     {
	CommsSend(e, command);
	if (!waitonly)
	  {
	     XSync(disp, False);
	     exit(0);
	  }
     }

   XSync(disp, False);

#if USE_READLINE
   rl_callback_handler_install("", process_line);
#else
   stdin_state = fcntl(0, F_GETFL, 0);
   fcntl(0, F_SETFL, O_NONBLOCK);
#endif
   atexit(restore_stdin_state);

   j = 0;
   for (;;)
     {
	if (waitonly)
	  {
	     XNextEvent(disp, &ev);
	     if (ev.type == ClientMessage)
		HandleComms(&ev);
	     else if (ev.type == DestroyNotify)
		exit(0);
	     XSync(disp, False);
	  }
	else
	  {
	     FD_ZERO(&fd);
	     FD_SET(0, &fd);
	     FD_SET(ConnectionNumber(disp), &fd);
	     if (select(ConnectionNumber(disp) + 1, &fd, NULL, NULL, NULL) < 0)
		exit(0);
	     XSync(disp, False);

	     if (FD_ISSET(0, &fd))
	       {
#if USE_READLINE
		  rl_callback_read_char();
#else
		  int                 k, ret;

		  k = 0;
		  while ((ret = read(0, &(buf[j]), 1) > 0))
		    {
		       k = 1;
		       if (buf[j] == '\n')
			 {
			    buf[j] = 0;
			    if (strlen(buf) > 0)
			       process_line(buf);
			    j = -1;
			 }
		       j++;
		    }
		  if ((ret < 0) || ((k == 0) && (ret == 0)))
		     exit(0);
#endif
	       }
	     else if (FD_ISSET(ConnectionNumber(disp), &fd))
	       {
		  while (XPending(disp))
		    {
		       XNextEvent(disp, &ev);
		       if (ev.type == ClientMessage)
			  HandleComms(&ev);
		       else if (ev.type == DestroyNotify)
			  exit(0);
		    }
		  XSync(disp, False);
	       }
	  }
     }

   return 0;
}
