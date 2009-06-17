/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_H
#define _EX_H

#include <Eet.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Ecore_X_Cursor.h>
#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <Epsilon.h>
#include <Epsilon_Request.h>
#include <Efreet_Mime.h>
#include <etk/Etk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#include "config.h"

#if HAVE_ENGRAVE
#include <Engrave.h>
#endif

#define ETK_DEFAULT_ICON_SET_FILE ETK_PACKAGE_DATA_DIR "/stock_icons/default.edj"

#define EX_DEFAULT_BLUR_THRESH 2
#define EX_DEFAULT_SHARPEN_THRESH 2
#define EX_DEFAULT_BRIGHTEN_THRESH 100
#define EX_DEFAULT_SLIDE_INTERVAL 5.0
#define EX_DEFAULT_COMMENTS_HIDDEN 0
#define EX_DEFAULT_COMMENTS_VISIBLE 1
#define EX_DEFAULT_WINDOW_WIDTH 700
#define EX_DEFAULT_WINDOW_HEIGHT 500

#ifdef EX_DEBUG
# define D(x)  do {printf(__FILE__ ":%d:  ", __LINE__); printf x; fflush(stdout);} while (0)
#else
# define D(x)  ((void) 0)
#endif

typedef struct _Exhibit Exhibit;
typedef struct _Ex_Options Ex_Options;
typedef struct _Ex_Options_Dialog Ex_Options_Dialog;
typedef struct _Ex_Tab Ex_Tab;
typedef struct _Ex_Thumb Ex_Thumb;
typedef struct _Ex_Filedialog Ex_Filedialog;
typedef struct _Ex_Config_Version Ex_Config_Version;

typedef enum _Ex_Images
{
   EX_IMAGE_FIT_TO_WINDOW = -1,
   EX_IMAGE_ONE_TO_ONE = -2,
   EX_IMAGE_ZOOM_IN = -3,
   EX_IMAGE_ZOOM_OUT = -4
} Ex_Images;

typedef enum _Ex_Sort
{
   EX_SORT_BY_DATE = -1,
   EX_SORT_BY_SIZE = -2,
   EX_SORT_BY_NAME = -3,
   EX_SORT_BY_RESOLUTION = -4
} Ex_Sort;

typedef enum _Ex_Tree_Update
{
   EX_TREE_UPDATE_FILES = -1,
   EX_TREE_UPDATE_DIRS = -2,
   EX_TREE_UPDATE_ALL = -3,
} Ex_Tree_Update;

struct _Ex_Options
{
   char  *app1;
   char  *app2;
   char  *app3;
   char  *app4;
   char  *app1_cmd;
   char  *app2_cmd;
   char  *app3_cmd;
   char  *app4_cmd;
   
   char  *fav_path;
   char  *dl_path;
   
   double blur_thresh;
   double sharpen_thresh;
   double brighten_thresh;
   
   int thumbs_show;
   
   double slide_interval;   
   int comments_visible;
   int rotate_autosave;
   int default_view;   
   int default_sort;
   int default_sort_tmp;
   int show_all_filetypes;
   int list_hidden;
   int monitor_focus;
   
   int last_w;
   int last_h;
};

struct _Ex_Options_Dialog
{
   Etk_Widget *dv_btn_1;
   Etk_Widget *dv_btn_2;

   Etk_Widget *blur_thresh;
   Etk_Widget *sharpen_thresh;
   Etk_Widget *brighten_thresh;

   Etk_Widget *thumbs_show;

   Etk_Widget *slide_interval;
   Etk_Widget *comments_visible;
   Etk_Widget *rotate_autosave;
   Etk_Widget *default_sort;
   Etk_Widget *show_all_filetypes;
   Etk_Widget *list_hidden;
   Etk_Widget *monitor_focus;
   Etk_Widget *dl_path;
   Etk_Combobox_Item *sort_date;
   Etk_Combobox_Item *sort_size;
   Etk_Combobox_Item *sort_name;
   Etk_Combobox_Item *sort_resolution;

   Etk_Widget *app1;
   Etk_Widget *app2;
   Etk_Widget *app3;
   Etk_Widget *app4;
   Etk_Widget *app1_cmd;
   Etk_Widget *app2_cmd;
   Etk_Widget *app3_cmd;
   Etk_Widget *app4_cmd;
};

struct _Ex_Tab
{
   char          *dir;

   /* Path and filename of the dir to the 
      currently "set" image */
   char           *set_img_path; 
   char           *cur_file;
   int            image_loaded;

   int            num;

   Etk_Bool       fit_window;
   Ecore_File_Monitor *monitor;
   
   Eina_List     *dirs;
  
   Etk_Widget    *image;   
   Etk_Widget    *dtree;
   Etk_Widget    *itree;   
   Etk_Widget    *scrolled_view;
   Etk_Widget    *alignment;   
   Etk_Widget    *dialog; 
   
   Etk_Tree_Col  *dcol;
   Etk_Tree_Col  *icol;

   Etk_Tree_Model *imodel;
   
   struct {
      Etk_Bool    visible;
      Etk_Widget *vbox;
      Etk_Widget *frame;
      Etk_Widget *table;
      Etk_Widget *textview;
      Etk_Widget *save;
      Etk_Widget *revert;
      Etk_Widget *vbox2;
      Etk_Widget *hbox;
      char       *text;
   } comment;   
};


struct _Exhibit
{
   Etk_Widget    *vbox;
   Etk_Widget    *vboxf;
   Etk_Widget    *hbox;
   Etk_Widget    *menu_bar;
   Etk_Widget    *statusbar[4];
   Etk_Widget    *notebook;
   Etk_Widget    *table;
   Etk_Widget    *hpaned;
   Etk_Widget    *vpaned;
   Etk_Widget    *hpaned_shadow;
   Etk_Widget    *combobox_entry;
   Etk_Widget    *zoom_in[2];
   Etk_Widget    *zoom_out[2];
   Etk_Widget    *fit[2];
   Etk_Widget    *original[2];
   Etk_Widget    *sort_bar;
   Etk_Widget    *sizebar;
   Etk_Widget    *resbar;
   Etk_Widget    *zoombar;
   Etk_Widget    *menu;
   Etk_Widget    *win;
   Etk_Widget    *app1_menu;
   Etk_Widget    *app2_menu;
   Etk_Widget    *app3_menu;
   Etk_Widget    *app4_menu;
   Etk_Widget    *submenu;
   
   int            zoom;
   int            brightness;
   int            contrast;

   Eina_List     *tabs;
   Ex_Tab        *cur_tab;
   
   Ex_Options     *options;
   Ex_Options_Dialog *opt_dialog;
   Ex_Config_Version *version;   
   
   struct {
      int down;
      int x;
      int y;
   } mouse;
   
   struct {
      double       interval;
      Ecore_Timer *timer;
      Etk_Bool     active;
   } slideshow;   
};

struct _Ex_Thumb
{
   Exhibit  *e;
   char     *name;
   char     *image;
   Etk_Bool  selected;
   Epsilon  *ep;
   Ex_Tab   *tab;
};

struct _Ex_Filedialog
{
   Etk_Widget *win;
   Etk_Widget *filechooser;
   Etk_Image  *im;
   Etk_Widget *hbox;
   Exhibit *e;
};

struct _Ex_Config_Version
{
   int major;
   int minor;
   int patch;
};

#define WINDOW_TITLE "Exhibit"
#define ZOOM_MAX 16
#define ZOOM_MIN -16

#ifndef DATA64
#define DATA64 unsigned long long
#define DATA32 unsigned int
#define DATA16 unsigned short
#define DATA8  unsigned char
#endif

#define E_FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; } } while(0);

#include "exhibit_menus.h"
#include "exhibit_file.h"
#include "exhibit.h"
#include "exhibit_image.h"
#include "exhibit_main.h"
#include "exhibit_menus.h"
#include "exhibit_sort.h"
#include "exhibit_thumb.h"
#include "exhibit_tab.h"
#include "exhibit_slideshow.h"
#include "exhibit_favorites.h"
#include "exhibit_options.h"
#include "exhibit_comment.h"
#include "exhibit_model_wobbly.h"

extern Exhibit *e;

#endif
