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

static Window       comms_win = 0;

static Atom         XA_ENLIGHTENMENT_COMMS = 0;
static Atom         XA_ENL_MSG = 0;

void
CommsSetup(void)
{
   char                s[1024];

   EDBUG(5, "CommsSetup");

   comms_win = XCreateSimpleWindow(disp, root.win, -100, -100, 5, 5, 0, 0, 0);
   XSelectInput(disp, comms_win, StructureNotifyMask | SubstructureNotifyMask);
   Esnprintf(s, sizeof(s), "WINID %8x", (int)comms_win);
   XA_ENLIGHTENMENT_COMMS = XInternAtom(disp, "ENLIGHTENMENT_COMMS", False);
   XChangeProperty(disp, comms_win, XA_ENLIGHTENMENT_COMMS, XA_STRING, 8,
		   PropModeReplace, (unsigned char *)s, strlen(s));
   XChangeProperty(disp, root.win, XA_ENLIGHTENMENT_COMMS, XA_STRING, 8,
		   PropModeReplace, (unsigned char *)s, strlen(s));

   XA_ENL_MSG = XInternAtom(disp, "ENL_MSG", False);

   EDBUG_RETURN_;
}

void
CommsFindCommsWindow(void)
{
   unsigned char      *s;
   Atom                a, ar;
   unsigned long       num, after;
   int                 format;
   Window              rt;
   int                 dint;
   unsigned int        duint;

   EDBUG(6, "CommsFindCommsWindow");
   a = XA_ENLIGHTENMENT_COMMS;
   if (a != None)
     {
	s = NULL;
	XGetWindowProperty(disp, root.win, a, 0, 14, False, AnyPropertyType,
			   &ar, &format, &num, &after, &s);
	if (s)
	  {
	     comms_win = 0;
	     sscanf((char *)s, "%*s %lx", &comms_win);
	     XFree(s);
	  }
	else
	  {
	     (comms_win = 0);
	  }
	if (comms_win)
	  {
	     if (!EGetGeometry
		 (disp, comms_win, &rt, &dint, &dint, &duint, &duint, &duint,
		  &duint))
		comms_win = 0;
	     s = NULL;
	     if (comms_win)
	       {
		  XGetWindowProperty(disp, comms_win, a, 0, 14, False,
				     AnyPropertyType, &ar, &format, &num,
				     &after, &s);
		  if (s)
		     XFree(s);
		  else
		     comms_win = 0;
	       }
	  }
     }
   EDBUG_RETURN_;
}

static void
CommsDoSend(Window win, const char *s)
{
   char                ss[21];
   int                 i, j, k, len;
   XEvent              ev;

   EDBUG(5, "CommsDoSend");

   if ((!win) || (!s))
      EDBUG_RETURN_;

   len = strlen(s);
   ev.xclient.type = ClientMessage;
   ev.xclient.serial = 0;
   ev.xclient.send_event = True;
   ev.xclient.window = win;
   ev.xclient.message_type = XA_ENL_MSG;
   ev.xclient.format = 8;
   for (i = 0; i < len + 1; i += 12)
     {
	Esnprintf(ss, sizeof(ss), "%8x", (int)comms_win);
	for (j = 0; j < 12; j++)
	  {
	     ss[8 + j] = s[i + j];
	     if (!s[i + j])
		j = 12;
	  }
	ss[20] = 0;
	for (k = 0; k < 20; k++)
	   ev.xclient.data.b[k] = ss[k];
	XSendEvent(disp, win, False, 0, (XEvent *) & ev);
     }
   EDBUG_RETURN_;
}

void
CommsSend(Client * c, const char *s)
{
   EDBUG(5, "CommsSend");

   if (!c)
      EDBUG_RETURN_;

   CommsDoSend(c->win, s);

   EDBUG_RETURN_;
}

/*
 * When we are running in multi-head, connect to the master wm process
 * and send the message
 */
void
CommsSendToMasterWM(const char *s)
{
   EDBUG(5, "CommsSendToMasterWM");

   if (Mode.wm.master)
      EDBUG_RETURN_;

   CommsDoSend(RootWindow(disp, Mode.wm.master_screen), s);

   EDBUG_RETURN_;
}

/*
 * When we are running in multi-head, connect to the slave wm processes
 * and broadcast the message
 */
void
CommsBroadcastToSlaveWMs(const char *s)
{
   int                 screen;

   EDBUG(5, "CommsBroadcastToSlaveWMs");

   if (!Mode.wm.master || Mode.wm.single)
      EDBUG_RETURN_;

   for (screen = 0; screen < Mode.display.screens; screen++)
     {
	if (screen != Mode.wm.master_screen)
	   CommsDoSend(RootWindow(disp, screen), s);
     }

   EDBUG_RETURN_;
}

static char        *
CommsGet(Client ** c, XClientMessageEvent * ev)
{
   char                s[13], s2[9], *msg, st[32];
   int                 i;
   Window              win = 0;
   Client             *cl;

   EDBUG(5, "CommsGet");
   if ((!ev) || (!c))
      EDBUG_RETURN(NULL);
   if (ev->message_type != XA_ENL_MSG)
      EDBUG_RETURN(NULL);

   s[12] = 0;
   s2[8] = 0;
   msg = NULL;
   for (i = 0; i < 8; i++)
      s2[i] = ev->data.b[i];
   for (i = 0; i < 12; i++)
      s[i] = ev->data.b[i + 8];
   sscanf(s2, "%lx", &win);
   cl = (Client *) FindItem(NULL, win, LIST_FINDBY_ID, LIST_TYPE_CLIENT);
   if (!cl)
     {
	cl = MakeClient(win);
	if (!cl)
	   EDBUG_RETURN(NULL);
	Esnprintf(st, sizeof(st), "%8x", (int)win);
	cl->name = Estrdup(st);
	AddItem((void *)cl, st, cl->win, LIST_TYPE_CLIENT);
	XSelectInput(disp, win, StructureNotifyMask | SubstructureNotifyMask);
     }
   if (cl->msg)
     {
	/* append text to end of msg */
	cl->msg = Erealloc(cl->msg, strlen(cl->msg) + strlen(s) + 1);
	if (!cl->msg)
	   EDBUG_RETURN(NULL);
	strcat(cl->msg, s);
     }
   else
     {
	/* new msg */
	cl->msg = Emalloc(strlen(s) + 1);
	if (!cl->msg)
	   EDBUG_RETURN(NULL);
	strcpy(cl->msg, s);
     }
   if (strlen(s) < 12)
     {
	msg = cl->msg;
	cl->msg = NULL;
	*c = cl;
     }
   EDBUG_RETURN(msg);
}

void
CommsBroadcast(const char *s)
{
   char              **l;
   int                 num, i;
   Client             *c;

   EDBUG(5, "CommsBroadcast");
   l = ListItems(&num, LIST_TYPE_CLIENT);
   if (!s)
      EDBUG_RETURN_;
   for (i = 0; i < num; i++)
     {
	c = (Client *) FindItem(l[i], 0, LIST_FINDBY_NAME, LIST_TYPE_CLIENT);
	if (c)
	   CommsSend(c, s);
     }
   freestrlist(l, num);
   EDBUG_RETURN_;
}

Client             *
MakeClient(Window win)
{
   Client             *c;

   EDBUG(6, "MakeClient");
   c = Emalloc(sizeof(Client));
   if (!c)
      EDBUG_RETURN(NULL);
   c->name = NULL;
   c->win = win;
   c->msg = NULL;
   c->clientname = NULL;
   c->version = NULL;
   c->author = NULL;
   c->email = NULL;
   c->web = NULL;
   c->address = NULL;
   c->info = NULL;
   c->pmap = 0;
   EDBUG_RETURN(c);
}

void
ListFreeClient(void *ptr)
{
   Client             *c;

   EDBUG(6, "ListFreeClient");
   c = (Client *) ptr;
   if (!c)
      EDBUG_RETURN_;
   if (c->name)
      Efree(c->name);
   if (c->msg)
      Efree(c->msg);
   if (c->clientname)
      Efree(c->clientname);
   if (c->version)
      Efree(c->version);
   if (c->author)
      Efree(c->author);
   if (c->email)
      Efree(c->email);
   if (c->web)
      Efree(c->web);
   if (c->address)
      Efree(c->address);
   if (c->info)
      Efree(c->info);
   Efree(c);
   EDBUG_RETURN_;
}

void
DeleteClient(Client * c)
{
   Client             *cc;

   EDBUG(6, "DeleteClient");
   cc = RemoveItem(NULL, c->win, LIST_FINDBY_ID, LIST_TYPE_CLIENT);
   ListFreeClient(cc);
   EDBUG_RETURN_;
}

void
HandleComms(XClientMessageEvent * ev)
{
   Client             *c;
   char               *s;
   const char         *s1, *s2;

   EDBUG(4, "HandleComms");

   s = CommsGet(&c, ev);
   if (!s)
      EDBUG_RETURN_;

   if (!HandleIPC(s, c))
     {
	s1 = (c->clientname) ? c->clientname : "UNKNOWN";
	s2 = (c->version) ? c->version : "UNKNOWN";
	DialogOK(_("E IPC Error"),
		 _("Received Unknown Client Message.\n"
		   "Client Name:    %s\n" "Client Version: %s\n"
		   "Message Contents:\n\n" "%s\n"), s1, s2, s);
	SoundPlay("SOUND_ERROR_IPC");
     }
   Efree(s);

   EDBUG_RETURN_;
}
