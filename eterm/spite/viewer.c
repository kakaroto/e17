/*
 *      SPITE v0.2 (C) 2000 Michael Lea (mazeone@dcaa.net)
 *
 *              SPITE Provides Independent Theming for Eterm.
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2, or (at your option)
 *      any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see the file COPYING); if not, write to the
 *      Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *      Boston, MA  02111-1307, USA
 *
 */

#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <pwd.h>
#include "eterm.h"
#include "action.h"

#define BUFFER_LEN 1024

GtkWidget *window = NULL, *bigtable, *pagetable, *notebook, *frame, *label, *pagebox, *frametable, *subframetable, *button, *buttonbox;
GSList *group;
gchar *clist_txt[1] = {"Images"};

int
main(int argc, char *argv[])
{
  gtk_init (&argc, &argv);
  create_contexts();
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GTK_WIDGET_SET_FLAGS(window, GTK_CAN_FOCUS);
  GTK_WIDGET_SET_FLAGS(window, GTK_CAN_DEFAULT);
  gtk_window_set_policy(GTK_WINDOW (window), TRUE, TRUE, FALSE);
  gtk_window_set_title(GTK_WINDOW (window), "spite-0.2");
  gtk_window_set_wmclass(GTK_WINDOW (window), "spite", "spite");
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC (on_exit_application), NULL);
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (on_exit_application), NULL);

  /* Creating a new notebook */
  bigtable = gtk_table_new(3,6,FALSE);
  gtk_container_add (GTK_CONTAINER (window), bigtable);
  gtk_widget_show(bigtable);
  notebook = gtk_notebook_new ();
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
  gtk_table_attach_defaults(GTK_TABLE(bigtable), notebook, 0,6,0,1);
  gtk_widget_show(notebook);
  /* The frame is where all the crap gets drawn*/
  /* COLORS PAGE */
  frame=gtk_frame_new(NULL);
  gtk_widget_show(frame);
  label=gtk_label_new("Colors");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame,label);
  gtk_widget_show(label);
  
  pagetable = gtk_table_new(6, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), pagetable);
  gtk_widget_show(pagetable);
  
  label = gtk_label_new("foreground color:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 0, 1);
  col_fore_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(col_fore_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), col_fore_entry, 1, 2, 0, 1);
  gtk_signal_connect(GTK_OBJECT(col_fore_entry), "changed",
			   GTK_SIGNAL_FUNC(col_entry_changed), col_fore_entry);

  label = gtk_label_new("background color:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 1, 2);
  col_back_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(col_back_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), col_back_entry, 1, 2, 1, 2);
  gtk_signal_connect_after(GTK_OBJECT(col_back_entry), "changed", 
			   GTK_SIGNAL_FUNC(col_entry_changed), "back");

  label = gtk_label_new("cursor color:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 2, 3);
  col_curs_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(col_curs_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), col_curs_entry, 1, 2, 2, 3);
  gtk_signal_connect_after(GTK_OBJECT(col_curs_entry), "changed",
			   GTK_SIGNAL_FUNC(col_entry_changed), "curs");

  label = gtk_label_new("cursor text color:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 3, 4);
  col_curstext_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(col_curstext_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), col_curstext_entry, 1, 2, 3, 4);
  gtk_signal_connect_after(GTK_OBJECT(col_curstext_entry), "changed", 
			   GTK_SIGNAL_FUNC(col_entry_changed), "curstext");

  label = gtk_label_new("pointer color:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 4, 5);
  col_point_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(col_point_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), col_point_entry, 1, 2, 4, 5);
  gtk_signal_connect_after(GTK_OBJECT(col_point_entry), "changed",
			   GTK_SIGNAL_FUNC(col_entry_changed), "point");


  col_vid_btn = gtk_radio_button_new_with_label (NULL, "video normal");
  gtk_widget_show(col_vid_btn);
  gtk_table_attach_defaults(GTK_TABLE (pagetable), col_vid_btn, 0, 1, 5, 6);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (col_vid_btn), TRUE);
  group = gtk_radio_button_group (GTK_RADIO_BUTTON (col_vid_btn));
  button = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (col_vid_btn)), "video reversed");
  gtk_widget_show(button);
  gtk_table_attach_defaults(GTK_TABLE (pagetable), button, 1, 2, 5, 6);

  /* ATTRIBUTES PAGE */
  frame = gtk_frame_new(NULL);
  gtk_widget_show(frame);
  label = gtk_label_new("X Attributes");
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);
  gtk_widget_show(label);

  pagebox = gtk_vbox_new(FALSE, 2);
  gtk_container_add(GTK_CONTAINER (frame), pagebox);
  gtk_widget_show(pagebox);

   frame = gtk_frame_new("Geometry");
   gtk_widget_show(frame);
   gtk_box_pack_start(GTK_BOX (pagebox), frame, FALSE, FALSE, 0);
   frametable = gtk_table_new(4, 2, FALSE);
   gtk_container_add(GTK_CONTAINER (frame), frametable);
   label = gtk_label_new("width:");
   gtk_widget_show(label);
   gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 0, 1);
   atr_width_entry = gtk_entry_new_with_max_length(50);
   gtk_signal_connect(GTK_OBJECT (atr_width_entry), "changed",
		      GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
   gtk_table_attach_defaults(GTK_TABLE (frametable), atr_width_entry, 1, 2, 0, 1);
   gtk_widget_show(atr_width_entry);
    
   label = gtk_label_new("height:");
   gtk_widget_show(label);
   gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 1, 2);
   atr_height_entry = gtk_entry_new_with_max_length(50);
   gtk_signal_connect(GTK_OBJECT (atr_height_entry), "changed",
		      GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
   gtk_table_attach_defaults(GTK_TABLE (frametable), atr_height_entry, 1, 2, 1, 2);
   gtk_widget_show(atr_height_entry);

   label=gtk_label_new("offset X:");
   gtk_widget_show(label);
   gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0,1,2,3);
   atr_offX_entry=gtk_entry_new_with_max_length(50);
   gtk_signal_connect(GTK_OBJECT(atr_offX_entry), "changed",
                       GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
   gtk_table_attach_defaults(GTK_TABLE(frametable), atr_offX_entry, 1,2,2,3);
   gtk_widget_show(atr_offX_entry);
    
   label=gtk_label_new("offset Y:");
   gtk_widget_show(label);
   gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0,1,3,4);
   atr_offY_entry=gtk_entry_new_with_max_length(50);
   gtk_signal_connect(GTK_OBJECT(atr_offY_entry), "changed",
		      GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
   gtk_table_attach_defaults(GTK_TABLE(frametable), atr_offY_entry, 1,2,3,4);
   gtk_widget_show(atr_offY_entry);
   gtk_widget_show(frametable);

  frame=gtk_frame_new("Names");
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX(pagebox), frame, TRUE, TRUE,0);
  frametable = gtk_table_new(3, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), frametable);
  gtk_widget_show(frametable);
  label=gtk_label_new("title:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0, 1, 0, 1);
  atr_title_entry=gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT(atr_title_entry), "changed",
		     GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE(frametable), atr_title_entry, 1, 2, 0, 1);
  gtk_widget_show(atr_title_entry);
  
  label = gtk_label_new("name:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0, 1, 1, 2);       
  atr_name_entry=gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT(atr_name_entry), "changed",
		     GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE(frametable), atr_name_entry, 1, 2, 1, 2);
  gtk_widget_show(atr_name_entry);
  
  label = gtk_label_new("iconname:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0, 1, 2, 3);    
  atr_iconname_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT(atr_iconname_entry), "changed",
		     GTK_SIGNAL_FUNC(atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE(frametable), atr_iconname_entry, 1, 2, 2, 3);
  gtk_widget_show(atr_iconname_entry);

  frame=gtk_frame_new("Fonts");
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX(pagebox), frame, TRUE, TRUE,0);
  frametable = gtk_table_new(8, 3, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), frametable);
  gtk_widget_show(frametable);

  label = gtk_label_new("font 0:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 0, 1);
  atr_font0_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font0_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font0_entry, 1, 2, 0, 1);
  gtk_widget_show(atr_font0_entry);

  atr_f0_def_btn = gtk_radio_button_new_with_label (NULL, "default");
  gtk_widget_show(atr_f0_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f0_def_btn, 2, 3, 0, 1);
  group = gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f0_def_btn));

  label = gtk_label_new("font 1:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 1, 2);
  atr_font1_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font1_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font1_entry, 1, 2, 1, 2);
  gtk_widget_show(atr_font1_entry);

  atr_f1_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f0_def_btn)), "default");
  gtk_widget_show(atr_f1_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f1_def_btn, 2, 3, 1, 2);

  label = gtk_label_new("font 2:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 2, 3);
  atr_font2_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font2_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font2_entry, 1, 2, 2, 3);
  gtk_widget_show(atr_font2_entry);

  atr_f2_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f1_def_btn)), "default");
  gtk_widget_show(atr_f2_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f2_def_btn, 2, 3, 2, 3);

  label = gtk_label_new("font 3:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 3, 4);
  atr_font3_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font3_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font3_entry, 1, 2, 3, 4);
  gtk_widget_show(atr_font3_entry);

  atr_f3_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f1_def_btn)), "default");
  gtk_widget_show(atr_f3_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f3_def_btn, 2, 3, 3, 4);

  label = gtk_label_new("font 4:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 4, 5);
  atr_font4_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font4_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font4_entry, 1, 2, 4, 5);
  gtk_widget_show(atr_font4_entry);

  atr_f4_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f1_def_btn)), "default");
  gtk_widget_show(atr_f4_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f4_def_btn, 2, 3, 4, 5);

  label = gtk_label_new("font 5:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 5, 6);
  atr_font5_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font5_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font5_entry, 1, 2, 5, 6);
  gtk_widget_show(atr_font5_entry);

  atr_f5_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f1_def_btn)), "default");
  gtk_widget_show(atr_f5_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f5_def_btn, 2, 3, 5, 6);

  label = gtk_label_new("font 6:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 6, 7);
  atr_font6_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font6_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font6_entry, 1, 2, 6, 7);
  gtk_widget_show(atr_font6_entry);

  atr_f6_def_btn = gtk_radio_button_new_with_label (gtk_radio_button_group (GTK_RADIO_BUTTON (atr_f1_def_btn)), "default");
  gtk_widget_show(atr_f6_def_btn);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_f6_def_btn, 2, 3, 6, 7);

  label = gtk_label_new("bold font:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 7, 8);
  atr_font_bold_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect_after(GTK_OBJECT (atr_font_bold_entry), "changed",
			   GTK_SIGNAL_FUNC (atr_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), atr_font_bold_entry, 1, 2, 7, 8);
  gtk_widget_show(atr_font_bold_entry);

  /*TOGGLES PAGE*/
  frame = gtk_frame_new(NULL);
  gtk_widget_show(frame);
  label = gtk_label_new("Toggles");
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);
  gtk_widget_show(label);

  pagetable = gtk_table_new(12, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), pagetable);
  gtk_widget_show(pagetable);
  tog_map_alert = gtk_check_button_new_with_label ("map alert");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_map_alert, 0, 1, 0, 1); 
  gtk_widget_show (tog_map_alert); 
  tog_visual_bell = gtk_check_button_new_with_label ("visual bell");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_visual_bell, 1, 2, 0, 1); 
  gtk_widget_show (tog_visual_bell); 
  tog_login_shell = gtk_check_button_new_with_label ("login shell");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_login_shell, 0, 1, 1, 2); 
  gtk_widget_show (tog_login_shell); 
  tog_scrollbar = gtk_check_button_new_with_label ("scrollbar");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_scrollbar, 1, 2, 1, 2); 
  gtk_widget_show (tog_scrollbar); 
  tog_utmp_logging = gtk_check_button_new_with_label ("utmp logging");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_utmp_logging, 0, 1, 2, 3); 
  gtk_widget_show (tog_utmp_logging); 
  tog_select_line = gtk_check_button_new_with_label ("select line");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_select_line, 1, 2, 2, 3); 
  gtk_widget_show (tog_select_line); 
  tog_meta8 = gtk_check_button_new_with_label ("meta8");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_meta8, 0, 1, 3, 4); 
  gtk_widget_show (tog_meta8); 
  tog_iconic = gtk_check_button_new_with_label ("iconic");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_iconic, 1, 2, 3, 4); 
  gtk_widget_show (tog_iconic); 
  tog_home_on_output = gtk_check_button_new_with_label ("home on output");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_home_on_output, 0, 1, 4, 5); 
  gtk_widget_show (tog_home_on_output); 
  tog_home_on_input = gtk_check_button_new_with_label ("home on input");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_home_on_input, 1, 2, 4, 5); 
  gtk_widget_show (tog_home_on_input); 
  tog_scrollbar_right = gtk_check_button_new_with_label ("scrollbar right");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_scrollbar_right, 0, 1, 5, 6); 
  gtk_widget_show (tog_scrollbar_right); 
  tog_scrollbar_floating = gtk_check_button_new_with_label ("scrollbar floating");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_scrollbar_floating, 1, 2, 5, 6); 
  gtk_widget_show (tog_scrollbar_floating); 
  tog_borderless = gtk_check_button_new_with_label ("borderless");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_borderless, 0, 1, 6, 7); 
  gtk_widget_show (tog_borderless); 
  tog_double_buffer = gtk_check_button_new_with_label ("double buffering");   
  gtk_table_attach_defaults(GTK_TABLE(pagetable), tog_double_buffer, 1, 2, 6, 7); 
  gtk_widget_show (tog_double_buffer); 

  /* MISC PAGE */
  frame=gtk_frame_new(NULL);
  gtk_widget_show(frame);
  label=gtk_label_new("Misc");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame,label);
  gtk_widget_show(label);
  
  pagetable = gtk_table_new(7, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), pagetable);
  gtk_widget_show(pagetable);
  
  label = gtk_label_new("print pipe:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 0, 1);
  misc_print_pipe_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_print_pipe_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_print_pipe_entry, 1, 2, 0, 1);
  gtk_signal_connect_after(GTK_OBJECT(misc_print_pipe_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("save lines:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 1, 2);
  misc_save_lines_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_save_lines_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_save_lines_entry, 1, 2, 1, 2);
  gtk_signal_connect_after(GTK_OBJECT(misc_save_lines_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("cut chars:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 2, 3);
  misc_cut_chars_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_cut_chars_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_cut_chars_entry, 1, 2, 2, 3);
  gtk_signal_connect_after(GTK_OBJECT(misc_cut_chars_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("border width:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 3, 4);
  misc_border_width_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_border_width_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_border_width_entry, 1, 2, 3, 4);
  gtk_signal_connect_after(GTK_OBJECT(misc_border_width_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("line space:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 4, 5);
  misc_line_space_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_line_space_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_line_space_entry, 1, 2, 4, 5);
  gtk_signal_connect_after(GTK_OBJECT(misc_line_space_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("term name:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 5, 6);
  misc_term_name_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_term_name_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_term_name_entry, 1, 2, 5, 6);
  gtk_signal_connect_after(GTK_OBJECT(misc_term_name_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  label = gtk_label_new("exec:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), label, 0, 1, 6, 7);
  misc_exec_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(misc_exec_entry);
  gtk_table_attach_defaults(GTK_TABLE(pagetable), misc_exec_entry, 1, 2, 6, 7);
  gtk_signal_connect_after(GTK_OBJECT(misc_exec_entry), "changed",
			   GTK_SIGNAL_FUNC(misc_entry_changed), NULL);

  /* IMAGES PAGE */
  frame=gtk_frame_new(NULL);
  gtk_widget_show(frame);
  label=gtk_label_new("Images");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);
  gtk_widget_show(label);
  
  pagebox = gtk_vbox_new(FALSE, 2);
  gtk_container_add(GTK_CONTAINER (frame), pagebox);
  gtk_widget_show(pagebox);

  frame = gtk_frame_new("Global");
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX (pagebox), frame, FALSE, FALSE, 0);

  frametable = gtk_table_new(3, 2, FALSE);
  gtk_container_add(GTK_CONTAINER (frame), frametable);
  gtk_widget_show(frametable);
  label = gtk_label_new("path:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 0, 1);
  im_path_entry = gtk_entry_new_with_max_length(100);
  gtk_signal_connect(GTK_OBJECT (im_path_entry), "changed",
                     GTK_SIGNAL_FUNC (im_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_path_entry, 1, 2, 0, 1);
  gtk_widget_show(im_path_entry);

  label = gtk_label_new("icon:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 1, 2);
  im_icon_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect(GTK_OBJECT (im_icon_entry), "changed",
                     GTK_SIGNAL_FUNC (im_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_icon_entry, 1, 2, 1, 2);
  gtk_widget_show(im_icon_entry);

  label = gtk_label_new("anim files:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 2, 3);
  im_anim_entry = gtk_entry_new_with_max_length(50);
  gtk_signal_connect(GTK_OBJECT (im_anim_entry), "changed",
                     GTK_SIGNAL_FUNC (im_entry_changed), NULL);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_anim_entry, 1, 2, 2, 3);
  gtk_widget_show(im_anim_entry);

  frame = gtk_frame_new("Widgets");
  gtk_widget_show(frame);
  gtk_box_pack_start(GTK_BOX (pagebox), frame, FALSE, FALSE, 0);

  frametable = gtk_table_new(7, 5, FALSE);
  gtk_container_add(GTK_CONTAINER(frame), frametable);
  gtk_widget_show(frametable);

  label = gtk_label_new("Widget:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE(frametable), label, 0, 1, 0, 1);

  images = g_list_append(images, "background");
  images = g_list_append(images, "trough");
  images = g_list_append(images, "anchor");
  images = g_list_append(images, "thumb");
  images = g_list_append(images, "up arrow");
  images = g_list_append(images, "down arrow");
  images = g_list_append(images, "menu");
  images = g_list_append(images, "submenu");
  images = g_list_append(images, "button bar");
  images = g_list_append(images, "button");
  
  images_cbox = gtk_combo_new();
  gtk_combo_set_popdown_strings (GTK_COMBO (images_cbox), images);
  gtk_entry_set_text (GTK_ENTRY (GTK_COMBO(images_cbox)->entry), "background");
  gtk_entry_set_editable(GTK_ENTRY (GTK_COMBO (images_cbox)->entry), FALSE);
  gtk_table_attach_defaults(GTK_TABLE(frametable), images_cbox, 1, 5, 0, 1);
  gtk_signal_connect(GTK_OBJECT (GTK_COMBO (images_cbox)->entry), "changed",
                     GTK_SIGNAL_FUNC (im_type_cb), NULL);

  gtk_widget_show(images_cbox);

  label = gtk_label_new("mode:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 1, 2);

  im_modes = g_list_append(im_modes, "image");
  im_modes = g_list_append(im_modes, "trans");
  im_modes = g_list_append(im_modes, "viewport");
  im_modes = g_list_append(im_modes, "auto");
  im_modes_cbox = gtk_combo_new();
  gtk_combo_set_popdown_strings (GTK_COMBO (im_modes_cbox), im_modes);
  gtk_entry_set_text (GTK_ENTRY(GTK_COMBO(im_modes_cbox)->entry), "image");
  gtk_entry_set_editable(GTK_ENTRY (GTK_COMBO (im_modes_cbox)->entry), FALSE);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_modes_cbox, 1, 2, 1, 2);
  gtk_widget_show(im_modes_cbox);

  frame = gtk_frame_new("allowed");
  gtk_widget_show(frame);
  gtk_table_attach_defaults(GTK_TABLE (frametable), frame, 2, 3, 1, 2);
  subframetable = gtk_table_new(1, 3, FALSE);
  gtk_container_add(GTK_CONTAINER (frame), subframetable);
  gtk_widget_show(subframetable);
  
  im_image_btn = gtk_check_button_new_with_label("image");
  gtk_widget_show(im_image_btn);
  gtk_table_attach_defaults(GTK_TABLE (subframetable), im_image_btn, 0, 1, 0, 1);
  
  im_trans_btn = gtk_check_button_new_with_label("trans");
  gtk_widget_show(im_trans_btn);
  gtk_table_attach_defaults(GTK_TABLE (subframetable), im_trans_btn, 0, 1, 1, 2);
  
  im_viewport_btn = gtk_check_button_new_with_label("viewport");
  gtk_widget_show(im_viewport_btn);
  gtk_table_attach_defaults(GTK_TABLE (subframetable), im_viewport_btn, 0, 1, 2, 3);
  
  im_auto_btn = gtk_check_button_new_with_label("auto");
  gtk_widget_show(im_auto_btn);
  gtk_table_attach_defaults(GTK_TABLE (subframetable), im_auto_btn, 0, 1, 3, 4);

  label = gtk_label_new("Widget state");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 3, 4);

  im_states = g_list_append(im_states, "normal");
  im_states = g_list_append(im_states, "selected");
  im_states = g_list_append(im_states, "clicked");
  im_states_cbox = gtk_combo_new();
  gtk_combo_set_popdown_strings (GTK_COMBO (im_states_cbox), im_states);
  gtk_entry_set_text (GTK_ENTRY(GTK_COMBO(im_states_cbox)->entry), "normal");
  gtk_entry_set_editable(GTK_ENTRY (GTK_COMBO (im_states_cbox)->entry), FALSE);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_states_cbox, 1, 3, 3, 4);
  gtk_signal_connect(GTK_OBJECT (GTK_COMBO (im_states_cbox)->entry), "changed",
                     GTK_SIGNAL_FUNC (im_type_cb), NULL);
  gtk_widget_show(im_states_cbox);

  label = gtk_label_new("File:");
  gtk_widget_show(label);
  gtk_table_attach_defaults(GTK_TABLE (frametable), label, 0, 1, 4, 5);

  im_file_entry = gtk_entry_new_with_max_length(50);
  gtk_widget_show(im_file_entry);
  gtk_table_attach_defaults(GTK_TABLE (frametable), im_file_entry, 1, 3, 4, 5);


  /* Buttons to save or cancel, not on a notebook page */   
  buttonbox = gtk_hbox_new (TRUE, 2);
  gtk_container_border_width (GTK_CONTAINER (buttonbox), 10);
  gtk_table_attach_defaults(GTK_TABLE(bigtable), buttonbox, 0, 1, 1, 2);
  gtk_widget_show (buttonbox);
  /* OK button saves and exits */
  button=gtk_button_new_with_label("OK");
  gtk_signal_connect(GTK_OBJECT (button), "clicked",
		     GTK_SIGNAL_FUNC(button_handler), (gpointer) "OK");
  gtk_box_pack_start (GTK_BOX(buttonbox),button, TRUE, TRUE,0);
  gtk_widget_show(button);
  /* Apply saves but doesn't exit */
  button=gtk_button_new_with_label("Test");
  gtk_signal_connect(GTK_OBJECT (button), "clicked",
		     GTK_SIGNAL_FUNC(button_handler), "TEST");
  gtk_box_pack_start (GTK_BOX(buttonbox),button, TRUE, TRUE,0);
  gtk_widget_show(button);
  /* Cancel exits without saving */
  button = gtk_button_new_with_label ("Cancel");
  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
			     GTK_SIGNAL_FUNC(on_exit_application), NULL);
  gtk_box_pack_start (GTK_BOX (buttonbox), button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  gtk_widget_show(window);
  set_defaults();

  gtk_main();
  return (0);
}
