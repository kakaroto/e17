/*
 * Gives the enstrom effect to the attached gevasobj when the mouse moves over it
 *
 *
 * Copyright (C) 2002 Ben Martin.
 *
 * Original author: Ben Martin
 *
 * See COPYING for full details of copying & use of this software.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 */

#include "config.h"
/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

/* Always disable NLS, since we have no config.h; 
 * a real app would not do this of course.
 */
#undef ENABLE_NLS

#ifdef ENABLE_NLS
#include<libintl.h>
#define _(String) dgettext("gtk+",String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else							/* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain)
#endif							/* ENABLE_NLS */


#include "gevasevh_throb.h"
#include "gevasimage.h"

enum {
	ARG_0,						/* Skip 0, an invalid argument ID */
	ARG_SCALE_X,
	ARG_SCALE_Y
};


static void gevasevh_throb_class_init(GtkgEvasEvHThrobClass * klass);
static void gevasevh_throb_init(GtkgEvasEvHThrob * ev);
/* GtkObject functions */
static void gevasevh_throb_destroy(GtkObject * object);
static void
gevasevh_throb_get_arg(GtkObject * object, GtkArg * arg, guint arg_id);
static void
gevasevh_throb_set_arg(GtkObject * object, GtkArg * arg, guint arg_id);

/**
 * reset the color and size back to original
 */
static void resetobj( GtkgEvasEvHThrob *ev )
{
    GtkgEvasEvH* evh = GTK_GEVASEVH( ev );
    GtkgEvasObj* go = evh->eobj;

    gevasobj_move( go,   ev->x, ev->y);
    gevasobj_resize( go, ev->w, ev->h);
    gevasimage_set_image_fill( GTK_GEVASIMAGE( go ), 0, 0, ev->w, ev->h);
    gevasobj_set_color(    go, ev->r, ev->g, ev->b, ev->a);
}


/**
 * This function is very much derived from
 * enstrom_icon_hilite_timer() in enstrom by raster
 *
 * the algo should be very similar, but lots of changes for ecore timers
 * and evas1 -> evas2 API
 *
 */
static gboolean throbFunction(gpointer user_data)
{
	GtkgEvasEvHThrob *ev = GTK_GEVASEVH_THROB(user_data);
    GtkgEvasEvH* evh = GTK_GEVASEVH( ev );
    
    double t;
    double ix, iy, iw, ih;
    double x, y, w, h;
    GtkgEvasObj* go = evh->eobj;
    GTimeVal tv;
   
    g_get_current_time( &tv );
    guint32 delta = (tv.tv_usec - ev->m_timeTracker.tv_usec) / 1000;
/*     printf("tv.tv_usec:%ld  ev->m_timeTracker.tv_usec:%ld\n",tv.tv_usec,ev->m_timeTracker.tv_usec); */
/*     printf("tv.tv_sec :%ld  ev->m_timeTracker.tv_sec :%ld\n",tv.tv_sec,ev->m_timeTracker.tv_sec); */
/*     printf("interval initial:%ld sec:%ld\n", delta, (tv.tv_sec - ev->m_timeTracker.tv_sec)); */
    if( tv.tv_sec - ev->m_timeTracker.tv_sec > 0 )
        delta += (tv.tv_sec - ev->m_timeTracker.tv_sec) * 1000;
    
/*     tv.tv_usec -= ev->m_timeTracker.tv_usec; */
/*     tv.tv_sec  -= ev->m_timeTracker.tv_sec; */
/*     if( tv.tv_sec < 0 ) */
/*         tv.tv_usec += abs(tv.tv_sec)*1000; */

    t = delta / 1000.0;
/*     printf("interval final:%ld t:%f \n", delta, t); */
    
    if (t > 0.5) t = 0.5;

    x = ev->x;
    y = ev->y;
    w = ev->w;
    h = ev->h;
    
//    gevasobj_get_geometry( go, &x, &y, &w, &h);
    iw = w * (1.0 + (t * 1));
    ih = h * (1.0 + (t * 1));
    ix = x + ((w - iw) / 2);
    iy = y + ((h - ih) / 2);
    gevasobj_move( go, ix, iy);
    gevasobj_resize( go, iw, ih);
    gevasimage_set_image_fill( GTK_GEVASIMAGE( go ), 0, 0, iw, ih);
    gevasobj_set_color( go, 255, 255, 255, (1.0 - (t * 2)) * 255);
   
    if (t >= 0.5)
    {
/*         printf("resetting timer\n"); */
        g_get_current_time( &ev->m_timeTracker );

        resetobj( ev );
    }
    

    ev->m_val++;
    return 1;
}


GEVASEV_HANDLER_RET
gevasev_throb_mouse_in(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	double x = 0, y = 0, w = 0, h = 0, sw = 0, sh = 0;
	GtkgEvasEvHThrob *ev;
	GtkgEvasObj *ggobj = GTK_GEVASOBJ(gevasobj);
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_THROB(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_THROB(object);


    GtkgEvasEvH* evh = GTK_GEVASEVH( ev );
    GtkgEvasObj* go = evh->eobj;
    gevasobj_get_geometry( go, &ev->x, &ev->y, &ev->w, &ev->h);
    gevasobj_get_color(    go, &ev->r, &ev->g, &ev->b, &ev->a);
    
    g_get_current_time( &ev->m_timeTracker );

    ev->m_val = 0;
    ev->m_throbTimer = g_timeout_add( ev->m_interval,
                                      throbFunction,
                                      ev );
    
    return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_throb_mouse_out(GtkObject * object, GtkObject * gevasobj, int _b,
							  int _x, int _y)
{
	double x = 0, y = 0, w = 0, h = 0;
	GtkgEvasEvHThrob *ev;
	GtkgEvasObj *ggobj = GTK_GEVASOBJ(gevasobj);
	g_return_val_if_fail(object != NULL, GEVASEV_HANDLER_RET_NEXT);
	g_return_val_if_fail(GTK_IS_GEVASEVH_THROB(object),
						 GEVASEV_HANDLER_RET_NEXT);
	ev = GTK_GEVASEVH_THROB(object);

    if( ev->m_throbTimer )
        g_source_remove( ev->m_throbTimer );

    resetobj( ev );
    
    return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_throb_mouse_down(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_throb_mouse_up(GtkObject * object, GtkObject * gevasobj, int _b,
							 int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}

GEVASEV_HANDLER_RET
gevasev_throb_mouse_move(GtkObject * object, GtkObject * gevasobj, int _b,
							   int _x, int _y)
{
	return GEVASEV_HANDLER_RET_NEXT;
}


static GtkObjectClass *parent_class = NULL;

guint gevasevh_throb_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"GtkgEvasEvHThrob",
			sizeof(GtkgEvasEvHThrob),
			sizeof(GtkgEvasEvHThrobClass),
			(GtkClassInitFunc) gevasevh_throb_class_init,
			(GtkObjectInitFunc) gevasevh_throb_init,
			/* reserved_1 */ NULL,
			/* reserved_2 */ NULL,
			(GtkClassInitFunc) NULL,
		};

		ev_type = gtk_type_unique(gevasevh_get_type(), &ev_info);
	}

	return ev_type;
}

static void gevasevh_throb_class_init(GtkgEvasEvHThrobClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasEvHClass *evh_klass;

	object_class = (GtkObjectClass *) klass;
	evh_klass    = (GtkgEvasEvHClass *) klass;
	parent_class = gtk_type_class(gevasevh_get_type());

	object_class->destroy = gevasevh_throb_destroy;
	object_class->get_arg = gevasevh_throb_get_arg;
	object_class->set_arg = gevasevh_throb_set_arg;

	evh_klass->handler_mouse_in   = gevasev_throb_mouse_in;
	evh_klass->handler_mouse_out  = gevasev_throb_mouse_out;
	evh_klass->handler_mouse_down = gevasev_throb_mouse_down;
	evh_klass->handler_mouse_up   = gevasev_throb_mouse_up;
	evh_klass->handler_mouse_move = gevasev_throb_mouse_move;

/* 	gtk_object_add_arg_type(GTK_GEVASEVH_THROB_SCALE_X, */
/* 							GTK_TYPE_DOUBLE, GTK_ARG_READWRITE, ARG_SCALE_X); */
/* 	gtk_object_add_arg_type(GTK_GEVASEVH_THROB_SCALE_Y, */
/* 							GTK_TYPE_DOUBLE, GTK_ARG_READWRITE, ARG_SCALE_Y); */
}

static void gevasevh_throb_init(GtkgEvasEvHThrob * ev)
{
    ev->m_throbTimer = 0;
    ev->m_interval = 40;
}

GtkObject *gevasevh_throb_new( GtkgEvasObj* go )
{
	GtkgEvasEvHThrob *ev;
	GtkgEvasEvH *hev;

	ev = gtk_type_new(gevasevh_throb_get_type());
	hev = (GtkgEvasEvH *) ev;

    gevasobj_add_evhandler( go, GTK_OBJECT(ev) );
    gevasevh_set_gevasobj( ev, go );
    
    return GTK_OBJECT(ev);
}

/* GtkObject functions */


static void gevasevh_throb_destroy(GtkObject * object)
{
	GtkgEvasEvHThrob *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_THROB(object));
	ev = GTK_GEVASEVH_THROB(object);

    if( ev->m_throbTimer )
        g_source_remove( ev->m_throbTimer );
    
	/* Chain up */
	if (GTK_OBJECT_CLASS(parent_class)->destroy)
		(*GTK_OBJECT_CLASS(parent_class)->destroy) (object);
}


static void
gevasevh_throb_set_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHThrob *ev;

	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_THROB(object));
	ev = GTK_GEVASEVH_THROB(object);

	switch (arg_id) {

		default:
			break;
	}
}

static void
gevasevh_throb_get_arg(GtkObject * object, GtkArg * arg, guint arg_id)
{
	GtkgEvasEvHThrob *ev;
	g_return_if_fail(object != NULL);
	g_return_if_fail(GTK_IS_GEVASEVH_THROB(object));
	ev = GTK_GEVASEVH_THROB(object);

	switch (arg_id) {

		default:
			arg->type = GTK_TYPE_INVALID;
			break;
	}
}
