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

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "bits.h"

#define QUEUE_DRAW \
if (current_idle) gtk_idle_remove(current_idle);\
current_idle = gtk_idle_add(view_redraw, NULL);

extern GtkWidget *main_win;
extern char etcher_config[4096];

Evas view_evas = NULL;
gint render_method = 0;
gint zoom_method = 0;
guint current_idle = 0;
gboolean no_splash = FALSE;
char *load_file = NULL;

static int new_evas = 1;
static int new_fade = 0;
static Evas_Object o_logo = NULL, o_info1 = NULL, o_info2, o_info3, o_info4;
static Evas_Object o_handle1 = NULL, o_handle2, o_handle3, o_handle4, o_edge1, o_edge2, o_edge3, o_edge4, o_backing, o_pointer = NULL;
static Evas_Object o_select_rect, o_select_line1, o_select_line2, o_select_line3, o_select_line4;
static Evas_Object o_select_abs1, o_select_rel1, o_select_adj1, o_select_abs2, o_select_rel2, o_select_adj2;
static double backing_x, backing_y, backing_w, backing_h;
static gint draft_mode = 1;
static gint zoom_x, zoom_y, zoom_scale = 4;

static Ebits_Object bits = NULL;
static Ebits_Object_Bit_State selected_state = NULL;
static int current_state = 0;

static void update_selection_from_widget(void);
static void update_widget_from_selection(void);
static void update_widget_from_simple_selection(void);
void zoom_redraw(int xx, int yy);
static void handle_bg_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_bit_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_bit_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_bit_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_adjuster_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_adjuster_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void handle_adjuster_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static gint view_shrink_logo(gpointer data);
static gint view_fade_info(gpointer data);
static gint view_scroll_logo(gpointer data);
static gint view_scroll_info(gpointer data);
gint view_configure_handles(gpointer data);

#define GET_SPIN(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); selected_state->description->val = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
#define GET_ENTRY(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); if (selected_state->description->val) free(selected_state->description->val); if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "")) selected_state->description->val = NULL; else selected_state->description->val = strdup(gtk_entry_get_text(GTK_ENTRY(w)));
#define GET_ALL_SPIN(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); bits->description->val = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
#define SET_SPIN(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), selected_state->description->val);
#define SET_ENTRY(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); if (selected_state->description->val) gtk_entry_set_text(GTK_ENTRY(w), selected_state->description->val); else gtk_entry_set_text(GTK_ENTRY(w), "");
#define SET_ALL_SPIN(name, val) w = gtk_object_get_data(GTK_OBJECT(main_win), name); gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), bits->description->val);

static void
update_selection_from_widget(void)
{
   GtkWidget *w;
   Evas_List l;
   
   if (selected_state)
     {
	Ebits_Object_Bit_State selected;
	
	selected = selected_state;
        GET_SPIN("tl_abs_h", rel1.x);
	GET_SPIN("tl_abs_v", rel1.y);
	GET_ENTRY("tl_rel", rel1.name);
	GET_SPIN("tl_rel_h", rel1.rx);
	GET_SPIN("tl_rel_v", rel1.ry);
	GET_SPIN("tl_adj_h", rel1.ax);
	GET_SPIN("tl_adj_v", rel1.ay);
	
	GET_SPIN("br_abs_h", rel2.x);
	GET_SPIN("br_abs_v", rel2.y);
	GET_ENTRY("br_rel", rel2.name);
	GET_SPIN("br_rel_h", rel2.rx);
	GET_SPIN("br_rel_v", rel2.ry);
	GET_SPIN("br_adj_h", rel2.ax);
	GET_SPIN("br_adj_v", rel2.ay);
	
	GET_SPIN("content_alignment_h", align.w);
	GET_SPIN("content_alignment_v", align.h);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "aspect");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	  {
	     GET_SPIN("aspect_h", aspect.x);
	     GET_SPIN("aspect_v", aspect.y);
	  }
	else
	  {
	     selected_state->description->aspect.x = 0;
	     selected_state->description->aspect.y = 0;
	     SET_SPIN("aspect_h", aspect.x);
	     SET_SPIN("aspect_v", aspect.y);
	  }
	GET_SPIN("min_h", min.w);
	GET_SPIN("min_v", min.h);
	GET_SPIN("max_h", max.w);
	GET_SPIN("max_v", max.h);
	GET_SPIN("step_h", step.x);
	GET_SPIN("step_v", step.y);

	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_h");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	   selected_state->description->max.w = 0;
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_v");
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	   selected_state->description->max.h = 0;
	
	if (selected_state->description->sync)
	  {
	     Evas_List ll;
	     
	     for (ll = selected_state->description->sync; ll; ll = ll->next)
		free(ll->data);
	     evas_list_free(selected_state->description->sync);
	     selected_state->description->sync = NULL;
	  }
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");
	  {
	     GList *l;
	     
	     for (l = GTK_CLIST(w)->row_list; l; l = l->next)
	       {
		  GtkCListRow *row;
		  GtkCellText *cell;
		  
		  row = l->data;
		  cell = (GtkCellText *)row->cell;
		  if (row->state == GTK_STATE_SELECTED)
		     selected_state->description->sync = 
		     evas_list_append(selected_state->description->sync, 
				      strdup(cell->text));
	       }
	  }
	GET_ENTRY("name", name);
	GET_ENTRY("class", class);
	GET_ENTRY("img_normal", normal.image);
	GET_ENTRY("img_hilited", hilited.image);
	GET_ENTRY("img_clicked", clicked.image);
	GET_ENTRY("img_disabled", disabled.image);
	GET_SPIN("border_l", border.l);
	GET_SPIN("border_r", border.r);
	GET_SPIN("border_t", border.t);
	GET_SPIN("border_b", border.b);
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_h");
	if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Fill"))
	   selected_state->description->tile.w = 0;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile"))
	   selected_state->description->tile.w = 1;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile Integer"))
	   selected_state->description->tile.w = 2;
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_v");
	if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Fill"))
	   selected_state->description->tile.h = 0;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile"))
	   selected_state->description->tile.h = 1;
	else if (!strcmp(gtk_entry_get_text(GTK_ENTRY(w)), "Tile Integer"))
	   selected_state->description->tile.h = 2;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     gchar *text;
	     gint row;
	     
	     state = l->data;
	     text = state->description->name;
	     if (!text) text = "";
	     row = gtk_clist_append(GTK_CLIST(w), &text);
	     if (state == selected)
		gtk_clist_select_row(GTK_CLIST(w), row, 0);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
     }
   if (bits)
     {
       GET_ALL_SPIN("bit_min_h", min.w);
       GET_ALL_SPIN("bit_min_v", min.h);
       GET_ALL_SPIN("bit_max_h", max.w);
       GET_ALL_SPIN("bit_max_v", max.h);
       GET_ALL_SPIN("bit_pad_l", padding.l);
       GET_ALL_SPIN("bit_pad_r", padding.r);
       GET_ALL_SPIN("bit_pad_t", padding.t);
       GET_ALL_SPIN("bit_pad_b", padding.b);
       GET_ALL_SPIN("bit_inset_l", inset.l);
       GET_ALL_SPIN("bit_inset_r", inset.r);
       GET_ALL_SPIN("bit_inset_t", inset.t);
       GET_ALL_SPIN("bit_inset_b", inset.b);
       GET_ALL_SPIN("bit_step_h", step.x);
       GET_ALL_SPIN("bit_step_v", step.y);
     }
}

static void
update_widget_from_selection(void)
{
   GtkWidget *w;
   Evas_List l;

   if (selected_state)
     {
	Ebits_Object_Bit_State selected;
	
	selected = selected_state;
	
	  {
	     GList *bitnames = NULL;
	     
	     for (l = bits->bits; l; l = l->next)
	       {
		  Ebits_Object_Bit_State state;
		  gchar *text;
		  
		  state = l->data;
		  text = state->description->name;
		  if (!text) text = "";
		  bitnames = g_list_append(bitnames, text);
	       }
	     w = gtk_object_get_data(GTK_OBJECT(main_win), "tl_rel_combo");
	     gtk_combo_set_popdown_strings(GTK_COMBO(w), bitnames);
	     w = gtk_object_get_data(GTK_OBJECT(main_win), "br_rel_combo");
	     gtk_combo_set_popdown_strings(GTK_COMBO(w), bitnames);
	     if (bitnames) g_list_free(bitnames);
	  }
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
	gtk_widget_set_sensitive(w, 1);
	SET_SPIN("tl_abs_h", rel1.x);
	SET_SPIN("tl_abs_v", rel1.y);
	SET_ENTRY("tl_rel", rel1.name);
	SET_SPIN("tl_rel_h", rel1.rx);
	SET_SPIN("tl_rel_v", rel1.ry);
	SET_SPIN("tl_adj_h", rel1.ax);
	SET_SPIN("tl_adj_v", rel1.ay);
	
	SET_SPIN("br_abs_h", rel2.x);
	SET_SPIN("br_abs_v", rel2.y);
	SET_ENTRY("br_rel", rel2.name);
	SET_SPIN("br_rel_h", rel2.rx);
	SET_SPIN("br_rel_v", rel2.ry);
	SET_SPIN("br_adj_h", rel2.ax);
	SET_SPIN("br_adj_v", rel2.ay);
	
	SET_SPIN("content_alignment_h", align.w);
	SET_SPIN("content_alignment_v", align.h);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "aspect");
	if ((selected_state->description->aspect.x > 0) &&
	    (selected_state->description->aspect.y > 0))
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);	   
	SET_SPIN("aspect_h", aspect.x);
	SET_SPIN("aspect_v", aspect.y);
	SET_SPIN("min_h", min.w);
	SET_SPIN("min_v", min.h);
	SET_SPIN("max_h", max.w);
	SET_SPIN("max_v", max.h);
	SET_SPIN("step_h", step.x);
	SET_SPIN("step_v", step.y);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_h");
	if (selected_state->description->max.w == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_v");
	if (selected_state->description->max.h == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");

	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	  {
	     Evas_List l, ll;
	     
	     for (l = selected_state->o->description->bits; l; l = l->next)
	       {
		  Ebits_Object_Bit_Description bit;
		  gint row;
		  gchar *text[1];
		  
		  bit = l->data;
		  if (bit->name) text[0] = bit->name;
		  else  text[0] = "";
		  row = gtk_clist_append(GTK_CLIST(w), text);
		  for (ll = selected_state->description->sync; ll; ll = ll->next)
		    {
		       char *name;
		       
		       name = ll->data;
		       if (!strcmp(text[0], name))
			  gtk_clist_select_row(GTK_CLIST(w), row, 0);
		    }
	       }
	  }
	gtk_clist_thaw(GTK_CLIST(w));
	
	SET_ENTRY("name", name);
	SET_ENTRY("class", class);
	SET_ENTRY("img_normal", normal.image);
	SET_ENTRY("img_hilited", hilited.image);
	SET_ENTRY("img_clicked", clicked.image);
	SET_ENTRY("img_disabled", disabled.image);
	SET_SPIN("border_l", border.l);
	SET_SPIN("border_r", border.r);
	SET_SPIN("border_t", border.t);
	SET_SPIN("border_b", border.b);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_h");
	if (selected_state->description->tile.w == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (selected_state->description->tile.w == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (selected_state->description->tile.w == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_v");
	if (selected_state->description->tile.h == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (selected_state->description->tile.h == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (selected_state->description->tile.h == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     gchar *text;
	     gint row;
	     
	     state = l->data;
	     text = state->description->name;
	     if (!text) text = "";
	     row = gtk_clist_append(GTK_CLIST(w), &text);
	     if (state == selected)
		gtk_clist_select_row(GTK_CLIST(w), row, 0);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(main_win), "properties");
	gtk_widget_set_sensitive(w, 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_unselect_all(GTK_CLIST(w));	
     }
   if (bits)
     {
       SET_ALL_SPIN("bit_min_h", min.w);
       SET_ALL_SPIN("bit_min_v", min.h);
       SET_ALL_SPIN("bit_max_h", max.w);
       SET_ALL_SPIN("bit_max_v", max.h);
       SET_ALL_SPIN("bit_pad_l", padding.l);
       SET_ALL_SPIN("bit_pad_r", padding.r);
       SET_ALL_SPIN("bit_pad_t", padding.t);
       SET_ALL_SPIN("bit_pad_b", padding.b);
       SET_ALL_SPIN("bit_inset_l", inset.l);
       SET_ALL_SPIN("bit_inset_r", inset.r);
       SET_ALL_SPIN("bit_inset_t", inset.t);
       SET_ALL_SPIN("bit_inset_b", inset.b);
       SET_ALL_SPIN("bit_step_h", step.x);
       SET_ALL_SPIN("bit_step_v", step.y);
     }
}

static void
update_widget_from_simple_selection(void)
{
   GtkWidget *w;
   Evas_List l;

   if (selected_state)
     {
	Ebits_Object_Bit_State selected;
	
	selected = selected_state;
	
	SET_SPIN("tl_abs_h", rel1.x);
	SET_SPIN("tl_abs_v", rel1.y);
	SET_ENTRY("tl_rel", rel1.name);
	SET_SPIN("tl_rel_h", rel1.rx);
	SET_SPIN("tl_rel_v", rel1.ry);
	SET_SPIN("tl_adj_h", rel1.ax);
	SET_SPIN("tl_adj_v", rel1.ay);
	
	SET_SPIN("br_abs_h", rel2.x);
	SET_SPIN("br_abs_v", rel2.y);
	SET_ENTRY("br_rel", rel2.name);
	SET_SPIN("br_rel_h", rel2.rx);
	SET_SPIN("br_rel_v", rel2.ry);
	SET_SPIN("br_adj_h", rel2.ax);
	SET_SPIN("br_adj_v", rel2.ay);
	
	SET_SPIN("content_alignment_h", align.w);
	SET_SPIN("content_alignment_v", align.h);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "aspect");
	if ((selected_state->description->aspect.x > 0) &&
	    (selected_state->description->aspect.y > 0))
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);	   
	SET_SPIN("aspect_h", aspect.x);
	SET_SPIN("aspect_v", aspect.y);
	SET_SPIN("min_h", min.w);
	SET_SPIN("min_v", min.h);
	SET_SPIN("max_h", max.w);
	SET_SPIN("max_v", max.h);
	SET_SPIN("step_h", step.x);
	SET_SPIN("step_v", step.y);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_h");
	if (selected_state->description->max.w == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "use_contents_v");
	if (selected_state->description->max.h == 0)
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
	else
	   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "sync_list");

	SET_ENTRY("name", name);
	SET_ENTRY("class", class);
	SET_ENTRY("img_normal", normal.image);
	SET_ENTRY("img_hilited", hilited.image);
	SET_ENTRY("img_clicked", clicked.image);
	SET_ENTRY("img_disabled", disabled.image);
	SET_SPIN("border_l", border.l);
	SET_SPIN("border_r", border.r);
	SET_SPIN("border_t", border.t);
	SET_SPIN("border_b", border.b);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_h");
	if (selected_state->description->tile.w == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (selected_state->description->tile.w == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (selected_state->description->tile.w == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");
	w = gtk_object_get_data(GTK_OBJECT(main_win), "tile_v");
	if (selected_state->description->tile.h == 0)
	   gtk_entry_set_text(GTK_ENTRY(w), "Fill");
	else if (selected_state->description->tile.h == 1)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile");
	else if (selected_state->description->tile.h == 2)
	   gtk_entry_set_text(GTK_ENTRY(w), "Tile Integer");
	
     }
   if (bits)
     {
       SET_ALL_SPIN("bit_min_h", min.w);
       SET_ALL_SPIN("bit_min_v", min.h);
       SET_ALL_SPIN("bit_max_h", max.w);
       SET_ALL_SPIN("bit_max_v", max.h);
       SET_ALL_SPIN("bit_pad_l", padding.l);
       SET_ALL_SPIN("bit_pad_r", padding.r);
       SET_ALL_SPIN("bit_pad_t", padding.t);
       SET_ALL_SPIN("bit_pad_b", padding.b);
       SET_ALL_SPIN("bit_inset_l", inset.l);
       SET_ALL_SPIN("bit_inset_r", inset.r);
       SET_ALL_SPIN("bit_inset_t", inset.t);
       SET_ALL_SPIN("bit_inset_b", inset.b);
       SET_ALL_SPIN("bit_step_h", step.x);
       SET_ALL_SPIN("bit_step_v", step.y);
     }
}

static void
update_visible_selection(void)
{
   if (selected_state)
     {
	int w, h;
	
	evas_move(view_evas, o_select_rect, 
		  selected_state->o->state.x + selected_state->x, 
		  selected_state->o->state.y + selected_state->y);
	evas_resize(view_evas, o_select_rect, selected_state->w, selected_state->h);
	evas_set_line_xy(view_evas, o_select_line1,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y + selected_state->h - 1);
	evas_set_line_xy(view_evas, o_select_line2,
			 selected_state->o->state.x + selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x + selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y + selected_state->h - 1);
	evas_set_line_xy(view_evas, o_select_line3,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y,
			 selected_state->o->state.x + selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y);
	evas_set_line_xy(view_evas, o_select_line4,
			 selected_state->o->state.x + selected_state->x,
			 selected_state->o->state.y + selected_state->y + selected_state->h - 1,
			 selected_state->o->state.x + selected_state->x + selected_state->w - 1,
			 selected_state->o->state.y + selected_state->y + selected_state->h - 1);
	evas_get_image_size(view_evas, o_select_abs1, &w, &h);
	evas_move(view_evas, o_select_abs1, 
		  selected_state->o->state.x + selected_state->x - w, 
		  selected_state->o->state.y + selected_state->y - h);
	evas_get_image_size(view_evas, o_select_rel1, &w, &h);
	evas_move(view_evas, o_select_rel1, 
		  selected_state->o->state.x + selected_state->x - w, 
		  selected_state->o->state.y + selected_state->y);
	evas_get_image_size(view_evas, o_select_adj1, &w, &h);
	evas_move(view_evas, o_select_adj1, 
		  selected_state->o->state.x + selected_state->x, 
		  selected_state->o->state.y + selected_state->y - h);
	
	evas_get_image_size(view_evas, o_select_abs2, &w, &h);
	evas_move(view_evas, o_select_abs2, 
		  selected_state->o->state.x + selected_state->x + selected_state->w, 
		  selected_state->o->state.y + selected_state->y + selected_state->h);
	evas_get_image_size(view_evas, o_select_rel2, &w, &h);
	evas_move(view_evas, o_select_rel2, 
		  selected_state->o->state.x + selected_state->x + selected_state->w, 
		  selected_state->o->state.y + selected_state->y + selected_state->h - h);
	evas_get_image_size(view_evas, o_select_adj2, &w, &h);
	evas_move(view_evas, o_select_adj2, 
		  selected_state->o->state.x + selected_state->x + selected_state->w - w, 
		  selected_state->o->state.y + selected_state->y + selected_state->h);
	evas_show(view_evas, o_select_rect);
	evas_show(view_evas, o_select_line1);
	evas_show(view_evas, o_select_line2);
	evas_show(view_evas, o_select_line3);
	evas_show(view_evas, o_select_line4);
	evas_show(view_evas, o_select_abs1);
	evas_show(view_evas, o_select_rel1);
	evas_show(view_evas, o_select_adj1);
	evas_show(view_evas, o_select_abs2);
	evas_show(view_evas, o_select_rel2);
	evas_show(view_evas, o_select_adj2);
     }
   else
     {
	evas_hide(view_evas, o_select_rect);
	evas_hide(view_evas, o_select_line1);
	evas_hide(view_evas, o_select_line2);
	evas_hide(view_evas, o_select_line3);
	evas_hide(view_evas, o_select_line4);
	evas_hide(view_evas, o_select_abs1);
	evas_hide(view_evas, o_select_rel1);
	evas_hide(view_evas, o_select_adj1);
	evas_hide(view_evas, o_select_abs2);
	evas_hide(view_evas, o_select_rel2);
	evas_hide(view_evas, o_select_adj2);
     }
}

gint
view_redraw(gpointer data)
{
   evas_render(view_evas);
   zoom_redraw(zoom_x, zoom_y);
   current_idle = 0;
   return FALSE;
}

void
zoom_redraw(int xx, int yy)
{
   GtkWidget *zoom, *view;
   GdkPixmap *pmap;
   GdkGC *gc;
   int x, y;
   
   view = gtk_object_get_data(GTK_OBJECT(main_win), "view");
   zoom = gtk_object_get_data(GTK_OBJECT(main_win), "zoom");
   gc = gdk_gc_new(view->window);
   if (zoom_method == 0)
      pmap = gdk_pixmap_new(view->window, zoom->allocation.width, 
			    zoom->allocation.height, -1);
   else
      pmap = zoom->window;
   gdk_draw_rectangle(pmap, gc, 1, 0, 0, zoom->allocation.width, zoom->allocation.height);
   for (y = 0; y < (zoom->allocation.height + 1) / zoom_scale; y++)
     {
	int i;
	
	for (i = 0; i < zoom_scale; i++)
	  {
	     gdk_window_copy_area(pmap, gc, 
				  0, (y * zoom_scale) + i, 
				  view->window,
				  xx - ((zoom->allocation.width + 1) / (2*zoom_scale)),
				  yy - ((zoom->allocation.height + 1) / (2*zoom_scale)) + y,
				  (zoom->allocation.width + 1) / zoom_scale,
				  1);
	  }
     }
   for (x = ((zoom->allocation.width + 1) / zoom_scale) - 1; x >= 0; x--)
     {
	int i;
	
	for (i = zoom_scale-1; i >= 0; i--)
	  {
	     gdk_window_copy_area(pmap, gc, 
				  (x * zoom_scale) + i, 0, 
				  pmap,
				  x, 0,
				  1,
				  zoom->allocation.height);
	  }
     }
   if (zoom_method == 0)
     {
	gdk_window_copy_area(zoom->window, gc, 
			     0, 0, 
			     pmap,
			     0, 0,
			     zoom->allocation.width,
			     zoom->allocation.height);
	gdk_pixmap_unref(pmap);
     }
   gdk_gc_destroy(gc);
}

static void
handle_bg_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   selected_state = NULL;
   update_widget_from_selection();
   update_visible_selection();
   QUEUE_DRAW;
}

static void
handle_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_put_data(_e, _o, "x", (void *)_x);
   evas_put_data(_e, _o, "y", (void *)_y);
   selected_state = NULL;
   update_widget_from_selection();
   update_visible_selection();
   QUEUE_DRAW;
}

static void
handle_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_remove_data(_e, _o, "clicked");
}

static void
handle_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   if (evas_get_data(_e, _o, "clicked"))
     {
	int x, y;
	
	x = (int)evas_get_data(_e, _o, "x");
	y = (int)evas_get_data(_e, _o, "y");
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
	if (_o == o_handle1)
	  {
	     backing_x += (_x - x);
	     backing_y += (_y - y);
	     backing_w += (x - _x);
	     backing_h += (y - _y);
	     if (backing_w < 1) 
		{
		   backing_x -= 1 - backing_w;
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_y -= 1 - backing_h;
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle2)
	  {
	     backing_y += (_y - y);
	     backing_w += (_x - x);
	     backing_h += (y - _y);
	     if (backing_w < 1) 
		{
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_y -= 1 - backing_h;
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle3)
	  {
	     backing_x += (_x - x);
	     backing_w += (x - _x);
	     backing_h += (_y - y);
	     if (backing_w < 1) 
		{
		   backing_x -= 1 - backing_w;
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_h = 1;
		}
	  }
	else if (_o == o_handle4)
	  {
	     backing_w += (_x - x);
	     backing_h += (_y - y);
	     if (backing_w < 1) 
		{
		   backing_w = 1;
		}
	     if (backing_h < 1) 
		{
		   backing_h = 1;
		}
	  }
	else if (_o == o_backing)
	  {
	     backing_x += (_x - x);
	     backing_y += (_y - y);
	  }
	view_configure_handles(NULL);
     }
}

static void
handle_bit_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   selected_state = state;
   if (draft_mode)
     {
	update_visible_selection();
	evas_put_data(_e, _o, "clicked", (void *)1);
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
     }
   update_widget_from_selection();
   QUEUE_DRAW;
}

static void
handle_bit_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   evas_remove_data(_e, _o, "clicked");
   QUEUE_DRAW;
}

static void
handle_bit_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;

   state = _data;
   if ((draft_mode) && (selected_state))
     {
	if (evas_get_data(_e, _o, "clicked"))
	  {
	     int x, y;
	     
	     x = (int)evas_get_data(_e, _o, "x");
	     y = (int)evas_get_data(_e, _o, "y");
	     evas_put_data(_e, _o, "x", (void *)_x);
	     evas_put_data(_e, _o, "y", (void *)_y);
	     
	     selected_state->description->rel1.x += (_x - x);
	     selected_state->description->rel1.y += (_y - y);
	     selected_state->description->rel2.x += (_x - x);
	     selected_state->description->rel2.y += (_y - y);
	     
	     ebits_move(bits, backing_x, backing_y);
	     ebits_resize(bits, backing_w + 10, backing_h + 10);
	     ebits_resize(bits, backing_w, backing_h);
	     update_widget_from_selection();
	     update_visible_selection();
	     ebits_set_state(bits, current_state);
	  }
     }
   QUEUE_DRAW;
}

static void
handle_adjuster_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   Ebits_Object_Bit_State state;

   evas_put_data(_e, _o, "clicked", (void *)1);
   evas_put_data(_e, _o, "x", (void *)_x);
   evas_put_data(_e, _o, "y", (void *)_y);
   QUEUE_DRAW;
}

static void
handle_adjuster_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   evas_remove_data(_e, _o, "clicked");
   QUEUE_DRAW;
}

static void
handle_adjuster_mouse_move (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
   if (evas_get_data(_e, _o, "clicked"))
     {
	int x, y, dx, dy;
	
	x = (int)evas_get_data(_e, _o, "x");
	y = (int)evas_get_data(_e, _o, "y");
	evas_put_data(_e, _o, "x", (void *)_x);
	evas_put_data(_e, _o, "y", (void *)_y);
	dx = (_x - x);
	dy = (_y - y);
	
	if (_o == o_select_abs1)
	  {
             selected_state->description->rel1.x += dx;
	     selected_state->description->rel1.y += dy;
	  }
	else if (_o == o_select_rel1)
	  {
	     Ebits_Object_Bit_State state;
	     int rw, rh;
	     double ix, iy;
	     
	     state = NULL;
	     if (selected_state->description->rel1.name)
		state = _ebits_get_bit_name(bits, selected_state->description->rel1.name);
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
	     if (rw > 0) ix = (double)dx / (double)rw;
	     if (rh > 0) iy = (double)dy / (double)rh;
	     selected_state->description->rel1.rx += ix;
	     selected_state->description->rel1.ry += iy;
	  }
	else if (_o == o_select_adj1)
	  {
	     selected_state->description->rel1.ax += dx;
	     selected_state->description->rel1.ay += dy;
	  }
	else if (_o == o_select_abs2)
	  {
             selected_state->description->rel2.x += dx;
	     selected_state->description->rel2.y += dy;
	  }
	else if (_o == o_select_rel2)
	  {
	     Ebits_Object_Bit_State state;
	     int rw, rh;
	     double ix, iy;
	     
	     state = NULL;
	     if (selected_state->description->rel2.name)
		state = _ebits_get_bit_name(bits, selected_state->description->rel2.name);
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
	     if (rw > 0) ix = (double)dx / (double)rw;
	     if (rh > 0) iy = (double)dy / (double)rh;
	     selected_state->description->rel2.rx += ix;
	     selected_state->description->rel2.ry += iy;
	  }
	else if (_o == o_select_adj2)
	  {
	     selected_state->description->rel2.ax += dx;
	     selected_state->description->rel2.ay += dy;
	  }
	     
	ebits_move(bits, backing_x, backing_y);
	ebits_resize(bits, backing_w + 10, backing_h + 10);
	ebits_resize(bits, backing_w, backing_h);
	update_widget_from_simple_selection();
	update_visible_selection();
	ebits_set_state(bits, current_state);
     }
   QUEUE_DRAW;
}

gint
view_configure_handles(gpointer data)
{
   int w, h;
   GtkWidget *entry;
   gchar buf[32];

   if (!bits) return 0;
   if (!o_handle1) return 0;
   
   ebits_move(bits, backing_x, backing_y);
   ebits_resize(bits, backing_w, backing_h);
   
   update_visible_selection();
   
   backing_x = bits->state.x;
   backing_y = bits->state.y;
   backing_w = bits->state.w;
   backing_h = bits->state.h;
   
   evas_get_image_size(view_evas, o_handle1, &w, &h);
   evas_move(view_evas, o_handle1, backing_x - w, backing_y - h);

   evas_get_image_size(view_evas, o_handle2, &w, &h);
   evas_move(view_evas, o_handle2, backing_x + backing_w, backing_y - h);
   
   evas_get_image_size(view_evas, o_handle3, &w, &h);
   evas_move(view_evas, o_handle3, backing_x - w, backing_y + backing_h);
   
   evas_get_image_size(view_evas, o_handle4, &w, &h);
   evas_move(view_evas, o_handle4, backing_x + backing_w, backing_y + backing_h);

   evas_move(view_evas, o_backing, backing_x - 1, backing_y  - 1);
   evas_resize(view_evas, o_backing, backing_w + 2, backing_h + 2);
   
   evas_set_line_xy(view_evas, o_edge1, 
		    backing_x - 1, backing_y - 1, 
		    backing_x - 1, backing_y + backing_h);
   evas_set_line_xy(view_evas, o_edge2, 
		    backing_x - 1, backing_y  - 1, 
		    backing_x + backing_w, backing_y - 1);
   evas_set_line_xy(view_evas, o_edge3, 
		    backing_x - 1, backing_y + backing_h, 
		    backing_x + backing_w, backing_y + backing_h);
   evas_set_line_xy(view_evas, o_edge4, 
		    backing_x + backing_w, backing_y  - 1, 
		    backing_x + backing_w, backing_y + backing_h);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "x");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_x);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "y");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_y);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "width");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_w);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "height");
   g_snprintf(buf, sizeof(buf), "%3.0f", backing_h);
   gtk_entry_set_text(GTK_ENTRY(entry), buf);
   
   QUEUE_DRAW;
   return 0;
}

gint
view_create_handles(gpointer data)
{
   evas_show(view_evas, o_handle1 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle1.png"));
   evas_show(view_evas, o_handle2 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle2.png"));
   evas_show(view_evas, o_handle3 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle3.png"));
   evas_show(view_evas, o_handle4 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle4.png"));
   evas_show(view_evas, o_edge1 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge2 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge3 = evas_add_line(view_evas));
   evas_show(view_evas, o_edge4 = evas_add_line(view_evas));
   evas_show(view_evas, o_backing = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/backing.png"));
   evas_set_color(view_evas, o_edge1, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge2, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge3, 0, 0, 0, 255);
   evas_set_color(view_evas, o_edge4, 0, 0, 0, 255);
   evas_set_layer(view_evas, o_handle1, 5);
   evas_set_layer(view_evas, o_handle2, 5);
   evas_set_layer(view_evas, o_handle3, 5);
   evas_set_layer(view_evas, o_handle4, 5);
   evas_set_layer(view_evas, o_edge1, 4);
   evas_set_layer(view_evas, o_edge2, 4);
   evas_set_layer(view_evas, o_edge3, 4);
   evas_set_layer(view_evas, o_edge4, 4);
   evas_set_layer(view_evas, o_backing, 3);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle1, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle2, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle3, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_handle4, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_DOWN, handle_mouse_down, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_UP, handle_mouse_up, NULL);
   evas_callback_add(view_evas, o_backing, CALLBACK_MOUSE_MOVE, handle_mouse_move, NULL);

   view_configure_handles(NULL);
   return 0;
}

static gint
view_shrink_logo(gpointer data)
{
   double x, y, w, h, hh;
   
   if (!o_logo) return FALSE;
   evas_get_geometry(view_evas, o_logo, &x, &y, &w, &h);
   w -= 8;
   hh = h;
   h = h * (w / (w + 8));
   if ((w > 0) && (h > 0))
     {
	evas_move(view_evas, o_logo,
		  x + 4, y + ((hh - h) / 2));
	evas_resize(view_evas, o_logo, w, h);
	evas_set_image_fill(view_evas, o_logo, 0, 0, w, h);
     }
   if (w > 0)
     {
	QUEUE_DRAW;
	gtk_timeout_add(50, view_shrink_logo, NULL);
     }
   else
     {
	evas_del_object(view_evas, o_logo);
	o_logo = NULL;
	if (!o_handle1) view_create_handles(NULL);
	QUEUE_DRAW;
     }
   return FALSE;
}

static gint
view_fade_info(gpointer data)
{
   static double val;
   int alpha;
   
   if (new_fade)
     {
	val = 0.0;	
	new_fade = 0;
     }
   if (!o_info1) return FALSE;
   alpha = (int)(255 * (1.0 - val));
   evas_set_color(view_evas, o_info1, 255, 255, 255, alpha);
   evas_set_color(view_evas, o_info2, 255, 255, 255, alpha);
   evas_set_color(view_evas, o_info3, 255, 255, 255, alpha);
   evas_set_color(view_evas, o_info4, 255, 255, 255, alpha);

   if (val < 1.0)
     {
	val += 0.01;
	QUEUE_DRAW;
	gtk_timeout_add(50, view_fade_info, NULL);
     }
   else
     {
	evas_del_object(view_evas, o_info1);
	evas_del_object(view_evas, o_info2);
	evas_del_object(view_evas, o_info3);
	evas_del_object(view_evas, o_info4);
	o_info1 = NULL;
	o_info2 = NULL;
	o_info3 = NULL;
	o_info4 = NULL;
	QUEUE_DRAW;
     }
   return FALSE;
}

static gint
view_scroll_logo(gpointer data)
{
   double x, y, w, h;
   int eh;

   if (!o_logo) return FALSE;
   evas_get_geometry(view_evas, o_logo, &x, &y, &w, &h);
   evas_get_drawable_size(view_evas, NULL, &eh);
   evas_move(view_evas, o_logo,
	     x, y + ((((eh - h) / 2) -y) / 10) + 1);
   if (y < ((eh - h) / 2))
     {
	QUEUE_DRAW;
	gtk_timeout_add(50, view_scroll_logo, NULL);
     }
   else
     {
	QUEUE_DRAW;
	gtk_timeout_add(3000, view_shrink_logo, NULL);
     }
   return FALSE;
}

static gint
view_scroll_info(gpointer data)
{
   double x, y, w, h, hh;
   static double pos, val;
   int ew, eh;

   if (!o_info1)
     {
	val = 0;
	evas_font_add_path(view_evas, PACKAGE_DATA_DIR"/pixmaps");
	o_info1 = evas_add_text(view_evas, "nationff", 20, 
				"Copyright (C) The Rasterman 2000");
	o_info2 = evas_add_text(view_evas, "nationff", 20, 
				"Version 1.0");
	o_info3 = evas_add_text(view_evas, "nationff", 20, 
				"Enlightenment Graphical Ebit Editor");
	o_info4 = evas_add_text(view_evas, "nationff", 20, 
				"http://www.enlightenment.org");
	evas_set_color(view_evas, o_info1, 255, 255, 255, 255);
	evas_set_color(view_evas, o_info2, 255, 255, 255, 255);
	evas_set_color(view_evas, o_info3, 255, 255, 255, 255);
	evas_set_color(view_evas, o_info4, 255, 255, 255, 255);
	evas_set_layer(view_evas, o_info1, 900);
	evas_set_layer(view_evas, o_info2, 900);
	evas_set_layer(view_evas, o_info3, 900);
	evas_set_layer(view_evas, o_info4, 900);
	evas_show(view_evas, o_info1);
	evas_show(view_evas, o_info2);
	evas_show(view_evas, o_info3);
	evas_show(view_evas, o_info4);
     }
   pos = cos((1.0 - val) * (3.141592654 / 2));
   evas_get_drawable_size(view_evas, &ew, &eh);
   evas_get_geometry(view_evas, o_info1, &x, &y, &w, &h);
   evas_move(view_evas, o_info1, 
	     (pos * (((double)ew - w) / 2)) + ((1.0 - pos) * (-w)),
	     ((eh / 2) + 16));
   hh = h;
   evas_get_geometry(view_evas, o_info2, &x, &y, &w, &h);
   evas_move(view_evas, o_info2, 
	     (pos * (((double)ew - w) / 2)) + ((1.0 - pos) * (ew)),
	     ((eh / 2) + 16 + hh));
   hh += h;
   evas_get_geometry(view_evas, o_info3, &x, &y, &w, &h);
   evas_move(view_evas, o_info3, 
	     (ew - w) / 2,
	     (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (eh)));
   hh += h;
   evas_get_geometry(view_evas, o_info4, &x, &y, &w, &h);
   evas_move(view_evas, o_info4,
	     (ew - w) / 2,
	     (pos * ((eh / 2) + 16 + hh)) + ((1.0 - pos) * (-h)));
   hh += h;
	     
   if (val < 1.0)
     {
	val += 0.02;
	QUEUE_DRAW;
	gtk_timeout_add(50, view_scroll_info, NULL);
     }
   else
     {
	QUEUE_DRAW;
	new_fade = 1;
	gtk_timeout_add(1000, view_fade_info, NULL);
     }
   return FALSE;
}


void
on_file_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top, *w;
   Ebits_Object newbits = NULL;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   if (gtk_object_get_data(GTK_OBJECT(top), "open"))
     {
	newbits = ebits_load(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));

	if (!newbits)
	  {
	    return;
	  }

	selected_state = NULL;

	if (bits)
	  ebits_free(bits);

	bits = newbits;

	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_clear(GTK_CLIST(w));
	update_visible_selection();

	if (bits) 
	   {
	      Evas_List l;
	      
	      ebits_add_to_evas(bits, view_evas);
	      ebits_set_layer(bits, 5);
	      ebits_move(bits, backing_x, backing_y);
	      ebits_resize(bits, backing_w, backing_h);
	      ebits_show(bits);
	      for (l = bits->bits; l; l = l->next)
		{
		   Ebits_Object_Bit_State state;
		   
		   state = l->data;
		   evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_DOWN, handle_bit_mouse_down, state);
		   evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_UP, handle_bit_mouse_up, state);
		   evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_MOVE, handle_bit_mouse_move, state);
		}
 	      w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	      gtk_clist_freeze(GTK_CLIST(w));
	      gtk_clist_clear(GTK_CLIST(w));
	      for (l = bits->bits; l; l = l->next)
		{
		   Ebits_Object_Bit_State state2;
		   gchar *text;
		   gint row;
		   
		   state2 = l->data;
		   text = state2->description->name;
		   if (!text) text = "";
		   row = gtk_clist_append(GTK_CLIST(w), &text);
		}
	      gtk_clist_thaw(GTK_CLIST(w));
	      update_widget_from_selection();
	      w = gtk_object_get_data(GTK_OBJECT(main_win), "file");
	      gtk_entry_set_text(GTK_ENTRY(w), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	   }
	QUEUE_DRAW;
	E_DB_STR_SET(etcher_config, "/paths/bit", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "new_image"))
     {
	Ebits_Object_Bit_State state;
	Evas_List l;
	char buf[512];
	int num, exists;
	
	state = ebits_new_bit(bits, gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_DOWN, handle_bit_mouse_down, state);
	evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_UP, handle_bit_mouse_up, state);
	evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_MOVE, handle_bit_mouse_move, state);
	QUEUE_DRAW;
	
	exists = 1;
	num = 1;
	while (exists)
	  {
	     exists = 0;
	     g_snprintf(buf, sizeof(buf),"Image_%i", num);
	     for (l = bits->bits; l; l = l->next)
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
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state2;
	     gchar *text;
	     gint row;
	     
	     state2 = l->data;
	     text = state2->description->name;
	     if (!text) text = "";
	     row = gtk_clist_append(GTK_CLIST(w), &text);
	     if (state2 == state)
		gtk_clist_select_row(GTK_CLIST(w), row, 0);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
	E_DB_STR_SET(etcher_config, "/paths/image", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "normal_image"))
     {
	GtkWidget *w;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "img_normal");
	gtk_entry_set_text(GTK_ENTRY(w), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	E_DB_STR_SET(etcher_config, "/paths/image", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "hilited_image"))
     {
	GtkWidget *w;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "img_hilited");
	gtk_entry_set_text(GTK_ENTRY(w), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	E_DB_STR_SET(etcher_config, "/paths/image", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "clicked_image"))
     {
	GtkWidget *w;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "img_clicked");
	gtk_entry_set_text(GTK_ENTRY(w), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	E_DB_STR_SET(etcher_config, "/paths/image", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "disabled_image"))
     {
	GtkWidget *w;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "img_disabled");
	gtk_entry_set_text(GTK_ENTRY(w), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	E_DB_STR_SET(etcher_config, "/paths/image", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }
   else if (gtk_object_get_data(GTK_OBJECT(top), "save"))
     {
	char *file;
	GtkWidget *w;
	
	if (bits->description->file) free(bits->description->file);
	file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(top));
	bits->description->file = strdup(file);
	w = gtk_object_get_data(GTK_OBJECT(main_win), "file");
	gtk_entry_set_text(GTK_ENTRY(w), file);
	if (bits) ebits_save(bits, file);
	E_DB_STR_SET(etcher_config, "/paths/bit", 
		     gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	e_db_flush();
     }

   gtk_widget_destroy(top);
}


void
on_file_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   gtk_widget_destroy(top);
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *file;
   GtkWidget *entry;
   gchar *name;
   
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
   name = gtk_entry_get_text(GTK_ENTRY(entry));
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "open", (gpointer)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/bit", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/bits/");
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), name);
	  }
	e_db_flush();
     }
   gtk_widget_show(file);
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *entry;
   gchar *name;
   
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
   name = gtk_entry_get_text(GTK_ENTRY(entry));
   if (bits) ebits_save(bits, name);
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *file;
   GtkWidget *entry;
   gchar *name;
   
   entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
   name = gtk_entry_get_text(GTK_ENTRY(entry));
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "save", (gpointer)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/bit", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), getenv("HOME"));
	  }
	e_db_flush();
     }
   gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), name);
   gtk_widget_show(file);
}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *dialog, *w;
   
   dialog = create_preferences();
   
   if (render_method == 0)
     {
	w = gtk_object_get_data(GTK_OBJECT(dialog), "render1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(dialog), "render2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
     }
   if (zoom_method == 0)
     {
	w = gtk_object_get_data(GTK_OBJECT(dialog), "zoom1");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(dialog), "zoom2");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), 1);
     }
   gtk_widget_show(dialog);
}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   exit(0);
}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *w;
   
   if (selected_state)
     {
	Evas_List l;
	ebits_del_bit(bits, selected_state);
	selected_state = NULL;
	update_visible_selection();
	update_widget_from_selection();
	QUEUE_DRAW;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     gchar *text;
	     
	     state = l->data;
	     text = state->description->name;
	     if (!text) text = "";
	     gtk_clist_append(GTK_CLIST(w), &text);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
     }
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   int w, h, ew;
   
   if (o_logo)
     {
	evas_del_object(view_evas, o_logo);
	o_logo = NULL;
     }
   o_logo = evas_add_image_from_file(view_evas, 
				     PACKAGE_DATA_DIR"/pixmaps/etcher.png");
   evas_set_layer(view_evas, o_logo, 900);
   evas_show(view_evas, o_logo);
   evas_get_image_size(view_evas, o_logo, &w, &h);
   evas_get_drawable_size(view_evas, &ew, NULL);
   evas_move(view_evas, o_logo, 
	     (ew - w) / 2,
	     -h);
   gtk_timeout_add(50, view_scroll_logo, NULL);   
   gtk_timeout_add(50, view_scroll_info, NULL);   
}


gboolean
on_view_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
   zoom_x = event->x;
   zoom_y = event->y;
   if (o_pointer)
     {
	evas_move(view_evas, o_pointer, event->x, event->y);
     }
   evas_event_move(view_evas, event->x, event->y);
   QUEUE_DRAW;
   return FALSE;
}


gboolean
on_view_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   evas_event_button_down(view_evas, event->x, event->y, event->button);
   return FALSE;
}


gboolean
on_view_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   evas_event_button_up(view_evas, event->x, event->y, event->button);
   return FALSE;
}

gboolean
on_view_enter_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
   if (o_pointer)
      evas_show(view_evas, o_pointer);
   QUEUE_DRAW;
   return FALSE;
}


gboolean
on_view_leave_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
   if (o_pointer)
      evas_hide(view_evas, o_pointer);
   QUEUE_DRAW;
   return FALSE;
}


gboolean
on_view_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
   if (new_evas)
     {
	Evas_Object o_bg;
	int w, h;
	
	new_evas = 0;
	gdk_window_set_back_pixmap(widget->window, NULL, FALSE);
	evas_set_output(view_evas, 
			GDK_WINDOW_XDISPLAY(widget->window), 
			GDK_WINDOW_XWINDOW(widget->window), 
			GDK_VISUAL_XVISUAL(gtk_widget_get_visual(widget)), 
			GDK_COLORMAP_XCOLORMAP(gtk_widget_get_colormap(widget)));
	evas_set_output_size(view_evas, 
			     widget->allocation.width, 
			     widget->allocation.height);
	evas_set_output_viewport(view_evas, 
				 0, 0, 
				 widget->allocation.width, 
				 widget->allocation.height);
	
	evas_set_font_cache(view_evas, 1 * 1024 * 1024);
	evas_set_image_cache(view_evas, 8 * 1024 * 1024);
	o_bg = evas_add_image_from_file(view_evas,
					  PACKAGE_DATA_DIR"/pixmaps/tile.png");
	evas_callback_add(view_evas, o_bg, CALLBACK_MOUSE_DOWN, handle_bg_mouse_down, NULL);
	evas_get_image_size(view_evas, o_bg, &w, &h);
	evas_set_image_fill(view_evas, o_bg, 0, 0, w, h);
	evas_show(view_evas, o_bg);
	evas_move(view_evas, o_bg, 0, 0);
	evas_resize(view_evas, o_bg, 9999, 9999);
	
	if (!no_splash)
	  {
	    o_logo = evas_add_image_from_file(view_evas, 
					      PACKAGE_DATA_DIR"/pixmaps/etcher.png");
	    evas_set_layer(view_evas, o_logo, 900);
	    evas_show(view_evas, o_logo);
	    evas_get_image_size(view_evas, o_logo, &w, &h);
	    evas_move(view_evas, o_logo, 
		      (widget->allocation.width - w) / 2,
		      -h);
	  }

	backing_x = 32;
	backing_y = 32;
	backing_w = widget->allocation.width - 64;
	backing_h = widget->allocation.height - 64;

	o_pointer = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/pointer.png");
	evas_set_layer(view_evas, o_pointer, 999);
	evas_set_pass_events(view_evas, o_pointer, 1);
	
	bits = ebits_new();
	bits->description = ebits_new_description();
	bits->description->file = strdup(gtk_entry_get_text(GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(main_win), "file"))));

	ebits_add_to_evas(bits, view_evas);
	ebits_show(bits);
	ebits_set_layer(bits, 50);
	ebits_move(bits, backing_x, backing_y);
	ebits_resize(bits, backing_w, backing_h);
	
	o_select_rect = evas_add_rectangle(view_evas);
	o_select_line1 = evas_add_line(view_evas);
	o_select_line2 = evas_add_line(view_evas);
	o_select_line3 = evas_add_line(view_evas);
	o_select_line4 = evas_add_line(view_evas);
	o_select_abs1 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_abs.png");
	o_select_rel1 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_rel.png");
	o_select_adj1 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_adj.png");
	o_select_abs2 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_abs.png");
	o_select_rel2 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_rel.png");
	o_select_adj2 = evas_add_image_from_file(view_evas, PACKAGE_DATA_DIR"/pixmaps/handle_adj.png");
	evas_set_color(view_evas, o_select_rect, 255, 255, 255, 80);
	evas_set_color(view_evas, o_select_line1, 200, 50, 50, 200);
	evas_set_color(view_evas, o_select_line2, 200, 50, 50, 200);
	evas_set_color(view_evas, o_select_line3, 200, 50, 50, 200);
	evas_set_color(view_evas, o_select_line4, 200, 50, 50, 200);
	evas_set_layer(view_evas, o_select_rect, 100);
	evas_set_layer(view_evas, o_select_line1, 100);
	evas_set_layer(view_evas, o_select_line2, 100);
	evas_set_layer(view_evas, o_select_line3, 100);
	evas_set_layer(view_evas, o_select_line4, 100);
	evas_set_layer(view_evas, o_select_abs1, 100);
	evas_set_layer(view_evas, o_select_rel1, 100);
	evas_set_layer(view_evas, o_select_adj1, 100);
	evas_set_layer(view_evas, o_select_abs2, 100);
	evas_set_layer(view_evas, o_select_rel2, 100);
	evas_set_layer(view_evas, o_select_adj2, 100);
	evas_set_pass_events(view_evas, o_select_rect, 1);
	evas_set_pass_events(view_evas, o_select_line1, 1);
	evas_set_pass_events(view_evas, o_select_line2, 1);
	evas_set_pass_events(view_evas, o_select_line3, 1);
	evas_set_pass_events(view_evas, o_select_line4, 1);
	evas_callback_add(view_evas, o_select_abs1, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_abs1, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_abs1, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	evas_callback_add(view_evas, o_select_rel1, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_rel1, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_rel1, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	evas_callback_add(view_evas, o_select_adj1, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_adj1, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_adj1, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	evas_callback_add(view_evas, o_select_abs2, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_abs2, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_abs2, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	evas_callback_add(view_evas, o_select_rel2, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_rel2, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_rel2, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	evas_callback_add(view_evas, o_select_adj2, CALLBACK_MOUSE_DOWN, handle_adjuster_mouse_down, NULL);
	evas_callback_add(view_evas, o_select_adj2, CALLBACK_MOUSE_UP, handle_adjuster_mouse_up, NULL);
	evas_callback_add(view_evas, o_select_adj2, CALLBACK_MOUSE_MOVE, handle_adjuster_mouse_move, NULL);
	
	if (!no_splash)
	  {
	    gtk_timeout_add(50, view_scroll_logo, NULL);
	    gtk_timeout_add(50, view_scroll_info, NULL);   
	  }
	else
	  {
	     if (!o_handle1) view_create_handles(NULL);
	     if (load_file)
	       {
		  bits = ebits_load(load_file);
		  
		  if (bits)
		    {
		       Evas_List l;
		       GtkWidget *w;
		       
		       selected_state = NULL;
		       w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
		       gtk_clist_clear(GTK_CLIST(w));
		       update_visible_selection();
		       
		       if (bits) 
			 {
			    ebits_add_to_evas(bits, view_evas);
			    ebits_set_layer(bits, 5);
			    ebits_move(bits, backing_x, backing_y);
			    ebits_resize(bits, backing_w, backing_h);
			    ebits_show(bits);
			    for (l = bits->bits; l; l = l->next)
			      {
				 Ebits_Object_Bit_State state;
				 
				 state = l->data;
				 evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_DOWN, handle_bit_mouse_down, state);
				 evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_UP, handle_bit_mouse_up, state);
				 evas_callback_add(view_evas, state->object, CALLBACK_MOUSE_MOVE, handle_bit_mouse_move, state);
			      }
			    w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
			    gtk_clist_freeze(GTK_CLIST(w));
			    gtk_clist_clear(GTK_CLIST(w));
			    for (l = bits->bits; l; l = l->next)
			      {
				 Ebits_Object_Bit_State state2;
				 gchar *text;
				 gint row;
				 
				 state2 = l->data;
				 text = state2->description->name;
				 if (!text) text = "";
				 row = gtk_clist_append(GTK_CLIST(w), &text);
			      }
			    gtk_clist_thaw(GTK_CLIST(w));
			    update_widget_from_selection();
			    w = gtk_object_get_data(GTK_OBJECT(main_win), "file");
			    gtk_entry_set_text(GTK_ENTRY(w), load_file);
			 }
		       E_DB_STR_SET(etcher_config, "/paths/bit", load_file);
		       e_db_flush();
		    }
	       }
	     
	  }
	  {
	  GdkPixmap *src, *mask;
	  GdkColor fg, bg;
	  GdkGC *gc;
	  GdkCursor *cursor;
	  
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
   evas_update_rect(view_evas, 
		    event->area.x,
		    event->area.y,
		    event->area.width,
		    event->area.height);
   QUEUE_DRAW;
   
   return FALSE;
}


gboolean
on_view_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
   if (!widget->window) return FALSE;
   if (!view_evas) return FALSE;
   
   evas_set_output_size(view_evas, 
			widget->allocation.width, 
			widget->allocation.height);
   evas_set_output_viewport(view_evas, 
			    0, 0, 
			    widget->allocation.width, 
			    widget->allocation.height);
   evas_update_rect(view_evas, 
		    0, 0, 
		    widget->allocation.width,
		    widget->allocation.height);
   QUEUE_DRAW;
   return FALSE;
}


gboolean
on_zoom_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
   QUEUE_DRAW;
   return FALSE;
}


gboolean
on_zoom_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
   QUEUE_DRAW;
   return FALSE;
}


void
on_images_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   Evas_List l;
   int count;
   Ebits_Object_Bit_State selected;

   selected = selected_state;
   for (count = 0, l = bits->bits; l; l = l->next, count++)
     {
	if (count == row) selected_state = l->data;
     }
   if (selected != selected_state)
     {
	update_visible_selection();
	update_widget_from_selection();
	QUEUE_DRAW;
     }
}


void
on_states_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   if (bits)
     {
	current_state = row;
	ebits_set_state(bits, current_state);
	QUEUE_DRAW;
     }
}


void
on_new_image_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *file;
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "new_image", (void *)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/image", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
	e_db_flush();
     }
   gtk_widget_show(file);
}


void
on_new_icon_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
}


void
on_new_text_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
}


void
on_raise_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
   if (selected_state)
     {
	bits->bits = evas_list_remove(bits->bits, selected_state);
	bits->bits = evas_list_append(bits->bits, selected_state);
	bits->description->bits = evas_list_remove(bits->description->bits, selected_state->description);
	bits->description->bits = evas_list_append(bits->description->bits, selected_state->description);
	if (selected_state->object) 
	   evas_raise(selected_state->o->state.evas, selected_state->object);
	update_widget_from_selection();
	update_visible_selection();
	QUEUE_DRAW;
     }
}


void
on_lower_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
   if (selected_state)
     {
	bits->bits = evas_list_remove(bits->bits, selected_state);
	bits->bits = evas_list_prepend(bits->bits, selected_state);
	bits->description->bits = evas_list_remove(bits->description->bits, selected_state->description);
	bits->description->bits = evas_list_prepend(bits->description->bits, selected_state->description);
	if (selected_state->object) 
	   evas_lower(selected_state->o->state.evas, selected_state->object);
	update_widget_from_selection();
	update_visible_selection();
	QUEUE_DRAW;
     }
}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *w;
   
   if (selected_state)
     {
	Evas_List l;
	ebits_del_bit(bits, selected_state);
	selected_state = NULL;
	update_visible_selection();
	update_widget_from_selection();
	QUEUE_DRAW;
	
	w = gtk_object_get_data(GTK_OBJECT(main_win), "images");
	gtk_clist_freeze(GTK_CLIST(w));
	gtk_clist_clear(GTK_CLIST(w));
	for (l = bits->bits; l; l = l->next)
	  {
	     Ebits_Object_Bit_State state;
	     gchar *text;
	     
	     state = l->data;
	     text = state->description->name;
	     if (!text) text = "";
	     gtk_clist_append(GTK_CLIST(w), &text);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
     }
}


void
on_reset_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
   int ew, eh;
   
   evas_get_drawable_size(view_evas, &ew, &eh);
   backing_x = 32;
   backing_y = 32;
   backing_w = ew - 64;
   backing_h = eh - 64;
   view_configure_handles(NULL);
}


void
on_prop_apply_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   update_selection_from_widget();
   ebits_move(bits, backing_x, backing_y);
   ebits_resize(bits, backing_w + 10, backing_h + 10);
   ebits_resize(bits, backing_w, backing_h);
   update_visible_selection();
   ebits_set_state(bits, current_state);
   update_widget_from_selection();
   QUEUE_DRAW;
}


void
on_prop_reset_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   update_widget_from_selection();
}


void
on_draft_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   draft_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
   if (draft_mode)
     {
	evas_show(view_evas, o_handle1);
	evas_show(view_evas, o_handle2);
	evas_show(view_evas, o_handle3);
	evas_show(view_evas, o_handle4);
	evas_show(view_evas, o_edge1);
	evas_show(view_evas, o_edge2);
	evas_show(view_evas, o_edge3);
	evas_show(view_evas, o_edge4);
	evas_show(view_evas, o_backing);
	if (selected_state)
	  {
	     evas_show(view_evas, o_select_rect);
	     evas_show(view_evas, o_select_line1);
	     evas_show(view_evas, o_select_line2);
	     evas_show(view_evas, o_select_line3);
	     evas_show(view_evas, o_select_line4);
	     evas_show(view_evas, o_select_abs1);
	     evas_show(view_evas, o_select_rel1);
	     evas_show(view_evas, o_select_adj1);
	     evas_show(view_evas, o_select_abs2);
	     evas_show(view_evas, o_select_rel2);
	     evas_show(view_evas, o_select_adj2);
	  }
     }
   else
     {
	evas_hide(view_evas, o_handle1);
	evas_hide(view_evas, o_handle2);
	evas_hide(view_evas, o_handle3);
	evas_hide(view_evas, o_handle4);
	evas_hide(view_evas, o_edge1);
	evas_hide(view_evas, o_edge2);
	evas_hide(view_evas, o_edge3);
	evas_hide(view_evas, o_edge4);
	evas_hide(view_evas, o_backing);
	if (selected_state)
	  {
	     evas_hide(view_evas, o_select_rect);
	     evas_hide(view_evas, o_select_line1);
	     evas_hide(view_evas, o_select_line2);
	     evas_hide(view_evas, o_select_line3);
	     evas_hide(view_evas, o_select_line4);
	     evas_hide(view_evas, o_select_abs1);
	     evas_hide(view_evas, o_select_rel1);
	     evas_hide(view_evas, o_select_adj1);
	     evas_hide(view_evas, o_select_abs2);
	     evas_hide(view_evas, o_select_rel2);
	     evas_hide(view_evas, o_select_adj2);
	  }
     }
   QUEUE_DRAW;
}


gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  exit(0);
  return FALSE;
}


gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_software_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   render_method = 0;
   E_DB_INT_SET(etcher_config, "/display/render_method", (int)render_method);   
   e_db_flush();
   gtk_widget_destroy(top);
   gtk_main_quit();
}

void
on_3d_hardware_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   render_method = 1;
   E_DB_INT_SET(etcher_config, "/display/render_method", (int)render_method);
   e_db_flush();
   gtk_widget_destroy(top);
   gtk_main_quit();
}


void
on_browse_clicked_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *file;
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "clicked_image", (void *)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/image", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
	e_db_flush();
     }
   gtk_widget_show(file);
}


void
on_borwse_hilited_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *file;
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "hilited_image", (void *)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/image", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
	e_db_flush();
     }
   gtk_widget_show(file);

}


void
on_browse_normal_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *file;

   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "normal_image", (void *)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/image", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
	e_db_flush();
     }
   gtk_widget_show(file);

}


void
on_browse_disabled_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *file;
   
   file = create_filesel();
   gtk_object_set_data(GTK_OBJECT(file), "disabled_image", (void *)1);
     {
	char *dir = NULL;
	int ok = 0;
	
	E_DB_STR_GET(etcher_config, "/paths/image", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
	e_db_flush();
     }
   gtk_widget_show(file);
}


gboolean
on_preferences_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gtk_widget_destroy(widget);
   return FALSE;
}


void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top, *w;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "render1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      render_method = 0;
   else
      render_method = 1;
   w = gtk_object_get_data(GTK_OBJECT(top), "zoom1");
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      zoom_method = 0;
   else
      zoom_method = 1;
   
   E_DB_INT_SET(etcher_config, "/display/render_method", (int)render_method);
   E_DB_INT_SET(etcher_config, "/display/zoom_method", (int)zoom_method);
   e_db_flush();
   gtk_widget_destroy(top);
}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;

   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   gtk_widget_destroy(top);
}


void
on_zoomin_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  zoom_scale++;
  zoom_redraw(zoom_x, zoom_y);
}


void
on_zoomout_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  if (zoom_scale > 1)
    zoom_scale--;
  zoom_redraw(zoom_x, zoom_y);
}

