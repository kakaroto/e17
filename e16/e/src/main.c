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
#if USE_COMPOSITE
#include "ecompmgr.h"
#endif

const char          e_wm_name[] = "Enlightenment";
const char          e_wm_version[] =
#ifdef ENLIGHTENMENT_RELEASE
   "enlightenment-" ENLIGHTENMENT_VERSION "-" ENLIGHTENMENT_RELEASE;
#else
   "enlightenment-" ENLIGHTENMENT_VERSION;
#endif
const char          e_wm_date[] = E_CHECKOUT_DATE;

Display            *disp;
RealRoot            RRoot;
VirtRoot            VRoot;
EConf               Conf;
EMode               Mode;

#ifdef USE_EXT_INIT_WIN
Window              init_win_ext = None;
#endif

static void         ESetSavePrefix(const char *path);
static void         ECheckEprog(const char *name);
static void         EDirUserSet(const char *dir);
static void         EDirUserCacheSet(const char *dir);
static void         EDirsSetup(void);
static void         RunInitPrograms(void);

int
main(int argc, char **argv)
{
   int                 i;
   struct utsname      ubuf;
   char               *str, *dstr;

   /* This function runs all the setup for startup, and then 
    * proceeds into the primary event loop at the end.
    */

   /* Init state variable struct */
   memset(&Mode, 0, sizeof(EMode));
   Mode.mode = MODE_NONE;
   Mode.wm.exec_name = argv[0];
   Mode.wm.startup = 1;
   Mode.move.check = 1;

   str = getenv("EDEBUG");
   if (str)
      EventDebugInit(str);
   str = getenv("EDEBUG_COREDUMP");
   if (str)
      Mode.wm.coredump = 1;

#if 0				/* No! */
   str = getenv("ECONFDIR");
   if (str)
      EDirUserSet(str);
   str = getenv("ECACHEDIR");
   if (str)
      EDirUserCacheSet(str);
#endif

   /* Set up the internal data lists used to find everything */
   ListsInit(LIST_TYPE_COUNT);

   srand(time(NULL));

   if (!uname(&ubuf))
      Mode.wm.machine_name = Estrdup(ubuf.nodename);
   if (!Mode.wm.machine_name)
      Mode.wm.machine_name = Estrdup("localhost");

   /* Now we're going to interpret any of the commandline parameters
    * that are passed to it -- Well, at least the ones that we
    * understand.
    */

   Mode.theme.path = NULL;
   dstr = NULL;

   for (i = 1; i < argc; i++)
     {
	if ((!strcmp("-t", argv[i]) ||
	     !strcmp("-theme", argv[i])) && (argc - i > 1))
	  {
	     i++;
	     Mode.theme.path = Estrdup(argv[i]);
	  }
	else if ((!strcmp("-econfdir", argv[i])) && (argc - i > 1))
	  {
	     EDirUserSet(argv[++i]);
	  }
	else if ((!strcmp("-ecachedir", argv[i])) && (argc - i > 1))
	  {
	     EDirUserCacheSet(argv[++i]);
	  }
	else if ((!strcmp("-display", argv[i])) && (argc - i > 1))
	  {
	     dstr = argv[++i];
	  }
	else if (!strcmp("-f", argv[i]))
	  {
	     Mode.wm.restart = 1;
	  }
	else if (!strcmp("-s", argv[i]) || !strcmp("-single", argv[i]))
	  {
	     Mode.wm.single = 1;
	  }
	else if ((!strcmp("-smid", argv[i]) ||
		  !strcmp("-clientId", argv[i]) ||
		  !strcmp("--sm-client-id", argv[i])) && (argc - i > 1))
	  {
	     SetSMID(argv[++i]);
	  }
	else if ((!strcmp("-smfile", argv[i])) && (argc - i > 1))
	  {
	     ESetSavePrefix(argv[++i]);
	  }
#ifdef USE_EXT_INIT_WIN
	else if ((!strcmp("-ext_init_win", argv[i])) && (argc - i > 1))
	  {
	     init_win_ext = atoi(argv[++i]);
	     Mode.wm.restart = 1;
	  }
#endif
	else if ((!strcmp("-w", argv[i]) || !strcmp("-window", argv[i])) &&
		 (argc - i > 1))
	  {
	     sscanf(argv[++i], "%dx%d", &VRoot.w, &VRoot.h);
	     Mode.wm.window = 1;
	     Mode.wm.single = 1;
	  }
	else if ((!strcmp("-h", argv[i])) || (!strcmp("-help", argv[i])) ||
		 (!strcmp("-?", argv[i])) || (!strcmp("--help", argv[i])))
	  {
	     printf("enlightenment options:\n"
		    "\t-display display_name\n"
		    "\t-ecachedir /path/to/cached/dir\n"
		    "\t-econfdir /path/to/config/dir\n"
		    "\t-ext_init_win window_id\n"
		    "\t[-s | -single]\n"
		    "\t-smfile file\n"
		    "\t[-smid | -clientId | --sm-client-id] id\n"
		    "\t[-t | -theme] theme\n"
		    "\t[-v | -verbose]\n"
		    "\t[-V | -version | --version]\n" "\t-w WxH\n");
	     exit(0);
	  }
	else if ((!strcmp("-V", argv[i])) ||
		 (!strcmp("-version", argv[i])) ||
		 (!strcmp("--version", argv[i])))
	  {
	     printf("Enlightenment %s - %s\n",
		    ENLIGHTENMENT_VERSION, E_CHECKOUT_DATE);
	     exit(0);
	  }
	else if ((!strcmp("-v", argv[i])) || (!strcmp("-verbose", argv[i])))
	  {
	     EventDebugSet(EDBUG_TYPE_VERBOSE, 1);
	  }
#if USE_COMPOSITE
	else if ((!strcmp("-C", argv[i])))
	  {
	     ECompMgrParseArgs(argv[++i]);
	  }
#endif
     }

   /* Initialise internationalisation */
   LangInit();

   /* run most of the setup */
   AlertInit();			/* Set up all the text bits that belong on the GSOD */
   SignalsSetup();
   SetupX(dstr);		/* This is where the we fork per screen */
   /* X is now running, and we have forked per screen */

   ECheckEprog("epp");
   ECheckEprog("eesh");
   EDirsSetup();

   /* Set default save file prefix if not already set */
   ESetSavePrefix(NULL);

   /* So far nothing should rely on a selected settings or theme. */
   ConfigurationLoad();		/* Load settings */

   /* The theme path must now be available for config file loading. */
   ThemePathFind();

   /* Set the Environment variables */
   Esetenv("EVERSION", ENLIGHTENMENT_VERSION, 1);
   Esetenv("EROOT", EDirRoot(), 1);
   Esetenv("EBIN", EDirBin(), 1);
   Esetenv("ETHEME", Mode.theme.path, 1);
   Esetenv("ECONFDIR", EDirUser(), 1);
   Esetenv("ECACHEDIR", EDirUserCache(), 1);

   /* Unmap the clients */
   MapUnmap(0);

   ModulesSignal(ESIGNAL_INIT, NULL);

   /* Move elsewhere... */
   HintsInit();
   CommsInit();
   SessionInit();
   LoadSnapInfo();

   /* Load the theme */
   ThemeConfigLoad();

   /* Do initial configuration */
   ModulesSignal(ESIGNAL_CONFIGURE, NULL);

   /* Set root window cursor */
   ECsrApply(ECSR_ROOT, VRoot.win);

#ifdef USE_EXT_INIT_WIN
   /* Kill the E process owning the "init window" */
   if (Mode.wm.master && init_win_ext)
     {
	if (EventDebug(EDBUG_TYPE_SESSION))
	   Eprintf("Kill init window %#lx\n", init_win_ext);
	XKillClient(disp, init_win_ext);
	init_win_ext = 0;
     }
#endif

   if (!Mode.wm.restart)
      StartupWindowsCreate(0);

   /* sync just to make sure */
   ecore_x_sync();
   Mode.queue_up = DRAW_QUEUE_ENABLE;

   /* let's make sure we set this up and go to our desk anyways */
   GotoDesktop(DesksGetCurrent());
   ecore_x_sync();

#ifdef SIGCONT
   for (i = 0; i < Mode.wm.child_count; i++)
      kill(Mode.wm.children[i], SIGCONT);
#endif

   ModulesSignal(ESIGNAL_START, NULL);

   /* Map the clients */
   MapUnmap(1);

   RunInitPrograms();
   SpawnSnappedCmds();

   Conf.startup.firsttime = 0;
   Mode.wm.save_ok = 1;
   Mode.wm.startup = 0;
   autosave();

   /* The primary event loop */
   for (;;)
      WaitEvent();

   /* Of course, we should NEVER get to this point */

   return 1;
}

static void
RunDocBrowser(void)
{
   char                file[FILEPATH_LEN_MAX];

   Esnprintf(file, sizeof(file), "%s/edox", EDirBin());
   if (!canexec(file))
      return;
   Esnprintf(file, sizeof(file), "%s/E-docs", EDirRoot());
   if (!canread(file))
      return;

   if (fork())
      return;

   Esnprintf(file, sizeof(file), "exec %s/edox %s/E-docs",
	     EDirBin(), EDirRoot());

   execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file, NULL);

   exit(0);
}

static void
RunMenuGen(void)
{

   char                file[FILEPATH_LEN_MAX];

   if (fork())
      return;

   Esnprintf(file, sizeof(file), "exec %s/scripts/e_gen_menu", EDirRoot());
   execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file, NULL);
   exit(0);
}

static void
RunInitPrograms(void)
{
   if (Mode.wm.session_start)
     {
	if (Conf.session.cmd_init && canexec(Conf.session.cmd_init))
	   system(Conf.session.cmd_init);
     }

   if (Conf.session.cmd_start && canexec(Conf.session.cmd_start))
      system(Conf.session.cmd_start);

   if (Conf.startup.firsttime && Mode.wm.master)
     {
	RunMenuGen();
	RunDocBrowser();
     }
}

void
EExit(int exitcode)
{
   int                 i;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("EExit(%d)\n", exitcode);

   if (disp)
     {
	ecore_x_ungrab();
	GrabPointerRelease();
	XAllowEvents(disp, AsyncBoth, CurrentTime);

	/* XSetInputFocus(disp, None, RevertToParent, CurrentTime); */
	/* I think this is a better way to release the grabs: (felix) */
	XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSelectInput(disp, VRoot.win, 0);
	EDisplayClose();
     }

   SignalsRestore();

   if (Mode.wm.master)
     {
	for (i = 0; i < Mode.wm.child_count; i++)
	   kill(Mode.wm.children[i], SIGINT);
     }
   else
     {
	exitcode = 0;
     }

   exit(exitcode);
}

static char        *userDir = NULL;
static char        *cacheDir = NULL;

const char         *
EDirBin(void)
{
   return ENLIGHTENMENT_BIN;
}

const char         *
EDirRoot(void)
{
   return ENLIGHTENMENT_ROOT;
}

static void
EDirUserSet(const char *dir)
{
   if (userDir)
      Efree(userDir);
   userDir = Estrdup(dir);
}

static void
EDirUserCacheSet(const char *dir)
{
   if (cacheDir)
      Efree(cacheDir);
   cacheDir = Estrdup(dir);
}

const char         *
EDirUser(void)
{
   char               *home, buf[4096];

   if (userDir)
      return userDir;

   home = homedir(getuid());
   Esnprintf(buf, sizeof(buf), "%s/.e16", home);
   Efree(home);
   userDir = Estrdup(buf);

   return userDir;
}

const char         *
EDirUserCache(void)
{
   if (!cacheDir)
      cacheDir = Estrdup(EDirUser());
   return cacheDir;
}

static void
EDirCheck(const char *dir)
{
   if (!isdir(dir))
     {
	Alert(_("The directory %s is apparently not a directory\n"
		"This is a fatal condition.\n" "Please remove this file\n"),
	      dir);
	EExit(1);
     }
   if (!canexec(dir))
     {
	Alert(_("Do not have execute access to %s\n"
		"This is a fatal condition.\n"
		"Please check the ownership and permissions of this\n"
		"directory and take steps to rectify this.\n"), dir);
	EExit(1);
     }
   if (!canread(dir))
     {
	Alert(_("Do not have read access to %s\n" "This is a fatal condition.\n"
		"Please check the ownership and permissions of this\n"
		"directory and take steps to rectify this.\n"), dir);
	EExit(1);
     }
   if (!canwrite(dir))
     {
	Alert(_("Do not have write access to %s\n"
		"This is a fatal condition.\n"
		"Please check the ownership and permissions of this\n"
		"directory and take steps to rectify this.\n"), dir);
	EExit(1);
     }
}

void
EDirMake(const char *base, const char *name)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "%s/%s", base, name);
   if (!exists(s))
      E_md(s);
   else
      EDirCheck(s);
}

static void
EDirsSetup(void)
{
   char                s[1024], ss[1024], *home;

   home = homedir(getuid());
   if (home)
     {
	EDirCheck(home);
	Efree(home);
     }

   Esnprintf(s, sizeof(s), "%s", EDirUser());
   if (exists(s))
     {
	if (!isdir(s))
	  {
	     Esnprintf(ss, sizeof(ss), "%s.old", EDirUser());
	     E_mv(s, ss);
	     E_md(s);
	  }
	else
	   EDirCheck(s);
     }
   else
      E_md(s);

   EDirMake(EDirUser(), "themes");
   EDirMake(EDirUser(), "backgrounds");
   EDirMake(EDirUser(), "menus");

   EDirMake(EDirUserCache(), "cached");
   EDirMake(EDirUserCache(), "cached/cfg");
}

/*
 * The user control config is called "~/.e16/e_config-$DISPLAY"
 * The client data appends ".clients" onto this filename and the snapshot data
 * appends ".snapshots".
 */
static char        *
default_save_prefix(void)
{
   static char        *def_prefix = NULL;
   char               *s, buf[1024];

   if (def_prefix)
      return def_prefix;

   if (Mode.wm.window)
      Esnprintf(buf, sizeof(buf), "%s/e_config-window", EDirUser());
   else
      Esnprintf(buf, sizeof(buf), "%s/e_config-%s", EDirUser(),
		Mode.display.name);
   def_prefix = Estrdup(buf);

   for (s = def_prefix; (s = strchr(s, ':')); *s = '-');

   return def_prefix;
}

static char        *save_prefix = NULL;

static void
ESetSavePrefix(const char *path)
{
   if (save_prefix && path == NULL)
      return;

   if (save_prefix)
      Efree(save_prefix);

   if (!path)
      save_prefix = Estrdup(default_save_prefix());
   else
      save_prefix = Estrdup(path);
}

const char         *
EGetSavePrefix(void)
{
   return save_prefix;
}

const char         *
EGetSavePrefixCommon(void)
{
   static char        *pfx = NULL;
   char                buf[1024];

   if (pfx)
      return pfx;

   Esnprintf(buf, sizeof(buf), "%s/e_config", EDirUser());
   pfx = Estrdup(buf);

   return pfx;
}

static void
ECheckEprog(const char *name)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "%s/%s", EDirBin(), name);

   if (!exists(s))
     {
	Alert(_("!!!!!!!! ERROR ERROR ERROR ERROR !!!!!!!!\n" "\n"
		"Enlightenment's utility executable cannot be found at:\n"
		"\n" "%s\n"
		"This is a fatal error and Enlightenment will cease to run.\n"
		"Please rectify this situation and ensure it is installed\n"
		"correctly.\n" "\n"
		"The reason this could be missing is due to badly created\n"
		"packages, someone manually deleting that program or perhaps\n"
		"an error in installing Enlightenment.\n"), s);
	EExit(0);
     }

   if (!canexec(s))
     {
	Alert(_("!!!!!!!! ERROR ERROR ERROR ERROR !!!!!!!!\n" "\n"
		"Enlightenment's utility executable is not able to be executed:\n"
		"\n" "%s\n"
		"This is a fatal error and Enlightenment will cease to run.\n"
		"Please rectify this situation and ensure it is installed\n"
		"correctly.\n"), s);
	EExit(0);
     }
}
