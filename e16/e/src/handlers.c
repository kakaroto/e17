/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

void
HandleSigHup(int num)
{
   EDBUG(7, "HandleSigHup");
   num = 0;
   doExit("restart");
   EDBUG_RETURN_;
}

void
HandleSigInt(int num)
{
   EDBUG(7, "HandleSigQuit");
   num = 0;
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigQuit(int num)
{
   EDBUG(7, "HandleSigQuit");
   num = 0;
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigIll(int num)
{
   EDBUG(7, "HandleSigIll");
   num = 0;
   if (disp)
      UngrabX();
   DialogAlert(_
               ("Enlightenment performed an Illegal Instruction.\n" "\n"
                "This most likely is due to you having installed an run a\n"
                "binary of Enlightenment that was compiled for a make or model\n"
                "of CPU not 100%% identical or compatible with yours. Please\n"
                "either obtain the correct package for your system, or\n"
                "re-compile Enlightenment and possibly any support libraries\n"
                "that you got in binary format to run Enlightenment.\n"));
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigAbrt(int num)
{
   EDBUG(7, "HandleSigAbrt");
   num = 0;
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigFpe(int num)
{
   EDBUG(7, "HandleSigFpe");
   num = 0;
   if (disp)
      UngrabX();
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
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigSegv(int num)
{
   static int          loop_count = 0;

   EDBUG(7, "HandleSigSegv");
   if (loop_count > 0)
      abort();
   loop_count++;
   if (disp)
      UngrabX();
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
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigPipe(int num)
{
   EDBUG(7, "HandleSigPipe");
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigAlrm(int num)
{
   EDBUG(7, "HandleSigAlrm");
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigTerm(int num)
{
   EDBUG(7, "HandleSigTerm");
   num = 0;
   doExit("error");
   EDBUG_RETURN_;
}

void
HandleSigUsr1(int num)
{
   EDBUG(7, "HandleSigUsr1");
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigUsr2(int num)
{
   EDBUG(7, "HandleSigUsr2");
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigChild(int num)
{
   int                 status;

   EDBUG(7, "HandleSigChild");
   num = 0;
#ifndef __EMX__
   while (waitpid(-1, &status, WNOHANG) > 0);
#else
   waitpid(-1, &status, WNOHANG);
#endif
   EDBUG_RETURN_;
}

void
HandleSigTstp(int num)
{
   EDBUG(7, "HandleSigTstp");
   num = 0;
   EDBUG_RETURN_;
}

void
HandleSigBus(int num)
{
   EDBUG(7, "HandleSigBus");
   num = 0;
   if (disp)
      UngrabX();
   DialogAlert(_
               ("Enlightenment caused Bus Error.\n" "\n"
                "It is suggested you check your hardware and OS installation.\n"
                "It is highly unusual to cause Bus Errors on operational\n"
                "hardware.\n"));
   EDBUG_RETURN_;
}

void
EHandleXError(Display * d, XErrorEvent * ev)
{
/*  char                buf[64]; */

   EDBUG(7, "EHandleXError");
   if ((ev->request_code == X_ChangeWindowAttributes)
       && (ev->error_code == BadAccess))
     {
        if ((!no_overwrite) && (mode.xselect))
          {
             ASSIGN_ALERT(_("Another Window Manager is already running"),
                          _("OK (edit file)"), "", _("Cancel (do NOT edit)"));
             Alert(_
                   ("Another Window Manager is already running.\n" "\n"
                    "You will have to quit your current Window Manager first before\n"
                    "you can successfully run Enlightenment.\n" "\n"
                    "If you haven't edited your user start-up files, Enlightenment\n"
                    "can do that now for you, so when you log in again after\n"
                    "quitting your current window manager, you will have\n"
                    "Enlightenment running.\n" "\n"
                    "If you want to do this, click OK, otherwise hit cancel\n"
                    "to abort this operation and edit the files by hand.\n" "\n"
                    "WARNING WARNING WARNING WARNING!\n" "\n"
                    "It is possible that this MAY not properly edit your files.\n"));
             ASSIGN_ALERT(_("Are you sure?"), _("YES (edit file)"), "",
                          _("NO (do not edit)"));
             Alert(_
                   ("Are you absolutely sure you want to have Enlightenment\n"
                    "edit your start-up files for you?\n" "\n"
                    "If your start-up files are highly customised this may not\n"
                    "work.\n" "\n" "Are you ABSOLUTELY sure?\n"));
             RESET_ALERT;
             AddE();
             EExit((void *)1);
          }
     }
/*  XGetErrorText (disp, ev->error_code, buf, 63);
 * fprintf(stderr, "Whee %i: %s : %i\n", time(NULL), buf, ev->request_code); */
   d = NULL;
   EDBUG_RETURN_;
}

void
HandleXIOError(Display * d)
{
   EDBUG(7, "HandleXIOError");
   disp = NULL;
   doExit("error");
   EDBUG_RETURN_;
   d = NULL;
}
