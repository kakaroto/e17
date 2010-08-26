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


#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "action.h"
#include "eterm.h"

#define BUFFER_LEN 1023

extern void
col_entry_changed(GtkWidget *widget, GtkWidget *entry)
{
    color->foreground = gtk_entry_get_text (GTK_ENTRY (col_fore_entry));
    color->background = gtk_entry_get_text (GTK_ENTRY (col_back_entry));
    color->cursor = gtk_entry_get_text (GTK_ENTRY (col_curs_entry));
    color->cursor_text = gtk_entry_get_text (GTK_ENTRY (col_curstext_entry));
    color->pointer = gtk_entry_get_text (GTK_ENTRY (col_point_entry));
}

extern void
atr_entry_changed(GtkWidget *widget, gpointer data)
{
  attributes->height = atoi(gtk_entry_get_text (GTK_ENTRY (atr_height_entry)));
  attributes->width = atoi(gtk_entry_get_text (GTK_ENTRY (atr_width_entry)));
  attributes->offX = atoi(gtk_entry_get_text (GTK_ENTRY (atr_offX_entry)));
  attributes->offY = atoi(gtk_entry_get_text (GTK_ENTRY (atr_offY_entry)));
  attributes->title = gtk_entry_get_text (GTK_ENTRY (atr_title_entry));
  attributes->name = gtk_entry_get_text (GTK_ENTRY (atr_name_entry));
  attributes->iconname = gtk_entry_get_text (GTK_ENTRY (atr_iconname_entry));
  attributes->font0 = gtk_entry_get_text (GTK_ENTRY (atr_font0_entry));
  attributes->font1 = gtk_entry_get_text (GTK_ENTRY (atr_font1_entry));
  attributes->font2 = gtk_entry_get_text (GTK_ENTRY (atr_font2_entry));
  attributes->font3 = gtk_entry_get_text (GTK_ENTRY (atr_font3_entry));
  attributes->font4 = gtk_entry_get_text (GTK_ENTRY (atr_font4_entry));
  attributes->font5 = gtk_entry_get_text (GTK_ENTRY (atr_font5_entry));
  attributes->font6 = gtk_entry_get_text (GTK_ENTRY (atr_font6_entry));
  attributes->font_bold = gtk_entry_get_text (GTK_ENTRY (atr_font_bold_entry)); 
}

extern void
misc_entry_changed(GtkWidget *widget, gpointer data)
{
  misc->print_pipe = gtk_entry_get_text(GTK_ENTRY (misc_print_pipe_entry));
  misc->save_lines = gtk_entry_get_text(GTK_ENTRY (misc_save_lines_entry));
  misc->cut_chars = gtk_entry_get_text(GTK_ENTRY (misc_cut_chars_entry));
  misc->border_width = gtk_entry_get_text(GTK_ENTRY (misc_border_width_entry));
  misc->line_space = gtk_entry_get_text(GTK_ENTRY (misc_line_space_entry));
  misc->term_name = gtk_entry_get_text(GTK_ENTRY (misc_term_name_entry));
  misc->exec = gtk_entry_get_text(GTK_ENTRY (misc_exec_entry));
}

extern void
im_entry_changed(GtkWidget *widget, gpointer data)
{
  imageclass->path = gtk_entry_get_text(GTK_ENTRY (im_path_entry));
  imageclass->icon = gtk_entry_get_text(GTK_ENTRY (im_icon_entry));
  imageclass->anim = gtk_entry_get_text(GTK_ENTRY (im_anim_entry));
}

extern void
im_type_cb(GtkWidget *widget, gpointer data)
{
  char *cur_im_widget, *cur_im_state;

  cur_im_widget = (char *) malloc(BUFFER_LEN);
  cur_im_widget = gtk_entry_get_text(GTK_ENTRY (GTK_COMBO (images_cbox)->entry));
  cur_im_state = (char *) malloc(BUFFER_LEN);
  cur_im_state = gtk_entry_get_text(GTK_ENTRY (GTK_COMBO (im_states_cbox)->entry));

  printf("cur_im_widget: %s\ncur_im_state: %s\n", cur_im_widget, cur_im_state);

  if (strcmp(cur_im_widget, "trough") == 0){
    if (strcmp(cur_im_state, "normal") == 0){
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_trough->normal->file);  
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0){
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_trough->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_trough->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "anchor") == 0){
    if (strcmp(cur_im_state, "normal") == 0){
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_anchor->normal->file);
    }
    else {
      if(strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_anchor->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_anchor->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "up arrow") == 0){
    if (strcmp(cur_im_state, "normal") == 0){
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_up_arrow->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0){
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_up_arrow->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_up_arrow->clicked->file);
      }
    }
  }            
  if (strcmp(cur_im_widget, "down arrow") == 0){
    if (strcmp(cur_im_state, "normal") == 0) {
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_down_arrow->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_down_arrow->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_down_arrow->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "menu") == 0){
    if(strcmp(cur_im_state, "normal") == 0) {
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_menu->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_menu->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_menu->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "submenu") == 0) {
    if (strcmp(cur_im_state, "normal") == 0) {
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_submenu->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_submenu->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_submenu->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "button bar") == 0){
    if(strcmp(cur_im_state, "normal") == 0) {
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_buttonbar->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_buttonbar->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_buttonbar->clicked->file);
      }
    }
  }
  if (strcmp(cur_im_widget, "button") == 0) {
    if (strcmp(cur_im_state, "normal") == 0) {
      gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_button->normal->file);
    }
    else {
      if (strcmp(cur_im_state, "selected") == 0) {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_button->selected->file);
      }
      else {
        gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_button->clicked->file);
      }
    }
  }
}

extern void
im_states_cb(GtkWidget *widget, gpointer data)
{
  printf("here i am!\n");
}

extern void
get_toggles(void)
{
  if (GTK_TOGGLE_BUTTON (tog_map_alert)->active)
    toggles->map_alert = 1;
  else
    toggles->map_alert = 0;
  if (GTK_TOGGLE_BUTTON (tog_visual_bell)->active)
    toggles->visual_bell = 1;
  else
    toggles->visual_bell = 0;
  if (GTK_TOGGLE_BUTTON (tog_login_shell)->active)
    toggles->login_shell = 1;
  else
    toggles->login_shell = 0;
  if (GTK_TOGGLE_BUTTON (tog_scrollbar)->active)
    toggles->scrollbar = 1;
  else
    toggles->scrollbar = 0;
  if (GTK_TOGGLE_BUTTON (tog_utmp_logging)->active)
    toggles->utmp_logging = 1;
  else
    toggles->utmp_logging = 0;
  if (GTK_TOGGLE_BUTTON (tog_select_line)->active)
    toggles->select_line = 1;
  else
    toggles->select_line = 0;
  if (GTK_TOGGLE_BUTTON (tog_meta8)->active)
    toggles->meta8 = 1;
  else
    toggles->meta8 = 0;
  if (GTK_TOGGLE_BUTTON (tog_iconic)->active)
    toggles->iconic = 1;
  else
    toggles->iconic = 0;
  if (GTK_TOGGLE_BUTTON (tog_home_on_output)->active)
    toggles->home_on_output = 1;
  else
    toggles->home_on_output = 0;
  if (GTK_TOGGLE_BUTTON (tog_home_on_input)->active)
    toggles->home_on_input = 1;
  else
    toggles->home_on_input = 0;
  if (GTK_TOGGLE_BUTTON (tog_scrollbar_right)->active)
    toggles->scrollbar_right = 1;
  else
    toggles->scrollbar_right = 0;
  if (GTK_TOGGLE_BUTTON (tog_scrollbar_floating)->active)
    toggles->scrollbar_floating = 1;
  else
    toggles->scrollbar_floating = 0;
  if (GTK_TOGGLE_BUTTON (tog_borderless)->active)
    toggles->borderless = 1;
  else
    toggles->borderless = 0;
  if (GTK_TOGGLE_BUTTON (tog_double_buffer)->active)
    toggles->double_buffer = 1;
  else
    toggles->double_buffer = 0;
}

extern void
get_radio(void)
{
  if (GTK_TOGGLE_BUTTON (col_vid_btn)->active)
    color->video = 1;
  else
    color->video = 0;
  if (GTK_TOGGLE_BUTTON (atr_f0_def_btn)->active)
    attributes->font_default = 0;
  else if (GTK_TOGGLE_BUTTON (atr_f1_def_btn)->active)
    attributes->font_default=1;
  else if (GTK_TOGGLE_BUTTON (atr_f2_def_btn)->active)
    attributes->font_default=2;
  else if (GTK_TOGGLE_BUTTON (atr_f3_def_btn)->active)
    attributes->font_default=3;
  else if (GTK_TOGGLE_BUTTON (atr_f4_def_btn)->active)
    attributes->font_default=4;
  else if (GTK_TOGGLE_BUTTON (atr_f5_def_btn)->active)
    attributes->font_default=5;
  else if (GTK_TOGGLE_BUTTON (atr_f6_def_btn)->active)
    attributes->font_default=6;
}

extern void
button_handler(GtkWidget *widget, gpointer data)
{
  char *tn;

  tn = (char *) malloc(1024);

  if (strcmp((char *)data,"OK") == 0 ){
    create_dialog();
  }
  else if (strcmp((char *) data, "theme_OK") == 0){
    strcpy(tn, gtk_entry_get_text(GTK_ENTRY (theme_name_entry)));
    save_theme(tn);
    gtk_widget_destroy(dialog_window);
    on_exit_application(NULL, NULL);
  }
}

extern void
save_theme(char *theme_name)
{
  char *home_dir, *theme_dir, *theme_file;
  FILE *fPtr;
  get_toggles();
  get_radio();
  home_dir = (char *) malloc(1024);
  home_dir = getenv("HOME");
  theme_dir = (char *) malloc(1024);
  sprintf(theme_dir, "%s/.Eterm/themes/%s", home_dir, theme_name);
  mkdir(theme_dir, S_IRWXU);
  theme_file = (char *) malloc(1024);
  sprintf(theme_file, "%s/theme.cfg", theme_dir);

  printf("theme_file = %s\n", theme_file);

  if((fPtr = fopen(theme_file, "w"))){
    fprintf(fPtr, "<Eterm-0.9>\n");
    fprintf(fPtr, "begin main\n");
    fprintf(fPtr, "   begin color\n");
    fprintf(fPtr, "      foreground %s\n", color->foreground);
    fprintf(fPtr, "      background %s\n", color->background);
    fprintf(fPtr, "      cursor %s\n", color->cursor);
    fprintf(fPtr, "      cursor_text %s\n", color->cursor_text);
    fprintf(fPtr, "      pointer %s\n", color->pointer);
    if (color->video)
      fprintf(fPtr, "      video normal\n");
    else
      fprintf(fPtr, "      video reverse\n");
    fprintf(fPtr, "   end color\n");
    fprintf(fPtr, "   begin attributes\n");
    fprintf(fPtr, "      geom %dx%d+%d+%d\n", attributes->width, attributes->height, attributes->offX, attributes->offY);
    fprintf(fPtr, "      title %s\n", attributes->title);
    fprintf(fPtr, "      name %s\n", attributes->name);
    fprintf(fPtr, "      iconname %s\n", attributes->iconname);
    fprintf(fPtr, "      font default %d\n", attributes->font_default);
    fprintf(fPtr, "      font 0 %s\n", attributes->font0);
    fprintf(fPtr, "      font 1 %s\n", attributes->font1);
    fprintf(fPtr, "      font 2 %s\n", attributes->font2);
    fprintf(fPtr, "      font 3 %s\n", attributes->font3);
    fprintf(fPtr, "      font 4 %s\n", attributes->font4);
    fprintf(fPtr, "      font 5 %s\n", attributes->font5);
    fprintf(fPtr, "      font 6 %s\n", attributes->font6);
    fprintf(fPtr, "      font bold %s\n", attributes->font_bold);  
    fprintf(fPtr, "   end attributes\n");
    fprintf(fPtr, "   begin toggles\n");
    fprintf(fPtr, "      map_alert %d\n", toggles->map_alert);
    fprintf(fPtr, "      visual_bell %d\n", toggles->visual_bell);
    fprintf(fPtr, "      login_shell %d\n", toggles->login_shell);
    fprintf(fPtr, "      scrollbar %d\n", toggles->scrollbar);
    fprintf(fPtr, "      utmp_logging %d\n", toggles->utmp_logging);
    fprintf(fPtr, "      select_line %d\n", toggles->select_line);
    fprintf(fPtr, "      meta8 %d\n", toggles->meta8);
    fprintf(fPtr, "      iconic %d\n", toggles->iconic);
    fprintf(fPtr, "      home_on_output %d\n", toggles->home_on_output);
    fprintf(fPtr, "      home_on_input %d\n", toggles->home_on_input);
    fprintf(fPtr, "      scrollbar_right %d\n", toggles->scrollbar_right);
    fprintf(fPtr, "      scrollbar_floating %d\n", toggles->scrollbar_floating);
    fprintf(fPtr, "      borderless %d\n", toggles->borderless);
    fprintf(fPtr, "      double_buffer %d\n", toggles->double_buffer);
    fprintf(fPtr, "   end toggles\n");
    fprintf(fPtr, "   begin misc\n");
    fprintf(fPtr, "      print_pipe %s\n", misc->print_pipe);
    fprintf(fPtr, "      save_lines %s\n", misc->save_lines);
    fprintf(fPtr, "      cut_chars %s\n", misc->cut_chars);
    fprintf(fPtr, "      border_width %s\n", misc->border_width);
    fprintf(fPtr, "      line_space %s\n", misc->line_space);
    fprintf(fPtr, "      term_name %s\n", misc->term_name);
    fprintf(fPtr, "      exec %s\n", misc->exec);
    fprintf(fPtr, "   end misc\n");
    fprintf(fPtr, "end main\n");
    fclose(fPtr);
  }
  else
    printf("err...\n");
}

extern void
on_exit_application (GtkWidget * widget, gpointer data)
{
  gtk_exit (0);
  return;
  data = NULL;
  widget = NULL;
}

extern void
create_dialog(void)
{
  GtkWidget *button;
  GtkWidget *label;

  dialog_window = gtk_dialog_new ();

  gtk_signal_connect (GTK_OBJECT (dialog_window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &dialog_window);

  gtk_window_set_title (GTK_WINDOW (dialog_window), "Theme Name");
  gtk_container_set_border_width (GTK_CONTAINER (dialog_window), 0);
  gtk_widget_set_usize (dialog_window, 200, 110);
  label = gtk_label_new("Theme Name:");
  gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
  gtk_widget_show(label);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox),
		      label, TRUE, TRUE, 0);

  theme_name_entry = gtk_entry_new_with_max_length(25);
  gtk_widget_show(theme_name_entry);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->vbox),
		      theme_name_entry, TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("OK");
  gtk_signal_connect(GTK_OBJECT (button), "clicked",
		     GTK_SIGNAL_FUNC (button_handler), "theme_OK"); 
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area), 
		      button, TRUE, TRUE, 0);
  gtk_widget_grab_default (button);
  gtk_widget_show (button);
  

  button = gtk_button_new_with_label ("Toggle");
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog_window)->action_area),
			  button, TRUE, TRUE, 0);
  gtk_widget_show (button);

  gtk_widget_show (dialog_window);  
}

void
set_defaults(void)
{
  gtk_entry_set_text(GTK_ENTRY (col_fore_entry), "white");
  gtk_entry_set_text(GTK_ENTRY (col_back_entry), "black");
  gtk_entry_set_text(GTK_ENTRY (col_curs_entry), "#FFFF00");
  gtk_entry_set_text(GTK_ENTRY (col_curstext_entry), "#880000");
  gtk_entry_set_text(GTK_ENTRY (col_point_entry), "white");
  gtk_entry_set_text(GTK_ENTRY (atr_name_entry), "%appname()");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (atr_f2_def_btn), TRUE);
  gtk_entry_set_text(GTK_ENTRY (atr_font0_entry), "5x7");
  gtk_entry_set_text(GTK_ENTRY (atr_font1_entry), "6x10");
  gtk_entry_set_text(GTK_ENTRY (atr_font2_entry), "fixed");
  gtk_entry_set_text(GTK_ENTRY (atr_font3_entry), "8x13");
  gtk_entry_set_text(GTK_ENTRY (atr_font4_entry), "9x15");
  gtk_entry_set_text(GTK_ENTRY (atr_font5_entry), "10x20");
  gtk_entry_set_text(GTK_ENTRY (atr_font6_entry), "12x24");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_map_alert), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_login_shell), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_scrollbar), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_utmp_logging), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_home_on_output), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_home_on_input), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tog_scrollbar_right), TRUE);
  gtk_entry_set_text(GTK_ENTRY (misc_print_pipe_entry), "\"lpr\"");
  gtk_entry_set_text(GTK_ENTRY (misc_save_lines_entry), "1024");
  gtk_entry_set_text(GTK_ENTRY (misc_cut_chars_entry), "\"\\t\\\\\\`\\\\\\\"\\'&() *,;<=>?@[]{|}\"");
  gtk_entry_set_text(GTK_ENTRY (misc_border_width_entry), "5");
  gtk_entry_set_text(GTK_ENTRY (misc_line_space_entry), "2");
  gtk_entry_set_text(GTK_ENTRY (misc_term_name_entry), "xterm");
  gtk_entry_set_text(GTK_ENTRY (im_path_entry), "\"./pix/:~/.Eterm/:~/.Eterm/themes/Eterm/pix:~/.Eterm/pix/:/usr/share/Eterm/pix/\"");
  strcpy(imageclass->image_bg->normal->file, "%random(`cat pixmaps.list`)");
  strcpy(imageclass->image_bg->mode, "image");
  imageclass->image_bg->allowed = 'd';
  strcpy(imageclass->image_trough->mode, "image");
  imageclass->image_trough->allowed ='f';
  strcpy(imageclass->image_trough->normal->file, "bar_vertical_3.png");
  strcpy(imageclass->image_trough->normal->color, "black #666666");
  strcpy(imageclass->image_trough->normal->geom, ":scale");
  strcpy(imageclass->image_trough->normal->border, "2 2 2 3");
  strcpy(imageclass->image_anchor->mode, "image");
  imageclass->image_anchor->allowed = '9';
  strcpy(imageclass->image_anchor->normal->color, "black #666666");
  strcpy(imageclass->image_anchor->normal->file, "bar_vertical_1.png");
  strcpy(imageclass->image_anchor->normal->geom, ":scale");
  strcpy(imageclass->image_anchor->normal->border, "2 2 2 3");
  strcpy(imageclass->image_anchor->selected->file, "bar_vertical_2.png");
  strcpy(imageclass->image_anchor->selected->geom, ":scale");
  strcpy(imageclass->image_anchor->selected->border, "2 2 2 3");
  strcpy(imageclass->image_up_arrow->mode, "image");
  imageclass->image_up_arrow->allowed = '9';
  strcpy(imageclass->image_up_arrow->normal->file, "button_arrow_up_1.png");
  strcpy(imageclass->image_up_arrow->normal->color, "black #666666");
  strcpy(imageclass->image_up_arrow->normal->geom, ":scale");
  strcpy(imageclass->image_up_arrow->normal->border, "2 2 2 2");
  strcpy(imageclass->image_up_arrow->selected->file, "button_arrow_up_2.png");
  strcpy(imageclass->image_up_arrow->selected->geom, ":scale");
  strcpy(imageclass->image_up_arrow->selected->border, "2 2 2 2");
  strcpy(imageclass->image_up_arrow->clicked->file, "button_arrow_up_3.png");
  strcpy(imageclass->image_up_arrow->clicked->geom, ":scale");
  strcpy(imageclass->image_up_arrow->clicked->border, "2 2 2 2");
  strcpy(imageclass->image_down_arrow->mode, "image");
  imageclass->image_down_arrow->allowed = '9';
  strcpy(imageclass->image_down_arrow->normal->file, "button_arrow_down_1.png");
  strcpy(imageclass->image_down_arrow->normal->color, "black #666666");
  strcpy(imageclass->image_down_arrow->normal->geom, ":scale");
  strcpy(imageclass->image_down_arrow->normal->border, "2 2 2 2");
  strcpy(imageclass->image_down_arrow->selected->file, "button_arrow_down_2.png");
  strcpy(imageclass->image_down_arrow->selected->geom, ":scale");
  strcpy(imageclass->image_down_arrow->selected->border, "2 2 2 2");
  strcpy(imageclass->image_down_arrow->clicked->file, "button_arrow_down_3.png");
  strcpy(imageclass->image_down_arrow->clicked->geom, ":scale");
  strcpy(imageclass->image_down_arrow->clicked->border, "2 2 2 2");
  strcpy(imageclass->image_menu->mode, "image");
  imageclass->image_menu->allowed = '9';
  strcpy(imageclass->image_menu->normal->file, "bar_horizontal_1.png");
  strcpy(imageclass->image_menu->normal->color, "black #666666");
  strcpy(imageclass->image_menu->normal->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_menu->normal->border, "2 3 2 3");
  strcpy(imageclass->image_menu->selected->file, "bar_horizontal_2.png");
  strcpy(imageclass->image_menu->selected->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_menu->selected->border, "2 3 2 3");
  strcpy(imageclass->image_menu->clicked->file, "bar_horizontal_2.png");
  strcpy(imageclass->image_menu->clicked->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_menu->clicked->border, "2 3 2 3");
  strcpy(imageclass->image_submenu->mode, "image");
  imageclass->image_submenu->allowed = '9';
  strcpy(imageclass->image_submenu->normal->file, "menu1.png");
  strcpy(imageclass->image_submenu->normal->color, "black #666666");
  strcpy(imageclass->image_submenu->normal->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_submenu->normal->border, "3 15 3 3");
  strcpy(imageclass->image_submenu->selected->file, "menu2.png");
  strcpy(imageclass->image_submenu->selected->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_submenu->selected->border, "3 15 3 3");
  strcpy(imageclass->image_submenu->clicked->file, "menu3.png");
  strcpy(imageclass->image_submenu->clicked->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_submenu->clicked->border, "3 15 3 3");
  strcpy(imageclass->image_buttonbar->mode, "image");
  imageclass->image_buttonbar->allowed = '9';
  strcpy(imageclass->image_buttonbar->normal->file, "bar_horizontal_1.png");
  strcpy(imageclass->image_buttonbar->normal->color, "black #999999");
  strcpy(imageclass->image_buttonbar->normal->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_buttonbar->normal->border, "2 3 2 2");
  strcpy(imageclass->image_buttonbar->selected->file, "bar_horizontal_1.png");
  strcpy(imageclass->image_buttonbar->selected->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_buttonbar->selected->border, "2 3 2 2");
  strcpy(imageclass->image_buttonbar->clicked->file, "bar_horizontal_1.png");
  strcpy(imageclass->image_buttonbar->clicked->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_buttonbar->clicked->border, "2 3 2 2");
  strcpy(imageclass->image_button->mode, "image");
  imageclass->image_button->allowed = '9';
  strcpy(imageclass->image_button->normal->file, "bar_horizontal_1.png");
  strcpy(imageclass->image_button->normal->color, "black #cccccc");
  strcpy(imageclass->image_button->normal->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_button->normal->border, "2 3 3 2");
  strcpy(imageclass->image_button->selected->file, "bar_horizontal_2.png");
  strcpy(imageclass->image_button->selected->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_button->selected->border, "2 3 2 2");
  strcpy(imageclass->image_button->clicked->file, "bar_horizontal_3.png");
  strcpy(imageclass->image_button->clicked->geom, "100x100+0+0:scale");
  strcpy(imageclass->image_button->clicked->border, "2 3 2 2");

  gtk_entry_set_text(GTK_ENTRY (im_file_entry), imageclass->image_bg->normal->file);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (im_image_btn), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (im_trans_btn), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (im_viewport_btn), TRUE);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (im_auto_btn), TRUE);
}
