#include "config.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ipc.h"
#include "viewer.h"

typedef struct _client
{
   gchar              *name;
   Window              win;
   gchar              *msg;
   gchar              *clientname;
   gchar              *version;
   gchar              *author;
   gchar              *email;
   gchar              *web;
   gchar              *address;
   gchar              *info;
} Client;

Client             *e_client = NULL;

static Window       comms_win = 0;
static Window       my_win = 0;
static GdkWindow   *gdkwin = NULL;
static GdkWindow   *gdkwin2 = NULL;
static void         (*msg_receive_callback) (gchar * msg) = NULL;

static void         CommsSetup(void);
static GdkFilterReturn CommsFilter(GdkXEvent * gdk_xevent, GdkEvent * event,
				   gpointer data);
static Window       CommsFindCommsWindow(void);
static gchar       *CommsGet(Client ** c, XEvent * ev);
static Client      *MakeClient(Window win);
static void         ListFreeClient(void *ptr);
extern gchar        in_init;
extern gint         gdk_error_warnings;

gint
CommsInit(void      (*msg_receive_func) (gchar * msg))
{
   Window              win;
   gchar               st[32];
   Client             *cl;

   CommsSetup();
   comms_win = win = CommsFindCommsWindow();
   cl = MakeClient(win);
   g_snprintf(st, sizeof(st), "%8x", (int)win);
   cl->name = g_strdup(st);
   e_client = cl;
   gdkwin = gdk_window_foreign_new(win);
   gdk_window_add_filter(gdkwin, CommsFilter, NULL);
   gdkwin2 = gdk_window_foreign_new(my_win);
   gdk_window_add_filter(gdkwin2, CommsFilter, NULL);
   XSelectInput(GDK_DISPLAY(), win,
		StructureNotifyMask | SubstructureNotifyMask);
   msg_receive_callback = msg_receive_func;
   return 1;
}

void
CommsSend(const gchar * s)
{
   gchar               ss[21];
   int                 i, j, k, len;
   XEvent              ev;
   static Atom         a = 0;
   Client             *c;

   c = e_client;
   if ((!s) || (!c))
      return;
   len = strlen(s);
   if (!a)
      a = XInternAtom(GDK_DISPLAY(), "ENL_MSG", True);
   ev.xclient.type = ClientMessage;
   ev.xclient.serial = 0;
   ev.xclient.send_event = True;
   ev.xclient.window = c->win;
   ev.xclient.message_type = a;
   ev.xclient.format = 8;

   for (i = 0; i < len + 1; i += 12)
     {
	g_snprintf(ss, sizeof(ss), "%8x", (int)my_win);
	for (j = 0; j < 12; j++)
	  {
	     ss[8 + j] = s[i + j];
	     if (!s[i + j])
		j = 12;
	  }
	ss[20] = 0;
	for (k = 0; k < 20; k++)
	   ev.xclient.data.b[k] = ss[k];
	XSendEvent(GDK_DISPLAY(), c->win, False, 0, (XEvent *) & ev);
     }
   return;
}

static              GdkFilterReturn
CommsFilter(GdkXEvent * gdk_xevent, GdkEvent * event, gpointer data)
{
   XEvent             *xevent;
   gchar              *msg = NULL;
   Client             *c = NULL;
   static Atom         a;

   data = NULL;

   if (!a)
      a = XInternAtom(GDK_DISPLAY(), "ENL_MSG", True);
   xevent = (XEvent *) gdk_xevent;
   switch (xevent->type)
     {
     case DestroyNotify:
	if (xevent->xdestroywindow.window == comms_win)
	  {
	     gint                i;

	     comms_win = 0;
	     if (!in_init)
	       {
		  for (i = 0; ((i < 20) && (!comms_win)); i++)
		    {
		       if ((comms_win = CommsFindCommsWindow()))
			 {
			    gchar               st[256];

			    ListFreeClient(e_client);
			    e_client = MakeClient(comms_win);
			    g_snprintf(st, sizeof(st), "%8x", (int)comms_win);
			    e_client->name = g_strdup(st);
			    if (gdkwin)
			       gdk_window_unref(gdkwin);
			    gdkwin = gdk_window_foreign_new(comms_win);
			    gdk_window_add_filter(gdkwin, CommsFilter, NULL);
			    XSelectInput(GDK_DISPLAY(), comms_win,
					 StructureNotifyMask |
					 SubstructureNotifyMask);
			 }
		       sleep(1);
		    }
	       }
	     if (!comms_win)
	       {
		  GtkWidget          *win, *label, *align, *frame, *button,
		     *vbox;

#ifdef ENABLE_GTK2
		  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
#else
		  win = gtk_window_new(GTK_WINDOW_DIALOG);
#endif
		  gtk_window_set_policy(GTK_WINDOW(win), 0, 0, 1);
		  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
		  frame = gtk_frame_new(NULL);
		  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
		  align = gtk_alignment_new(0.0, 0.0, 0.0, 0.0);
		  gtk_container_set_border_width(GTK_CONTAINER(align), 32);
		  vbox = gtk_vbox_new(FALSE, 5);
		  button = gtk_button_new_with_label("Bye bye!");
		  gtk_signal_connect(GTK_OBJECT(button), "clicked",
				     GTK_SIGNAL_FUNC(on_exit_application),
				     NULL);
		  label =
		     gtk_label_new("EEEEEEEEEEEEEEK! HELP! HEEEEEEEEEELP!\n"
				   "\n" "Enlightenemnt dissapeared on me!\n"
				   "\n"
				   "Someone help me - I'm drowning - drowning\n"
				   "\n"
				   "That's it. I'm out of here. I can't deal with the\n"
				   "idea of losing Enlightenment. I'm going to jump and\n"
				   "don't try and stop me.\n" "\n");
		  gtk_container_add(GTK_CONTAINER(win), frame);
		  gtk_container_add(GTK_CONTAINER(frame), align);
		  gtk_container_add(GTK_CONTAINER(align), vbox);
		  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
		  gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
		  gtk_widget_show_all(win);
		  gtk_main();
		  exit(1);
	       }
	  }
	else
	  {
	     gdk_window_destroy_notify(((GdkEventAny *) event)->window);
	  }
	return GDK_FILTER_REMOVE;
	break;
     case ClientMessage:
	if (xevent->xclient.message_type != a)
	   return GDK_FILTER_CONTINUE;
	msg = CommsGet(&c, xevent);
	if (msg)
	  {
	     if (msg_receive_callback)
		(*msg_receive_callback) (msg);
	     g_free(msg);
	     return GDK_FILTER_REMOVE;
	  }
	break;
     default:
	return GDK_FILTER_REMOVE;
     }
   return GDK_FILTER_REMOVE;
}

static void
CommsSetup(void)
{
   my_win = XCreateSimpleWindow(GDK_DISPLAY(), GDK_ROOT_WINDOW(),
				-100, -100, 5, 5, 0, 0, 0);
}

static              Window
CommsFindCommsWindow(void)
{
   unsigned char      *s;
   Atom                a, ar;
   unsigned long       num, after;
   int                 format;
   Window              win = 0;
   Window              rt;
   int                 dint;
   unsigned int        duint;

   a = XInternAtom(GDK_DISPLAY(), "ENLIGHTENMENT_COMMS", True);
   if (a != None)
     {
	s = NULL;
	XGetWindowProperty(GDK_DISPLAY(), GDK_ROOT_WINDOW(), a, 0, 14, False,
			   AnyPropertyType, &ar, &format, &num, &after, &s);
	if (s)
	  {
	     sscanf((char *)s, "%*s %x", (unsigned int *)&win);
	     XFree(s);
	  }
	if (win)
	  {
	     gint                p;

	     p = gdk_error_warnings;
	     gdk_error_warnings = 0;
	     if (!XGetGeometry(GDK_DISPLAY(), win, &rt, &dint, &dint,
			       &duint, &duint, &duint, &duint))
		win = 0;
	     gdk_flush();
	     gdk_error_warnings = p;
	     s = NULL;
	     if (win)
	       {
		  XGetWindowProperty(GDK_DISPLAY(), win, a, 0, 14, False,
				     AnyPropertyType, &ar, &format, &num,
				     &after, &s);
		  if (s)
		     XFree(s);
		  else
		     win = 0;
	       }
	  }
     }
   return win;
}

static gchar       *
CommsGet(Client ** c, XEvent * ev)
{
   gchar               s[13], s2[9], *msg;
   int                 i;
   Window              win;
   Client             *cl;
   static Atom         a;

   if (!a)
      a = XInternAtom(GDK_DISPLAY(), "ENL_MSG", True);
   if ((!ev) || (!c))
      return (NULL);
   if (ev->type != ClientMessage)
      return (NULL);
   if (ev->xclient.message_type != a)
      return (NULL);
   s[12] = 0;
   s2[8] = 0;
   msg = NULL;
   for (i = 0; i < 8; i++)
      s2[i] = ev->xclient.data.b[i];
   for (i = 0; i < 12; i++)
      s[i] = ev->xclient.data.b[i + 8];
   sscanf(s2, "%x", (int *)&win);
   cl = e_client;
   if (!cl)
      return (NULL);
   if (cl->msg)
     {
	/* append text to end of msg */
	cl->msg = g_realloc(cl->msg, strlen(cl->msg) + strlen(s) + 1);
	if (!cl->msg)
	   return (NULL);
	strcat(cl->msg, s);
     }
   else
     {
	/* new msg */
	cl->msg = g_malloc(strlen(s) + 1);
	if (!cl->msg)
	   return (NULL);
	strcpy(cl->msg, s);
     }
   if (strlen(s) < 12)
     {
	msg = cl->msg;
	cl->msg = NULL;
	*c = cl;
     }
   return (msg);
}

static Client      *
MakeClient(Window win)
{
   Client             *c;

   c = g_malloc(sizeof(Client));
   if (!c)
      return (NULL);
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
   return (c);
}

static void
ListFreeClient(void *ptr)
{
   Client             *c;

   c = (Client *) ptr;
   if (!c)
      return;
   if (c->name)
      g_free(c->name);
   if (c->msg)
      g_free(c->msg);
   if (c->clientname)
      g_free(c->clientname);
   if (c->version)
      g_free(c->version);
   if (c->author)
      g_free(c->author);
   if (c->email)
      g_free(c->email);
   if (c->web)
      g_free(c->web);
   if (c->address)
      g_free(c->address);
   if (c->info)
      g_free(c->info);
   g_free(c);
   return;
}
