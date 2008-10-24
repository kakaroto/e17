/*
 * A test for gevas, you can also learn alot of the API from this.
 *
 *
 * Copyright (C) 2002 Ben Martin
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
 */
#include "config.h"

#include <gevas.h>
#include <gevasimage.h>
#include <gevastext.h>
#include <gevasevh_alpha.h>
#include <gevasevh_drag.h>
#include <gevasevh_to_gtk_signals.h>
#include <gevasevh_emouse_over.h>
#include <gevasevh_popup.h>
#include <gevasevh_obj_changer.h>
#include <gevasevh_clicks.h>
#include <gevasevh_group_selector.h>
#include <gevasevh_selectable.h>
#include <gevastwin.h>
#include <gevasgrad.h>

#include <gtk/gtk.h>

#include <stdio.h>


//#define FONT_NAME  "verdana.ttf"
#define FONT_NAME  "andover"

//#define PACKAGE_DATA_DIR "/usr/share/gevas"


static gint delete_event_cb(GtkWidget * w, GdkEventAny * e, gpointer data);
static gint gpopmenu_handle_cb(GtkWidget * window, gpointer data);

/* nasty kludges having these global... but hey this is C :-) */
static int raptor_w, raptor_h;
Evas_Object* menu_raptor = 0;
GtkWidget *wtoy;
GtkWidget *gevas;				/* needed for the Evas_Object raw callback :( */

GtkObject *evh_selector = 0;


/** gevas gtk+ callbacks **/
static gboolean gtk_mouse_raw_cb(GtkObject * object, gpointer data)
{
	printf("******** gtk_mouse_raw_cb\n");
	return FALSE;
}

static gboolean
gtk_mouse_down_cb(GtkObject * object,
				  GtkObject * gevasobj, gint _b, gint _x, gint _y,
				  gpointer data)
{
	printf("******** gtk_mouse_down_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}

static gboolean
gtk_mouse_up_cb(GtkObject * object,
				GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_mouse_up_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}

static gboolean
gtk_mouse_move_cb(GtkObject * object,
				  GtkObject * gevasobj, gint _b, gint _x, gint _y,
				  gpointer data)
{
	printf("*** gtk_mouse_move_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}

static gboolean
gtk_mouse_in_cb(GtkObject * object,
				GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_mouse_in_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}

static gboolean
gtk_mouse_out_cb(GtkObject * object,
				 GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_mouse_out_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}



/*********************************/


static gboolean
gtk_bg_mouse_in_cb(GtkObject * object,
				GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_bg_mouse_in_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}

static gboolean
gtk_bg_mouse_out_cb(GtkObject * object,
				 GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_bg_mouse_out_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}



/**********************************/


/** Here we create / modify the menu structure before showing it */
static gboolean
gtk_popup_activate_cb(GtkObject * object,
					  GtkObject * gevasobj, gint _b, gint _x, gint _y,
					  gpointer data)
{
	static GtkMenu *menu = 0;
	static gint number_shows = 1;
	static gchar number_shows_buf[1024];
	static GtkWidget *number_shows_mitem = 0;
	GtkWidget *mitem;
	gchar *label;

	printf("*** gtk_popup_activate_cb b:%d x:%d y:%d\n", _b, _x, _y);

	if (!menu) {
		menu = (GtkMenu *) gtk_menu_new();
		label = "cut";
		gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
		gtk_signal_connect(GTK_OBJECT(mitem), "activate",
						   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

		label = "copy";
		gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
		gtk_signal_connect(GTK_OBJECT(mitem), "activate",
						   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

		label = "emboss";
		gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
		gtk_signal_connect(GTK_OBJECT(mitem), "activate",
						   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

		sprintf(number_shows_buf, "number_shows:%d", number_shows);
		label = number_shows_buf;
		gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
		gtk_signal_connect(GTK_OBJECT(mitem), "activate",
						   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

		number_shows_mitem = mitem;
	} else {
			/** monster the menu so the user knows we are here **/
		GtkWidget *menu_item = number_shows_mitem;

		if (GTK_BIN(menu_item)->child) {
			GtkWidget *child = GTK_BIN(menu_item)->child;

			/* do stuff with child */
			if (GTK_IS_LABEL(child)) {
				sprintf(number_shows_buf, "number_shows:%d", number_shows);
				gtk_label_set_text(GTK_LABEL(child), number_shows_buf);
			}
		}

	}

	if (menu) {
		GdkEvent *gev = gevas_get_current_event(gevasobj_get_gevas(gevasobj));

		gtk_widget_show_all(GTK_WIDGET(menu));

		gtk_menu_popup(menu, NULL, NULL, NULL, NULL, _b, gev->button.time);
	}
	number_shows++;

	return FALSE;
}


static gboolean
gtk_dclick_cb(GtkObject * object,
			  GtkObject * gevasobj, gint _b, gint _x, gint _y, gpointer data)
{
	printf("*** gtk_dclick_cb b:%d x:%d y:%d\n", _b, _x, _y);
	return FALSE;
}


/** raw evas cllbacks **/
#define RAPTOR_GROW_SIZE 20
void _mouse_in(void *_data, Evas* _e, Evas_Object* _o, void *event_info )
{
    Evas_Event_Mouse_In* ev = (Evas_Event_Mouse_In*)event_info;
    int _b = ev->buttons;
    int _x = ev->output.x;
    int _y = ev->output.y;
	Evas_Coord x = 0, y = 0, w = 0, h = 0;

	printf("mouse in evas callback...\n");

    evas_object_geometry_get( _o, &x, &y, &w, &h);
	evas_object_resize( _o, raptor_w + RAPTOR_GROW_SIZE,
				raptor_h + RAPTOR_GROW_SIZE);
	evas_object_move( _o, x - RAPTOR_GROW_SIZE / 2, y - RAPTOR_GROW_SIZE / 2);
    evas_object_image_fill_set( _o, 0, 0, raptor_w + RAPTOR_GROW_SIZE,
						raptor_h + RAPTOR_GROW_SIZE);
	gevas_queue_redraw((GtkgEvas *) _data);
}
void _mouse_out(void *_data, Evas* _e, Evas_Object* _o, void *event_info )
{
    Evas_Event_Mouse_Out* ev = (Evas_Event_Mouse_Out*)event_info;
    int _b = ev->buttons;
    int _x = ev->output.x;
    int _y = ev->output.y;
    Evas_Coord x = 0, y = 0, w = 0, h = 0;
	printf("mouse out evas callback...\n");
    evas_object_geometry_get( _o, &x, &y, &w, &h);
	evas_object_resize( _o, raptor_w, raptor_h);
	evas_object_move( _o, x + RAPTOR_GROW_SIZE / 2, y + RAPTOR_GROW_SIZE / 2);
	evas_object_image_fill_set( _o, 0, 0, raptor_w, raptor_h);
	gevas_queue_redraw((GtkgEvas *) _data);
}

static int dragging = 0;
void _mouse_down(void *_data, Evas* _e, Evas_Object* _o, void *event_info )
{
    Evas_Event_Mouse_Down* ev = (Evas_Event_Mouse_Down*)event_info;
    int _b = ev->button;
    int _x = ev->output.x;
    int _y = ev->output.y;

	printf("mouse_down evas callback %d\n", (int) _o);

    if (_b == 1) {
		dragging = 1;
	}
	if (_b == 3 && _o == menu_raptor) {
		static GtkMenu *menu = 0;	/* static var for demo only... */
		GtkWidget *mitem;
		gchar *label;

		if (!menu) {
			menu = (GtkMenu *) gtk_menu_new();
			label = "cut";
			gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
			gtk_signal_connect(GTK_OBJECT(mitem), "activate",
							   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

			label = "copy";
			gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
			gtk_signal_connect(GTK_OBJECT(mitem), "activate",
							   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);

			label = "emboss";
			gtk_menu_append(menu, mitem = gtk_menu_item_new_with_label(label));
			gtk_signal_connect(GTK_OBJECT(mitem), "activate",
							   GTK_SIGNAL_FUNC(gpopmenu_handle_cb), label);
		}
		{
			GdkEvent *gev = gevas_get_current_event((GtkgEvas *) _data);

			gtk_widget_show_all(GTK_WIDGET(menu));

            /**  To keep evas happy about the mouse state **/
            time_t tt = time( 0 );
            evas_event_feed_mouse_up( _e, _b, EVAS_BUTTON_NONE, tt, 0 );

			gtk_menu_popup(menu, NULL, NULL, NULL, NULL, _b, gev->button.time);
		}
	}
}
void _mouse_up(void *_data, Evas* _e, Evas_Object* _o, void *event_info )
{
    Evas_Event_Mouse_Up* ev = (Evas_Event_Mouse_Up*)event_info;
    int _b = ev->button;
    int _x = ev->output.x;
    int _y = ev->output.y;

	printf("_mouse_up evas callback %d\n", (int) _o);


	if (_b == 1) {
		dragging = 0;
	}

}

void _mouse_move(void *_data, Evas* _e, Evas_Object* _o, void *event_info )
{
    Evas_Event_Mouse_Move* ev = (Evas_Event_Mouse_Move*)event_info;
    int _b = ev->buttons;
    int _x = ev->cur.output.x;
    int _y = ev->cur.output.y;

	printf("_mouse_move evas callback %d\n", (int) _o);

    if (dragging) {
		evas_object_move( _o, _x, _y);
		gevas_queue_redraw((GtkgEvas *) _data);
	}
}

/** If using the raw evas API, we need to cleanup Evas_Objects ourself **/
static gint cleanup_evas_raw_cb(GtkObject * o, gpointer data)
{
	Evas* evas;
	printf("finishing off raw evasobj for gevas:%d\n", (int) gevas);
	evas = gevas_get_evas(GTK_GEVAS(gevas));
	printf("finishing off raw evasobj:%d for evas:%d\n", (int) data,
		   (int) evas);
    evas_object_del( (Evas_Object*) data );

    exit(0);
//	return FALSE;
}


/***********************************************/
/***********************************************/
/***********************************************/

    enum 
        {
            TARGET_IMAGE_FILENAME=1,
            TARGET_URI_LIST,
            TARGET_TEXT_LIST,
            TARGET_TEXT_PLAIN
        };


static void __drag_data_get(GtkWidget          *widget,
                            GdkDragContext     *context,
                            GtkSelectionData   *selection_data,
                            guint               info,
                            guint               time,
                            gpointer data)
                            
{
#define BUFSIZE		2048
    gint ddn_flow=BUFSIZE-1;
    gboolean data_sent = FALSE;
    static gchar ddn_bufstat[BUFSIZE];	/* Data exchange buffer. */
    gchar* ddn_buf = ddn_bufstat;
    GtkgEvasEvHGroupSelector* ev = GTK_GEVASEVH_GROUP_SELECTOR(data);
    Eina_List* el = 0;

    printf("testgevas.c __drag_data_get() info:%d\n",info);
    if((data == NULL) || (context == NULL))
        return;

    switch( info )
    {
    case TARGET_IMAGE_FILENAME:

        sprintf(ddn_buf,"");
        el = gevasevh_group_selector_get_selection_objs(ev);
        while(el)
        {
            if( el->data )
            {

                if(GTK_IS_GEVASIMAGE(el->data))
                {
                    printf("image! %s\n",gevasimage_get_image_name(el->data));
                    snprintf( ddn_buf, ddn_flow, "%s\n",
                              gevasimage_get_image_name(el->data));
                    ddn_flow -= strlen(ddn_buf);
                    ddn_buf += strlen(ddn_buf);
                }
                else
                {
                    snprintf( ddn_buf, ddn_flow, "Name:%s\n",
                              gevasobj_get_name(GTK_GEVASOBJ(el->data)));
                    ddn_flow -= strlen(ddn_buf);
                    ddn_buf += strlen(ddn_buf);
                }
                printf("ddn_bufs:%s ddn_flow:%d\n", ddn_bufstat, ddn_flow);
            }
            el = el->next;
        }
        ddn_buf = ddn_bufstat;

        
/*        sprintf(ddn_buf, PACKAGE_DATA_DIR "/drawer_closed.png");*/
        break;
        
    case TARGET_URI_LIST:
    case TARGET_TEXT_LIST:
    case TARGET_TEXT_PLAIN:

        /* All I am doing here is filling ddn_buf with lines of data */
        sprintf(ddn_buf,"");
        el = gevasevh_group_selector_get_selection_objs(ev);
        while(el)
        {
            if( el->data )
            {
                printf("ddn_bufs:%s ddn_flow:%d\n", ddn_bufstat, ddn_flow);
                
                snprintf( ddn_buf, ddn_flow, "Name:%s\n",gevasobj_get_name(GTK_GEVASOBJ(el->data)));
                ddn_flow -= strlen(ddn_buf);
                ddn_buf += strlen(ddn_buf);
            }
            el = el->next;
        }
        ddn_buf = ddn_bufstat;
        

        /* quick hack */    
        /*        sprintf(ddn_buf,"www.freshmeat.net"); */
    }

    

    /* Send out the data using the `selection' system,
     * this is normally used for `cut and paste' but
     * gtk uses it for drag and drop as well. When
     * sending a string, gtk will ensure that a null
     * terminating byte is added to the end so we
     * do not need to add it.
     */
    gtk_selection_data_set(
        selection_data,
        GDK_SELECTION_TYPE_STRING,
        8,	/* 8 bits per character. */
        ddn_buf, strlen(ddn_buf)
        );
    data_sent = TRUE;


	/* If we did not send out any data (for whatever reason),
	 * then send out an error response since this function
	 * needs to gaurantee a response when reasonably possible.
	 */
	if(!data_sent)
	{
		const char *strptr = "Error";
        gtk_selection_data_set( selection_data,
			GDK_SELECTION_TYPE_STRING,
			8,	/* 8 bits per character. */ strptr, strlen(strptr));
		data_sent = TRUE;
	}

	return;
    
}



static void __drag_data_received(
	GtkWidget *widget,
	GdkDragContext *dc,
	gint x, gint y,
	GtkSelectionData *selection_data,
	guint info, guint t,
	gpointer data
    )
{
    int i=0;
    char* str;
	GtkgEvasImage *gi;
    
   	/* Important, check if we got data. */
	if(selection_data == NULL)
		return;
    if(selection_data->length < 0)
        return;

    /* Check that its a type of data we want */
    if( info < TARGET_IMAGE_FILENAME || info > TARGET_TEXT_PLAIN )
        return;

    /* We only like strings */
    if(selection_data->type != GDK_SELECTION_TYPE_STRING)
        return;

    
    switch( info )
    {
    case TARGET_IMAGE_FILENAME:
        
        str = selection_data->data;

/*        str = PACKAGE_DATA_DIR "/drawer_closed.png";*/

        for(i=0; str[i]; i++ )
        {
            if( str[i] == '\n' )
                str[i] = '\0';
        }
        
        
        printf("got a drop for an image file:%s x:%d y:%d\n",str,x,y);
        
        
        gi = gevasimage_new();
        gevasobj_set_gevas(gi, gevas);

        gevasimage_set_image_name(gi, str);
        gevasobj_move(GTK_GEVASOBJ(gi), x, y);
        gevasobj_set_layer( GTK_GEVASOBJ(gi), 11);
        gevasobj_show(GTK_GEVASOBJ(gi));
        gevas_queue_redraw( GTK_GEVAS(gevas) );
        
        break;
        
    case TARGET_URI_LIST:
    case TARGET_TEXT_LIST:
    case TARGET_TEXT_PLAIN:
        /* We only dump it to stdout for now */        
        printf("__drag_data_received() selection_data->data:%s\n",selection_data->data);
    }
    
}



void setup_dnd(GtkWidget * gevas)
{
    GtkTargetList* tlist=0;
    static GtkTargetEntry d_types[] = {
        { "text/image-filename-list", 0, TARGET_IMAGE_FILENAME },
        { "text/uri-list", 0, TARGET_URI_LIST },
        { "text/plain", 0, TARGET_TEXT_PLAIN },
        { "text", 0, TARGET_TEXT_LIST }
    };
    int num_d_types = 3;
    

    tlist = gtk_target_list_new (d_types, num_d_types);
    gevasevh_group_selector_set_drag_targets(
        GTK_GEVASEVH_GROUP_SELECTOR(evh_selector),
        tlist);

    /* wash our hands of the drag list from here on out */
    gtk_target_list_unref(tlist);

    gtk_signal_connect(GTK_OBJECT(gevas), "drag_data_get",
                       GTK_SIGNAL_FUNC(__drag_data_get), evh_selector);



    /* Setup as a drop target */
    gtk_drag_dest_set( GTK_WIDGET(gevas),
                       GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT |
                       GTK_DEST_DEFAULT_DROP,
                       d_types, num_d_types,
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);

    gtk_signal_connect(GTK_OBJECT(gevas), "drag_data_received",
                       GTK_SIGNAL_FUNC(__drag_data_received), evh_selector);


}


void setup_bg(GtkWidget * gevas)
{
	int w, h;
	GtkgEvasImage *gevas_image;
	GtkObject *evh;

	gevas_image = gevasimage_new();
	gevasobj_set_gevas(gevas_image, gevas);
	printf(" Trying to load bg.png from %s\n",PACKAGE_DATA_DIR "/bg.png");
	printf(" please note that if the image is not at the location, this will\n");
	printf(" quitely fail and look really bad!\n");

	gevasimage_set_image_name(gevas_image, PACKAGE_DATA_DIR "/bg.png");
    gevasimage_get_image_size(GTK_GEVASOBJ(gevas_image), &w, &h);
	gevasimage_set_image_fill(GTK_GEVASOBJ(gevas_image), 0,0,w,h);
	gevasobj_move(GTK_GEVASOBJ(gevas_image), 0, 0);
    fprintf(stderr,"3\n\n");
	gevasobj_set_layer(GTK_GEVASOBJ(gevas_image), -9999);
    fprintf(stderr,"3.5\n\n");
	gevasobj_resize(GTK_GEVASOBJ(gevas_image), 9999,9999);
    fprintf(stderr,"3.6\n\n");
	gevasobj_show(GTK_GEVASOBJ(gevas_image));
    fprintf(stderr,"4\n\n");


	/** Make this a group_selector **/
 	evh_selector = evh = gevasevh_group_selector_new();
    fprintf(stderr,"...  GTK_IS_GEVASEVH1  %d\n",GTK_IS_GEVASEVH(evh));
    gevasevh_group_selector_set_object( (GtkgEvasEvHGroupSelector*)evh, GTK_GEVASOBJ(gevas_image));
    fprintf(stderr,"...  GTK_IS_GEVASEVH2  %d\n",GTK_IS_GEVASEVH(evh));
    
    fprintf(stderr,"gevasobj_add_evhandler() image:%p evh:%p\n",gevas_image,evh);
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);
    setup_dnd(gevas);
    

	/** test the evas callback to gtk+ signal connections **/
    
	evh = gevasevh_to_gtk_signal_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	gtk_signal_connect(GTK_OBJECT(evh), "mouse_in",
					   GTK_SIGNAL_FUNC(gtk_bg_mouse_in_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_out",
					   GTK_SIGNAL_FUNC(gtk_bg_mouse_out_cb), NULL);
    
}


void setup_raptor(GtkWidget * gevas)
{
	int w=0, h=0;
	GtkgEvasImage *gevas_image;
	GtkObject *evh;

	gevas_image = gevasimage_new();
    gevasobj_set_gevas(gevas_image, gevas);

    
	gevasimage_set_image_name(gevas_image, PACKAGE_DATA_DIR "/raptor.png");
 	gevasimage_get_image_size(GTK_GEVASOBJ(gevas_image), &w, &h); 
    gevasimage_set_image_fill(GTK_GEVASOBJ(gevas_image), 0, 0, w, h ); 
    fprintf(stderr,"raptor is w:%ld h:%ld\n", w, h ); 
    
	raptor_w = w, raptor_h = h;
	gevasobj_move(GTK_GEVASOBJ(gevas_image), 20, 60);
	gevasobj_set_layer(GTK_GEVASOBJ(gevas_image), 0);
	gevasobj_show(GTK_GEVASOBJ(gevas_image));

	
	/** This event handler does nothing, it doesn't/shouldn't be added **/
	evh = gevasevh_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	/** This handler will let the user drag the raptor around **/
	evh = gevasevh_drag_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	/** This handler makes the raptor 10% larger when the mouse enters it **/
	evh = gevasevh_emouse_over_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	/** Test to see if the setters work **/
	gevasevh_emouse_over_scale_x(evh, 1.05);
	gevasevh_emouse_over_scale_y(evh, 1.05);

	/** test the evas callback to gtk+ signal connections **/
	evh = gevasevh_to_gtk_signal_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	gtk_signal_connect(GTK_OBJECT(evh), "mouse_down",
					   GTK_SIGNAL_FUNC(gtk_mouse_down_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_up",
					   GTK_SIGNAL_FUNC(gtk_mouse_up_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_move",
					   GTK_SIGNAL_FUNC(gtk_mouse_move_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_in",
					   GTK_SIGNAL_FUNC(gtk_mouse_in_cb), NULL);
	gtk_signal_connect(GTK_OBJECT(evh), "mouse_out",
					   GTK_SIGNAL_FUNC(gtk_mouse_out_cb), NULL);
    
	/** connect a static popup menu to this raptor **/
/*
	{
		GtkMenu* menu = 0;
		GtkMenuItem* mitem;
		gchar* label;
		
		if(!menu) {
			menu = (GtkMenu*)gtk_menu_new();
			label = "cut";
			gtk_menu_append (menu,mitem = gtk_menu_item_new_with_label (label));
			gtk_signal_connect(GTK_OBJECT(mitem),"activate",
				GTK_SIGNAL_FUNC(gpopmenu_handle_cb),label);
			
			label = "copy";
			gtk_menu_append (menu,mitem = gtk_menu_item_new_with_label (label));
			gtk_signal_connect(GTK_OBJECT(mitem),"activate",
				GTK_SIGNAL_FUNC(gpopmenu_handle_cb),label);
			
			label = "emboss";
			gtk_menu_append (menu,mitem = gtk_menu_item_new_with_label (label));
			gtk_signal_connect(GTK_OBJECT(mitem),"activate",
				GTK_SIGNAL_FUNC(gpopmenu_handle_cb),label);
		}

		evh = gevasevh_popup_new();
		gevasobj_add_evhandler( gevas_image, evh );
		gevasevh_popup_set_gtk_menu( evh, menu );
	}
*/

	/** Connect a dynamic popup menu to this raptor **/
	evh = gevasevh_popup_new();
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);

	gtk_signal_connect(GTK_OBJECT(evh), "popup_activate",
					   GTK_SIGNAL_FUNC(gtk_popup_activate_cb), NULL);

/* 	gevasevh_throb_new( GTK_GEVASOBJ(gevas_image) ); */
    
}

void setup_menu_raptor(GtkWidget * gevas)
{
	Evas* evas = gevas_get_evas(GTK_GEVAS(gevas));

    menu_raptor = evas_object_image_add( evas );
    evas_object_image_file_set( menu_raptor, PACKAGE_DATA_DIR "/raptor2.jpg", NULL);
    
    if (menu_raptor)
    {
        {
            int w=0, h=0;
            Evas_Object* o = menu_raptor;
            
            evas_object_image_size_get( o, &w, &h );
            evas_object_image_fill_set( o, 0, 0, w, h );
            evas_object_resize( o, w, h );
        }
        
        evas_object_move( menu_raptor, 700, 90);
		evas_object_layer_set( menu_raptor, -1);
		evas_object_show( menu_raptor);

        evas_object_event_callback_add( menu_raptor,
                                        EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, gevas);
		evas_object_event_callback_add( menu_raptor,
                                       EVAS_CALLBACK_MOUSE_UP, _mouse_up, gevas);
		evas_object_event_callback_add( menu_raptor,
                                       EVAS_CALLBACK_MOUSE_MOVE, _mouse_move, gevas);
    }

}


/** lets make this object also selectable **/
void make_selectable( GtkgEvasObj* object )
{
	GtkgEvasObj *ct;
	GtkObject *evh = gevasevh_selectable_new( evh_selector );
    gevasevh_selectable_set_confine( GTK_GEVASEVH_SELECTABLE(evh), 1 );

	gevasobj_add_evhandler(object, evh);
	gevasevh_selectable_set_normal_gevasobj(GTK_GEVASEVH_SELECTABLE(evh),
											object);

	ct = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(
		GTK_OBJECT(object)));

    evas_object_color_set(gevasobj_get_evasobj(GTK_OBJECT(ct)), 255, 200, 255, 200);
/*     gevasgrad_add_color(ct, 180, 150, 70, 25, 8); */
/* 	gevasgrad_add_color(ct, 180, 150, 70, 30, 8); */
/* 	gevasgrad_add_color(ct, 180, 150, 70, 40, 8); */
/* 	gevasgrad_add_color(ct, 180, 150, 70, 50, 8); */
/* 	gevasgrad_add_color(ct, 180, 150, 70, 60, 8); */
/* 	gevasgrad_add_color(ct, 200, 170, 90, 150, 8); */

    gevasgrad_add_color(ct, 120, 150, 170, 45, 8);
	gevasgrad_add_color(ct, 200, 170, 90, 150, 16);

/* 	gevasgrad_seal(ct); */
	gevasgrad_set_angle(ct, 150);
	gevasobj_resize( ct, 200,100);
	gevasobj_set_layer(ct, 9999);

	gevasevh_selectable_set_selected_gevasobj( evh, ct );

    
}

void make_text(GtkWidget * gevas)
{
	GtkgEvasObj *t1, *t2, *t3, *t4, *t5, *ct;
	GtkObject *evh;
	GtkObject *evh_drag;
	GtkObject *evh_dclick;

	/** This handler will let the user drag the text around **/
	/** we can happily use the same object for all 3 texts  **/
	evh_drag = evh = gevasevh_drag_new();


	ct = t1 = (GtkgEvasObj *) gevastext_new(GTK_GEVAS(gevas));
	gevastext_set_font(ct, FONT_NAME, 70);
	gevastext_set_string(ct, "Gtk+2");
	gevasobj_move(ct, 10, 10);
	gevasobj_set_layer(ct, 5);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 200,200,200, 155);
	gevasobj_add_evhandler(ct, evh);
	evh_dclick = gevasevh_clicks_new();
	gevasobj_add_evhandler(ct, evh_dclick);
	gtk_signal_connect(GTK_OBJECT(evh_dclick), "dclick",
					   GTK_SIGNAL_FUNC(gtk_dclick_cb), ct);

    gevasobj_set_name( ct, "Gtk+ object");
	make_selectable( ct );

	ct = t2 = (GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), FONT_NAME, 70, "Evas");
	gevasobj_move(ct, 500, 50);
	gevasobj_set_layer(ct, 6);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 140, 255, 140, 255);
/*	gevasobj_add_evhandler(ct, evh);*/
    gevasobj_set_name( ct, "Evas object");
    make_selectable( ct );

	ct = t3 = (GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), FONT_NAME, 90,"Linux");
    
	gevasobj_set_gevas(ct, gevas);
	gevasobj_move(ct, 150, 500);
	gevasobj_set_layer(ct, 7);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 255, 255, 255, 180);
	gevasobj_add_evhandler(ct, evh);
	evh = gevasevh_alpha_new();
	gevasobj_add_evhandler(ct, evh);
    gevasobj_set_name( ct, "Linux object");
	make_selectable( ct );

	/** lets do some funky mouse over action **/

	ct = t4 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), FONT_NAME, 50,
										   "The Cat!");
	gevasobj_set_gevas(ct, gevas);
	gevasobj_move(ct, 350, 400);
	gevasobj_set_layer(ct, 1);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 255, 255, 255, 255);
	gevasobj_add_evhandler(ct, evh_drag);

	ct = t5 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), FONT_NAME, 55,
										   "Purrs");
	gevasobj_set_gevas(ct, gevas);
	gevasobj_move(ct, 350, 400);
	gevasobj_set_layer(ct, 1);
/*	gevasobj_show( ct );	*/
	gevasobj_set_color(ct, 255, 211, 211, 255);
	gevasobj_add_evhandler(ct, evh_drag);

	evh = gevasevh_obj_changer_new();
	gevasevh_obj_changer_set_cold_gevasobj(evh, t4);
	gevasevh_obj_changer_set_hot_gevasobj(evh, t5);

}

void make_draw_icon(GtkWidget * gevas)
{
	GtkgEvasObj *ct;
	GtkObject *evh_changer;
	GtkObject *evh_drag;
	GtkObject *evh;

	/** The handlers will let the user drag the image around **/
	/** we can happily use the same object for all 3 texts  **/
	/** Also we change the image to react to user mouse state **/
	evh_drag = gevasevh_drag_new();
	evh_changer = gevasevh_obj_changer_new();

	ct = GTK_GEVASOBJ(gevasimage_new());
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/drawer_closed.png");
	gevasobj_move(ct, 440, 50);
	gevasobj_set_layer(ct, 0);
	gevasobj_show(ct);
	gevasobj_add_evhandler(ct, evh_drag);
	gevasevh_obj_changer_set_cold_gevasobj(evh_changer, ct);

	ct = GTK_GEVASOBJ(gevasimage_new());
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/drawer_open.png");
	gevasobj_move(ct, 440, 50);
	gevasobj_set_layer(ct, 0);
	gevasobj_add_evhandler(ct, evh_drag);
	gevasevh_obj_changer_set_hot_gevasobj(evh_changer, ct);

	ct = GTK_GEVASOBJ(gevasimage_new());
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/drawer_open_socks.png");
	gevasobj_move(ct, 440, 50);
	gevasobj_set_layer(ct, 0);
	gevasobj_add_evhandler(ct, evh_drag);
	gevasevh_obj_changer_set_hot_click1_gevasobj(evh_changer, ct);

	ct = GTK_GEVASOBJ(gevasimage_new());
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/delete.png");
	gevasobj_move(ct, 440, 50);
	gevasobj_set_layer(ct, 0);
	gevasobj_add_evhandler(ct, evh_drag);
	gevasevh_obj_changer_set_hot_click3_gevasobj(evh_changer, ct);



    
}

void make_grad(GtkWidget * gevas)
{
    
/*     { */
/*         Evas_Object* ob; */
/*         Evas* evas = gevas_get_evas(GTK_GEVAS(gevas)); */
        
/*         ob = evas_object_gradient_add(evas); */
/*         evas_object_move(ob, 0, 0); */
/*         evas_object_resize(ob, 90, 90); */
/*         evas_object_color_set(ob, 255, 255, 255, 255); */
/*         evas_object_gradient_color_add(ob, 255, 255, 255, 255, 10); */
/*         evas_object_gradient_color_add(ob, 250, 240, 50, 180, 10); */
/*         evas_object_gradient_color_add(ob, 220, 60, 0, 120, 10); */
/*         evas_object_gradient_color_add(ob, 200, 0, 0, 80, 10); */
/*         evas_object_gradient_color_add(ob, 0, 0, 0, 0, 10); */
/*         evas_object_gradient_angle_set(ob, 45.0); */
/*         evas_object_show(ob); */
/*     } */
    
    
	GtkgEvasObj *ct;
	GtkObject *evh_drag;

/* 	ct = GTK_GEVASOBJ(gevasgrad_new(GTK_GEVAS(gevas))); */
/*  	gevasobj_move(ct, 0, 0);  */
/*  	gevasobj_resize(ct, 100, 100); */
/*     evas_object_color_set(gevasobj_get_evasobj(GTK_OBJECT(ct)), 255, 255, 255, 255); */
/* 	gevasgrad_add_color(ct, 255, 255, 255, 255, 8); */
/* 	gevasgrad_add_color(ct, 255, 255, 0, 200, 8); */
/* 	gevasgrad_add_color(ct, 255, 0, 0, 150, 8); */
/* 	gevasgrad_add_color(ct, 0, 0, 0, 0, 8); */
/*     gevasgrad_set_angle(ct, 55); */
/*  	gevasobj_show(ct);  */

    
	evh_drag = gevasevh_drag_new();

	ct = GTK_GEVASOBJ(gevasgrad_new(GTK_GEVAS(gevas)));

    gevasobj_set_layer(ct, 3);
	gevasobj_move(ct, 440, 350);
	gevasobj_resize(ct, 100, 100);
	gevasobj_add_evhandler(ct, evh_drag);

    evas_object_color_set(gevasobj_get_evasobj(GTK_OBJECT(ct)), 255, 200, 255, 200);
	gevasgrad_add_color(ct, 255, 255, 255, 255, 8);
	gevasgrad_add_color(ct, 255, 255, 0, 200, 8);
	gevasgrad_add_color(ct, 255, 0, 0, 150, 8);
	gevasgrad_add_color(ct, 0, 0, 0, 0, 8);

	/** When we are done adding colors, we seal the gradient **/
	gevasgrad_set_angle(ct, 55);
	gevasobj_show(ct);
}


void make_twin(GtkWidget * gevas)
{
	GtkgEvasObj *ct, *t1, *t2;
	GtkObject *evh_drag = gevasevh_drag_new();
	GtkObject *twin;

	t1 = ct = GTK_GEVASOBJ(gevasimage_new());
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/cd.png");
	gevasobj_move(ct, 440, 350);
	gevasobj_set_layer(ct, 8);
	gevasobj_show(ct);
    make_selectable( ct );
    

	gevasobj_add_evhandler(ct, evh_drag);

	t2 = ct = (GtkgEvasObj *) gevastext_new(GTK_GEVAS(gevas));
	gevastext_set_font(ct, FONT_NAME, 30);
	gevastext_set_string(ct, "seedy");
	gevasobj_set_layer(ct, 8);
	gevasobj_show(ct);
	gevasobj_move(ct, 440, 550);
	gevasobj_set_color(ct, 255, 255, 255, 255);
	gevasobj_add_evhandler(ct, evh_drag);

	twin = GTK_OBJECT(gevastwin_new());
	gevastwin_set_main_obj(twin, t1);
	gevastwin_set_aux_obj(twin, t2);
	printf("mainobj:%ld auxobj:%ld\n", t1, t2);

    gevasevh_throb_new_for_twin( twin, t1 );
}


void make_e_throbber(GtkWidget * gevas)
{
	GtkgEvasObj *ct;
	GtkgEvasObj *go;

	ct = GTK_GEVASOBJ(gevasimage_new());
	go = GTK_GEVASOBJ( ct );
	gevasobj_set_gevas(ct, gevas);
	gevasimage_set_image_name(ct, PACKAGE_DATA_DIR "/e_logo.png");
	gevasobj_move(ct, 500, 500);
	gevasobj_set_layer(ct, 100);
	gevasobj_show(ct);
//    make_selectable( ct );

	gevasevh_throb_new( go );
}


int main(int argc, char *argv[])
{
    
	GtkWidget *window;

    fprintf(stderr,"main()\n");

    gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    fprintf(stderr,"test gevas starting\n\n");
    gevas_new_gtkscrolledwindow( (GtkgEvas**)(&gevas), &wtoy );

    fprintf(stderr,"main(2)\n");

    /* The above line is the same as these three, except that it can give more*/
    /* speed because gevas can optimize redraws knowing that it is in a scrolled window.*/
/*    wtoy = gtk_scrolled_window_new(NULL, NULL);*/
/*    gevas = gevas_new();*/
/*	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(wtoy), gevas);*/

    gevas_add_fontpath( GTK_GEVAS(gevas), PACKAGE_DATA_DIR );
    gevas_add_fontpath( GTK_GEVAS(gevas), g_get_current_dir() );
    gevas_add_fontpath( GTK_GEVAS(gevas), "/usr/X11R6/lib/X11/fonts/msttcorefonts" );

    
    gtk_container_add(GTK_CONTAINER(window), wtoy);
    fprintf(stderr,"main(3)\n");

    
    gtk_widget_set_usize(gevas, 3000, 3000);
    fprintf(stderr,"main(4)\n");
    
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    fprintf(stderr,"main(5)\n");
   
/*	gevas_set_checked_bg(gevas, 1);*/
/*	gevas_set_render_mode( gevas, RENDER_METHOD_3D_HARDWARE ); */
/*     gevas_set_render_mode(gevas, RENDER_METHOD_ALPHA_SOFTWARE); */
    printf("F\n\n");
	gevas_set_size_request_x(gevas, 200);
	gevas_set_size_request_y(gevas, 200);
    printf("G\n\n");
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wtoy), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);




    

	gtk_window_set_title(GTK_WINDOW(window), "gevas is Gtk Evas");
    printf("H\n\n");

	gtk_signal_connect(GTK_OBJECT(window),
					   "delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);
    printf("I\n\n");

	gtk_widget_show_all(window);
    printf("J\n\n");
    

    gevas_add_fontpath(GTK_GEVAS(gevas), PACKAGE_DATA_DIR);
	printf("added a font path to %s\n", PACKAGE_DATA_DIR);

    printf("1\n\n");
    
    
	setup_bg(gevas);

    fprintf(stderr,"setting up raptor\n");
	setup_raptor(gevas);

    fprintf(stderr,"setting up raptor with context menu attached\n");
 	setup_menu_raptor(gevas);
    
    fprintf(stderr,"making some text objects\n");
 	make_text(gevas); 

    fprintf(stderr,"setting up draw icon\n");
 	make_draw_icon(gevas); 
    
    fprintf(stderr,"setting up a gradient\n");
 	make_grad(gevas); 

    fprintf(stderr,"setting up a twin object\n");
 	make_twin(gevas); 

    fprintf(stderr,"setting up a throbbing e logo\n");
    make_e_throbber(gevas); 

    fprintf(stderr,"setting middle button scrolls\n");

	gevas_set_middleb_scrolls(GTK_GEVAS(gevas), 1,
							  gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(wtoy)),
							  gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(wtoy)));

    fprintf(stderr,"connecting close signal\n");
    /** Menu raptor uses the evas raw API, so we have to free it ourself **/
	gtk_signal_connect(GTK_OBJECT(gevas), "destroy",
					   GTK_SIGNAL_FUNC(cleanup_evas_raw_cb), menu_raptor);


    fprintf(stderr,"role the dice\n");
    gtk_main();



	return 0;
}



static gint delete_event_cb(GtkWidget * window, GdkEventAny * e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}


static gint gpopmenu_handle_cb(GtkWidget * window, gpointer data)
{
	printf("*** popup menu chioce: %s\n", (gchar *) data);
	return FALSE;
}
