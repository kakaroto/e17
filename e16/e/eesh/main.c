/*
 Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors

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

/* Global vars */
Display            *disp;
List                lists;

static char         buf[10240];
static int          stdin_state;
static char        *display_name;
static Client      *e;

static void
process_line(char *line)
{
   if (line == NULL)
      exit(0);
   if (*line == '\0')
      return;

   CommsSend(e, line);
   XSync(disp, False);
}

static void
stdin_state_setup(void)
{
   stdin_state = fcntl(0, F_GETFL, 0);
   fcntl(0, F_SETFL, O_NONBLOCK);
}

static void
stdin_state_restore(void)
{
   fcntl(0, F_SETFL, stdin_state);
}

static void
stdin_read(void)
{
   static int          j = 0;
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
}

int
main(int argc, char **argv)
{
   XEvent              ev;
   Client             *me;
   int                 i;
   fd_set              fd;
   char               *command;
   Window              my_win, comms_win;
   char                waitonly, complete;

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
	       {
		  display_name = argv[++i];
		  display_name = Estrdup(display_name);
	       }
	  }
	else if ((!strcmp(argv[i], "-h")) ||
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

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   /* Or set with the -display option */
   disp = XOpenDisplay(display_name);
   if (!disp)
     {
	Alert("Failed to connect to X server\n");
	exit(1);
     }

   my_win = CommsSetup();
   comms_win = CommsFindCommsWindow();

   e = MakeClient(comms_win);
   AddItem(e, "E", e->win, LIST_TYPE_CLIENT);

   /* Not sure this is used... */
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
	/* Non-interactive */
	CommsSend(e, command);
	XSync(disp, False);
	if (!waitonly)
	   goto done;
     }
   else
     {
	/* Interactive */
	stdin_state_setup();
	atexit(stdin_state_restore);
     }

   for (;;)
     {
	FD_ZERO(&fd);
	if (!command)
	   FD_SET(0, &fd);
	FD_SET(ConnectionNumber(disp), &fd);

	if (select(ConnectionNumber(disp) + 1, &fd, NULL, NULL, NULL) < 0)
	   break;

	XSync(disp, False);

	if (FD_ISSET(0, &fd))
	  {
	     stdin_read();
	  }
	else if (FD_ISSET(ConnectionNumber(disp), &fd))
	  {
	     while (XPending(disp))
	       {
		  XNextEvent(disp, &ev);
		  switch (ev.type)
		    {
		    case ClientMessage:
		       complete = HandleComms(&ev);
		       if (waitonly && complete)
			  goto done;
		       break;
		    case DestroyNotify:
		       goto done;
		    }
	       }
	     XSync(disp, False);
	  }
     }

 done:
   return 0;
}

void
Alert(const char *fmt, ...)
{
   va_list             ap;

   EDBUG(7, "Alert");
   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   va_end(ap);
   EDBUG_RETURN_;
}

#if !USE_LIBC_STRDUP
char               *
Estrdup(const char *s)
{
   char               *ss;
   int                 sz;

   EDBUG(9, "Estrdup");
   if (!s)
      EDBUG_RETURN(NULL);
   sz = strlen(s);
   ss = Emalloc(sz + 1);
   strncpy(ss, s, sz + 1);
   EDBUG_RETURN(ss);
}
#endif
