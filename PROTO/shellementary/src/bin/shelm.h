#ifndef SHELLEMENTARY_H
#define SHELLEMENTARY_H

// includes
#include <Eina.h>
#include <Elementary.h>
#include <Ecore_Getopt.h>
#include <Ecore.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
// gettext stuff
#include <locale.h>
#include "config.h"
#define _(x) gettext(x)
#ifdef E_FREE
#undef E_FREE
#endif
#define E_FREE(ptr) if(ptr) { free(ptr); ptr = NULL;}

# ifdef SH_API
#  undef SH_API
# endif
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define SH_API __attribute__ ((visibility("default")))
#  else
#   define SH_API
#  endif
# else
#  define SH_API
# endif

// where to store stuff?
typedef struct _arguments_struct Arguments_struct;

struct _arguments_struct
{
  // window stuff
  char *window_title;
  char *window_text;
  int window_width;
  int window_height;
  char *window_bg;
  // dialogs enabled?
  Eina_Bool dlg_entry_enabled;
  Eina_Bool dlg_error_enabled;
  Eina_Bool dlg_warning_enabled;
  Eina_Bool dlg_info_enabled;
  Eina_Bool dlg_textinfo_enabled;
  Eina_Bool dlg_list_enabled;
  Eina_Bool dlg_question_enabled;
  Eina_Bool dlg_clock_enabled;
  Eina_Bool dlg_scale_enabled;
  // entry dialog
  char *entry_entry_text;
  Eina_Bool entry_hide_text; 
  // text info dialog
  char *textinfo_filename;
  Eina_Bool textinfo_editable;
  Eina_Bool textinfo_nowrap;
  // clock dialog
  Eina_Bool clock_show_seconds;
  Eina_Bool clock_show_am_pm;
  char *clock_time;
  Eina_Bool clock_editable;
  // scale dialog
  double scale_value;
  double scale_min_value;
  double scale_max_value;
  char *scale_step;
  Eina_Bool scale_print_partial;
  Eina_Bool scale_hide_value;
  Eina_Bool scale_inverted;
  char *scale_unit_format;
  char *scale_label;
  char *scale_icon;
  Eina_Bool scale_vertical;
  // quit booleans
  Eina_Bool quit_bool;
  Eina_Bool quit_cancel_bool;
};

// inititate pointer struct for storing stuff
Arguments_struct *arguments;

// general prototypes
static Eina_Bool       _args_init(int argc, char **argv); // parse commandline arguments

// macros prototypes - provide easy creation of widgets.
SH_API Evas_Object    *create_window(const char *window_name, const char *window_title, void *destroy);
SH_API Evas_Object    *create_background(Evas_Object *parent, const char *window_background, Eina_Bool window_resizable);
SH_API Evas_Object    *create_box(Evas_Object *parent, Eina_Bool is_horizontal);
SH_API Evas_Object    *create_frame(Evas_Object *parent, Eina_Bool window_resizable);
SH_API Evas_Object    *create_button(Evas_Object *parent, const char *button_icon, const char *label);
SH_API Evas_Object    *create_label(Evas_Object *parent, const char *text);
SH_API Evas_Object    *create_scroller(Evas_Object *parent, Eina_Bool scroller_has_one_line);
SH_API Evas_Object    *create_entry(Evas_Object *parent, Eina_Bool entry_has_one_line, const char *entry_text, Eina_Bool entry_hide_text, Eina_Bool entry_line_nowrap, Eina_Bool entry_editable);
SH_API Evas_Object    *create_icon(Evas_Object *parent, const char *iconfile);
SH_API Evas_Object    *create_list(Evas_Object *parent);
SH_API Evas_Object    *create_clock(Evas_Object *parent, Eina_Bool show_seconds, Eina_Bool show_am_pm, const char *time, Eina_Bool is_editable);
SH_API Evas_Object    *create_slider(Evas_Object *parent, double slider_value, double slider_min_value, double slider_max_value, const char *slider_step, Eina_Bool slider_partial, Eina_Bool slider_hide_value, Eina_Bool slider_inverted, const char *slider_unit_format, const char *slider_label, const char *slider_icon, Eina_Bool slider_vertical);

// callback prototypes
SH_API void            destroy(void *data, Evas_Object *obj, void *event_info); // destroy callback for window
SH_API void            list_dialog_callback(void *data, Evas_Object *obj, void *event_info); // callback for list dialog - print selected item
SH_API void            cancel_callback(void *data, Evas_Object *obj, void *event_info); // clicked Cancel button, should return value 1
SH_API void            entry_callback(void *data, Evas_Object *obj, void *event_info); // prints entry text
SH_API void            slider_callback(void *data, Evas_Object *obj, void *event_info); // print slider value when clicked OK
SH_API void            clock_callback(void *data, Evas_Object *obj, void *event_info); // print clock info
SH_API int             _read_stdin_entry(void *data, Ecore_Fd_Handler *fd_handler);
SH_API int             _read_stdin_list(void *data, Ecore_Fd_Handler *fd_handler);

// dialog prototypes
void                   shelm_about_dialog(); // when no arguments entered
void                   shelm_entry_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char *entry_text, Eina_Bool entry_hide_text); // entry dialog
void                   shelm_simple_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char *window_icccm_name, const char *window_default_title, const char *window_default_icon); // error, warning, info dialogs
void                   shelm_question_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background); // question dialog
void                   shelm_scale_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, double slider_value, double slider_min_value, double slider_max_value, const char *slider_step, Eina_Bool slider_partial, Eina_Bool slider_hide_value, Eina_Bool slider_inverted, const char *slider_unit_format, const char *slider_label, const char *slider_icon, Eina_Bool slider_vertical);
void                   shelm_clock_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, Eina_Bool show_seconds, Eina_Bool show_am_pm, const char *time, Eina_Bool is_editable);
void                   shelm_textinfo_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char *textinfo_filename, Eina_Bool textinfo_editable, Eina_Bool textinfo_nowrap);
void                   shelm_list_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char **listitems, int args_num, int argc);

#endif

