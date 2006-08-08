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
#include "dialog.h"
#include "file.h"
#include "user.h"

static void
runApp(const char *exe, const char *params)
{
   char               *sh;
   char               *path;
   char               *real_exec;
   int                 fd;

   if (fork())
      return;

   setsid();
   /* Close all file descriptors except the std 3 */
   for (fd = 3; fd < 1024; fd++)
      close(fd);

   LangExport();

   sh = usershell(getuid());
   if (exe)
     {
	path = pathtoexec(exe);
	if (path)
	  {
	     Efree(path);
	     real_exec = Emalloc(strlen(params) + 6);
	     if (!real_exec)
		return;
	     sprintf(real_exec, "exec %s", params);
	     execl(sh, sh, "-c", real_exec, NULL);
	     exit(0);
	  }

	if (!Mode.wm.startup)
	  {
	     path = pathtofile(exe);
	     if (!path)
	       {
		  /* absolute path */
		  if (isabspath(exe))
		     DialogAlertOK(_
				   ("There was an error running the program:\n"
				    "%s\n"
				    "This program could not be executed.\n"
				    "This is because the file does not exist.\n"),
				   exe);
		  /* relative path */
		  else
		     DialogAlertOK(_
				   ("There was an error running the program:\n"
				    "%s\n"
				    "This program could not be executed.\n"
				    "This is most probably because this "
				    "program is not in the\n"
				    "path for your shell which is %s. "
				    "I suggest you read the manual\n"
				    "page for that shell and read up how to "
				    "change or add to your\n"
				    "execution path.\n"), exe, sh);
	       }
	     else
		/* it is a node on the filing sys */
	       {
		  /* it's a file */
		  if (isfile(path))
		    {
		       /* can execute it */
		       if (canexec(path))
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "I am unsure as to why you could not "
					 "do this. The file exists,\n"
					 "is a file, and you are allowed to "
					 "execute it. I suggest you look\n"
					 "into this.\n"), path);
		       /* not executable file */
		       else
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file exists, is a "
					 "file, but you are unable\n"
					 "to execute it because you do not "
					 "have execute "
					 "access to this file.\n"), path);
		    }
		  /* it's not a file */
		  else
		    {
		       /* its a dir */
		       if (isdir(path))
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file is in fact "
					 "a directory.\n"), path);
		       /* its not a file or a dir */
		       else
			  DialogAlertOK(_
					("There was an error running the program:\n"
					 "%s\n"
					 "This program could not be executed.\n"
					 "This is because the file is not a "
					 "regular file.\n"), path);
		    }
		  Efree(path);
	       }
	  }
	exit(100);
     }
   real_exec = Emalloc(strlen(params) + 6);
   if (!real_exec)
      return;
   sprintf(real_exec, "exec %s", params);
   execl(sh, sh, "-c", real_exec, NULL);
   exit(0);
}

int
execApplication(const char *params)
{
   char                exe[FILEPATH_LEN_MAX];
   const char         *s = params;

   if (!s)
      return -1;

   sscanf(s, "%4000s", exe);
   runApp(exe, s);

   return 0;
}

void
Espawn(int argc __UNUSED__, char **argv)
{
   int                 fd;

   if (!argv || !argv[0])
      return;

   if (fork())
      return;

   setsid();
   /* Close all file descriptors except the std 3 */
   for (fd = 3; fd < 1024; fd++)
      close(fd);

   LangExport();

   execvp(argv[0], argv);

   DialogAlertOK(_("There was an error running the program:\n%s\n"), argv[0]);
   exit(100);
}

void
EspawnCmd(const char *cmd)
{
   int                 argc;
   char              **argv;

   argv = StrlistDecodeEscaped(cmd, &argc);
   Espawn(argc, argv);
   StrlistFree(argv, argc);
}

void
ActionsHandleMotion(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveHandleMotion();
	break;

     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeHandleMotion();
	break;

     default:
	break;
     }
}

int
ActionsSuspend(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveSuspend();
	break;
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(NULL);
	break;
     }

   return 0;
}

int
ActionsResume(void)
{
   switch (Mode.mode)
     {
     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveResume();
	break;
     }

   return 0;
}

int
ActionsEnd(EWin * ewin)
{
   int                 did_end = 1;

   switch (Mode.mode)
     {
     case MODE_RESIZE:
     case MODE_RESIZE_H:
     case MODE_RESIZE_V:
	ActionResizeEnd(ewin);
	Mode.action_inhibit = 1;
	break;

     case MODE_MOVE_PENDING:
     case MODE_MOVE:
	ActionMoveEnd(ewin);
	Mode.action_inhibit = 1;
	break;

     default:
	did_end = 0;
	break;
     }

   return did_end;
}

static void
_DlgFillAbout(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di;
   char                buf[1024];

   Esnprintf(buf, sizeof(buf), _("About Enlightenment %s"), e_wm_version);
   DialogSetTitle(d, buf);

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_IMAGE);
   DialogItemImageSetFile(di, "pix/about.png");

   di = DialogAddItem(table, DITEM_TEXT);
   Esnprintf(buf, sizeof(buf),
	     _("Welcome to the %s version\n"
	       "of the Enlightenment window manager.\n"
	       "Enlightenment is still under development, but\n"
	       "we have tried to iron out all the bugs that\n"
	       "we can find. If you find a bug in the software,\n"
	       "please do not hesitate to send in a bug report.\n"
	       "See \"Help\" for information on joining the\n"
	       "mailing list.\n" "\n"
	       "This code was last updated on:\n%s\n" "\n"
	       "Good luck. We hope you enjoy the software.\n" "\n"
	       "The Rasterman - raster@rasterman.com\n"
	       "Mandrake - mandrake@mandrake.net\n"
	       "Kim Woelders - kim@woelders.dk\n"), e_wm_version, e_wm_date);
   DialogItemSetText(di, buf);

   DialogAddButton(d, _("OK"), NULL, 1, DLG_BUTTON_OK);
   DialogBindKey(d, "Return", DialogCallbackClose, 0);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
}

static const DialogDef DlgAbout = {
   "ABOUT_ENLIGHTENMENT",
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   _DlgFillAbout,
   0, NULL,
};

void
About(void)
{
   DialogShowSimple(&DlgAbout, NULL);
}

#if 0				/* FIXME - Fix */
struct _keyset
{
   const char         *sym;
   int                 state;
   const char         *ch;
};

static int
doInsertKeys(EWin * edummy __UNUSED__, const char *params)
{
   static const struct _keyset ks[] = {
      {"a", 0, "a"},
      {"b", 0, "b"},
      {"c", 0, "c"},
      {"d", 0, "d"},
      {"e", 0, "e"},
      {"f", 0, "f"},
      {"g", 0, "g"},
      {"h", 0, "h"},
      {"i", 0, "i"},
      {"j", 0, "j"},
      {"k", 0, "k"},
      {"l", 0, "l"},
      {"m", 0, "m"},
      {"n", 0, "n"},
      {"o", 0, "o"},
      {"p", 0, "p"},
      {"q", 0, "q"},
      {"r", 0, "r"},
      {"s", 0, "s"},
      {"t", 0, "t"},
      {"u", 0, "u"},
      {"v", 0, "v"},
      {"w", 0, "w"},
      {"x", 0, "x"},
      {"y", 0, "y"},
      {"z", 0, "z"},
      {"a", ShiftMask, "A"},
      {"b", ShiftMask, "B"},
      {"c", ShiftMask, "C"},
      {"d", ShiftMask, "D"},
      {"e", ShiftMask, "E"},
      {"f", ShiftMask, "F"},
      {"g", ShiftMask, "G"},
      {"h", ShiftMask, "H"},
      {"i", ShiftMask, "I"},
      {"j", ShiftMask, "J"},
      {"k", ShiftMask, "K"},
      {"l", ShiftMask, "L"},
      {"m", ShiftMask, "M"},
      {"n", ShiftMask, "N"},
      {"o", ShiftMask, "O"},
      {"p", ShiftMask, "P"},
      {"q", ShiftMask, "Q"},
      {"r", ShiftMask, "R"},
      {"s", ShiftMask, "S"},
      {"t", ShiftMask, "T"},
      {"u", ShiftMask, "U"},
      {"v", ShiftMask, "V"},
      {"w", ShiftMask, "W"},
      {"x", ShiftMask, "X"},
      {"y", ShiftMask, "Y"},
      {"z", ShiftMask, "Z"},
      {"grave", 0, "`"},
      {"1", 0, "1"},
      {"2", 0, "2"},
      {"3", 0, "3"},
      {"4", 0, "4"},
      {"5", 0, "5"},
      {"6", 0, "6"},
      {"7", 0, "7"},
      {"8", 0, "8"},
      {"9", 0, "9"},
      {"0", 0, "0"},
      {"minus", 0, "-"},
      {"equal", 0, "="},
      {"bracketleft", 0, "["},
      {"bracketright", 0, "]"},
      {"backslash", 0, "\\\\"},
      {"semicolon", 0, "\\s"},
      {"apostrophe", 0, "\\a"},
      {"comma", 0, ","},
      {"period", 0, "."},
      {"slash", 0, "/"},
      {"grave", ShiftMask, "~"},
      {"1", ShiftMask, "!"},
      {"2", ShiftMask, "@"},
      {"3", ShiftMask, "#"},
      {"4", ShiftMask, "$"},
      {"5", ShiftMask, "%"},
      {"6", ShiftMask, "^"},
      {"7", ShiftMask, "&"},
      {"8", ShiftMask, "*"},
      {"9", ShiftMask, "("},
      {"0", ShiftMask, ")"},
      {"minus", ShiftMask, "_"},
      {"equal", ShiftMask, "+"},
      {"bracketleft", ShiftMask, "{"},
      {"bracketright", ShiftMask, "}"},
      {"backslash", ShiftMask, "|"},
      {"semicolon", ShiftMask, ":"},
      {"apostrophe", ShiftMask, "\\q"},
      {"comma", ShiftMask, "<"},
      {"period", ShiftMask, ">"},
      {"slash", ShiftMask, "?"},
      {"space", ShiftMask, " "},
      {"Return", ShiftMask, "\\n"},
      {"Tab", ShiftMask, "\\t"}
   };

   if (params)
     {
	Window              win = 0;
	int                 i, rev;
	const char         *s;
	XKeyEvent           ev;

	s = params;
	XGetInputFocus(disp, &win, &rev);
	if (win)
	  {
	     SoundPlay("SOUND_INSERT_KEYS");
	     ev.window = win;
	     for (i = 0; i < (int)strlen(s); i++)
	       {
		  int                 j;

		  ev.x = Mode.events.x;
		  ev.y = Mode.events.y;
		  ev.x_root = Mode.events.x;
		  ev.y_root = Mode.events.y;
		  for (j = 0; j < (int)(sizeof(ks) / sizeof(struct _keyset));
		       j++)
		    {
		       if (!strncmp(ks[j].ch, &(s[i]), strlen(ks[j].ch)))
			 {
			    i += (strlen(ks[j].ch) - 1);
			    ev.keycode =
			       XKeysymToKeycode(disp,
						XStringToKeysym(ks[j].sym));
			    ev.state = ks[j].state;
			    ev.type = KeyPress;
			    XSendEvent(disp, win, False, 0, (XEvent *) & ev);
			    ev.type = KeyRelease;
			    XSendEvent(disp, win, False, 0, (XEvent *) & ev);
			    j = 0x7fffffff;
			 }
		    }
	       }
	  }
     }
   return 0;
}
#endif
