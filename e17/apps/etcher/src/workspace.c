#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <Evas.h>
#include <Edb.h>
#include <gdk/gdkx.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bits.h"
#include "callbacks.h"
#include "etching.h"
#include "globals.h"
#include "interface.h"
#include "lights.h"
#include "macros.h"
#include "preferences.h"
#include "support.h"
#include "splash.h"
#include "workspace.h"

typedef struct workspace
{
   Evas_List           etchings;
   Etching            *e;

   Evas                view_evas;
   guint               current_idle;

   int                 new_evas;

   struct
   {
      Evas_Object         grid, bg_rect;

      struct
      {
	 Evas_Object         tl, tr, bl, br;
      }
      handles;
      struct
      {
	 Evas_Object         l, r, t, b;
      }
      border;
      struct
      {
	 Evas_Object         l, r, t, b;
      }
      selection;
      struct
      {
	 Evas_Object         abs, rel, adj;
      }
      selection_tl;
      struct
      {
	 Evas_Object         abs, rel, adj;
      }
      selection_br;

      Evas_Object         pointer, backing, sel;
   }
   obj;

   double              backing_x, backing_y, backing_w, backing_h;
   gint                draft_mode;
   gint                zoom_x, zoom_y;
   gint                zoom_scale;
   LampColor           color;
}
Workspace;

Workspace           ws;

static void         handle_bg_mouse_down(void *_data, Evas _e, Evas_Object _o,
					 int _b, int _x, int _y);
static void         handle_mouse_down(void *_data, Evas _e, Evas_Object _o,
				      int _b, int _x, int _y);
static void         handle_mouse_up(void *_data, Evas _e, Evas_Object _o,
				    int _b, int _x, int _y);
static void         handle_mouse_move(void *_data, Evas _e, Evas_Object _o,
				      int _b, int _x, int _y);
static void         handle_bit_mouse_down(void *_data, Evas _e, Evas_Object _o,
					  int _b, int _x, int _y);
static void         handle_bit_mouse_up(void *_data, Evas _e, Evas_Object _o,
					int _b, int _x, int _y);
static void         handle_bit_mouse_move(void *_data, Evas _e, Evas_Object _o,
					  int _b, int _x, int _y);
static void         handle_adjuster_mouse_down(void *_data, Evas _e,
					       Evas_Object _o, int _b, int _x,
					       int _y);
static void         handle_adjuster_mouse_up(void *_data, Evas _e,
					     Evas_Object _o, int _b, int _x,
					     int _y);
static void         handle_adjuster_mouse_move(void *_data, Evas _e,
					       Evas_Object _o, int _b, int _x,
					       int _y);

static gint         workspace_create_handles(gpointer data);
static gint         workspace_configure_handles(gpointer data);

static double
get_spin(char *name)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), name);
   return gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
}

static void
get_entry(char *name, char **val)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), name);

   if (*val)
      free(*val);

   if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), ""))
      *val = NULL;
   else
      *val = strdup(gtk_entry_get_text(GTK_ENTRY(w)));
}

static void
set_spin(char *name, double val)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), name);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), val);
}

static void
set_entry(char *name, char *val)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), name);
   if (val)
      gtk_entry_set_text(GTK_ENTRY(w), val);
   else
      gtk_entry_set_text(GTK_ENTRY(w), "");
}

void
workspace_init(void)
{
   bzero(&ws, sizeof(Workspace));

   ws.zoom_scale = 4;
   ws.draft_mode = 1;
   ws.new_evas = 1;
   ws.view_evas = evas_new();
   ws.color = Red;
   if (pref_get_render_method() == Hardware)
      evas_set_output_method(ws.view_evas, RENDER_METHOD_3D_HARDWARE);
   else
      evas_set_output_method(ws.view_evas, RENDER_METHOD_ALPHA_SOFTWARE);
}

void
workspace_set_light(LampColor color)
{
   GdkPixmap          *gdk_pixmap;
   GtkWidget          *w;
   GdkBitmap          *mask;
   GtkStyle           *style;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "lamp");
   style = gtk_widget_get_style(main_win);
   gtk_widget_realize(w);

   if (ws.color != color)
     {
	if (color == Green)
	   gdk_pixmap =
	      gdk_pixmap_create_from_xpm_d(w->window, &mask,
					   &style->
					   bg
					   [GTK_STATE_NORMAL], light_green_xpm);
	else
	   gdk_pixmap =
	      gdk_pixmap_create_from_xpm_d(w->window, &mask,
					   &style->
					   bg[GTK_STATE_NORMAL], light_red_xpm);
	ws.color = color;

	gtk_pixmap_set(GTK_PIXMAP(w), gdk_pixmap, mask);
     }
}

void
workspace_set_current_etching(Etching * e)
{
   GtkWidget          *w;
   Evas_List           l;
   Ebits_Object        bits;

   if (e)
     {
	if (ws.e)
	  {
	     /* FIXME: hide/remove current stuff here properly. */
	     ebits_hide(etching_get_bits(ws.e));
	  }

	ws.e = e;

	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_clear(GTK_CLIST(w));
	workspace_update_visible_selection();

	bits = etching_get_bits(ws.e);
	ebits_add_to_evas(bits, ws.view_evas);
	ebits_set_layer(bits, 5);
	ebits_move(bits, ws.backing_x, ws.backing_y);
	ebits_resize(bits, ws.backing_w, ws.backing_h);
	ebits_show(bits);

	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;

	     state = l->data;
	     evas_callback_add(ws.view_evas, state->object,
			       CALLBACK_MOUSE_DOWN,
			       handle_bit_mouse_down, state);
	     evas_callback_add(ws.view_evas, state->object,
			       CALLBACK_MOUSE_UP, handle_bit_mouse_up, state);
	     evas_callback_add(ws.view_evas, state->object,
			       CALLBACK_MOUSE_MOVE,
			       handle_bit_mouse_move, state);
	  }

	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));

	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state2;
	     gchar              *text;
	     gint                row;

	     state2 = l->data;
	     text = state2->description->name;
	     if (!text)
		text = "";
	     row = gtk_clist_append(GTK_CLIST(w), &text);
	  }

	gtk_clist_thaw(GTK_CLIST(w));

	workspace_update_widget_from_selection();

	w = gtk_object_get_data(GTK_OBJECT(main_win), "file");
	gtk_entry_set_text(GTK_ENTRY(w), etching_get_filename(e));

	if (etching_is_dirty(e))
	   workspace_set_light(Red);
	else
	   workspace_set_light(Green);

	E_DB_STR_SET(pref_get_config(), "/paths/bit", etching_get_filename(e));
	e_db_flush();
     }
}

Etching            *
workspace_get_current_etching(void)
{
   return ws.e;
}

void
workspace_add_etching(Etching * e)
{
   if (e && etching_get_bits(e))
      evas_list_append(ws.etchings, e);
}

void
workspace_remove_etching(Etching * e)
{
   if (e)
     {
	if (e == ws.e)
	  {
	     ws.e = NULL;
	     /* FIXME: hide etc. here */
	  }

	evas_list_remove(ws.etchings, e);
     }
}

Evas
workspace_get_evas(void)
{
   return ws.view_evas;
}

void
workspace_add_item(char *filename)
{
   Ebits_Object_Bit_State state;
   Evas_List           l;
   char                buf[512];
   int                 num, exists;

   if (!ws.e || !filename)
      return;

   state = ebits_new_bit(etching_get_bits(ws.e), filename);
   evas_callback_add(ws.view_evas, state->object, CALLBACK_MOUSE_DOWN,
		     handle_bit_mouse_down, state);
   evas_callback_add(ws.view_evas, state->object, CALLBACK_MOUSE_UP,
		     handle_bit_mouse_up, state);
   evas_callback_add(ws.view_evas, state->object, CALLBACK_MOUSE_MOVE,
		     handle_bit_mouse_move, state);
   workspace_queue_draw();

   exists = 1;
   num = 1;
   while (exists)
     {
	exists = 0;
	g_snprintf(buf, sizeof(buf), "Image_%i", num);
	for (l = etching_get_bits(ws.e)->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state2;

	     state2 = l->data;
	     if (state2->description->name)
	       {
		  if (!strcmp(state2->description->name, buf))
		    {
		       exists = 1;
		       num++;
		    }
	       }
	  }
     }

   state->description->name = strdup(buf);

   workspace_update_relative_combos();
   workspace_update_image_list();
   workspace_update_sync_list();
   etching_set_dirty(ws.e);

   E_DB_STR_SET(pref_get_config(), "/paths/image", filename);
   e_db_flush();
}

void
workspace_show_splash(void)
{
   show_splash(ws.view_evas, &ws.current_idle, &workspace_redraw);
}

void
workspace_queue_draw(void)
{
   QUEUE_DRAW(ws.current_idle, workspace_redraw);
}

void
workspace_update_states(void)
{
   Ebits_Object        o;
   Evas_List           l;
   GtkWidget          *states, *states2;
   gchar              *text;

   states = gtk_object_get_data(GTK_OBJECT(main_win), "states");
   states2 = gtk_object_get_data(GTK_OBJECT(main_win), "states2");

   gtk_clist_unselect_all(GTK_CLIST(states));
   gtk_clist_clear(GTK_CLIST(states));

   text = "Normal";
   gtk_clist_append(GTK_CLIST(states), &text);
   text = "Hilited";
   gtk_clist_append(GTK_CLIST(states), &text);
   text = "Clicked";
   gtk_clist_append(GTK_CLIST(states), &text);
   text = "Disabled";
   gtk_clist_append(GTK_CLIST(states), &text);

   gtk_clist_unselect_all(GTK_CLIST(states2));
   gtk_clist_clear(GTK_CLIST(states2));

   text = "Normal";
   gtk_clist_append(GTK_CLIST(states2), &text);
   text = "Hilited";
   gtk_clist_append(GTK_CLIST(states2), &text);
   text = "Clicked";
   gtk_clist_append(GTK_CLIST(states2), &text);
   text = "Disabled";
   gtk_clist_append(GTK_CLIST(states2), &text);

   o = etching_get_bits(ws.e);

   for (l = ebits_get_state_names(o); l; l = l->next)
     {
	text = l->data;

	gtk_clist_append(GTK_CLIST(states), &text);
	gtk_clist_append(GTK_CLIST(states2), &text);
     }
}

void
workspace_update_selection_from_widget(void)
{
   GtkWidget          *w;

   Ebits_Object        bits;
   Ebits_Object_Bit_State selected;

   bits = etching_get_bits(ws.e);
   selected = etching_get_selected_item(ws.e);

   if (selected)
     {
	Evas_List           l, ll;
	gchar              *prev_i1 = NULL, *prev_i2 = NULL, *prev_i3 =
	   NULL, *prev_i4 = NULL, key[4096];

	selected->description->rel1.x = get_spin("tl_abs_h");
	selected->description->rel1.y = get_spin("tl_abs_v");

	get_entry("tl_rel", &(selected->description->rel1.name));
	get_entry("br_rel", &(selected->description->rel2.name));

	selected->description->rel1.rx = get_spin("tl_rel_h");
	selected->description->rel1.ry = get_spin("tl_rel_v");
	selected->description->rel1.ax = get_spin("tl_adj_h");
	selected->description->rel1.ay = get_spin("tl_adj_v");
	selected->description->rel2.x = get_spin("br_abs_h");
	selected->description->rel2.y = get_spin("br_abs_v");
	selected->description->rel2.rx = get_spin("br_rel_h");
	selected->description->rel2.ry = get_spin("br_rel_v");
	selected->description->rel2.ax = get_spin("br_adj_h");
	selected->description->rel2.ay = get_spin("br_adj_v");
	selected->description->align.w = get_spin("content_alignment_h");
	selected->description->align.h = get_spin("content_alignment_v");

	w = gtk_object_get_data(GTK_OBJECT(main_win), "aspect");

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	  {
	     selected->description->aspect.x = get_spin("aspect_h");
	     selected->description->aspect.y = get_spin("aspect_v");
	  }
	else
	  {
	     selected->description->aspect.x = 0;
	     selected->description->aspect.y = 0;
	     set_spin("aspect_h", selected->description->aspect.x);
	     set_spin("aspect_v", selected->description->aspect.y);
	  }

	selected->description->min.w = get_spin("min_h");
	selected->description->min.h = get_spin("min_v");
	selected->description->max.w = get_spin("max_h");
	selected->description->max.h = get_spin("max_v");
	selected->description->step.x = get_spin("step_h");
	selected->description->step.y = get_spin("step_v");

	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_h");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	   selected->description->max.w = 0;
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_v");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	   selected->description->max.h = 0;

	if (selected->description->sync)
	  {
	     Evas_List           ll;

	     for (ll = selected->description->sync; ll; ll = ll->next)
		free(ll->data);
	     evas_list_free(selected->description->sync);
	     selected->description->sync = NULL;
	  }
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");
	{
	   GList              *l;

	   for (l = GTK_CLIST(w)->row_list; l; l = l->next)
	     {
		GtkCListRow        *row;
		GtkCellText        *cell;

		row = l->data;
		cell = (GtkCellText *) row->cell;
		if (row->state == GTK_STATE_SELECTED)
		   selected->description->sync =
		      evas_list_append(selected->
				       description->sync, strdup(cell->text));
	     }
	}

	if (selected->description->normal.image)
	   prev_i1 = selected->description->normal.image;
	if (selected->description->hilited.image)
	   prev_i2 = selected->description->hilited.image;
	if (selected->description->clicked.image)
	   prev_i3 = selected->description->clicked.image;
	if (selected->description->disabled.image)
	   prev_i4 = selected->description->disabled.image;

	get_entry("name", &(selected->description->name));
	get_entry("class", &(selected->description->class));
	get_entry("color_class", &(selected->description->color_class));

	selected->description->border.l = get_spin("border_l");
	selected->description->border.r = get_spin("border_r");
	selected->description->border.t = get_spin("border_t");
	selected->description->border.b = get_spin("border_b");

	if (gtk_object_get_data
	    (GTK_OBJECT(main_win), "properties_state_normal_image"))
	   selected->description->normal.image =
	      g_strdup(gtk_object_get_data
		       (GTK_OBJECT(main_win), "properties_state_normal_image"));
	if (gtk_object_get_data
	    (GTK_OBJECT(main_win), "properties_state_hilited_image"))
	   selected->description->hilited.image =
	      g_strdup(gtk_object_get_data
		       (GTK_OBJECT(main_win),
			"properties_state_hilited_image"));
	if (gtk_object_get_data
	    (GTK_OBJECT(main_win), "properties_state_clicked_image"))
	   selected->description->clicked.image =
	      g_strdup(gtk_object_get_data
		       (GTK_OBJECT(main_win),
			"properties_state_clicked_image"));
	if (gtk_object_get_data
	    (GTK_OBJECT(main_win), "properties_state_disabled_image"))
	   selected->description->disabled.image =
	      g_strdup(gtk_object_get_data
		       (GTK_OBJECT(main_win),
			"properties_state_disabled_image"));

	if (selected->description->normal.image)
	  {
	     if ((!prev_i1 ||
		  strcmp(selected->description->normal.image, prev_i1)) &&
		 selected->normal.image)
	       {
		  imlib_context_set_image(selected->normal.image);
		  imlib_free_image();
		  selected->normal.image = NULL;
		  selected->normal.saved = 0;
	       }
	  }

	if (selected->description->hilited.image)
	  {
	     if ((!prev_i2 ||
		  strcmp(selected->description->hilited.image, prev_i2)) &&
		 selected->hilited.image)
	       {
		  imlib_context_set_image(selected->hilited.image);
		  imlib_free_image();
		  selected->hilited.image = NULL;
		  selected->hilited.saved = 0;
	       }
	  }

	if (selected->description->clicked.image)
	  {
	     if ((!prev_i3 ||
		  strcmp(selected->description->clicked.image, prev_i3)) &&
		 selected->clicked.image)
	       {
		  imlib_context_set_image(selected->clicked.image);
		  imlib_free_image();
		  selected->clicked.image = NULL;
		  selected->clicked.saved = 0;
	       }
	  }

	if (selected->description->disabled.image)
	  {
	     if ((!prev_i4 ||
		  strcmp(selected->description->disabled.image, prev_i4)) &&
		 selected->disabled.image)
	       {
		  imlib_context_set_image(selected->disabled.image);
		  imlib_free_image();
		  selected->disabled.image = NULL;
		  selected->disabled.saved = 0;
	       }
	  }

	for (l = ebits_get_state_names(bits); l; l = l->next)
	  {
	     gchar              *image;

	     snprintf(key, sizeof(key),
		      "properties_state_%s_image", (gchar *) l->data);

	     image = gtk_object_get_data(GTK_OBJECT(main_win), key);

	     if (!image)
		continue;

	     for (ll = selected->state_source_description; ll; ll = ll->next)
	       {
		  Ebits_State_Source_Description ss_d;

		  ss_d = ll->data;

		  if ((!ss_d->state_d->image ||
		       strcmp(ss_d->state_d->image, image)) && ss_d->image)
		    {
		       imlib_context_set_image(ss_d->image);
		       imlib_free_image();
		       ss_d->image = NULL;
		       ss_d->saved = 0;
		    }
	       }
	     ebits_add_bit_state(selected, l->data, strdup(image));
	  }

	_ebits_evaluate(selected);

	if (prev_i1)
	   g_free(prev_i1);
	if (prev_i2)
	   g_free(prev_i2);
	if (prev_i3)
	   g_free(prev_i3);
	if (prev_i4)
	   g_free(prev_i4);

	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_h");
	if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Fill"))
	   selected->description->tile.w = 0;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile"))
	   selected->description->tile.w = 1;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile Integer"))
	   selected->description->tile.w = 2;
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_v");
	if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Fill"))
	   selected->description->tile.h = 0;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile"))
	   selected->description->tile.h = 1;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile Integer"))
	   selected->description->tile.h = 2;

	workspace_update_image_list();
     }

   if (bits)
     {
	bits->description->min.w = get_spin("bit_min_h");
	bits->description->min.h = get_spin("bit_min_v");
	bits->description->max.w = get_spin("bit_max_h");
	bits->description->max.h = get_spin("bit_max_v");
	bits->description->preferred.w = get_spin("bit_pref_h");
	bits->description->preferred.h = get_spin("bit_pref_v");
	bits->description->padding.l = get_spin("bit_pad_l");
	bits->description->padding.r = get_spin("bit_pad_r");
	bits->description->padding.t = get_spin("bit_pad_t");
	bits->description->padding.b = get_spin("bit_pad_b");
	bits->description->inset.l = get_spin("bit_inset_l");
	bits->description->inset.r = get_spin("bit_inset_r");
	bits->description->inset.t = get_spin("bit_inset_t");
	bits->description->inset.b = get_spin("bit_inset_b");
	bits->description->step.x = get_spin("bit_step_h");
	bits->description->step.y = get_spin("bit_step_v");
     }
}

void
workspace_update_widget_from_selection(void)
{
   GtkWidget          *w;
   Ebits_Object_Bit_State selected;
   Ebits_Object        bits;

   selected = etching_get_selected_item(ws.e);
   bits = etching_get_bits(ws.e);

   if (selected)
     {

	/* update_relative_combos(); */

	w = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
	gtk_widget_set_sensitive(w, 1);
	set_spin("tl_abs_h", selected->description->rel1.x);
	set_spin("tl_abs_v", selected->description->rel1.y);
	set_entry("tl_rel", selected->description->rel1.name);
	set_spin("tl_rel_h", selected->description->rel1.rx);
	set_spin("tl_rel_v", selected->description->rel1.ry);
	set_spin("tl_adj_h", selected->description->rel1.ax);
	set_spin("tl_adj_v", selected->description->rel1.ay);

	set_spin("br_abs_h", selected->description->rel2.x);
	set_spin("br_abs_v", selected->description->rel2.y);
	set_entry("br_rel", selected->description->rel2.name);
	set_spin("br_rel_h", selected->description->rel2.rx);
	set_spin("br_rel_v", selected->description->rel2.ry);
	set_spin("br_adj_h", selected->description->rel2.ax);
	set_spin("br_adj_v", selected->description->rel2.ay);

	set_spin("content_alignment_h", selected->description->align.w);
	set_spin("content_alignment_v", selected->description->align.h);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "aspect");
	if ((etching_get_selected_item(ws.e)->description->aspect.x > 0)
	    && (etching_get_selected_item(ws.e)->description->aspect.y > 0))
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	set_spin("aspect_h", selected->description->aspect.x);
	set_spin("aspect_v", selected->description->aspect.y);
	set_spin("min_h", selected->description->min.w);
	set_spin("min_v", selected->description->min.h);
	set_spin("max_h", selected->description->max.w);
	set_spin("max_v", selected->description->max.h);
	set_spin("step_h", selected->description->step.x);
	set_spin("step_v", selected->description->step.y);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_h");
	if (etching_get_selected_item(ws.e)->description->max.w == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_v");
	if (etching_get_selected_item(ws.e)->description->max.h == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);

	workspace_update_sync_list();

	set_entry("name", selected->description->name);
	set_entry("class", selected->description->class);
	set_entry("color_class", selected->description->color_class);
	set_spin("border_l", selected->description->border.l);
	set_spin("border_r", selected->description->border.r);
	set_spin("border_t", selected->description->border.t);
	set_spin("border_b", selected->description->border.b);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_h");
	if (etching_get_selected_item(ws.e)->description->tile.w == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (etching_get_selected_item(ws.e)->description->tile.w == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (etching_get_selected_item(ws.e)->description->tile.w == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_v");
	if (etching_get_selected_item(ws.e)->description->tile.h == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (etching_get_selected_item(ws.e)->description->tile.h == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (etching_get_selected_item(ws.e)->description->tile.h == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");

	gtk_object_set_data(GTK_OBJECT(main_win),
			    "properties_state_normal_image",
			    g_strdup(selected->description->normal.image));
	gtk_object_set_data(GTK_OBJECT(main_win),
			    "properties_state_hilited_image",
			    g_strdup(selected->description->hilited.image));
	gtk_object_set_data(GTK_OBJECT(main_win),
			    "properties_state_clicked_image",
			    g_strdup(selected->description->clicked.image));
	gtk_object_set_data(GTK_OBJECT(main_win),
			    "properties_state_disabled_image",
			    g_strdup(selected->description->disabled.image));

	{
	   Evas_List           l;

	   for (l = selected->description->state_description; l; l = l->next)
	     {
		Ebits_State_Description state_d;
		gchar               key[4096];

		state_d = l->data;

		snprintf(key, sizeof(key),
			 "properties_state_%s_image", state_d->name);

		gtk_object_set_data(GTK_OBJECT(main_win), key, state_d->image);
	     }
	}
	/* update_image_list(); */
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
	gtk_widget_set_sensitive(w, 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_unselect_all(GTK_CLIST(w));
	w = gtk_object_get_data(GTK_OBJECT(main_win), "states2");
	gtk_clist_unselect_all(GTK_CLIST(w));
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");
	gtk_clist_unselect_all(GTK_CLIST(w));
     }
   if (bits)
     {
	set_spin("bit_min_h", bits->description->min.w);
	set_spin("bit_min_v", bits->description->min.h);
	set_spin("bit_max_h", bits->description->max.w);
	set_spin("bit_max_v", bits->description->max.h);
	set_spin("bit_pref_h", bits->description->preferred.w);
	set_spin("bit_pref_v", bits->description->preferred.h);
	set_spin("bit_pad_l", bits->description->padding.l);
	set_spin("bit_pad_r", bits->description->padding.r);
	set_spin("bit_pad_t", bits->description->padding.t);
	set_spin("bit_pad_b", bits->description->padding.b);
	set_spin("bit_inset_l", bits->description->inset.l);
	set_spin("bit_inset_r", bits->description->inset.r);
	set_spin("bit_inset_t", bits->description->inset.t);
	set_spin("bit_inset_b", bits->description->inset.b);
	set_spin("bit_step_h", bits->description->step.x);
	set_spin("bit_step_v", bits->description->step.y);
     }
}

void
workspace_update_visible_selection(void)
{
   Ebits_Object_Bit_State selected_state;

   selected_state = etching_get_selected_item(ws.e);

   if (selected_state)
     {
	int                 w, h;

	evas_move(ws.view_evas, ws.obj.sel,
		  selected_state->o->state.x + selected_state->x,
		  selected_state->o->state.y + selected_state->y);
	evas_resize(ws.view_evas, ws.obj.sel, selected_state->w,
		    selected_state->h);
	evas_set_line_xy(ws.view_evas, ws.obj.selection.l,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y +
			 selected_state->y + selected_state->h - 1);
	evas_set_line_xy(ws.view_evas, ws.obj.selection.r,
			 selected_state->o->state.x +
			 selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x +
			 selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y +
			 selected_state->y + selected_state->h - 1);
	evas_set_line_xy(ws.view_evas, ws.obj.selection.t,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x +
			 selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y);
	evas_set_line_xy(ws.view_evas, ws.obj.selection.b,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y +
			 selected_state->y + selected_state->h - 1,
			 selected_state->o->state.x +
			 selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y +
			 selected_state->y + selected_state->h - 1);
	evas_get_image_size(ws.view_evas, ws.obj.selection_tl.abs, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_tl.abs,
		  selected_state->o->state.x + selected_state->x - w,
		  selected_state->o->state.y + selected_state->y - h);
	evas_get_image_size(ws.view_evas, ws.obj.selection_tl.rel, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_tl.rel,
		  selected_state->o->state.x + selected_state->x - w,
		  selected_state->o->state.y + selected_state->y);
	evas_get_image_size(ws.view_evas, ws.obj.selection_tl.adj, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_tl.adj,
		  selected_state->o->state.x + selected_state->x,
		  selected_state->o->state.y + selected_state->y - h);

	evas_get_image_size(ws.view_evas, ws.obj.selection_br.abs, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_br.abs,
		  selected_state->o->state.x + selected_state->x +
		  selected_state->w,
		  selected_state->o->state.y + selected_state->y +
		  selected_state->h);
	evas_get_image_size(ws.view_evas, ws.obj.selection_br.rel, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_br.rel,
		  selected_state->o->state.x + selected_state->x +
		  selected_state->w,
		  selected_state->o->state.y + selected_state->y +
		  selected_state->h - h);
	evas_get_image_size(ws.view_evas, ws.obj.selection_br.adj, &w, &h);
	evas_move(ws.view_evas, ws.obj.selection_br.adj,
		  selected_state->o->state.x + selected_state->x +
		  selected_state->w - w,
		  selected_state->o->state.y + selected_state->y +
		  selected_state->h);
	evas_show(ws.view_evas, ws.obj.sel);
	evas_show(ws.view_evas, ws.obj.selection.l);
	evas_show(ws.view_evas, ws.obj.selection.r);
	evas_show(ws.view_evas, ws.obj.selection.t);
	evas_show(ws.view_evas, ws.obj.selection.b);
	evas_show(ws.view_evas, ws.obj.selection_tl.abs);
	evas_show(ws.view_evas, ws.obj.selection_tl.rel);
	evas_show(ws.view_evas, ws.obj.selection_tl.adj);
	evas_show(ws.view_evas, ws.obj.selection_br.abs);
	evas_show(ws.view_evas, ws.obj.selection_br.rel);
	evas_show(ws.view_evas, ws.obj.selection_br.adj);
     }
   else
     {
	evas_hide(ws.view_evas, ws.obj.sel);
	evas_hide(ws.view_evas, ws.obj.selection.l);
	evas_hide(ws.view_evas, ws.obj.selection.r);
	evas_hide(ws.view_evas, ws.obj.selection.t);
	evas_hide(ws.view_evas, ws.obj.selection.b);
	evas_hide(ws.view_evas, ws.obj.selection_tl.abs);
	evas_hide(ws.view_evas, ws.obj.selection_tl.rel);
	evas_hide(ws.view_evas, ws.obj.selection_tl.adj);
	evas_hide(ws.view_evas, ws.obj.selection_br.abs);
	evas_hide(ws.view_evas, ws.obj.selection_br.rel);
	evas_hide(ws.view_evas, ws.obj.selection_br.adj);
     }
}

void
workspace_update_relative_combos(void)
{
   GtkWidget          *w;
   GList              *bitnames = NULL;
   Evas_List           l;

   for (l = etching_get_bits(ws.e)->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	gchar              *text;

	state = l->data;
	text = state->description->name;
	if (!text)
	   text = "";
	bitnames = g_list_append(bitnames, text);
     }

   if (!bitnames)
      return;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "tl_rel_combo");
   gtk_combo_set_popdown_strings(GTK_COMBO(w), bitnames);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "br_rel_combo");
   gtk_combo_set_popdown_strings(GTK_COMBO(w), bitnames);

   if (bitnames)
      g_list_free(bitnames);
}

void
workspace_update_image_list(void)
{
   GtkWidget          *w;
   Evas_List           l = NULL;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "images");

   gtk_clist_freeze(GTK_CLIST(w));
   gtk_clist_clear(GTK_CLIST(w));

   for (l = etching_get_bits(ws.e)->bits; l; l = l->next)
     {
	Ebits_Object_Bit_State state;
	gchar              *text;
	gint                row;

	state = l->data;
	text = state->description->name;
	if (!text)
	   text = "";

	row = gtk_clist_append(GTK_CLIST(w), &text);

	if (etching_get_selected_item(ws.e))
	  {
	     if (state == etching_get_selected_item(ws.e))
		gtk_clist_select_row(GTK_CLIST(w), row, 0);
	  }
     }
   gtk_clist_thaw(GTK_CLIST(w));
}

void
workspace_update_sync_list(void)
{
   GtkWidget          *w;

   if (etching_get_selected_item(ws.e))
     {
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");

	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));

	{
	   Evas_List           l = NULL, ll = NULL;

	   for (l =
		etching_get_selected_item(ws.e)->o->description->
		bits; l; l = l->next)
	     {
		Ebits_Object_Bit_Description bit;
		gint                row;
		gchar              *text[1];

		bit = l->data;
		if (bit->name)
		   text[0] = bit->name;
		else
		   text[0] = "";

		row = gtk_clist_append(GTK_CLIST(w), text);

		for (ll =
		     etching_get_selected_item(ws.e)->
		     description->sync; ll; ll = ll->next)
		  {
		     char               *name;

		     name = ll->data;
		     if (!strcmp(text[0], name))
			gtk_clist_select_row(GTK_CLIST(w), row, 0);
		  }
	     }
	}
	gtk_clist_thaw(GTK_CLIST(w));
     }
}

gint
workspace_redraw(gpointer data)
{
   evas_render(ws.view_evas);
   workspace_zoom_redraw(ws.zoom_x, ws.zoom_y);
   ws.current_idle = 0;

   return FALSE;
}

void
workspace_zoom_redraw(int xx, int yy)
{
   GtkWidget          *zoom, *view, *w;
   GdkPixmap          *pmap;
   GdkGC              *gc;
   int                 x, y;
   char                s[100];

   w = gtk_object_get_data(GTK_OBJECT(main_win), "coords");
   sprintf(s, "%i, %i", xx, yy);
   gtk_label_set_text(GTK_LABEL(w), s);

   view = gtk_object_get_data(GTK_OBJECT(main_win), "view");
   zoom = gtk_object_get_data(GTK_OBJECT(main_win), "zoom");
   gc = gdk_gc_new(view->window);
   if (pref_get_zoom_method() == Smooth)
      pmap = gdk_pixmap_new(view->window, zoom->allocation.width,
			    zoom->allocation.height, -1);
   else
      pmap = zoom->window;

   gdk_draw_rectangle(pmap, gc, 1, 0, 0, zoom->allocation.width,
		      zoom->allocation.height);

   for (y = 0; y < (zoom->allocation.height + 1) / ws.zoom_scale; y++)
     {
	int                 i;

	for (i = 0; i < ws.zoom_scale; i++)
	  {
	     gdk_window_copy_area(pmap, gc,
				  0, (y * ws.zoom_scale) + i,
				  view->window,
				  xx -
				  ((zoom->allocation.width +
				    1) / (2 * ws.zoom_scale)),
				  yy -
				  ((zoom->allocation.height +
				    1) / (2 * ws.zoom_scale)) + y,
				  (zoom->allocation.width +
				   1) / ws.zoom_scale, 1);
	  }
     }
   for (x = ((zoom->allocation.width + 1) / ws.zoom_scale) - 1; x >= 0; x--)
     {
	int                 i;

	for (i = ws.zoom_scale - 1; i >= 0; i--)
	  {
	     gdk_window_copy_area(pmap, gc,
				  (x * ws.zoom_scale) + i, 0,
				  pmap, x, 0, 1, zoom->allocation.height);
	  }
     }

   if (pref_get_zoom_method() == Smooth)
     {
	gdk_window_copy_area(zoom->window, gc,
			     0, 0,
			     pmap,
			     0, 0,
			     zoom->allocation.width, zoom->allocation.height);
	gdk_pixmap_unref(pmap);
     }
   gdk_gc_destroy(gc);
}

void
workspace_zoom_in(void)
{
   if (ws.zoom_scale < 10)
      ws.zoom_scale++;

   workspace_zoom_redraw(ws.zoom_x, ws.zoom_y);
}

void
workspace_zoom_out(void)
{
   if (ws.zoom_scale > 1)
      ws.zoom_scale--;

   workspace_zoom_redraw(ws.zoom_x, ws.zoom_y);
}

static void
handle_bg_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		     int _y)
{
   etching_set_selected_item(ws.e, NULL);
   workspace_update_widget_from_selection();
   workspace_update_visible_selection();
   workspace_queue_draw();
}

static void
handle_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_put_data(_e, _o, "x", (void *)_x);
   evas_put_data(_e, _o, "y", (void *)_y);

   workspace_update_widget_from_selection();
   workspace_update_visible_selection();
   workspace_queue_draw();
}

static void
handle_mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_remove_data(_e, _o, "clicked");
}

static void
handle_mouse_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   if (evas_get_data(_e, _o, "clicked"))
     {
	int                 x, y;

	x = (int)evas_get_data(_e, _o, "x");
	y = (int)evas_get_data(_e, _o, "y");
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
	if (_o == ws.obj.handles.tl)
	  {
	     ws.backing_x += (_x - x);
	     ws.backing_y += (_y - y);
	     ws.backing_w += (x - _x);
	     ws.backing_h += (y - _y);
	     if (ws.backing_w < 1)
	       {
		  ws.backing_x -= 1 - ws.backing_w;
		  ws.backing_w = 1;
	       }
	     if (ws.backing_h < 1)
	       {
		  ws.backing_y -= 1 - ws.backing_h;
		  ws.backing_h = 1;
	       }
	  }
	else if (_o == ws.obj.handles.tr)
	  {
	     ws.backing_y += (_y - y);
	     ws.backing_w += (_x - x);
	     ws.backing_h += (y - _y);
	     if (ws.backing_w < 1)
	       {
		  ws.backing_w = 1;
	       }
	     if (ws.backing_h < 1)
	       {
		  ws.backing_y -= 1 - ws.backing_h;
		  ws.backing_h = 1;
	       }
	  }
	else if (_o == ws.obj.handles.bl)
	  {
	     ws.backing_x += (_x - x);
	     ws.backing_w += (x - _x);
	     ws.backing_h += (_y - y);
	     if (ws.backing_w < 1)
	       {
		  ws.backing_x -= 1 - ws.backing_w;
		  ws.backing_w = 1;
	       }
	     if (ws.backing_h < 1)
	       {
		  ws.backing_h = 1;
	       }
	  }
	else if (_o == ws.obj.handles.br)
	  {
	     ws.backing_w += (_x - x);
	     ws.backing_h += (_y - y);
	     if (ws.backing_w < 1)
	       {
		  ws.backing_w = 1;
	       }
	     if (ws.backing_h < 1)
	       {
		  ws.backing_h = 1;
	       }
	  }
	else if (_o == ws.obj.backing)
	  {
	     ws.backing_x += (_x - x);
	     ws.backing_y += (_y - y);
	  }
	workspace_configure_handles(NULL);
	etching_set_dirty(ws.e);
     }
}

static void
handle_bit_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		      int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   etching_set_selected_item(ws.e, state);

   if (ws.draft_mode)
     {
	workspace_update_visible_selection();
	evas_put_data(_e, _o, "clicked", (void *)1);
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
     }

   workspace_update_image_list();
   workspace_update_sync_list();
   workspace_update_widget_from_selection();
   workspace_queue_draw();
}

static void
handle_bit_mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		    int _y)
{
   evas_remove_data(_e, _o, "clicked");
   workspace_queue_draw();
}

static void
handle_bit_mouse_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
		      int _y)
{
   Ebits_Object_Bit_State selected_state;
   Ebits_Object        bits;
   int                 x, y;

   if ((ws.draft_mode) && (selected_state = etching_get_selected_item(ws.e)))
     {
	if (evas_get_data(_e, _o, "clicked"))
	  {
	     bits = etching_get_bits(ws.e);

	     x = (int)evas_get_data(_e, _o, "x");
	     y = (int)evas_get_data(_e, _o, "y");
	     evas_put_data(_e, _o, "x", (void *)_x);
	     evas_put_data(_e, _o, "y", (void *)_y);

	     selected_state->description->rel1.x += (_x - x);
	     selected_state->description->rel1.y += (_y - y);
	     selected_state->description->rel2.x += (_x - x);
	     selected_state->description->rel2.y += (_y - y);

	     ebits_move(bits, ws.backing_x, ws.backing_y);
	     ebits_resize(bits, ws.backing_w + 10, ws.backing_h + 10);
	     ebits_resize(bits, ws.backing_w, ws.backing_h);
	     workspace_update_widget_from_selection();
	     workspace_update_visible_selection();
	     ebits_set_state(bits, etching_get_current_state(ws.e));
	     etching_set_dirty(ws.e);
	  }
     }
   workspace_queue_draw();
}

static void
handle_adjuster_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			   int _y)
{
   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_put_data(_e, _o, "x", (void *)_x);
   evas_put_data(_e, _o, "y", (void *)_y);
   workspace_queue_draw();
}

static void
handle_adjuster_mouse_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			 int _y)
{
   evas_remove_data(_e, _o, "clicked");
   workspace_queue_draw();
}

static void
handle_adjuster_mouse_move(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			   int _y)
{
   Ebits_Object_Bit_State state;
   Ebits_Object_Bit_State selected_state;
   Ebits_Object        bits;
   int                 x, y, dx, dy;

   if (evas_get_data(_e, _o, "clicked"))
     {
	selected_state = etching_get_selected_item(ws.e);
	bits = etching_get_bits(ws.e);

	x = (int)evas_get_data(_e, _o, "x");
	y = (int)evas_get_data(_e, _o, "y");
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
	dx = (_x - x);
	dy = (_y - y);

	if (_o == ws.obj.selection_tl.abs)
	  {
	     selected_state->description->rel1.x += dx;
	     selected_state->description->rel1.y += dy;
	  }
	else if (_o == ws.obj.selection_tl.rel)
	  {
	     int                 rw, rh;
	     double              ix, iy;

	     state = NULL;
	     if (selected_state->description->rel1.name)
		state = ebits_get_bit_name(bits,
					   selected_state->
					   description->rel1.name);
	     rw = bits->state.w;
	     rh = bits->state.h;
	     if (state)
	       {
		  rw = selected_state->w;
		  rh = selected_state->h;
	       }
	     rw += selected_state->description->rel1.ax;
	     rh += selected_state->description->rel1.ay;
	     ix = 0;
	     iy = 0;
	     if (rw > 0)
		ix = (double)dx / (double)rw;
	     if (rh > 0)
		iy = (double)dy / (double)rh;
	     selected_state->description->rel1.rx += ix;
	     selected_state->description->rel1.ry += iy;
	  }
	else if (_o == ws.obj.selection_tl.adj)
	  {
	     selected_state->description->rel1.ax += dx;
	     selected_state->description->rel1.ay += dy;
	  }
	else if (_o == ws.obj.selection_br.abs)
	  {
	     selected_state->description->rel2.x += dx;
	     selected_state->description->rel2.y += dy;
	  }
	else if (_o == ws.obj.selection_br.rel)
	  {
	     int                 rw, rh;
	     double              ix, iy;

	     state = NULL;
	     if (selected_state->description->rel2.name)
		state = ebits_get_bit_name(bits,
					   selected_state->
					   description->rel2.name);
	     rw = bits->state.w;
	     rh = bits->state.h;
	     if (state)
	       {
		  rw = selected_state->w;
		  rh = selected_state->h;
	       }
	     rw += selected_state->description->rel1.ax;
	     rh += selected_state->description->rel1.ay;
	     ix = 0;
	     iy = 0;
	     if (rw > 0)
		ix = (double)dx / (double)rw;
	     if (rh > 0)
		iy = (double)dy / (double)rh;
	     selected_state->description->rel2.rx += ix;
	     selected_state->description->rel2.ry += iy;
	  }
	else if (_o == ws.obj.selection_br.adj)
	  {
	     selected_state->description->rel2.ax += dx;
	     selected_state->description->rel2.ay += dy;
	  }

	ebits_move(bits, ws.backing_x, ws.backing_y);
	ebits_resize(bits, ws.backing_w + 10, ws.backing_h + 10);
	ebits_resize(bits, ws.backing_w, ws.backing_h);
	workspace_update_widget_from_selection();	/* that used to be a "simple_update..." call */
	workspace_update_visible_selection();
	ebits_set_state(bits, etching_get_current_state(ws.e));
	etching_set_dirty(ws.e);
     }

   workspace_queue_draw();
}

static              gint
workspace_configure_handles(gpointer data)
{
   Ebits_Object        bits;
   int                 w, h;
   GtkWidget          *entry;
   gchar               buf[32];

   if (!ws.obj.handles.tl)
      return 0;

   bits = etching_get_bits(ws.e);

   if (bits)
     {
	ebits_move(bits, ws.backing_x, ws.backing_y);
	ebits_resize(bits, ws.backing_w, ws.backing_h);
     }

   workspace_update_visible_selection();

   if (bits)
     {
	ws.backing_x = bits->state.x;
	ws.backing_y = bits->state.y;
	ws.backing_w = bits->state.w;
	ws.backing_h = bits->state.h;
     }

   evas_get_image_size(ws.view_evas, ws.obj.handles.tl, &w, &h);
   evas_move(ws.view_evas, ws.obj.handles.tl, ws.backing_x - w,
	     ws.backing_y - h);

   evas_get_image_size(ws.view_evas, ws.obj.handles.tr, &w, &h);
   evas_move(ws.view_evas, ws.obj.handles.tr, ws.backing_x + ws.backing_w,
	     ws.backing_y - h);

   evas_get_image_size(ws.view_evas, ws.obj.handles.bl, &w, &h);
   evas_move(ws.view_evas, ws.obj.handles.bl, ws.backing_x - w,
	     ws.backing_y + ws.backing_h);

   evas_get_image_size(ws.view_evas, ws.obj.handles.br, &w, &h);
   evas_move(ws.view_evas, ws.obj.handles.br, ws.backing_x + ws.backing_w,
	     ws.backing_y + ws.backing_h);

   evas_move(ws.view_evas, ws.obj.backing, ws.backing_x - 1, ws.backing_y - 1);
   evas_resize(ws.view_evas, ws.obj.backing, ws.backing_w + 2,
	       ws.backing_h + 2);

   evas_set_line_xy(ws.view_evas, ws.obj.border.l,
		    ws.backing_x - 1, ws.backing_y - 1,
		    ws.backing_x - 1, ws.backing_y + ws.backing_h);
   evas_set_line_xy(ws.view_evas, ws.obj.border.t,
		    ws.backing_x - 1, ws.backing_y - 1,
		    ws.backing_x + ws.backing_w, ws.backing_y - 1);
   evas_set_line_xy(ws.view_evas, ws.obj.border.b,
		    ws.backing_x - 1, ws.backing_y + ws.backing_h,
		    ws.backing_x + ws.backing_w, ws.backing_y + ws.backing_h);
   evas_set_line_xy(ws.view_evas, ws.obj.border.r,
		    ws.backing_x + ws.backing_w, ws.backing_y - 1,
		    ws.backing_x + ws.backing_w, ws.backing_y + ws.backing_h);

   entry = gtk_object_get_data(GTK_OBJECT(main_win), "x");
   g_snprintf(buf, sizeof(buf), "%3.0f", ws.backing_x);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);

   entry = gtk_object_get_data(GTK_OBJECT(main_win), "y");
   g_snprintf(buf, sizeof(buf), "%3.0f", ws.backing_y);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);

   entry = gtk_object_get_data(GTK_OBJECT(main_win), "width");
   g_snprintf(buf, sizeof(buf), "%3.0f", ws.backing_w);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);

   entry = gtk_object_get_data(GTK_OBJECT(main_win), "height");
   g_snprintf(buf, sizeof(buf), "%3.0f", ws.backing_h);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);

   workspace_queue_draw();
   return 0;
}

static              gint
workspace_create_handles(gpointer data)
{
   evas_show(ws.view_evas, ws.obj.handles.tl =
	     evas_add_image_from_file(ws.view_evas,
				      PACKAGE_DATA_DIR "/pixmaps/handle1.png"));
   evas_show(ws.view_evas, ws.obj.handles.tr =
	     evas_add_image_from_file(ws.view_evas,
				      PACKAGE_DATA_DIR "/pixmaps/handle2.png"));
   evas_show(ws.view_evas, ws.obj.handles.bl =
	     evas_add_image_from_file(ws.view_evas,
				      PACKAGE_DATA_DIR "/pixmaps/handle3.png"));
   evas_show(ws.view_evas, ws.obj.handles.br =
	     evas_add_image_from_file(ws.view_evas,
				      PACKAGE_DATA_DIR "/pixmaps/handle4.png"));
   evas_show(ws.view_evas, ws.obj.border.l = evas_add_line(ws.view_evas));
   evas_show(ws.view_evas, ws.obj.border.t = evas_add_line(ws.view_evas));
   evas_show(ws.view_evas, ws.obj.border.b = evas_add_line(ws.view_evas));
   evas_show(ws.view_evas, ws.obj.border.r = evas_add_line(ws.view_evas));
   evas_show(ws.view_evas, ws.obj.backing =
	     evas_add_image_from_file(ws.view_evas,
				      PACKAGE_DATA_DIR "/pixmaps/backing.png"));
   evas_set_color(ws.view_evas, ws.obj.border.l, 0, 0, 0, 255);
   evas_set_color(ws.view_evas, ws.obj.border.t, 0, 0, 0, 255);
   evas_set_color(ws.view_evas, ws.obj.border.b, 0, 0, 0, 255);
   evas_set_color(ws.view_evas, ws.obj.border.r, 0, 0, 0, 255);
   evas_set_layer(ws.view_evas, ws.obj.handles.tl, 5);
   evas_set_layer(ws.view_evas, ws.obj.handles.tr, 5);
   evas_set_layer(ws.view_evas, ws.obj.handles.bl, 5);
   evas_set_layer(ws.view_evas, ws.obj.handles.br, 5);
   evas_set_layer(ws.view_evas, ws.obj.border.l, 4);
   evas_set_layer(ws.view_evas, ws.obj.border.t, 4);
   evas_set_layer(ws.view_evas, ws.obj.border.b, 4);
   evas_set_layer(ws.view_evas, ws.obj.border.r, 4);
   evas_set_layer(ws.view_evas, ws.obj.backing, 3);
   evas_callback_add(ws.view_evas, ws.obj.handles.tl, CALLBACK_MOUSE_DOWN,
		     handle_mouse_down, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.tl, CALLBACK_MOUSE_UP,
		     handle_mouse_up, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.tl, CALLBACK_MOUSE_MOVE,
		     handle_mouse_move, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.tr, CALLBACK_MOUSE_DOWN,
		     handle_mouse_down, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.tr, CALLBACK_MOUSE_UP,
		     handle_mouse_up, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.tr, CALLBACK_MOUSE_MOVE,
		     handle_mouse_move, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.bl, CALLBACK_MOUSE_DOWN,
		     handle_mouse_down, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.bl, CALLBACK_MOUSE_UP,
		     handle_mouse_up, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.bl, CALLBACK_MOUSE_MOVE,
		     handle_mouse_move, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.br, CALLBACK_MOUSE_DOWN,
		     handle_mouse_down, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.br, CALLBACK_MOUSE_UP,
		     handle_mouse_up, NULL);
   evas_callback_add(ws.view_evas, ws.obj.handles.br, CALLBACK_MOUSE_MOVE,
		     handle_mouse_move, NULL);
   evas_callback_add(ws.view_evas, ws.obj.backing, CALLBACK_MOUSE_DOWN,
		     handle_mouse_down, NULL);
   evas_callback_add(ws.view_evas, ws.obj.backing, CALLBACK_MOUSE_UP,
		     handle_mouse_up, NULL);
   evas_callback_add(ws.view_evas, ws.obj.backing, CALLBACK_MOUSE_MOVE,
		     handle_mouse_move, NULL);

   workspace_configure_handles(NULL);
   return 0;
}

void
workspace_set_grid_image(char *filename)
{
   int                 ww, hh, ok, r, g, b;

   if (!filename)
      return;

   E_DB_STR_SET(pref_get_config(), "/grid/image", filename);

   evas_set_image_file(ws.view_evas, ws.obj.grid, filename);
   evas_get_image_size(ws.view_evas, ws.obj.grid, &ww, &hh);
   evas_set_image_fill(ws.view_evas, ws.obj.grid, 0, 0, ww, hh);
   evas_move(ws.view_evas, ws.obj.grid, 0, 0);
   evas_resize(ws.view_evas, ws.obj.grid, 9999, 9999);
   ok = 0;
   E_DB_INT_GET(pref_get_config(), "/grid/r", r, ok);
   E_DB_INT_GET(pref_get_config(), "/grid/g", g, ok);
   E_DB_INT_GET(pref_get_config(), "/grid/b", b, ok);

   if (ok)
      evas_set_color(ws.view_evas, ws.obj.grid, r, g, b, 255);

   e_db_flush();
}

void
workspace_set_grid_tint(int r, int g, int b)
{
   evas_set_color(ws.view_evas, ws.obj.grid, r, g, b, 255);
   workspace_queue_draw();
}

void
workspace_move_to(int x, int y)
{
   ws.zoom_x = x;
   ws.zoom_y = y;

   if (ws.obj.pointer)
      evas_move(ws.view_evas, ws.obj.pointer, x, y);

   evas_event_move(ws.view_evas, x, y);
   workspace_queue_draw();
}

void
workspace_button_press_event(int x, int y, int button)
{
   evas_event_button_down(ws.view_evas, x, y, button);
}

void
workspace_button_release_event(int x, int y, int button)
{
   evas_event_button_up(ws.view_evas, x, y, button);
}

void
workspace_enter_notify_event(GdkEventCrossing * ev)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "coords");
   gtk_widget_show(w);

   if (ws.obj.pointer)
     {
	/* move cursor to new location first, then show it.
	 * Avoids some flickering. */
	evas_move(ws.view_evas, ws.obj.pointer, ev->x, ev->y);
	evas_show(ws.view_evas, ws.obj.pointer);
     }
   workspace_queue_draw();
}

void
workspace_leave_notify_event(GdkEventCrossing * ev)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "coords");
   gtk_widget_hide(w);

   if (ws.obj.pointer)
      evas_hide(ws.view_evas, ws.obj.pointer);
   workspace_queue_draw();
}

void
workspace_expose_event(GtkWidget * widget, GdkEventExpose * ev)
{
   if (ws.new_evas)
     {
	int                 w, h;

	ws.new_evas = 0;
	gdk_window_set_back_pixmap(widget->window, NULL, FALSE);
	evas_set_output(ws.view_evas,
			GDK_WINDOW_XDISPLAY(widget->window),
			GDK_WINDOW_XWINDOW(widget->window),
			GDK_VISUAL_XVISUAL(gtk_widget_get_visual
					   (widget)),
			GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap
					       (widget)));
	evas_set_output_size(ws.view_evas, widget->allocation.width,
			     widget->allocation.height);
	evas_set_output_viewport(ws.view_evas, 0, 0,
				 widget->allocation.width,
				 widget->allocation.height);

	evas_set_font_cache(ws.view_evas, 1 * 1024 * 1024);
	evas_set_image_cache(ws.view_evas, 8 * 1024 * 1024);

	ws.obj.bg_rect = evas_add_rectangle(ws.view_evas);
	evas_set_layer(ws.view_evas, ws.obj.bg_rect, -9999);
	evas_set_color(ws.view_evas, ws.obj.bg_rect, 255, 255, 255, 255);
	evas_move(ws.view_evas, ws.obj.bg_rect, 0, 0);
	evas_resize(ws.view_evas, ws.obj.bg_rect, 9999, 9999);
	evas_show(ws.view_evas, ws.obj.bg_rect);
	{
	   char               *tile = NULL;
	   int                 ok = 0, r, g, b;

	   E_DB_STR_GET(pref_get_config(), "/grid/image", tile, ok);

	   if (!tile)
	      ws.obj.grid =
		 evas_add_image_from_file(ws.view_evas,
					  PACKAGE_DATA_DIR "/pixmaps/tile.png");
	   else
	     {
		ws.obj.grid = evas_add_image_from_file(ws.view_evas, tile);
		free(tile);
	     }

	   ok = 0;
	   E_DB_INT_GET(pref_get_config(), "/grid/r", r, ok);
	   E_DB_INT_GET(pref_get_config(), "/grid/g", g, ok);
	   E_DB_INT_GET(pref_get_config(), "/grid/b", b, ok);

	   if (ok)
	      evas_set_color(ws.view_evas, ws.obj.grid, r, g, b, 255);

	   e_db_flush();
	}

	evas_callback_add(ws.view_evas, ws.obj.grid,
			  CALLBACK_MOUSE_DOWN, handle_bg_mouse_down, NULL);
	evas_get_image_size(ws.view_evas, ws.obj.grid, &w, &h);
	evas_set_image_fill(ws.view_evas, ws.obj.grid, 0, 0, w, h);
	evas_show(ws.view_evas, ws.obj.grid);
	evas_move(ws.view_evas, ws.obj.grid, 0, 0);
	evas_resize(ws.view_evas, ws.obj.grid, 9999, 9999);

	/* Splash setup was here long ago -- cK */

	ws.backing_x = 32;
	ws.backing_y = 32;
	ws.backing_w = widget->allocation.width - 64;
	ws.backing_h = widget->allocation.height - 64;

	ws.obj.pointer =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/pointer.png");
	evas_set_layer(ws.view_evas, ws.obj.pointer, 999);
	evas_set_pass_events(ws.view_evas, ws.obj.pointer, 1);

	/*
	 * ws.handle_bits = ebits_new();
	 * ws.handle_bits->description = ebits_new_description();
	 * 
	 * ebits_add_to_evas(ws.handle_bits, ws.view_evas);
	 * ebits_show(ws.handle_bits);
	 * ebits_set_layer(ws.handle_bits, 50);
	 * ebits_move(ws.handle_bits, ws.backing_x, ws.backing_y);
	 * ebits_resize(ws.handle_bits, ws.backing_w, ws.backing_h);
	 */

	ws.obj.sel = evas_add_rectangle(ws.view_evas);
	ws.obj.selection.l = evas_add_line(ws.view_evas);
	ws.obj.selection.r = evas_add_line(ws.view_evas);
	ws.obj.selection.t = evas_add_line(ws.view_evas);
	ws.obj.selection.b = evas_add_line(ws.view_evas);
	ws.obj.selection_tl.abs =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_abs.png");
	ws.obj.selection_tl.rel =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_rel.png");
	ws.obj.selection_tl.adj =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_adj.png");
	ws.obj.selection_br.abs =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_abs.png");
	ws.obj.selection_br.rel =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_rel.png");
	ws.obj.selection_br.adj =
	   evas_add_image_from_file(ws.view_evas,
				    PACKAGE_DATA_DIR "/pixmaps/handle_adj.png");
	evas_set_color(ws.view_evas, ws.obj.sel, 255, 255, 255, 80);
	evas_set_color(ws.view_evas, ws.obj.selection.l, 200, 50, 50, 200);
	evas_set_color(ws.view_evas, ws.obj.selection.r, 200, 50, 50, 200);
	evas_set_color(ws.view_evas, ws.obj.selection.t, 200, 50, 50, 200);
	evas_set_color(ws.view_evas, ws.obj.selection.b, 200, 50, 50, 200);
	evas_set_layer(ws.view_evas, ws.obj.sel, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection.l, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection.r, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection.t, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection.b, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_tl.abs, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_tl.rel, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_tl.adj, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_br.abs, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_br.rel, 100);
	evas_set_layer(ws.view_evas, ws.obj.selection_br.adj, 100);
	evas_set_pass_events(ws.view_evas, ws.obj.sel, 1);
	evas_set_pass_events(ws.view_evas, ws.obj.selection.l, 1);
	evas_set_pass_events(ws.view_evas, ws.obj.selection.r, 1);
	evas_set_pass_events(ws.view_evas, ws.obj.selection.t, 1);
	evas_set_pass_events(ws.view_evas, ws.obj.selection.b, 1);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.abs,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.abs,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.abs,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.rel,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.rel,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.rel,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.adj,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.adj,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_tl.adj,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.abs,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.abs,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.abs,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.rel,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.rel,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.rel,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.adj,
			  CALLBACK_MOUSE_DOWN,
			  handle_adjuster_mouse_down, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.adj,
			  CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(ws.view_evas, ws.obj.selection_br.adj,
			  CALLBACK_MOUSE_MOVE,
			  handle_adjuster_mouse_move, NULL);

	if (!ws.obj.handles.tl)
	   workspace_create_handles(NULL);

	if (pref_splashscreen_enabled())
	  {
	     show_splash(ws.view_evas, &ws.current_idle, workspace_redraw);
	  }

/*      else */
	{
	   Evas_List           files;

	   if ((files = pref_get_files()))
	     {
		for (; files; files = files->next)
		  {
		     Etching            *e = etching_load(files->data);

		     workspace_add_etching(e);
		     workspace_set_current_etching(e);
		  }
	     }
	   else
	     {
		Etching            *e = etching_new("new.bits.db");

		workspace_add_etching(e);
		workspace_set_current_etching(e);

	     }
	   workspace_update_states();
	}

	{
	   GdkPixmap          *src, *mask;
	   GdkColor            fg, bg;
	   GdkGC              *gc;
	   GdkCursor          *cursor;

	   src = gdk_pixmap_new(widget->window, 1, 1, 1);
	   mask = gdk_pixmap_new(widget->window, 1, 1, 1);
	   gc = gdk_gc_new(mask);
	   fg.pixel = 0;
	   gdk_gc_set_foreground(gc, &fg);
	   gdk_draw_rectangle(mask, gc, 1, 0, 0, 1, 1);
	   cursor = gdk_cursor_new_from_pixmap(src, mask, &fg, &bg, 0, 0);
	   gdk_window_set_cursor(widget->window, cursor);
	   gdk_cursor_destroy(cursor);
	   gdk_pixmap_unref(src);
	   gdk_pixmap_unref(mask);
	   gdk_gc_destroy(gc);
	}

     }

   evas_update_rect(ws.view_evas, ev->area.x, ev->area.y, ev->area.width,
		    ev->area.height);
   workspace_queue_draw();
}

void
workspace_select_image(int index)
{
   Evas_List           l;
   int                 count;
   Ebits_Object_Bit_State selected;

   selected = etching_get_selected_item(ws.e);

   if (index == -1)
      etching_set_selected_item(ws.e, NULL);

   for (count = 0, l = etching_get_bits(ws.e)->bits; l; l = l->next, count++)
     {
	if (count == index)
	  {
	     etching_set_selected_item(ws.e, l->data);
	     break;
	  }
     }

   if (selected != etching_get_selected_item(ws.e))
     {
	workspace_update_visible_selection();
	workspace_update_widget_from_selection();
	workspace_queue_draw();
     }
}

void
workspace_configure_event(GtkWidget * widget)
{
   if (!ws.view_evas)
      return;

   evas_set_output_size(ws.view_evas,
			widget->allocation.width, widget->allocation.height);
   evas_set_output_viewport(ws.view_evas, 0, 0,
			    widget->allocation.width,
			    widget->allocation.height);
   evas_update_rect(ws.view_evas, 0, 0,
		    widget->allocation.width, widget->allocation.height);
   workspace_queue_draw();
}

void
workspace_set_state(char *state)
{
   if (etching_get_bits(ws.e))
     {
	etching_set_current_state(ws.e, state);
	ebits_set_state(etching_get_bits(ws.e), state);
	workspace_queue_draw();
     }
}

void
workspace_raise_selection(void)
{
   Ebits_Object_Bit_State selected_state;
   Ebits_Object        bits;

   selected_state = etching_get_selected_item(ws.e);

   if (selected_state)
     {
	bits = etching_get_bits(ws.e);

	bits->bits = evas_list_remove(bits->bits, selected_state);
	bits->bits = evas_list_append(bits->bits, selected_state);
	bits->description->bits =
	   evas_list_remove(bits->description->bits,
			    selected_state->description);
	bits->description->bits =
	   evas_list_append(bits->description->bits,
			    selected_state->description);

	if (selected_state->object)
	   evas_raise(selected_state->o->state.evas, selected_state->object);

	workspace_update_widget_from_selection();
	workspace_update_visible_selection();
	workspace_queue_draw();
	etching_set_dirty(ws.e);
     }
}

void
workspace_lower_selection(void)
{
   Ebits_Object_Bit_State selected_state;

   selected_state = etching_get_selected_item(ws.e);

   if (selected_state)
     {
	etching_get_bits(ws.e)->bits =
	   evas_list_remove(etching_get_bits(ws.e)->bits, selected_state);
	etching_get_bits(ws.e)->bits =
	   evas_list_prepend(etching_get_bits(ws.e)->bits, selected_state);
	etching_get_bits(ws.e)->description->bits =
	   evas_list_remove(etching_get_bits(ws.e)->description->
			    bits, selected_state->description);
	etching_get_bits(ws.e)->description->bits =
	   evas_list_prepend(etching_get_bits(ws.e)->description->
			     bits, selected_state->description);

	if (selected_state->object)
	   evas_lower(selected_state->o->state.evas, selected_state->object);

	workspace_update_widget_from_selection();
	workspace_update_visible_selection();
	workspace_queue_draw();
	etching_set_dirty(ws.e);
     }
}

void
workspace_delete_selection(void)
{
   if (etching_get_selected_item(ws.e))
     {
	ebits_del_bit(etching_get_bits(ws.e), etching_get_selected_item(ws.e));
	etching_set_selected_item(ws.e, NULL);
	workspace_update_visible_selection();
	workspace_update_widget_from_selection();
	workspace_queue_draw();
	workspace_update_relative_combos();
	workspace_update_image_list();
	workspace_update_sync_list();
	etching_set_dirty(ws.e);
     }
}

void
workspace_reset_selection(void)
{
   int                 ew, eh;

   evas_get_drawable_size(ws.view_evas, &ew, &eh);
   ws.backing_x = 32;
   ws.backing_y = 32;
   ws.backing_w = ew - 64;
   ws.backing_h = eh - 64;
   workspace_configure_handles(NULL);
   etching_set_dirty(ws.e);
}

void
workspace_set_draft_mode(int onoff)
{
   ws.draft_mode = onoff;

   if (ws.draft_mode)
     {
	evas_show(ws.view_evas, ws.obj.handles.tl);
	evas_show(ws.view_evas, ws.obj.handles.tr);
	evas_show(ws.view_evas, ws.obj.handles.bl);
	evas_show(ws.view_evas, ws.obj.handles.br);
	evas_show(ws.view_evas, ws.obj.border.l);
	evas_show(ws.view_evas, ws.obj.border.t);
	evas_show(ws.view_evas, ws.obj.border.b);
	evas_show(ws.view_evas, ws.obj.border.r);
	evas_show(ws.view_evas, ws.obj.backing);

	if (etching_get_selected_item(ws.e))
	  {
	     workspace_update_visible_selection();
	     evas_show(ws.view_evas, ws.obj.sel);
	     evas_show(ws.view_evas, ws.obj.selection.l);
	     evas_show(ws.view_evas, ws.obj.selection.r);
	     evas_show(ws.view_evas, ws.obj.selection.t);
	     evas_show(ws.view_evas, ws.obj.selection.b);
	     evas_show(ws.view_evas, ws.obj.selection_tl.abs);
	     evas_show(ws.view_evas, ws.obj.selection_tl.rel);
	     evas_show(ws.view_evas, ws.obj.selection_tl.adj);
	     evas_show(ws.view_evas, ws.obj.selection_br.abs);
	     evas_show(ws.view_evas, ws.obj.selection_br.rel);
	     evas_show(ws.view_evas, ws.obj.selection_br.adj);
	  }
     }
   else
     {
	evas_hide(ws.view_evas, ws.obj.handles.tl);
	evas_hide(ws.view_evas, ws.obj.handles.tr);
	evas_hide(ws.view_evas, ws.obj.handles.bl);
	evas_hide(ws.view_evas, ws.obj.handles.br);
	evas_hide(ws.view_evas, ws.obj.border.l);
	evas_hide(ws.view_evas, ws.obj.border.t);
	evas_hide(ws.view_evas, ws.obj.border.b);
	evas_hide(ws.view_evas, ws.obj.border.r);
	evas_hide(ws.view_evas, ws.obj.backing);

	if (etching_get_selected_item(ws.e))
	  {
	     evas_hide(ws.view_evas, ws.obj.sel);
	     evas_hide(ws.view_evas, ws.obj.selection.l);
	     evas_hide(ws.view_evas, ws.obj.selection.r);
	     evas_hide(ws.view_evas, ws.obj.selection.t);
	     evas_hide(ws.view_evas, ws.obj.selection.b);
	     evas_hide(ws.view_evas, ws.obj.selection_tl.abs);
	     evas_hide(ws.view_evas, ws.obj.selection_tl.rel);
	     evas_hide(ws.view_evas, ws.obj.selection_tl.adj);
	     evas_hide(ws.view_evas, ws.obj.selection_br.abs);
	     evas_hide(ws.view_evas, ws.obj.selection_br.rel);
	     evas_hide(ws.view_evas, ws.obj.selection_br.adj);
	  }
     }

   workspace_queue_draw();
}

void
workspace_apply_settings(void)
{
   workspace_update_selection_from_widget();
   ebits_move(etching_get_bits(ws.e), ws.backing_x, ws.backing_y);
   ebits_resize(etching_get_bits(ws.e), ws.backing_w + 10, ws.backing_h + 10);
   ebits_resize(etching_get_bits(ws.e), ws.backing_w, ws.backing_h);
   workspace_update_visible_selection();
   ebits_set_state(etching_get_bits(ws.e), etching_get_current_state(ws.e));
   workspace_update_widget_from_selection();
   workspace_queue_draw();
   etching_set_dirty(ws.e);
}
