/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _EDJE_EDITOR_MAIN_H_
#define _EDJE_EDITOR_MAIN_H_

#include <Edje.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>
#include "edje_editor_images.h"
#include "edje_editor_tree.h"
#include "edje_editor_group.h"
#include "edje_editor_part.h"
#include "edje_editor_state.h"
#include "edje_editor_position.h"
#include "edje_editor_text.h"
#include "edje_editor_rect.h"
#include "edje_editor_program.h"
#include "edje_editor_script.h"
#include "edje_editor_toolbar.h"
#include "edje_editor_canvas.h"
#include "edje_editor_consolle.h"
#include "edje_editor_dialogs.h"
#include "edje_editor_window.h"
#include "edje_editor_gradient.h"
#include "edje_editor_fill.h"
#include "edje_editor_spectra.h"
#include "edje_editor_spectra_widget.h"
#include "edje_editor_tree_model_spectra.h"
#include "edje_editor_data.h"

/* DEFINES */
#define USE_GL_ENGINE 0
#define DEBUG_MODE 0

#if DEBUG_MODE
   #define TREE_WIDTH 365
#else
   #define TREE_WIDTH 265
#endif

//All the enum used are declared here
enum various
{
   FILECHOOSER_OPEN,
   FILECHOOSER_IMAGE,
   FILECHOOSER_FONT,
   FILECHOOSER_SAVE_EDC,
   FILECHOOSER_SAVE_EDJ,
   TOOLBAR_NEW,
   TOOLBAR_OPEN,
   TOOLBAR_ADD,
   TOOLBAR_REMOVE,
   TOOLBAR_OPTIONS,
   TOOLBAR_DEBUG,
   TOOLBAR_SAVE,
   TOOLBAR_SAVE_EDC,
   TOOLBAR_SAVE_EDJ,
   TOOLBAR_PLAY,
   TOOLBAR_PAUSE,
   TOOLBAR_MOVE_UP,
   TOOLBAR_MOVE_DOWN,
   TOOLBAR_IMAGE_BROWSER,
   TOOLBAR_SPECTRUM,
   TOOLBAR_DATA,
   IMAGE_BROWSER_SHOW,
   IMAGE_BROWSER_CLOSE,
   TOOLBAR_FONT_BROWSER,
   TOOLBAR_OPTION_BG1,
   TOOLBAR_OPTION_BG2,
   TOOLBAR_OPTION_BG3,
   TOOLBAR_OPTION_BGC,
   TOOLBAR_OPTION_FOPA100,
   TOOLBAR_OPTION_FOPA50,
   TOOLBAR_OPTION_FOPA25,
   TOOLBAR_OPTION_FULLSCREEN,
   TOOLBAR_IMAGE_FILE_ADD,
   TOOLBAR_FONT_FILE_ADD,
   TOOLBAR_QUIT,
   COLOR_OBJECT_RECT,
   COLOR_OBJECT_TEXT,
   COLOR_OBJECT_SHADOW,
   COLOR_OBJECT_OUTLINE,
   COLOR_OBJECT_BG,
   NEW_IMAGE,
   NEW_GRADIENT,
   NEW_RECT,
   NEW_TEXT,
   NEW_SWAL,
   NEW_GROUPSWAL,
   NEW_PROG,
   NEW_DESC,
   NEW_GROUP,
   REMOVE_DESCRIPTION,
   REMOVE_PART,
   REMOVE_GROUP,
   REMOVE_PROG,
   REL1X_SPINNER,
   REL1Y_SPINNER,
   REL2X_SPINNER,
   REL2Y_SPINNER,
   REL1XO_SPINNER,
   REL1YO_SPINNER,
   REL2XO_SPINNER,
   REL2YO_SPINNER,
   MINW_SPINNER,
   MAXW_SPINNER,
   MINH_SPINNER,
   MAXH_SPINNER,
   STATE_ALIGNV_SPINNER,
   STATE_ALIGNH_SPINNER,
   TEXT_ALIGNV_SPINNER,
   TEXT_ALIGNH_SPINNER,
   BORDER_TOP,
   BORDER_LEFT,
   BORDER_RIGHT,
   BORDER_BOTTOM,
   ROW_GROUP,
   ROW_PART,
   ROW_DESC,
   ROW_PROG,
   DRAG_MINIARROW,
   DRAG_REL1,
   DRAG_REL2,
   DRAG_MOVE,
   DRAG_MOVEBOX,
   DRAG_GRAD_1,
   DRAG_GRAD_2,
   REL_COMBO_INTERFACE,
   IMAGE_TWEEN_UP,
   IMAGE_TWEEN_DOWN,
   IMAGE_TWEEN_ADD,
   IMAGE_TWEEN_DELETE,
   SPECTRA_ADD,
   SPECTRA_DELETE,
   SAVE_SCRIPT,
   RUN_PROG
};

struct Current_State
{
   char *open_file_name;      //Full path to the open edje file
   char *source_dir;          //Full path to sources
   char *main_source_file;    //Full path to the main edc file

   Etk_String *group;         //The current selected group name
   Etk_String *part;          //The current selected part name
   Etk_String *state;         //The current selected state name
   Etk_String *prog;          //The current selected prog name
   Etk_String *tween;         //The current selected tween name in the image frame
   Etk_String *spectra;       //The current selected spectra in the spectra editor
   
   Etk_String *edj_file_name;
   Etk_String *edj_temp_name;
   
   int fullscreen;            //The current main window state
}Cur;

/* GLOBALS */
int            FileChooserOperation;   //The current file chooser operation (FILECHOOSER_OPEN,FILECHOOSER_NEW etc)
int            ImageBroserUpdate;      //When set to true the image browser will update the current state with the selected image
char           *EdjeFile;              //The filename of the edje_editor.edj file (witch contain all the graphics used by the program)
Evas_Object    *edje_o;                //The edje object we are editing
Evas_Hash      *Parts_Hash;            //Associate part names with Etk_Tree_Row*


/* FUNCTION PROTOTYPES*/
void print_debug_info (int full);
void change_group     (char *group);
int  load_edje        (char *file);
void reload_edje      (void);


//This define is copied from edje_private.h (find a way to export it)
#define EDJE_PART_TYPE_NONE      0
#define EDJE_PART_TYPE_RECTANGLE 1
#define EDJE_PART_TYPE_TEXT      2
#define EDJE_PART_TYPE_IMAGE     3
#define EDJE_PART_TYPE_SWALLOW   4
#define EDJE_PART_TYPE_TEXTBLOCK 5
#define EDJE_PART_TYPE_GRADIENT  6
#define EDJE_PART_TYPE_GROUP     7
#define EDJE_PART_TYPE_LAST      8

#define EDJE_TEXT_EFFECT_NONE                0
#define EDJE_TEXT_EFFECT_PLAIN               1
#define EDJE_TEXT_EFFECT_OUTLINE             2
#define EDJE_TEXT_EFFECT_SOFT_OUTLINE        3
#define EDJE_TEXT_EFFECT_SHADOW              4
#define EDJE_TEXT_EFFECT_SOFT_SHADOW         5
#define EDJE_TEXT_EFFECT_OUTLINE_SHADOW      6
#define EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW 7
#define EDJE_TEXT_EFFECT_FAR_SHADOW          8
#define EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW     9
#define EDJE_TEXT_EFFECT_GLOW               10
#define EDJE_TEXT_EFFECT_LAST               11

#define EDJE_ACTION_TYPE_NONE          0
#define EDJE_ACTION_TYPE_STATE_SET     1
#define EDJE_ACTION_TYPE_ACTION_STOP   2
#define EDJE_ACTION_TYPE_SIGNAL_EMIT   3
#define EDJE_ACTION_TYPE_DRAG_VAL_SET  4
#define EDJE_ACTION_TYPE_DRAG_VAL_STEP 5
#define EDJE_ACTION_TYPE_DRAG_VAL_PAGE 6
#define EDJE_ACTION_TYPE_SCRIPT        7
#define EDJE_ACTION_TYPE_LAST          8

#define EDJE_TWEEN_MODE_NONE       0
#define EDJE_TWEEN_MODE_LINEAR     1
#define EDJE_TWEEN_MODE_SINUSOIDAL 2
#define EDJE_TWEEN_MODE_ACCELERATE 3
#define EDJE_TWEEN_MODE_DECELERATE 4
#define EDJE_TWEEN_MODE_LAST       5

#define EDJE_ASPECT_PREFER_NONE       0
#define EDJE_ASPECT_PREFER_VERTICAL   1
#define EDJE_ASPECT_PREFER_HORIZONTAL 2
#define EDJE_ASPECT_PREFER_BOTH       3


#endif
