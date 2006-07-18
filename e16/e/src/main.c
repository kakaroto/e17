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
#include "desktops.h"
#include "dialog.h"
#include "emodule.h"
#include "hints.h"
#include "session.h"
#include "snaps.h"
#include "timestamp.h"
#include "xwin.h"
#include <sys/utsname.h>
#include <signal.h>
#include <time.h>

const char          e_wm_name[] = "Enlightenment";
const char          e_wm_version[] = ENLIGHTENMENT_REVNAME;
const char          e_wm_date[] = E_CHECKOUT_DATE;

Display            *disp;
RealRoot            RRoot;
VirtRoot            VRoot;
EConf               Conf;
EMode               Mode;

static int          EoptGet(int argc, char **argv);
static void         EoptHelp(void);
static void         ECheckEprog(const char *name);
static void         EDirUserSet(const char *dir);
static void         EConfNameSet(const char *dir);
static void         EDirUserCacheSet(const char *dir);
static void         EDirsSetup(void);
static void         RunInitPrograms(void);

static int          eoptind = 0;
const char         *eoptarg = NULL;

typedef struct
{
   char                sopt;
   char                arg;
   const char         *lopt;
   const char         *oarg;
   const char         *desc;
} EOpt;

static const EOpt   Eopts[] = {
   {'d', 1, "display", "display", "Set display"},
   {'f', 0, "fast", NULL, "Fast startup"},
   {'h', 0, "help", NULL, "Show help"},
   {'m', 1, NULL, NULL, NULL},
   {'p', 1, "config-prefix", "prefix", "Configuration file name prefix"},
   {'P', 1, "econfdir", "path", "Set user configuration directory"},
   {'Q', 1, "ecachedir", "path", "Set user cache directory"},
   {'s', 1, "single", "screen", "Run only on given screen"},
   {'S', 1, "sm-client-id", "session id", "Set session manager ID"},
   {'t', 1, "theme", "name", "Select theme"},
   {'v', 0, "verbose", NULL, "Show additional info"},
   {'V', 0, "version", NULL, "Show version"},
   {'w', 1, "window", "WxH", "Run in window"},
   {'X', 1, NULL, NULL, NULL},
};

int
main(int argc, char **argv)
{
   int                 ch, i, loop;
   struct utsname      ubuf;
   const char         *str, *dstr;

   /* This function runs all the setup for startup, and then 
    * proceeds into the primary event loop at the end.
    */

   /* Init state variable struct */
   memset(&Mode, 0, sizeof(EMode));

   Mode.wm.master = 1;
   Mode.wm.pid = getpid();
   Mode.wm.exec_name = argv[0];
   Mode.wm.startup = 1;

   memset(&VRoot, 0, sizeof(VRoot));
   VRoot.scr = -1;

   Mode.mode = MODE_NONE;
   Mode.move.check = 1;

   str = getenv("EDEBUG");
   if (str)
      EventDebugInit(str);
   str = getenv("EDEBUG_COREDUMP");
   if (str)
      Mode.wm.coredump = 1;

   str = getenv("ECONFNAME");
   if (str)
      EConfNameSet(str);
   str = getenv("ECONFDIR");
   if (str)
      EDirUserSet(str);
   str = getenv("ECACHEDIR");
   if (str)
      EDirUserCacheSet(str);

   srand((unsigned int)time(NULL));

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

   for (loop = 1; loop;)
     {
	ch = EoptGet(argc, argv);
	if (ch <= 0)
	   break;
#if 0
	Eprintf("Opt: %c: %d - %s\n", ch, eoptind, eoptarg);
#endif
	switch (ch)
	  {
	  default:
	  case '?':
	     printf("e16: Ignoring: ");
	     for (i = eoptind; i < argc; i++)
		printf("%s ", argv[i]);
	     printf("\n");
	     loop = 0;
	     break;
	  case 'h':
	     EoptHelp();
	     exit(0);
	     break;
	  case 'd':
	     dstr = eoptarg;
	     break;
	  case 'f':
	     Mode.wm.restart = 1;
	     break;
	  case 'p':
	     EConfNameSet(eoptarg);
	     break;
	  case 'P':
	     EDirUserSet(eoptarg);
	     break;
	  case 'Q':
	     EDirUserCacheSet(eoptarg);
	     break;
	  case 's':
	     Mode.wm.single = 1;
	     VRoot.scr = strtoul(eoptarg, NULL, 10);
	     break;
	  case 'S':
	     SetSMID(eoptarg);
	     break;
	  case 't':
	     Mode.theme.path = Estrdup(eoptarg);
	     break;
	  case 'V':
	     printf("%s %s - %s\n", e_wm_name, e_wm_version, e_wm_date);
	     exit(0);
	     break;
	  case 'v':
	     EventDebugSet(EDBUG_TYPE_VERBOSE, 1);
	     break;
	  case 'w':
	     sscanf(eoptarg, "%dx%d", &VRoot.w, &VRoot.h);
	     Mode.wm.window = 1;
	     Mode.wm.single = 1;
	     Mode.wm.master = 0;
	     break;
#ifdef USE_EXT_INIT_WIN
	  case 'X':
	     ExtInitWinSet(strtoul(eoptarg, NULL, 0));
	     Mode.wm.restart = 1;
	     break;
#endif
	  case 'm':
	     Mode.wm.master = 0;
	     Mode.wm.master_screen = strtoul(eoptarg, NULL, 10);
	     break;
	  }
     }

   SignalsSetup();		/* Install signal handlers */

   SetupX(dstr);		/* This is where the we fork per screen */
   /* X is now running, and we have forked per screen */

   /* So far nothing should rely on a selected settings or theme. */
   ConfigurationLoad();		/* Load settings */

   /* Initialise internationalisation */
   LangInit();

   ECheckEprog("epp");
   ECheckEprog("eesh");
   EDirsSetup();

   /* The theme path must now be available for config file loading. */
   ThemePathFind();

   /* Set the Environment variables */
   Esetenv("EVERSION", e_wm_version);
   Esetenv("EROOT", EDirRoot());
   Esetenv("EBIN", EDirBin());
   Esetenv("ECONFDIR", EDirUser());
   Esetenv("ECACHEDIR", EDirUserCache());
   Esetenv("ETHEME", Mode.theme.path);

   /* Unmap the clients */
   MapUnmap(0);

   /* Move elsewhere? */
   HintsInit();
   CommsInit();
   SessionInit();
   LoadSnapInfo();

   ModulesSignal(ESIGNAL_INIT, NULL);

   /* Load the theme */
   ThemeConfigLoad();

   /* Do initial configuration */
   ModulesSignal(ESIGNAL_CONFIGURE, NULL);

   /* Set root window cursor */
   ECsrApply(ECSR_ROOT, VRoot.xwin);

#ifdef USE_EXT_INIT_WIN
   /* Kill the E process owning the "init window" */
   ExtInitWinKill();
#endif

   if (Mode.wm.window)
      EMapWindow(VRoot.win);

   /* let's make sure we set this up and go to our desk anyways */
   DeskGoto(DesksGetCurrent());
   ESync();

#ifdef SIGCONT
   for (i = 0; i < Mode.wm.child_count; i++)
      kill(Mode.wm.children[i], SIGCONT);
#endif

   ModulesSignal(ESIGNAL_START, NULL);
   DialogsInit();

   /* Map the clients */
   MapUnmap(1);

   RunInitPrograms();
   SpawnSnappedCmds();

   if (!Mode.wm.restart)
      StartupWindowsOpen();

   Conf.startup.firsttime = 0;
   Mode.wm.save_ok = Conf.autosave;
   Mode.wm.startup = 0;
   autosave();

   /* The primary event loop */
   EventsMain();

   SessionExit(EEXIT_QUIT, NULL);

   return 0;
}

void
EExit(int exitcode)
{
   int                 i;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("EExit(%d)\n", exitcode);

   if (disp)
     {
	EUngrabServer();
	GrabPointerRelease();
	XAllowEvents(disp, AsyncBoth, CurrentTime);

	/* XSetInputFocus(disp, None, RevertToParent, CurrentTime); */
	/* I think this is a better way to release the grabs: (felix) */
	XSetInputFocus(disp, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSelectInput(disp, VRoot.xwin, 0);
	EDisplayClose();
     }

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

/*
 * Command line parsing.
 * Not entirely standard compliant, but close enough.
 */
static int
EoptGet(int argc, char **argv)
{
   const char         *s;
   unsigned int        i, len;
   int                 lopt;
   const EOpt         *eopt;

   eoptind++;
   if (eoptind >= argc)
      return 0;

   s = argv[eoptind];
   if (*s++ != '-')
      return 0;

   lopt = 0;
   if (*s == '-')
     {
	lopt = 1;
	s++;
     }

   eoptarg = NULL;
   eopt = NULL;
   for (i = 0; i < sizeof(Eopts) / sizeof(EOpt); i++)
     {
	eopt = &Eopts[i];

	/* Short option */
	if (!lopt)
	  {
	     if (!eopt->sopt || eopt->sopt != s[0])
		continue;
	     if (eopt->arg)
	       {
		  if (s[1])
		    {
		       eoptarg = s + 1;
		       goto found;
		    }
		  goto found;
	       }
	     if (s[1])
		break;
	     goto found;
	  }

	if (!eopt->lopt)
	   continue;

	/* Long option */
	len = strlen(eopt->lopt);
	if (strncmp(eopt->lopt, s, len))
	   continue;
	if (eopt->arg)
	  {
	     if (s[len] == '\0')
		goto found;
	     if (s[len] != '=')
		break;
	     eoptarg = s + len + 1;
	  }
	goto found;
     }
   return '?';

 found:
   if (!eopt->arg || eoptarg)
      return eopt->sopt;

   if (eoptind >= argc - 1)
      return '?';		/* Missing param */

   eoptind++;
   eoptarg = argv[eoptind];
   return eopt->sopt;
}

static void
EoptHelp(void)
{
   unsigned int        i;
   const EOpt         *eopt;
   char                buf[256];

   printf("e16 options:\n");
   for (i = 0; i < sizeof(Eopts) / sizeof(EOpt); i++)
     {
	eopt = &Eopts[i];
	if (!eopt->desc)
	   continue;
	if (eopt->oarg)
	   Esnprintf(buf, sizeof(buf), "--%s <%s>", eopt->lopt, eopt->oarg);
	else
	   Esnprintf(buf, sizeof(buf), "--%s", eopt->lopt);
	printf("  -%c  %-30s\t%s\n", eopt->sopt, buf, eopt->desc);
     }
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

   LangExport();

   Esnprintf(file, sizeof(file), "exec %s/scripts/e_gen_menu", EDirRoot());
   execl(usershell(getuid()), usershell(getuid()), "-c", (char *)file, NULL);
   exit(0);
}

static void
RunInitPrograms(void)
{
   if (Mode.wm.session_start)
      SessionHelper(ESESSION_INIT);

   SessionHelper(ESESSION_START);

   if (Mode.firsttime && Mode.wm.master)
     {
	RunMenuGen();
	RunDocBrowser();
     }
}

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
EConfNameSet(const char *name)
{
   if (Mode.conf.name)
      Efree(Mode.conf.name);
   Mode.conf.name = Estrdup(name);
   Esetenv("ECONFNAME", Mode.conf.name);
}

static void
EDirUserSet(const char *dir)
{
   if (!strcmp(dir, EDirUser()))
      return;
   if (Mode.conf.dir)
      Efree(Mode.conf.dir);
   Mode.conf.dir = Estrdup(dir);
}

static void
EDirUserCacheSet(const char *dir)
{
   if (!strcmp(dir, EDirUser()))
      return;
   if (Mode.conf.cache_dir)
      Efree(Mode.conf.cache_dir);
   Mode.conf.cache_dir = Estrdup(dir);
}

static const char  *
EConfNameDefault(void)
{
   return "e_config";
}

static const char  *
EConfName(void)
{
   return (Mode.conf.name) ? Mode.conf.name : EConfNameDefault();
}

const char         *
EDirUser(void)
{
   static char        *user_dir = NULL;
   char               *home, buf[4096];

   if (Mode.conf.dir)
      return Mode.conf.dir;

   if (user_dir)
      return user_dir;

   home = homedir(getuid());
   Esnprintf(buf, sizeof(buf), "%s/.e16", home);
   Efree(home);
   user_dir = Estrdup(buf);

   return user_dir;
}

const char         *
EDirUserCache(void)
{
   if (Mode.conf.cache_dir)
      return Mode.conf.cache_dir;
   return EDirUser();
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

   Esnprintf(s, sizeof(s), "%s/menus", EDirUser());
   Mode.firsttime = !exists(s);

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
const char         *
EGetSavePrefix(void)
{
   static char        *def_prefix = NULL;
   char               *s, buf[1024];

   if (def_prefix)
      return def_prefix;

   if (Mode.conf.name)
      Esnprintf(buf, sizeof(buf), "%s/%s-%d", EDirUser(), EConfName(),
		VRoot.scr);
   else if (Mode.wm.window)
      Esnprintf(buf, sizeof(buf), "%s/%s-window", EDirUser(),
		EConfNameDefault());
   else
      Esnprintf(buf, sizeof(buf), "%s/%s-%s", EDirUser(), EConfNameDefault(),
		Mode.display.name);
   def_prefix = Estrdup(buf);

   for (s = def_prefix; (s = strchr(s, ':')) != NULL; *s = '-')
      ;

   return def_prefix;
}

const char         *
EGetSavePrefixCommon(void)
{
   static char        *pfx = NULL;
   char                buf[1024];

   if (pfx)
      return pfx;

   Esnprintf(buf, sizeof(buf), "%s/%s", EDirUser(), EConfName());
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
