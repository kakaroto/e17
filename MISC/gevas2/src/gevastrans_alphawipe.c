/*
 * For use with sprites, wipes from one frame to the next with an alpha blend
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

/* If this widget was in an application or library, 
 * this i18n stuff would be in some other header file.
 * (in Gtk, gtkintl.h; in the Gnome libraries, libgnome/gnome-i18nP.h; 
 *  in a Gnome application, libgnome/gnome-i18n.h)
 */

#include "config.h"
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


#include "gevastrans_alphawipe.h"
//#include "gevas_sprite.h"

enum {
	ARG_0 = 100,				/* Skip 0, an invalid argument ID */
	ARG_MAINOBJ,
	ARG_AUXOBJ
};

static void gevastrans_alphawipe_class_init(geTransAlphaWipeClass * klass);
static void gevastrans_alphawipe_init(geTransAlphaWipe * ev);

#define EVAS(ev) _gevas_evas( GTK_OBJECT(ev))
#define EVASO(ev) _gevas_get_obj( GTK_OBJECT(ev))

struct blendState
{
    geTransAlphaWipe* ev;
    GtkgEvasObj* fobj;
    GtkgEvasObj* tobj;
    guint blendAlphaJumpFactor;
};

static gboolean blendf(gpointer user_data)
{
    struct blendState* blendst = (struct blendState*)user_data;
    geTransAlphaWipe* ev = GTK_GEVTRANS_ALPHAWIPE( blendst->ev );
    GtkgEvasObj* fobj = blendst->fobj;
    GtkgEvasObj* tobj = blendst->tobj;
    int bfac = blendst->blendAlphaJumpFactor;
    int done = 0;
    int a = 0;
	Evas_Object* eo = gevasobj_get_evasobj( (GtkObject*)fobj );

    a = gevasobj_get_alpha( fobj );
    gevasobj_set_alpha( fobj, a-bfac );

//    printf("blendf() a:%ld bfac:%ld fobj:%p ev:%p eo:%p\n", a, bfac, fobj, ev, eo );

    a = gevasobj_get_alpha( tobj );
    gevasobj_set_alpha( tobj, a+bfac );
    
    done = a>=255;
    
    if( done )
        {
//            printf("blend complete\n");
//            gevasobj_hide( fobj );
            free( blendst );
        }
    return !done;
}

static void _perform( geTransAlphaWipe* ev, GtkgEvasObj* fobj, GtkgEvasObj* tobj )
{
    struct blendState* s = (struct blendState*)calloc( 1, sizeof(struct blendState) );

//    printf("_perform() interval:%ld\n", ev->m_blendTimerInterval );
//    printf("_perform() fobj:%p feo:%p\n", fobj, gevasobj_get_evasobj( (GtkObject*)fobj ) );
    
    s->fobj = fobj;
    s->tobj = tobj;
    s->ev   = ev;
    s->blendAlphaJumpFactor = ev->m_blendAlphaJumpFactor;

/*     gevasobj_hide( fobj ); */
/*     gevasobj_show( tobj ); */
    
    gevasobj_set_alpha( tobj, 0 );
    gevasobj_show( tobj );

    ev->m_blendTimer = g_timeout_add( ev->m_blendTimerInterval, blendf, s );
}

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

static GtkObjectClass *parent_class = NULL;

guint gevastrans_alphawipe_get_type(void)
{
	static guint ev_type = 0;

	if (!ev_type) {
		static const GtkTypeInfo ev_info = {
			"geTransAlphaWipe",
			sizeof(geTransAlphaWipe),
			sizeof(geTransAlphaWipeClass),
			(GtkClassInitFunc)  gevastrans_alphawipe_class_init,
			(GtkObjectInitFunc) gevastrans_alphawipe_init,
			/* reserved_1 */    NULL,
			/* reserved_2 */    NULL,
			(GtkClassInitFunc)  NULL,
		};

		ev_type = gtk_type_unique(gtk_object_get_type(), &ev_info);
	}

	return ev_type;
}


static void gevastrans_alphawipe_class_init(geTransAlphaWipeClass * klass)
{
	GtkObjectClass *object_class;
	GtkgEvasObjClass *gobj = (GtkgEvasObjClass *) klass;

	object_class = (GtkObjectClass *) klass;
	parent_class = gtk_type_class(gtk_object_get_type());
}

static void gevastrans_alphawipe_init(geTransAlphaWipe * ev)
{
    ev->perform = _perform;
    ev->m_blendTimer = 0;
    ev->m_blendTimerInterval = 20;
    ev->m_blendAlphaJumpFactor = 5;
}

geTransAlphaWipe *gevastrans_alphawipe_new()
{
	geTransAlphaWipe *ev;

	ev = gtk_type_new(gevastrans_alphawipe_get_type());

	return GTK_GEVTRANS_ALPHAWIPE(ev);
}

/*********************************************************************/

void gevastrans_perform( geTransAlphaWipe* thisp, GtkgEvasObj* fobj, GtkgEvasObj* tobj )
{
	geTransAlphaWipe* ev;
	g_return_if_fail(thisp != NULL);
	g_return_if_fail(fobj  != NULL);
	g_return_if_fail(tobj  != NULL);
	g_return_if_fail(GTK_IS_GEVTRANS_ALPHAWIPE(thisp));
    ev = GTK_GEVTRANS_ALPHAWIPE( thisp );

//    printf("gevastrans_perform() thisp:%p fobj:%p\n", thisp, fobj );
    ev->perform( ev, fobj, tobj );
}

