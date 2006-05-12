/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "session.h"
#include "xwin.h"
#include <sys/wait.h>
#include <signal.h>
#include <X11/Xproto.h>

static void
SignalHandler(int sig)
{
   static int          loop_count = 0;
   int                 status;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("SignalHandler signal=%d\n", sig);

   switch (sig)
     {
     case SIGHUP:
	SessionExit(EEXIT_RESTART, NULL);
	break;

     default:
     case SIGINT:
     case SIGQUIT:
     case SIGABRT:
     case SIGTERM:
	SessionExit(EEXIT_EXIT, NULL);
	break;

     case SIGPIPE:
     case SIGALRM:
     case SIGUSR1:
     case SIGUSR2:
	break;

     case SIGILL:
	if (disp)
	   EUngrabServer();
	DialogAlert(_
		    ("Enlightenment performed an Illegal Instruction.\n" "\n"
		     "This most likely is due to you having installed an run a\n"
		     "binary of Enlightenment that was compiled for a make or model\n"
		     "of CPU not 100%% identical or compatible with yours. Please\n"
		     "either obtain the correct package for your system, or\n"
		     "re-compile Enlightenment and possibly any support libraries\n"
		     "that you got in binary format to run Enlightenment.\n"));
	SessionExit(EEXIT_ERROR, NULL);
	break;

     case SIGFPE:
	if (disp)
	   EUngrabServer();
	DialogAlert(_
		    ("Enlightenment caused a Floating Point Exception.\n" "\n"
		     "This means that Enlightenment or support library routines it calls\n"
		     "have performed an illegal mathematical operation (most likely\n"
		     "dividing a number by zero). This is most likely a bug. It is\n"
		     "recommended to restart now. If you wish to help fix this please\n"
		     "compile Enlightenment with debugging symbols in and run\n"
		     "Enlightenment under gdb so you can backtrace for where it died and\n"
		     "send in a useful bug report with backtrace information and variable\n"
		     "dumps etc.\n"));
	SessionExit(EEXIT_ERROR, NULL);
	break;

     case SIGSEGV:
	if (loop_count > 0)
	   abort();
	loop_count++;
	if (disp)
	   EUngrabServer();
	DialogAlert(_
		    ("Enlightenment caused Segment Violation (Segfault)\n" "\n"
		     "This means that Enlightenment or support library routines it calls\n"
		     "have accessed areas of your system's memory that they are not\n"
		     "allowed access to. This is most likely a bug. It is recommended to\n"
		     "restart now. If you wish to help fix this please compile\n"
		     "Enlightenment with debugging symbols in and run Enlightenment\n"
		     "under gdb so you can backtrace for where it died and send in a\n"
		     "useful bug report with backtrace information and variable\n"
		     "dumps etc.\n"));
	abort();
	break;

     case SIGBUS:
	if (disp)
	   EUngrabServer();
	DialogAlert(_
		    ("Enlightenment caused Bus Error.\n" "\n"
		     "It is suggested you check your hardware and OS installation.\n"
		     "It is highly unusual to cause Bus Errors on operational\n"
		     "hardware.\n"));
	break;

     case SIGCHLD:
	while (waitpid(-1, &status, WNOHANG) > 0)
	   ;
	break;
     }
}

static void
doSignalsSetup(int setup)
{
   static const int    signals[] = {
      SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGPIPE,
      SIGALRM, SIGTERM, SIGUSR1, SIGUSR2, SIGCHLD, SIGBUS
   };
   unsigned int        i, sig;
   struct sigaction    sa;

   /*
    * We may be here after a fork/exec within in a signal handler.
    * Therefore, lets just clear the blocked signals.
    */
   sigemptyset(&sa.sa_mask);
   sigprocmask(SIG_SETMASK, &sa.sa_mask, (sigset_t *) NULL);

   for (i = 0; i < sizeof(signals) / sizeof(int); i++)
     {
	sig = signals[i];
	if (Mode.wm.coredump &&
	    (sig == SIGILL || sig == SIGFPE || sig == SIGSEGV || sig == SIGBUS))
	   continue;

	if (setup)
	  {
	     sa.sa_handler = SignalHandler;
	     sa.sa_flags = (sig == SIGCHLD) ? SA_RESTART : 0;
	  }
	else
	  {
	     sa.sa_handler = SIG_DFL;
	     sa.sa_flags = 0;
	  }
	sigemptyset(&sa.sa_mask);
	sigaction(sig, &sa, (struct sigaction *)0);
     }
}

void
SignalsSetup(void)
{
   /* This function will set up all the signal handlers for E */
   doSignalsSetup(1);
}

void
SignalsRestore(void)
{
   /* This function will restore all the signal handlers for E */
   doSignalsSetup(0);
}

void
HandleXError(Display * d __UNUSED__, XErrorEvent * ev)
{
   char                buf[64];

   if ((ev->request_code == X_ChangeWindowAttributes)
       && (ev->error_code == BadAccess))
     {
	if (Mode.wm.xselect)
	  {
	     AlertX(_("Another Window Manager is already running"),
		    _("OK"), NULL, NULL,
		    _("Another Window Manager is already running.\n" "\n"
		      "You will have to quit your current Window Manager first before\n"
		      "you can successfully run Enlightenment.\n"));
	     EExit(1);
	  }
     }

   if (EventDebug(1))
     {
	XGetErrorText(disp, ev->error_code, buf, 63);
	Eprintf("*** ERROR: xid=%#lx error=%i req=%i/%i: %s\n",
		ev->resourceid, ev->error_code,
		ev->request_code, ev->minor_code, buf);
     }
}

void
HandleXIOError(Display * d __UNUSED__)
{
   disp = NULL;
   SessionExit(EEXIT_ERROR, NULL);
}
