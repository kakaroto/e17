/*
 * A test for gevas, you can also learn alot of the API from this.
 *
 *
 * Copyright (C) 2000 Ben Martin
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
#include <gevas-config.h>

#include "gevas.h"
#include "gevasimage.h"

#include "gevastext.h"
#include "gevasevh_alpha.h"
#include "gevasevh_drag.h"
#include "gevasevh_to_gtk_signals.h"
#include "gevasevh_emouse_over.h"
#include "gevasevh_popup.h"
#include "gevasevh_obj_changer.h"
#include "gevasevh_clicks.h"
#include "gevasevh_group_selector.h"
#include "gevasevh_selectable.h"
#include "gevastwin.h"

#include "gevasgrad.h"

#include <gtk/gtk.h>

static gint delete_event_cb(GtkWidget * w, GdkEventAny * e, gpointer data);
static gint gpopmenu_handle_cb(GtkWidget * window, gpointer data);

/* nasty kludges having these global... but hey this is C :-) */
static int raptor_w, raptor_h;
Evas_Object menu_raptor = 0;
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



//////////////////////////////


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



//////////////////////////////


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
void _mouse_in(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	double x = 0, y = 0, w = 0, h = 0;

	printf("mouse in evas callback...\n");

	evas_get_geometry(_e, _o, &x, &y, &w, &h);
	evas_resize(_e, _o, raptor_w + RAPTOR_GROW_SIZE,
				raptor_h + RAPTOR_GROW_SIZE);
	evas_move(_e, _o, x - RAPTOR_GROW_SIZE / 2, y - RAPTOR_GROW_SIZE / 2);
	evas_set_image_fill(_e, _o, 0, 0, raptor_w + RAPTOR_GROW_SIZE,
						raptor_h + RAPTOR_GROW_SIZE);
	gevas_queue_redraw((GtkgEvas *) _data);
}
void _mouse_out(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	double x = 0, y = 0, w = 0, h = 0;
	printf("mouse out evas callback...\n");
	evas_get_geometry(_e, _o, &x, &y, &w, &h);
	evas_resize(_e, _o, raptor_w, raptor_h);
	evas_move(_e, _o, x + RAPTOR_GROW_SIZE / 2, y + RAPTOR_GROW_SIZE / 2);
	evas_set_image_fill(_e, _o, 0, 0, raptor_w, raptor_h);
	gevas_queue_redraw((GtkgEvas *) _data);
}

static int dragging = 0;
void _mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
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
			evas_event_button_up(_e, _x, _y, _b);

			gtk_menu_popup(menu, NULL, NULL, NULL, NULL, _b, gev->button.time);
		}
	}
}
void _mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	printf("_mouse_up evas callback %d\n", (int) _o);


	if (_b == 1) {
		dragging = 0;
	}

}

void _mouse_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	printf("_mouse_move evas callback %d\n", (int) _o);


	if (dragging) {
		evas_move(_e, _o, _x, _y);
		gevas_queue_redraw((GtkgEvas *) _data);
	}
}

/** If using the raw evas API, we need to cleanup Evas_Objects ourself **/
static gint cleanup_evas_raw_cb(GtkObject * o, gpointer data)
{
	Evas evas;
	printf("finishing off raw evasobj for gevas:%d\n", (int) gevas);
	evas = gevas_get_evas(GTK_GEVAS(gevas));
	printf("finishing off raw evasobj:%d for evas:%d\n", (int) data,
		   (int) evas);
	evas_del_object(evas, (Evas_Object) data);

	return FALSE;
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
	gevasobj_set_layer(GTK_GEVASOBJ(gevas_image), -9999);
	gevasobj_resize(GTK_GEVASOBJ(gevas_image), 9999,9999);
	gevasobj_show(GTK_GEVASOBJ(gevas_image));


	/** Make this a group_selector **/
	evh_selector = evh = gevasevh_group_selector_new();
	gevasevh_group_selector_set_object( (GtkgEvasEvHGroupSelector*)evh, 
		GTK_GEVASOBJ(gevas_image));
	gevasobj_add_evhandler(GTK_GEVASOBJ(gevas_image), evh);


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
	int w, h;
	GtkgEvasImage *gevas_image;
	GtkObject *evh;

	gevas_image = gevasimage_new();
	{
		gint ref_count;
		ref_count = ((GtkObject *) gevas_image)->ref_count;
		printf("test_1... ref_count:%d\n", ref_count);
	}

	gevasobj_set_gevas(gevas_image, gevas);
	{
		gint ref_count;
		ref_count = ((GtkObject *) gevas_image)->ref_count;
		printf("test_2... ref_count:%d\n", ref_count);
	}

	gevasimage_set_image_name(gevas_image, PACKAGE_DATA_DIR "/raptor.png");
	gevasimage_get_image_size(GTK_GEVASOBJ(gevas_image), &w, &h);
	raptor_w = w, raptor_h = h;
	gevasobj_move(GTK_GEVASOBJ(gevas_image), 20, 20);
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

}

void setup_menu_raptor(GtkWidget * gevas)
{
	Evas evas = gevas_get_evas(GTK_GEVAS(gevas));

	if (
		(menu_raptor =
		 evas_add_image_from_file(evas, PACKAGE_DATA_DIR "/raptor2.jpg"))) {
		evas_move(evas, menu_raptor, 700, 90);
		evas_set_layer(evas, menu_raptor, -1);
		evas_show(evas, menu_raptor);

		evas_callback_add(evas, menu_raptor,
						  CALLBACK_MOUSE_DOWN, _mouse_down, gevas);
		evas_callback_add(evas, menu_raptor,
						  CALLBACK_MOUSE_UP, _mouse_up, gevas);
		evas_callback_add(evas, menu_raptor,
						  CALLBACK_MOUSE_MOVE, _mouse_move, gevas);


	}

}


/** lets make this object also selectable **/
void make_selectable( GtkgEvasObj* object )
{
	GtkgEvasObj *ct;
	GtkObject *evh = gevasevh_selectable_new();

	gevasobj_add_evhandler(object, evh);
	gevasevh_selectable_set_normal_gevasobj(GTK_GEVASEVH_SELECTABLE(evh),
											object);

	ct = (GtkgEvasObj*)gevasgrad_new(gevasobj_get_gevas(
		GTK_OBJECT(object)));

	gevasgrad_add_color(ct, 180, 150, 70, 25, 8);
	gevasgrad_add_color(ct, 180, 150, 70, 30, 8);
	gevasgrad_add_color(ct, 180, 150, 70, 40, 8);
	gevasgrad_add_color(ct, 180, 150, 70, 50, 8);
	gevasgrad_add_color(ct, 180, 150, 70, 60, 8);
	gevasgrad_add_color(ct, 200, 170, 90, 150, 8);
/*	gevasgrad_add_color(ct, 200, 170, 90, 200, 8);*/
/*	gevasgrad_add_color(ct, 200, 170, 90, 255, 8);*/
	gevasgrad_seal(ct);
	gevasgrad_set_angle(ct, 315);
	gevasobj_resize( ct, 200,100);
	gevasobj_set_layer(ct, 9999);

	gevasevh_selectable_set_selected_gevasobj( evh, ct );
	gevasevh_selectable_set_selector(GTK_GEVASEVH_SELECTABLE(evh),
									 evh_selector);
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
	gevastext_set_font(ct, "andover", 70);
	gevastext_set_string(ct, "Gtk+");
	gevasobj_move(ct, 10, 10);
	gevasobj_set_layer(ct, 5);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 255, 150, 150, 255);
	gevasobj_add_evhandler(ct, evh);
	evh_dclick = gevasevh_clicks_new();
	gevasobj_add_evhandler(ct, evh_dclick);
	gtk_signal_connect(GTK_OBJECT(evh_dclick), "dclick",
					   GTK_SIGNAL_FUNC(gtk_dclick_cb), ct);

	make_selectable( ct );

	ct = t2 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), "andover", 70,
										   "Evas");
	gevasobj_move(ct, 500, 50);
	gevasobj_set_layer(ct, 6);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 140, 255, 140, 255);
/*	gevasobj_add_evhandler(ct, evh);*/
	make_selectable( ct );

	ct = t3 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), "andover", 90,
										   "Linux");
	gevasobj_set_gevas(ct, gevas);
	gevasobj_move(ct, 150, 500);
	gevasobj_set_layer(ct, 7);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 255, 255, 255, 180);
	gevasobj_add_evhandler(ct, evh);
	evh = gevasevh_alpha_new();
	gevasobj_add_evhandler(ct, evh);
	make_selectable( ct );

	/** lets do some funky mouse over action **/

	ct = t4 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), "andover", 50,
										   "The Cat!");
	gevasobj_set_gevas(ct, gevas);
	gevasobj_move(ct, 350, 400);
	gevasobj_set_layer(ct, 1);
	gevasobj_show(ct);
	gevasobj_set_color(ct, 255, 255, 255, 255);
	gevasobj_add_evhandler(ct, evh_drag);

	ct = t5 =
		(GtkgEvasObj *) gevastext_new_full(GTK_GEVAS(gevas), "andover", 55,
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
	GtkgEvasObj *ct;
	GtkObject *evh_drag;

	evh_drag = gevasevh_drag_new();

	ct = GTK_GEVASOBJ(gevasgrad_new(GTK_GEVAS(gevas)));
	gevasgrad_add_color(ct, 255, 255, 255, 255, 8);
	gevasgrad_add_color(ct, 255, 255, 0, 200, 8);
	gevasgrad_add_color(ct, 255, 0, 0, 150, 8);
	gevasgrad_add_color(ct, 0, 0, 0, 0, 8);

	/** When we are done adding colors, we seal the gradient **/
	gevasgrad_seal(ct);
	gevasobj_move(ct, 440, 350);
	gevasobj_set_layer(ct, 3);
	gevasobj_add_evhandler(ct, evh_drag);
	gevasobj_resize(ct, 100, 100);
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

	gevasobj_add_evhandler(ct, evh_drag);

	t2 = ct = (GtkgEvasObj *) gevastext_new(GTK_GEVAS(gevas));
	gevastext_set_font(ct, "andover", 30);
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

}

int main(int argc, char *argv[])
{
	GtkWidget *window;


	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gevas_new_gtkscrolledwindow(&gevas, &wtoy );

    /* The above line is the same as these three, except that it can give more*/
    /* speed because gevas can optimize redraws knowing that it is in a scrolled window.*/
/*    wtoy = gtk_scrolled_window_new(NULL, NULL);*/
/*    gevas = gevas_new();*/
/*	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(wtoy), gevas);*/


    gtk_container_add(GTK_CONTAINER(window), wtoy);

    
    gtk_widget_set_usize(gevas, 3000, 3000);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
/*	gevas_set_checked_bg(gevas, 1);*/
/*	gevas_set_render_mode( gevas, RENDER_METHOD_3D_HARDWARE ); */
    gevas_set_render_mode(gevas, RENDER_METHOD_ALPHA_SOFTWARE);
	gevas_set_size_request_x(gevas, 200);
	gevas_set_size_request_y(gevas, 200);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wtoy), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);




    

	gtk_window_set_title(GTK_WINDOW(window), "gevas is Gtk Evas");

	gtk_signal_connect(GTK_OBJECT(window),
					   "delete_event", GTK_SIGNAL_FUNC(delete_event_cb), NULL);

	gtk_widget_show_all(window);

	gevas_add_fontpath(GTK_GEVAS(gevas), PACKAGE_DATA_DIR);
	printf("added a font path to %s\n", PACKAGE_DATA_DIR);

	setup_bg(gevas);
	setup_raptor(gevas);
	setup_menu_raptor(gevas);
	make_text(gevas);
	make_draw_icon(gevas);
	make_grad(gevas);
	make_twin(gevas);

	gevas_set_middleb_scrolls(GTK_GEVAS(gevas), 1,
							  gtk_scrolled_window_get_hadjustment
							  (GTK_SCROLLED_WINDOW(wtoy)),
							  gtk_scrolled_window_get_vadjustment
							  (GTK_SCROLLED_WINDOW(wtoy))
		);


	/** Menu raptor uses the evas raw API, so we have to free it ourself **/
	gtk_signal_connect(GTK_OBJECT(gevas),
					   "destroy",
					   GTK_SIGNAL_FUNC(cleanup_evas_raw_cb), menu_raptor);


/*
{
Evas evas = gevas_get_evas( gevas );
evas_callback_add(evas, 0,CALLBACK_MOUSE_IN, _mouse_in, gevas);
evas_callback_add(evas, 0,CALLBACK_MOUSE_OUT, _mouse_out, gevas);
evas_callback_add(evas, 0,CALLBACK_MOUSE_DOWN, _mouse_down, gevas);
evas_callback_add(evas, 0,CALLBACK_MOUSE_UP, _mouse_up, gevas);
evas_callback_add(evas, 0,CALLBACK_MOUSE_MOVE, _mouse_move, gevas);
}
*/
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
