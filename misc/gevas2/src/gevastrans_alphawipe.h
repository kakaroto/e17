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


#ifndef INC_GTK_GEVASTRANS_ALWIPE_H
#define INC_GTK_GEVASTRANS_ALWIPE_H

#include <gtk/gtkobject.h>
#include "gevasobj.h"
#include "gevas.h"

#ifdef __cplusplus
extern "C" {
#endif							/* __cplusplus */
#define GTK_GEVTRANS_ALPHAWIPE(obj) \
    GTK_CHECK_CAST (obj, gevastrans_alphawipe_get_type (), geTransAlphaWipe)
#define GTK_GEVTRANS_ALPHAWIPE_CLASS(klass) \
    GTK_CHECK_CLASS_CAST (klass, gevastrans_alphawipe_get_type (), geTransAlphaWipeClass)
#define GTK_IS_GEVTRANS_ALPHAWIPE(obj) \
    GTK_CHECK_TYPE (obj, gevastrans_alphawipe_get_type ())

    typedef struct _geTransAlphaWipe geTransAlphaWipe;
	typedef struct _geTransAlphaWipeClass geTransAlphaWipeClass;

	struct _geTransAlphaWipe {
		GtkObject gobj;

        void (*perform)( geTransAlphaWipe* ev, GtkgEvasObj* fobj, GtkgEvasObj* tobj );
        guint m_blendTimer;
        guint m_blendTimerInterval;
        guint m_blendAlphaJumpFactor;
    };

	struct _geTransAlphaWipeClass {
		GtkObjectClass parent_class;
	};


	guint gevastrans_alphawipe_get_type(void);
	geTransAlphaWipe *gevastrans_alphawipe_new();

    /**
     * Assumed to be a stateless function by the sprite code.
     *
     * tobj is assumed to be not visible
     * the alpha values of both fobj and tobj can be set to anything by this
     * function
     *
     * It is assumed that the fobj and tobj are at the correct locations for the
     * final transition before this function is called.
     */
    void gevastrans_perform( geTransAlphaWipe* thisp, GtkgEvasObj* fobj, GtkgEvasObj* tobj );




#ifdef __cplusplus
}
#endif							/* __cplusplus */
#endif							/* __GTK_GEVASOBJ_H__ */
