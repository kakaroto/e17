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

#include <glib.h>
#include <gtk/gtk.h>

GtkWidget *col_fore_entry, *col_back_entry, *col_curs_entry, *col_curstext_entry, *col_point_entry, *col_vid_btn;
GtkWidget *atr_height_entry, *atr_width_entry, *atr_offX_entry, *atr_offY_entry, *atr_title_entry, *atr_name_entry;
GtkWidget *atr_iconname_entry, *atr_font_def_entry, *atr_font0_entry, *atr_font1_entry, *atr_font2_entry;
GtkWidget *atr_font3_entry, *atr_font4_entry, *atr_font5_entry, *atr_font6_entry, *atr_font_bold_entry, *atr_f0_def_btn;
GtkWidget *atr_f1_def_btn, *atr_f2_def_btn, *atr_f3_def_btn, *atr_f4_def_btn, *atr_f5_def_btn, *atr_f6_def_btn;
GtkWidget *tog_map_alert, *tog_visual_bell, *tog_login_shell, *tog_scrollbar, *tog_utmp_logging, *tog_select_line, *tog_meta8;
GtkWidget *tog_iconic, *tog_home_on_output, *tog_home_on_input, *tog_scrollbar_right, *tog_scrollbar_floating;
GtkWidget *tog_borderless, *tog_double_buffer;
GtkWidget *misc_print_pipe_entry, *misc_save_lines_entry, *misc_cut_chars_entry, *misc_border_width_entry;
GtkWidget *misc_line_space_entry, *misc_term_name_entry, *misc_exec_entry;
GtkWidget *dialog_window, *theme_name_entry;
GtkWidget *im_path_entry, *im_icon_entry, *im_anim_entry, *images_cbox, *im_states_cbox, *im_file_entry, *im_modes_cbox, *im_image_btn;
GtkWidget *im_trans_btn, *im_viewport_btn, *im_auto_btn;
GList *images, *im_states, *im_modes;

extern void col_entry_changed(GtkWidget *, GtkWidget *);
extern void atr_entry_changed(GtkWidget *, gpointer);
extern void misc_entry_changed(GtkWidget *, gpointer);
extern void im_entry_changed(GtkWidget *, gpointer);
extern void button_handler(GtkWidget *, gpointer);
extern void on_exit_application(GtkWidget *, gpointer);
extern void create_dialog(void);
extern void save_theme(char *);
extern void get_toggles(void);
extern void get_radio(void);
extern void set_defaults(void);
