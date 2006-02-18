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
#include "ecore-e16.h"
#include "emodule.h"
#include "ewins.h"
#include "snaps.h"
#include "xwin.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

#ifdef USE_EXT_INIT_WIN
static Window       new_init_win_ext = None;
#endif

#ifndef DEFAULT_SH_PATH
#ifdef __sgi
/*
 * It appears that SGI (at least IRIX 6.4) uses ksh as their sh, and it
 * seems to run in restricted mode, so things like restart fail miserably.
 * Let's use csh instead
 * -KDT 07/31/98
 */
#define DEFAULT_SH_PATH "/sbin/csh"
#else
#define DEFAULT_SH_PATH "/bin/sh"
#endif
#endif

static int          sm_fd = -1;

/* True if we are saving state for a doExit("restart") */
static int          restarting = False;

#if 0				/* Unused */

/* The saved window details */
static int          num_match = 0;

typedef struct _match
{
   char               *session_id;
   char               *name;
   char               *class;
   char               *role;
   char               *command;
   char                used;
   int                 x, y, w, h;
   int                 desktop, iconified, shaded, sticky, layer;
}
Match;

Match              *matches = NULL;

static void
SaveWindowStates(void)
{
   EWin               *const *lst, *ewin;
   int                 i, num, x, y;
   FILE               *f;
   char                s[4096], ss[4096];

   if (!Mode.wm.save_ok)
      return;

   Etmp(s);
   f = fopen(s, "w");
   if (f == NULL)
      return;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if ((!EwinIsInternal(ewin))
	    && ((ewin->icccm.wm_command) || (ewin->session_id)))
	  {
	     x = 0;
	     y = 0;
	     if (!EoIsSticky(ewin))
	       {
		  DeskGetArea(EoGetDesk(ewin), &x, &y);
		  x = x * VRoot.w;
		  y = y * VRoot.h;
	       }
	     fprintf(f, "[CLIENT] %i %i %i %i %i %i %i %i %i\n",
		     EoGetX(ewin) + x, EoGetY(ewin) + y, ewin->client.w,
		     ewin->client.h, EoGetDesk(ewin), ewin->state.iconified,
		     ewin->state.shaded, EoIsSticky(ewin), EoGetLayer(ewin));
	     if (ewin->session_id)
		fprintf(f, "  [SESSION_ID] %s\n", ewin->session_id);
	     if (ewin->icccm.wm_res_name)
		fprintf(f, "  [NAME] %s\n", ewin->icccm.wm_res_name);
	     if (ewin->icccm.wm_res_class)
		fprintf(f, "  [CLASS] %s\n", ewin->icccm.wm_res_class);
	     if (ewin->icccm.wm_role)
		fprintf(f, "  [ROLE] %s\n", ewin->icccm.wm_role);
	     if (ewin->icccm.wm_command)
		fprintf(f, "  [COMMAND] %s\n", ewin->icccm.wm_command);
	  }
     }
   fclose(f);

   Esnprintf(ss, sizeof(ss), "%s.clients", EGetSavePrefix());
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("SaveWindowStates: %s\n", ss);
   E_mv(s, ss);
   if (!isfile(ss))
      Alert(_("There was an error writing the clients "
	      "session save file.\n" "You may have run out of disk "
	      "space, not have permission\n"
	      "to write to your filing system "
	      "or other similar problems.\n"));
}

static void
LoadWindowStates(void)
{
   FILE               *f;
   char                s[4096], s1[4096];

   Esnprintf(s, sizeof(s), "%s.clients", EGetSavePrefix());
   f = fopen(s, "r");
   if (f)
     {
	while (fgets(s, sizeof(s), f))
	  {
	     s[strlen(s) - 1] = 0;
	     sscanf(s, "%4000s", s1);
	     if (!strcmp(s1, "[CLIENT]"))
	       {
		  num_match++;
		  matches = Erealloc(matches, sizeof(Match) * num_match);
		  matches[num_match - 1].session_id = NULL;
		  matches[num_match - 1].name = NULL;
		  matches[num_match - 1].class = NULL;
		  matches[num_match - 1].role = NULL;
		  matches[num_match - 1].command = NULL;
		  matches[num_match - 1].used = 0;
		  sscanf(s, "%*s %i %i %i %i %i %i %i %i %i",
			 &(matches[num_match - 1].x),
			 &(matches[num_match - 1].y),
			 &(matches[num_match - 1].w),
			 &(matches[num_match - 1].h),
			 &(matches[num_match - 1].desktop),
			 &(matches[num_match - 1].iconified),
			 &(matches[num_match - 1].shaded),
			 &(matches[num_match - 1].sticky),
			 &(matches[num_match - 1].layer));
	       }
	     else if (!strcmp(s1, "[SESSION_ID]"))
	       {
		  matches[num_match - 1].session_id = Estrdup(atword(s, 2));
	       }
	     else if (!strcmp(s1, "[NAME]"))
	       {
		  matches[num_match - 1].name = Estrdup(atword(s, 2));
	       }
	     else if (!strcmp(s1, "[CLASS]"))
	       {
		  matches[num_match - 1].class = Estrdup(atword(s, 2));
	       }
	     else if (!strcmp(s1, "[ROLE]"))
	       {
		  matches[num_match - 1].role = Estrdup(atword(s, 2));
		  /* Needed for matching X11R5 clients */
	       }
	     else if (!strcmp(s1, "[COMMAND]"))
	       {
		  matches[num_match - 1].command = Estrdup(atword(s, 2));
	       }
	  }
	fclose(f);
     }
}

/* These matching rules try to cover everyone with minimal work done
 * for clients that actually comply with the X11R6 ICCCM. */
void
MatchEwinToSM(EWin * ewin)
{
   int                 i, ax, ay;

   if (GetSMfd() < 0)
      return;

   for (i = 0; i < num_match; i++)
     {
	if ((!matches[i].used))
	  {
	     /* No match unless both have or both lack a session_id */
	     if (!ewin->session_id)
	       {
		  if (matches[i].session_id)
		     continue;
	       }
	     if (ewin->session_id)
	       {
		  if (!matches[i].session_id)
		     continue;
	       }
	     if ((ewin->session_id))
	       {
		  /* The X11R6 protocol guarantees matching session_ids */
		  if (strcmp(ewin->session_id, matches[i].session_id))
		     continue;
	       }
	     else
	       {
		  /* The X11R5 protocol was based around the WM_COMMAND
		   * property which should be preserved over sessions
		   * by compliant apps.
		   * 
		   * FIXME: Mozilla DELETES the WM_COMMAND property on 
		   * a regular basis so is is wise NOT to update 
		   * this property when it is set to NULL. */
		  if ((ewin->icccm.wm_command) && (matches[i].command)
		      && strcmp(ewin->icccm.wm_command, matches[i].command))
		     continue;
	       }

	     if ((ewin->icccm.wm_role) && (matches[i].role))
	       {
		  /* The X11R6 protocol guarantees that any WM_WINDOW_ROLE
		   * is unique among the windows sharing a SM_CLIENT_ID.
		   * 
		   * Clients which use the same WM_WINDOW_ROLE on two 
		   * windows are breaking the ICCCM even if they have 
		   * different WM_CLASS properties on those windows. */
		  if (strcmp(ewin->icccm.wm_role, matches[i].role))
		     continue;
	       }
	     else
	       {
		  /* The WM_CLASS is a stable basis for a test. */
		  if ((ewin->icccm.wm_res_class) && (matches[i].class)
		      && (strcmp(ewin->icccm.wm_res_class, matches[i].class)))
		     continue;
		  if ((ewin->icccm.wm_res_name) && (matches[i].name)
		      && (strcmp(ewin->icccm.wm_res_name, matches[i].name)))
		     continue;

		  /* Twm also matches on the WM_NAME but only when this value
		   * has not changed since the window was first mapped.
		   * This seems a bit kludgy to me. (: */
	       }

	     matches[i].used = 1;
	     ewin->state.placed = 1;
	     ewin->icccm.start_iconified = matches[i].iconified;
	     EoSetSticky(ewin, matches[i].sticky);
	     ewin->state.shaded = matches[i].shaded;
	     EoSetLayer(ewin, matches[i].layer);
	     if (!EoIsSticky(ewin))
		EoSetDesk(ewin, matches[i].desktop);
	     /* if it's NOT (X11R6 and already placed by the client) */
	     if (!((ewin->state.placed) && (ewin->session_id)))
	       {
		  DeskGetArea(EoGetDesk(ewin), &ax, &ay);
		  ewin->client.x = matches[i].x - (ax * VRoot.w);
		  ewin->client.y = matches[i].y - (ay * VRoot.h);
		  ewin->client.grav = NorthWestGravity;
		  ewin->client.w = matches[i].w;
		  ewin->client.h = matches[i].h;
		  EMoveResizeWindow(_EwinGetClientWin(ewin),
				    ewin->client.x, ewin->client.y,
				    ewin->client.w, ewin->client.h);
	       }
	     if (EventDebug(EDBUG_TYPE_SNAPS))
		Eprintf("Snap get sess  %#lx: %4d+%4d %4dx%4d: %s\n",
			_EwinGetClientXwin(ewin), ewin->client.x,
			ewin->client.y, ewin->client.w, ewin->client.h,
			EwinGetName(ewin));
	     break;
	  }
     }
}
#endif /* Unused */

void
autosave(void)
{
   if (!Mode.wm.save_ok)
      return;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("autosave\n");

   Real_SaveSnapInfo(0, NULL);

   /* Save the configuration parameters */
   ConfigurationSave();
}

#ifdef HAVE_X11_SM_SMLIB_H

#include <X11/SM/SMlib.h>

/*
 * NB! If the discard property is revived, the dual use of buf must be fixed.
 */
#define USE_DISCARD_PROPERTY 0

static char        *sm_client_id = NULL;
static SmcConn      sm_conn = NULL;

/* Used by multiheaded child processes to identify when they have
 * recieved the new sm_file value from the master_pid process */
static int          stale_sm_file = 0;

static void
set_save_props(SmcConn smc_conn, int master_flag)
{
   const char         *s;
   char               *user;
   const char         *program;
   char                priority = 10;
   char                style;
   int                 i, n;
   SmPropValue         programVal;
   SmPropValue         userIDVal;

#if USE_DISCARD_PROPERTY
   const char         *sh = "sh";
   const char         *c = "-c";
   const char         *sm_file;
   SmPropValue         discardVal[3];
   SmProp              discardProp;
#endif
   SmPropValue         restartVal[32];
   SmPropValue         styleVal;
   SmPropValue         priorityVal;
   SmProp              programProp;
   SmProp              userIDProp;
   SmProp              restartProp;
   SmProp              cloneProp;
   SmProp              styleProp;
   SmProp              priorityProp;
   SmProp             *props[7];
   char                bufs[32], bufm[32], bufx[32];

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("set_save_props\n");

   programProp.name = (char *)SmProgram;
   programProp.type = (char *)SmARRAY8;
   programProp.num_vals = 1;
   programProp.vals = &programVal;

   userIDProp.name = (char *)SmUserID;
   userIDProp.type = (char *)SmARRAY8;
   userIDProp.num_vals = 1;
   userIDProp.vals = &userIDVal;

#if USE_DISCARD_PROPERTY
   discardProp.name = (char *)SmDiscardCommand;
   discardProp.type = (char *)SmLISTofARRAY8;
   discardProp.num_vals = 3;
   discardProp.vals = discardVal;
#endif

   restartProp.name = (char *)SmRestartCommand;
   restartProp.type = (char *)SmLISTofARRAY8;
   restartProp.vals = restartVal;

   cloneProp.name = (char *)SmCloneCommand;
   cloneProp.type = (char *)SmLISTofARRAY8;
   cloneProp.vals = restartVal;

   styleProp.name = (char *)SmRestartStyleHint;
   styleProp.type = (char *)SmCARD8;
   styleProp.num_vals = 1;
   styleProp.vals = &styleVal;

   priorityProp.name = (char *)"_GSM_Priority";
   priorityProp.type = (char *)SmCARD8;
   priorityProp.num_vals = 1;
   priorityProp.vals = &priorityVal;

   if (master_flag)
      /* Master WM restarts immediately for a doExit("restart") */
      style = restarting ? SmRestartImmediately : SmRestartIfRunning;
   else
      /* Slave WMs never restart */
      style = SmRestartNever;

   user = username(getuid());
   /* The SM specs state that the SmProgram should be the argument passed
    * to execve. Passing argv[0] is close enough. */
   program = Mode.wm.exec_name;

   userIDVal.length = (user) ? strlen(user) : 0;
   userIDVal.value = user;
   programVal.length = strlen(program);
   programVal.value = (char *)program;
   styleVal.length = 1;
   styleVal.value = &style;
   priorityVal.length = 1;
   priorityVal.value = &priority;

#if USE_DISCARD_PROPERTY
   /* Tell session manager how to clean up our old data */
   sm_file = EGetSavePrefix();
   Esnprintf(buf, sizeof(buf), "rm %s*.clients.*", sm_file);

   discardVal[0].length = strlen(sh);
   discardVal[0].value = sh;
   discardVal[1].length = strlen(c);
   discardVal[1].value = c;
   discardVal[2].length = strlen(buf);
   discardVal[2].value = buf;	/* ??? Also used in restartVal ??? */
#endif

   n = 0;
   restartVal[n++].value = (char *)program;
   if (Mode.wm.single)
     {
	Esnprintf(bufs, sizeof(bufs), "%i", Mode.wm.master_screen);
	restartVal[n++].value = (char *)"-s";
	restartVal[n++].value = (char *)bufs;
     }
   else if (restarting && !Mode.wm.master)
     {
	Esnprintf(bufm, sizeof(bufm), "%i", Mode.wm.master_screen);
	restartVal[n++].value = (char *)"-m";
	restartVal[n++].value = bufm;
     }
#ifdef USE_EXT_INIT_WIN
   if (restarting)
     {
	Esnprintf(bufx, sizeof(bufx), "%#lx", new_init_win_ext);
	restartVal[n++].value = (char *)"-X";
	restartVal[n++].value = bufx;
     }
#endif
#if 0
   restartVal[n++].value = (char *)smfile;
   restartVal[n++].value = (char *)sm_file;
#endif
   s = Mode.conf.name;
   if (s)
     {
	restartVal[n++].value = (char *)"-p";
	restartVal[n++].value = (char *)s;
     }
   s = Mode.conf.dir;
   if (s)
     {
	restartVal[n++].value = (char *)"-P";
	restartVal[n++].value = (char *)s;
     }
   s = Mode.conf.cache_dir;
   if (s)
     {
	restartVal[n++].value = (char *)"-Q";
	restartVal[n++].value = (char *)s;
     }
   s = sm_client_id;
   restartVal[n++].value = (char *)"-S";
   restartVal[n++].value = (char *)s;

   for (i = 0; i < n; i++)
      restartVal[i].length = strlen(restartVal[i].value);

   restartProp.num_vals = n;

   /* SM specs require SmCloneCommand excludes "--sm-client-id" option */
   cloneProp.num_vals = restartProp.num_vals - 2;

   if (EventDebug(EDBUG_TYPE_SESSION))
      for (i = 0; i < restartProp.num_vals; i++)
	 Eprintf("restartVal[i]: %2d: %s\n", restartVal[i].length,
		 (char *)restartVal[i].value);

   n = 0;
   props[n++] = &programProp;
   props[n++] = &userIDProp;
#if USE_DISCARD_PROPERTY
   props[n++] = &discardProp;
#endif
   props[n++] = &restartProp;
   props[n++] = &cloneProp;
   props[n++] = &styleProp;
   props[n++] = &priorityProp;

   SmcSetProperties(smc_conn, n, props);
   if (user)
      Efree(user);
}

/* This function is usually exclusively devoted to saving data.
 * However, E sometimes wants to save state and exit immediately afterwards
 * so that the SM will restart it in a different theme. Therefore, we include
 * a suicide clause at the end.
 */
static void
callback_save_yourself2(SmcConn smc_conn, SmPointer client_data __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_yourself2\n");

#if 0				/* FIXME - Unused - Remove? */
   /* dont need anymore */
   /* autosave(); */
   if (!Mode.wm.master)
     {
	struct timeval      tv1, tv2;

	gettimeofday(&tv1, NULL);

	/* This loop should rarely be needed */
	while (stale_sm_file)
	  {
	     WaitEvent();
	     gettimeofday(&tv2, NULL);
	     if (tv2.tv_sec - tv1.tv_sec > 10)
	       {
		  SmcSaveYourselfDone(smc_conn, False);
		  return;
	       }
	  }
     }
   stale_sm_file = 1;
#endif

   set_save_props(smc_conn, Mode.wm.master);
   SmcSaveYourselfDone(smc_conn, True);
   if (restarting)
      EExit(0);
}

static void
callback_save_yourself(SmcConn smc_conn, SmPointer client_data __UNUSED__,
		       int save_style __UNUSED__, Bool shutdown __UNUSED__,
		       int interact_style __UNUSED__, Bool fast __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_yourself\n");

#if 0				/* FIXME - Unused - Remove? */
   if (Mode.wm.master)
     {
#if 0
	char                s[4096];
	int                 fd;

	Esnprintf(s, sizeof(s), "sm_file %s", EGetSavePrefix());
	fd = Emkstemp(s + 8);
	if (fd < 0)
	  {
	     SmcSaveYourselfDone(smc_conn, False);
	     return;
	  }
	SetSMFile(s + 8);
#endif

	CommsBroadcastToSlaveWMs(EGetSavePrefix());
	/* dont need */
	/* autosave(); */
#if 0
	if (strcmp(GetSMFile(), GetGenericSMFile()))
	  {
	     if (exists(GetGenericSMFile()))
		E_rm(GetGenericSMFile());
	     symlink(GetSMFile(), GetGenericSMFile());
	  }
#endif
     }
#endif

#if 0				/* Unused */
   SaveWindowStates();
#endif
   SmcRequestSaveYourselfPhase2(smc_conn, callback_save_yourself2, NULL);
}

static void
callback_die(SmcConn smc_conn __UNUSED__, SmPointer client_data __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_die\n");

   if (Mode.wm.master)
      SoundPlay("SOUND_EXIT");
   EExit(0);
}

static void
callback_save_complete(SmcConn smc_conn __UNUSED__,
		       SmPointer client_data __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_save_complete\n");
}

static void
callback_shutdown_cancelled(SmcConn smc_conn, SmPointer client_data __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("callback_shutdown_cancelled\n");

   SmcSaveYourselfDone(smc_conn, False);
}

static Atom         atom_sm_client_id;
static Atom         atom_wm_client_leader;

static IceConn      ice_conn;

static void
ice_io_error_handler(IceConn connection __UNUSED__)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("ice_io_error_handler\n");

   /* The less we do here the better - the default handler does an
    * exit(1) instead of closing the losing connection. */
}

#endif /* HAVE_X11_SM_SMLIB_H */

void
SessionInit(void)
{
#ifdef HAVE_X11_SM_SMLIB_H
   static SmPointer    context;
   SmcCallbacks        callbacks;

   atom_sm_client_id = XInternAtom(disp, "SM_CLIENT_ID", False);
   atom_wm_client_leader = XInternAtom(disp, "WM_CLIENT_LEADER", False);

   IceSetIOErrorHandler(ice_io_error_handler);

   callbacks.save_yourself.callback = callback_save_yourself;
   callbacks.die.callback = callback_die;
   callbacks.save_complete.callback = callback_save_complete;
   callbacks.shutdown_cancelled.callback = callback_shutdown_cancelled;

   callbacks.save_yourself.client_data = callbacks.die.client_data =
      callbacks.save_complete.client_data =
      callbacks.shutdown_cancelled.client_data = (SmPointer) NULL;
   if (getenv("SESSION_MANAGER"))
     {
	char                error_string_ret[4096] = "";
	char               *client_id = NULL;

	if (sm_client_id)
	   client_id = Estrdup(sm_client_id);
	sm_conn =
	   SmcOpenConnection(NULL, &context, SmProtoMajor, SmProtoMinor,
			     SmcSaveYourselfProcMask | SmcDieProcMask |
			     SmcSaveCompleteProcMask |
			     SmcShutdownCancelledProcMask, &callbacks,
			     client_id, &sm_client_id, 4096, error_string_ret);
	if (client_id)
	   Efree(client_id);

	if (error_string_ret[0])
	   fprintf(stderr, "While connecting to session manager:\n%s.",
		   error_string_ret);
     }
   if (sm_conn)
     {
	char                style[2];
	SmPropValue         styleVal;
	SmProp              styleProp;
	SmProp             *props[1];

	style[0] = SmRestartIfRunning;
	style[1] = 0;

	styleVal.length = 1;
	styleVal.value = style;

	styleProp.name = (char *)SmRestartStyleHint;
	styleProp.type = (char *)SmCARD8;
	styleProp.num_vals = 1;
	styleProp.vals = &styleVal;

	props[0] = &styleProp;

	ice_conn = SmcGetIceConnection(sm_conn);
	sm_fd = IceConnectionNumber(ice_conn);
	/* Just in case we are a copy of E created by a doExit("restart") */
	SmcSetProperties(sm_conn, 1, props);
	fcntl(sm_fd, F_SETFD, fcntl(sm_fd, F_GETFD, 0) | FD_CLOEXEC);
     }
   stale_sm_file = 1;
#endif /* HAVE_X11_SM_SMLIB_H */

#if 0				/* Unused */
   LoadWindowStates();
#endif

   if (!Conf.session.script)
      Conf.session.script = Estrdup("$EROOT/scripts/session.sh");
   if (!Conf.session.cmd_reboot)
      Conf.session.cmd_reboot = Estrdup("reboot");
   if (!Conf.session.cmd_halt)
      Conf.session.cmd_halt = Estrdup("poweroff");
}

void
ProcessICEMSGS(void)
{
#ifdef HAVE_X11_SM_SMLIB_H
   IceProcessMessagesStatus status;

   if (sm_fd < 0)
      return;
   status = IceProcessMessages(ice_conn, NULL, NULL);
   if (status == IceProcessMessagesIOError)
     {
	/* Less of the hope.... E survives */
	DialogAlert(_("ERROR!\n" "\n"
		      "Lost the Session Manager that was there?\n"
		      "Here here session manager... come here... want a bone?\n"
		      "Oh come now! Stop sulking! Bugger. Oh well. "
		      "Will continue without\n" "a session manager.\n" "\n"
		      "I'll survive somehow.\n" "\n" "\n" "... I hope.\n"));
	SmcCloseConnection(sm_conn, 0, NULL);
	sm_conn = NULL;
	sm_fd = -1;
     }
#endif /* HAVE_X11_SM_SMLIB_H */
}

int
GetSMfd(void)
{
   return sm_fd;
}

void
SessionGetInfo(EWin * ewin, Atom atom_change)
{
#ifdef HAVE_X11_SM_SMLIB_H
   Ecore_X_Window      win;
   int                 num;

   if (ewin->session_id)
     {
	Efree(ewin->session_id);
	ewin->session_id = NULL;
     }

   /* We can comply with the ICCCM because gtk is working correctly */
   if ((atom_change) &&
       (!(atom_change == atom_sm_client_id ||
	  atom_change == atom_wm_client_leader)))
      return;

   num = ecore_x_window_prop_window_get(_EwinGetClientXwin(ewin),
					atom_wm_client_leader, &win, 1);
   if (num > 0)
      ewin->session_id = ecore_x_window_prop_string_get(win, atom_sm_client_id);
#else
   ewin = NULL;
   atom_change = 0;
#endif /* HAVE_X11_SM_SMLIB_H */
}

void
SetSMID(const char *smid)
{
#ifdef HAVE_X11_SM_SMLIB_H
   sm_client_id = Estrdup(smid);
#else
   smid = NULL;
#endif /* HAVE_X11_SM_SMLIB_H */
}

void
SessionSave(int shutdown)
{
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("SessionSave(%d)\n", shutdown);

   /* dont' need anymore */
   /* autosave(); */
#ifdef HAVE_X11_SM_SMLIB_H
   if (shutdown && sm_conn)
     {
	SmcCloseConnection(sm_conn, 0, NULL);
	sm_conn = NULL;
	sm_fd = -1;
     }
#endif /* HAVE_X11_SM_SMLIB_H */
}

/*
 * Normally, the SM will throw away all the session data for a client
 * that breaks its connection unexpectedly. In order to avoid this we 
 * have to let the SM handle the restart (by setting a SmRestartStyleHint
 * of SmRestartImmediately). Rather than forcing all SM clients to do a
 * checkpoint (which would be a bit cleaner) we just save our own state
 * and then restore it on restart. We grab X input via the ext_init_win
 * so the our clients remain frozen while we are down.
 */
static void
doSMExit(int mode, const char *params)
{
   int                 l;
   char                s[1024];
   const char         *ss;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("doSMExit: mode=%d prm=%p\n", mode, params);

   restarting = True;

#if 0				/* Unused */
   SaveWindowStates();
#endif

   if (!params)
      SessionSave(1);
   Real_SaveSnapInfo(0, NULL);

   if (mode != EEXIT_THEME && mode != EEXIT_RESTART)
      SessionHelper(ESESSION_STOP);

   if (disp)
     {
	/* We may get here from HandleXIOError */
	EwinsSetFree();
	if (Mode.wm.startup && Mode.wm.exiting)
	   MapUnmap(1);
	XSelectInput(disp, VRoot.win, 0);
	ExtInitWinKill();
	ESync();

	/* Forget about cleaning up if no disp */
	ModulesSignal(ESIGNAL_EXIT, NULL);
     }

   ss = NULL;
   switch (mode)
     {
     case EEXIT_EXEC:
	SoundPlay("SOUND_EXIT");
	EDisplayClose();

	Esnprintf(s, sizeof(s), "exec %s", params);
	if (EventDebug(EDBUG_TYPE_SESSION))
	   Eprintf("doSMExit: %s\n", s);
	execl(DEFAULT_SH_PATH, DEFAULT_SH_PATH, "-c", s, NULL);
	break;

     case EEXIT_THEME:
	ss = params;
     case EEXIT_RESTART:
	SoundPlay("SOUND_WAIT");
#ifdef USE_EXT_INIT_WIN
	if (disp)
	   new_init_win_ext = ExtInitWinCreate();
#endif
	EDisplayClose();

	l = 0;
	l += Esnprintf(s + l, sizeof(s) - l, "exec %s -f", Mode.wm.exec_name);
	if (Mode.wm.single)
	   l += Esnprintf(s + l, sizeof(s) - l, " -s %d", VRoot.scr);
	else if (!Mode.wm.master)
	   l +=
	      Esnprintf(s + l, sizeof(s) - l, " -m %d", Mode.wm.master_screen);
#ifdef HAVE_X11_SM_SMLIB_H
	if (sm_client_id)
	   l += Esnprintf(s + l, sizeof(s) - l, " -S %s", sm_client_id);
#endif
#ifdef USE_EXT_INIT_WIN
	if (new_init_win_ext != None)
	   l += Esnprintf(s + l, sizeof(s) - l, " -X %li", new_init_win_ext);
#endif
	if (ss)
	   l += Esnprintf(s + l, sizeof(s) - l, " -t %s", ss);

	if (EventDebug(EDBUG_TYPE_SESSION))
	   Eprintf("doSMExit: %s\n", s);

	execl(DEFAULT_SH_PATH, DEFAULT_SH_PATH, "-c", s, NULL);
	break;
     }

   restarting = False;
   SoundPlay("SOUND_EXIT");
   EExit(0);
}

static void
SessionLogout(void)
{
#ifdef HAVE_X11_SM_SMLIB_H
   if (sm_conn)
     {
	SmcRequestSaveYourself(sm_conn, SmSaveBoth, True, SmInteractStyleAny,
			       False, True);
     }
   else
#endif /* HAVE_X11_SM_SMLIB_H */
     {
	doSMExit(EEXIT_EXIT, NULL);
     }
}

static void
LogoutCB(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
#ifdef HAVE_X11_SM_SMLIB_H
   if (sm_conn)
     {
	SessionLogout();
     }
   else
#endif /* HAVE_X11_SM_SMLIB_H */
     {
	/* 0:LogOut -: No    -or-        */
	/* 0:Halt 1:Reboot 2:LogOut -:No */
	switch (val)
	  {
	  case 0:
	     if (Conf.session.enable_reboot_halt)
		SessionExit(EEXIT_EXEC, Conf.session.cmd_halt);
	     else
		SessionExit(EEXIT_EXIT, NULL);
	     break;
	  case 1:
	     SessionExit(EEXIT_EXEC, Conf.session.cmd_reboot);
	     break;
	  case 2:
	     SessionExit(EEXIT_EXIT, NULL);
	     break;
	  }
     }
}

static void
SessionLogoutConfirm(void)
{
   Dialog             *d;
   EWin               *ewin;

   d = DialogFind("LOGOUT_DIALOG");
   if (!d)
     {
	SoundPlay("SOUND_LOGOUT");
	d = DialogCreate("LOGOUT_DIALOG");
	DialogSetTitle(d, _("Are you sure?"));
	DialogSetText(d, _("\n\n"
			   "    Are you sure you wish to log out ?    \n"
			   "\n\n"));
	if (Conf.session.enable_reboot_halt)
	  {
	     DialogAddButton(d, _("  Yes, Shut Down  "), LogoutCB, 1,
			     DLG_BUTTON_OK);
	     DialogAddButton(d, _("  Yes, Reboot  "), LogoutCB, 1,
			     DLG_BUTTON_OK);
	  }
	DialogAddButton(d, _("  Yes, Log Out  "), LogoutCB, 1, DLG_BUTTON_OK);
	DialogAddButton(d, _("  No  "), NULL, 1, DLG_BUTTON_CANCEL);
	DialogBindKey(d, "Escape", DialogCallbackClose, 1);
	DialogBindKey(d, "Return", LogoutCB, 0);
     }

   ShowDialog(d);

   ewin = FindEwinByDialog(d);
   if (ewin)
      ArrangeEwinCentered(ewin);
   return;
}

void
SessionExit(int mode, const char *param)
{
   if (mode == EEXIT_LOGOUT)
     {
	if (Conf.session.enable_logout_dialog)
	   SessionLogoutConfirm();
	else
	   SessionLogout();
	return;
     }

   if (Mode.wm.exiting++)
     {
	/* This may be possible during nested signal handling */
	Eprintf("SessionExit already in progress ... now exiting\n");
	exit(1);
     }

   doSMExit(mode, param);
}

static void
SessionRunProg(const char *prog, const char *params)
{
   char                buf[4096];
   const char         *s;

   if (params)
     {
	Esnprintf(buf, sizeof(buf), "%s %s", prog, params);
	s = buf;
     }
   else
     {
	s = prog;
     }
   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("SessionRunProg: %s\n", s);
   system(s);
}

void
SessionHelper(int when)
{
   switch (when)
     {
     case ESESSION_INIT:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "init");
#if ENABLE_OLD_SESSION_HELPERS
	else if (Conf.session.cmd_init)
	   SessionRunProg(Conf.session.cmd_init, NULL);
#endif
	break;
     case ESESSION_START:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "start");
#if ENABLE_OLD_SESSION_HELPERS
	else if (Conf.session.cmd_start)
	   SessionRunProg(Conf.session.cmd_start, NULL);
#endif
	break;
     case ESESSION_STOP:
	if (Conf.session.enable_script && Conf.session.script)
	   SessionRunProg(Conf.session.script, "stop");
#if ENABLE_OLD_SESSION_HELPERS
	else if (Conf.session.cmd_stop)
	   SessionRunProg(Conf.session.cmd_stop, NULL);
#endif
	break;
     }
}

/*
 * Session dialog
 */
static char         tmp_session_script;
static char         tmp_logout_dialog;
static char         tmp_reboot_halt;

static void
CB_ConfigureSession(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Conf.session.enable_script = tmp_session_script;
	Conf.session.enable_logout_dialog = tmp_logout_dialog;
	Conf.session.enable_reboot_halt = tmp_reboot_halt;
     }
   autosave();
}

void
SettingsSession(void)
{
   Dialog             *d;
   DItem              *table, *di;

   d = DialogFind("CONFIGURE_SESSION");
   if (d)
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_SESSION");

   tmp_session_script = Conf.session.enable_script;
   tmp_logout_dialog = Conf.session.enable_logout_dialog;
   tmp_reboot_halt = Conf.session.enable_reboot_halt;

   d = DialogCreate("CONFIGURE_SESSION");
   DialogSetTitle(d, _("Session Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
      DialogAddHeader(d, "pix/miscellaneous.png",
		      _("Enlightenment Session\n" "Settings Dialog\n"));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Session Script"));
   DialogItemCheckButtonSetPtr(di, &tmp_session_script);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Logout Dialog"));
   DialogItemCheckButtonSetPtr(di, &tmp_logout_dialog);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable Reboot/Halt on Logout"));
   DialogItemCheckButtonSetPtr(di, &tmp_reboot_halt);

   DialogAddFooter(d, DLG_OAC, CB_ConfigureSession);

   ShowDialog(d);
}
