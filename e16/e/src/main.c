/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "timestamp.h"
#include <sys/utsname.h>

static void
runDocBrowser(void)
{

   char                file[FILEPATH_LEN_MAX];

   if (fork())
      EDBUG_RETURN_;

   Esnprintf(file, sizeof(file), "exec %s/dox %s/E-docs", ENLIGHTENMENT_BIN,
	     ENLIGHTENMENT_ROOT);
   execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file, NULL);
   exit(0);

}

int
main(int argc, char **argv)
{
   int                 i;
   Background         *bg;
   ECursor            *ec = NULL;
   struct utsname      ubuf;
   char               *str;

   /* This function runs all the setup for startup, and then 
    * proceeds into the primary event loop at the end.
    */

   single_screen_mode = 0;
/*  unsetenv("LD_PRELOAD"); */

/* Part of gettext stuff */

   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

/* End of gettext stuff */

#ifdef DEBUG
   call_level = 0;
   debug_level = 0;
   str = getenv("EDBUG");
   if (str)
      debug_level = atoi(str);
#endif
   str = getenv("EDBUG_FLAGS");
   if (str)
      EventDebugInit(str);

   EDBUG(1, "main");

   /* go head and set up the internal data lists that enlightenment
    * uses for finding everything
    */
   lists = Emalloc(sizeof(List) * LIST_TYPE_COUNT);
   lists = memset(lists, 0, (sizeof(List) * LIST_TYPE_COUNT));

   /* Set up all the text bits that belong on the GSOD */
   AssignTitleText(_("Enlightenment Message Dialog"));
   AssignIgnoreText(_("Ignore this"));
   AssignRestartText(_("Restart Enlightenment"));
   AssignExitText(_("Quit Enlightenment"));

   /* We'll set up what the buttons do now, too */
   AssignRestartFunction(SessionExit, "restart");
   AssignExitFunction(SessionExit, NULL);

   srand(time(NULL));

   if (!uname(&ubuf))
      e_machine_name = duplicate(ubuf.nodename);
   if (!e_machine_name)
      e_machine_name = duplicate("localhost");
   command = duplicate(argv[0]);
   themepath[0] = 0;
   {
      int                 j = 0;

      /* Now we're going to interpret any of the commandline parameters
       * that are passed to it -- Well, at least the ones that we
       * understand.
       */

      for (j = 1; j < argc; j++)
	{
	   if ((!strcmp("-theme", argv[j])) && (argc - j > 1))
	     {
		Esnprintf(themepath, sizeof(themepath), "%s", argv[++j]);
	     }
	   else if ((!strcmp("-econfdir", argv[j])) && (argc - j > 1))
	     {
		SetEDir(argv[++j]);
	     }
	   else if ((!strcmp("-ecachedir", argv[j])) && (argc - j > 1))
	     {
		SetCacheDir(argv[++j]);
	     }
	   else if ((!strcmp("-display", argv[j])) && (argc - j > 1))
	     {
		dstr = argv[++j];
	     }
	   else if (!strcmp("-single", argv[j]))
	     {
		single_screen_mode = 1;
	     }
	   else if ((!strcmp("-smid", argv[j])) && (argc - j > 1))
	     {
		SetSMID(argv[++j]);
	     }
	   else if ((!strcmp("-clientId", argv[j])) && (argc - j > 1))
	     {
		SetSMID(argv[++j]);
	     }
	   else if ((!strcmp("--sm-client-id", argv[j])) && (argc - j > 1))
	     {
		SetSMID(argv[++j]);
	     }
	   else if ((!strcmp("-smfile", argv[j])) && (argc - j > 1))
	     {
		SetSMFile(argv[++j]);
	     }
	   else if ((!strcmp("-ext_init_win", argv[j])) && (argc - j > 1))
	     {
		init_win_ext = atoi(argv[++j]);
	     }
	   else if (!strcmp("-no_overwrite", argv[j]))
	     {
		no_overwrite = 1;
	     }
	   else if ((!strcmp("-help", argv[j]))
		    || (!strcmp("--help", argv[j]))
		    || (!strcmp("-h", argv[j])) || (!strcmp("-?", argv[j])))
	     {
		printf("enlightenment options:                      \n"
		       "\t-theme /path/to/theme                     \n"
		       "\t-econfdir /path/to/.enlightenment/conf/dir\n"
		       "\t-ecachedir /path/to/cached/dir            \n"
		       "\t[-smid | -clientId | --sm-client-id] id   \n"
		       "\t-smfile file                              \n"
		       "\t-ext_init_win window_id                   \n"
		       "\t-no_overwrite                             \n"
		       "\t[-v | -version | --version]               \n"
		       "\t-display display_name                     \n");
		exit(0);
	     }
	   else if ((!strcmp("-v", argv[j]))
		    || (!strcmp("-version", argv[j]))
		    || (!strcmp("--version", argv[j]))
		    || (!strcmp("-v", argv[j])))
	     {
		printf(_("Enlightenment Version: %s\nLast updated on: %s\n"),
		       ENLIGHTENMENT_VERSION, E_CHECKOUT_DATE);
		exit(0);
	     }
	}

      /* Set a default location for the "previous session" data when
       * we do not actually have a previous session. */
      SetSMFile(NULL);
   }

   if (themepath[0] == 0)
     {
	FILE               *f;
	char                s[FILEPATH_LEN_MAX];
	char               *file;

	file = FindFile("user_theme.cfg");
	if (file)
	  {
	     s[0] = 0;
#ifndef __EMX__
	     f = fopen(file, "r");
#else
	     f = fopen(file, "rt");
#endif
	     if (f)
	       {
		  if (fscanf(f, "%4000s", s) < 1)
		     s[0] = 0;
		  fclose(f);
		  if (s[0])
		     Esnprintf(themepath, sizeof(themepath), "%s", s);
	       }
	     Efree(file);
	  }

	if (themepath[0] == 0)
	  {
	     char               *def;

	     def = GetDefaultTheme();
	     if (def)
	       {
		  Esnprintf(themepath, sizeof(themepath), "%s", def);
		  Efree(def);
	       }
	  }
     }
   SetSMUserThemePath(themepath);

   /* run most of the setup */
   SignalsSetup();
   SetupX();
   BlumFlimFrub();
   ZoomInit();
   SetupDirs();
   InitDesktopBgs();
   GotoDesktop(0);
   CommsSetup();
   CommsFindCommsWindow();
   GrabX();
   LoadGroups();
   LoadSnapInfo();
   MapUnmap(0);

   /* make all of our fallback classes */
   SetupFallbackClasses();
   UngrabX();

   desks.desk[0].viewable = 0;
   /* now we're going to load the configuration/theme */
   LoadEConfig(themepath);

   desks.desk[0].viewable = 1;
   RefreshDesktop(0);
   if (mode.sound)
     {
	SoundPlay("SOUND_STARTUP");
	SoundFree("SOUND_STARTUP");
     }
   /* toss down the dragbar and related */
   InitDesktopControls();
   /* then draw all the buttons that belong on the desktop */
   ShowDesktopButtons();

   HintsInit();
   SessionInit();
   ShowDesktopControls();
   CheckEvent();
   /* retreive stuff from last time we were loaded if we're restarting */
   ICCCM_GetMainEInfo();
   SetupEnv();
   if (mode.mapslide)
      CreateStartupDisplay(0);
   MapUnmap(1);
   /* set some more hints */
   HintsSetDesktopViewport();
   desks.current = 0;
   /* Set up the internal pagers */
   IB_Setup();
   if (mode.show_pagers)
     {
	mode.show_pagers = 0;
	queue_up = 0;
	EnableAllPagers();
	queue_up = DRAW_QUEUE_ENABLE;
     }
   if (getpid() == master_pid && init_win_ext)
     {
	XKillClient(disp, init_win_ext);
	init_win_ext = 0;
     }
   HintsSetClientList();

   /* sync just to make sure */
   XSync(disp, False);
   queue_up = DRAW_QUEUE_ENABLE;

   /* hello!  we don't have a resizemode of 5! */
   if (mode.resizemode == 5)
      mode.resizemode = 0;
   /* of course, we have to set the cursors */
   ec = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_ECURSOR);
   if (ec)
     {
	ApplyECursor(root.win, ec);
	ec->ref_count++;
	ec->inroot = 1;
     }

   mode.startup = 0;
   /* ok - paranoia - save current settings to disk */
   if (root.scr == 0)
      autosave();
   /* let's make sure we set this up and go to our desk anyways */
   ICCCM_GetMainEInfo();
   GotoDesktop(desks.current);
   if (desks.current < (mode.numdesktops - 1))
     {
	char                ps = 0;

	if (!mode.mapslide)
	  {
	     ps = desks.slidein;
	     desks.slidein = 0;
	  }
	GotoDesktop(desks.current + 1);
	GotoDesktop(desks.current - 1);
	if (!mode.mapslide)
	   desks.slidein = ps;
     }
   else if (desks.current > 0)
     {
	char                ps = 0;

	if (!mode.mapslide)
	  {
	     ps = desks.slidein;
	     desks.slidein = 0;
	  }
	GotoDesktop(desks.current - 1);
	GotoDesktop(desks.current + 1);
	if (!mode.mapslide)
	   desks.slidein = ps;
     }
   XSync(disp, False);

   /* if we didn't have an external window piped to us, we'll do some stuff */
   if (!mode.mapslide)
      CreateStartupDisplay(0);

   if ((bg = RemoveItem("STARTUP_BACKGROUND_SIDEWAYS", 0,
			LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND)))
      FreeDesktopBG(bg);
   if ((bg = RemoveItem("STARTUP_BACKGROUND", 0,
			LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND)))
      FreeDesktopBG(bg);

#ifdef SIGCONT
   for (i = 0; i < child_count; i++)
      kill(e_children[i], SIGCONT);
#endif

   SetupUserInitialization();
   if (mode.firsttime)
      runDocBrowser();

   mode.startup = 1;
   SpawnSnappedCmds();
   mode.startup = 0;

   BadThemeDialog();
   /* The primary event loop */
   for (;;)
      WaitEvent();
   /* Of course, we should NEVER get to this point */
   EDBUG_RETURN(0);
}
