/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <signal.h>
#include <time.h>

static void
runDocBrowser(void)
{
   char                file[FILEPATH_LEN_MAX];

   Esnprintf(file, sizeof(file), "%s/edox", EDirBin());
   if (!canexec(file))
      return;
   Esnprintf(file, sizeof(file), "%s/E-docs", EDirRoot());
   if (!canread(file))
      return;

   if (fork())
      EDBUG_RETURN_;

   Esnprintf(file, sizeof(file), "exec %s/edox %s/E-docs",
	     EDirBin(), EDirRoot());

   execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file, NULL);

   exit(0);
}

int
main(int argc, char **argv)
{
   int                 i;
   struct utsname      ubuf;
   char               *str;

   /* This function runs all the setup for startup, and then 
    * proceeds into the primary event loop at the end.
    */

   /* Init state variable struct */
   memset(&Mode, 0, sizeof(EMode));
   Mode.mode = MODE_NONE;
   Mode.wm.startup = 1;
   Mode.move.check = 1;

   str = getenv("EDBUG");
   if (str)
      Mode.debug = atoi(str);
   str = getenv("EDBUG_FLAGS");
   if (str)
      EventDebugInit(str);
   str = getenv("EDBUG_COREDUMP");
   if (str)
      Mode.wm.coredump = 1;

   EDBUG(1, "main");

   /* go head and set up the internal data lists that enlightenment
    * uses for finding everything
    */
   lists = Ecalloc(LIST_TYPE_COUNT, sizeof(List));

   srand(time(NULL));

   if (!uname(&ubuf))
      e_machine_name = Estrdup(ubuf.nodename);
   if (!e_machine_name)
      e_machine_name = Estrdup("localhost");

   /* Initialise internationalisation */
   LangInit();

   SetSMProgName(argv[0]);

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
		EDirUserSet(argv[++j]);
	     }
	   else if ((!strcmp("-ecachedir", argv[j])) && (argc - j > 1))
	     {
		EDirUserCacheSet(argv[++j]);
	     }
	   else if ((!strcmp("-display", argv[j])) && (argc - j > 1))
	     {
		dstr = argv[++j];
	     }
	   else if (!strcmp("-single", argv[j]))
	     {
		Mode.wm.single = 1;
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
	   else if ((!strcmp("-window", argv[j])) && (argc - j > 1))
	     {
		sscanf(argv[++j], "%dx%d", &VRoot.w, &VRoot.h);
		Mode.wm.window = 1;
		Mode.wm.single = 1;
	     }
	   else if ((!strcmp("-h", argv[j])) || (!strcmp("-help", argv[j])) ||
		    (!strcmp("-?", argv[j])) || (!strcmp("--help", argv[j])))
	     {
		printf("enlightenment options:\n"
		       "\t-display display_name\n"
		       "\t-ecachedir /path/to/cached/dir\n"
		       "\t-econfdir /path/to/.enlightenment/conf/dir\n"
		       "\t-ext_init_win window_id\n"
		       "\t-no_overwrite\n"
		       "\t-smfile file\n"
		       "\t[-smid | -clientId | --sm-client-id] id\n"
		       "\t-theme /path/to/theme\n"
		       "\t[-v | -verbose]\n" "\t[-V | -version | --version]\n");
		exit(0);
	     }
	   else if ((!strcmp("-V", argv[j])) ||
		    (!strcmp("-version", argv[j])) ||
		    (!strcmp("--version", argv[j])))
	     {
		printf(_("Enlightenment Version: %s\nLast updated on: %s\n"),
		       ENLIGHTENMENT_VERSION, E_CHECKOUT_DATE);
		exit(0);
	     }
	   else if ((!strcmp("-v", argv[j])) || (!strcmp("-verbose", argv[j])))
	     {
		Mode.debug++;
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
	     f = fopen(file, "r");
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

	     def = ThemeGetDefault();
	     if (def)
	       {
		  Esnprintf(themepath, sizeof(themepath), "%s", def);
		  Efree(def);
	       }
	  }
     }
   SetSMUserThemePath(themepath);

   /* run most of the setup */
   AlertInit();			/* Set up all the text bits that belong on the GSOD */
   SignalsSetup();
   SetupX();			/* This is where the we fork per screen */
   BlumFlimFrub();
   ZoomInit();
   SetupDirs();
   InitDesktopBgs();
   CommsSetup();
   CommsFindCommsWindow();
   LoadGroups();
   LoadSnapInfo();

   GrabX();
   MapUnmap(0);
   UngrabX();

   /* make all of our fallback classes */
   SetupFallbackClasses();

   desks.desk[0].viewable = 0;
   /* now we're going to load the configuration/theme */
   LoadEConfig(themepath);
   SetAreaSize(Conf.areas.nx, Conf.areas.ny);
   TransparencySet(Conf.theme.transparency);

   desks.desk[0].viewable = 1;
   RefreshDesktop(0);
   if (Conf.sound)
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

   SetupEnv();

   if (Conf.mapslide)
      CreateStartupDisplay(0);

   /* Set up the iconboxes and pagers */
   IB_Setup();
   if (Conf.pagers.enable)
     {
	Conf.pagers.enable = 0;
	Mode.queue_up = 0;
	EnableAllPagers();
	Mode.queue_up = DRAW_QUEUE_ENABLE;
     }

   MapUnmap(1);

   /* Kill the E process owning the "init window" */
   if (Mode.wm.master && init_win_ext)
     {
	if (EventDebug(EDBUG_TYPE_SESSION))
	   Eprintf("Kill init window %#lx\n", init_win_ext);
	XKillClient(disp, init_win_ext);
	init_win_ext = 0;
     }

   /* sync just to make sure */
   XSync(disp, False);
   Mode.queue_up = DRAW_QUEUE_ENABLE;

   /* of course, we have to set the cursors */
   ECursorsInit();
   ECsrApply(ECSR_ROOT, VRoot.win);

#if 0				/* AAA: Why here? */
   Mode.wm.startup = 0;
#endif
   Mode.wm.save_ok = Mode.wm.master;
   /* ok - paranoia - save current settings to disk */
   if (VRoot.scr == 0)
      autosave();

   /* let's make sure we set this up and go to our desk anyways */
   GotoDesktop(desks.current);
#if 0				/* Why? */
   if (desks.current < (Conf.desks.num - 1))
     {
	char                ps = 0;

	if (!Conf.mapslide)
	  {
	     ps = Conf.desks.slidein;
	     Conf.desks.slidein = 0;
	  }
	GotoDesktop(desks.current + 1);
	GotoDesktop(desks.current - 1);
	if (!Conf.mapslide)
	   Conf.desks.slidein = ps;
     }
   else if (desks.current > 0)
     {
	char                ps = 0;

	if (!Conf.mapslide)
	  {
	     ps = Conf.desks.slidein;
	     Conf.desks.slidein = 0;
	  }
	GotoDesktop(desks.current - 1);
	GotoDesktop(desks.current + 1);
	if (!Conf.mapslide)
	   Conf.desks.slidein = ps;
     }
#endif
   XSync(disp, False);

   /* if we didn't have an external window piped to us, we'll do some stuff */
   if (!Conf.mapslide)
      CreateStartupDisplay(0);

   BackgroundDestroyByName("STARTUP_BACKGROUND_SIDEWAYS");
   BackgroundDestroyByName("STARTUP_BACKGROUND");

#ifdef SIGCONT
   for (i = 0; i < child_count; i++)
      kill(e_children[i], SIGCONT);
#endif

   SetupUserInitialization();
   if (Mode.firsttime)
      runDocBrowser();

#if 0				/* Not if AAA */
   Mode.wm.startup = 1;
#endif
   SpawnSnappedCmds();
   Mode.wm.startup = 0;

   ThemeBadDialog();

   BackgroundsInit();		/* Start the background accounting */

   /* Update pagers */
   for (i = 0; i < Conf.desks.num; i++)
      RedrawPagersForDesktop(i, 0);

   /* The primary event loop */
   for (;;)
      WaitEvent();

   /* Of course, we should NEVER get to this point */
   EDBUG_RETURN(0);
}
