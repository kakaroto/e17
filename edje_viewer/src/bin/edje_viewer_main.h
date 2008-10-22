#ifndef _EVM_H
#define _EVM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eina.h>
#include <Eet.h>
#include <Ecore_File.h>

#include <Edje.h>
#include <Edje_Edit.h>
#include <Etk.h>

#define WINDOW_TITLE "Edje Viewer"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

typedef enum _Tree_Search Tree_Search;
typedef struct _Gui Gui;
typedef struct _Edje_Viewer_Config Edje_Viewer_Config;
typedef struct _Demo_Edje Demo_Edje;
typedef struct _Collection Collection;


enum _Tree_Search
{
    TREE_SEARCH_START = 1,
    TREE_SEARCH_NEXT,
    TREE_SEARCH_PREV
};

struct _Gui
{
   Etk_Widget *win;
   Etk_Widget *tree;
   Etk_Widget *mdi_area;
   Etk_Widget *output;
   Etk_Widget *signal_entry;
   Etk_Widget *source_entry;
   
   Etk_Widget *fm_dialog;
   Etk_Widget *fm_chooser;

   Etk_Popup_Window *popup;
   Etk_Widget *search_entry;

   Etk_Tree_Row *part_row;

   Edje_Viewer_Config *config;

   char *path;
};

struct _Edje_Viewer_Config
{
   Eina_List *recent;

   int open_last;
   int sort_parts;

   int config_version;
};

struct _Demo_Edje
{
    Etk_Widget          *mdi_window;
    Etk_Widget          *etk_evas;
    Etk_Tree_Col        *tree_col;
    Etk_Tree_Row        *tree_row;
    Etk_Tree_Row        *part_row;
    Evas_Object         *edje_object;
    Evas_Coord	         minw, minh;
    Evas_Coord           maxw, maxh;
    char                *name;
    void                *data;
};

struct _Collection
{
    char        *file;
    char        *part;

    Demo_Edje   *de;
};

#include "edje_viewer_conf.h"
#include "edje_viewer_gui.h"
#include "edje_viewer_edje.h"

#endif
