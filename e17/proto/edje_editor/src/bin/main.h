#ifndef INCLUSION_GUARD
#define INCLUSION_GUARD

#include <Engrave.h>
#include <Edje.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <Ecore_Evas.h>

/* DEFINES */
#define FAKEWIN_BORDER_TOP    16
#define FAKEWIN_BORDER_LEFT   7
#define FAKEWIN_BORDER_RIGHT  4
#define FAKEWIN_BORDER_BOTTOM 4

#define TEST_DIRECT_EDJE   0

#define DEBUG_TREE         1
#if DEBUG_TREE
   #define TREE_WIDTH         365
#else
   #define TREE_WIDTH         265
#endif

#undef FREE
#define FREE(val) \
{ \
  free(val); val = NULL; \
}

#undef IF_FREE
#define IF_FREE(val) \
{ \
  if (val) FREE(val) \
  val = NULL; \
}


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
   TOOLBAR_MOVE_UP,
   TOOLBAR_MOVE_DOWN,
   TOOLBAR_OPTION_BG1,
   TOOLBAR_OPTION_BG2,
   TOOLBAR_OPTION_BG3,
   TOOLBAR_OPTION_BG4,
   TOOLBAR_IMAGE_FILE_ADD,
   TOOLBAR_FONT_FILE_ADD,
   COLOR_OBJECT_RECT,
   COLOR_OBJECT_TEXT,
   COLOR_OBJECT_SHADOW,
   COLOR_OBJECT_OUTLINE,
   NEW_IMAGE,
   NEW_RECT,
   NEW_TEXT,
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
   DRAG_MOVEBOX,
   REL_COMBO_INTERFACE,
   IMAGE_TWEEN_UP,
   IMAGE_TWEEN_DOWN,
   IMAGE_TWEEN_DELETE,
   IMAGE_TWEEN_RADIO,
   IMAGE_NORMAL_RADIO,
   SAVE_SCRIPT,
   LOAD_WIN,
   SAVE_WIN
};

struct Current_State
{
   Engrave_File  *ef;
   Engrave_Group *eg;
   Engrave_Part  *ep;
   Engrave_Program *epr;
   Engrave_Part_State *eps;
   char *open_file_name;      //Full path to the open edje file
   char *source_dir;          //Full path to sources
   char *main_source_file;    //Full path to the main edc file
#if TEST_DIRECT_EDJE
   Etk_String *group;
   Etk_String *part;
   Etk_String *state;
   
   Etk_String *edj_file_name;

#endif
}Cur;

/* GLOBALS */
int            FileChooserOperation;   //The current file chooser operation (FILECHOOSER_OPEN,FILECHOOSER_NEW etc)
char           *EdjeFile;              //The filename of the edje_editor.edj file (witch contain all the graphics used by the program)

Evas_Object    *engrave_canvas;        //The engrave canvas

Evas_Object	   *EV_fakewin;	         //The simple window implementation
Evas_Object    *EV_movebox;            //  FIXME
Evas_Object    *focus_handler;         //The yellow box around the selected part
Evas_Object    *rel1_handler;          //The red point
Evas_Object    *rel2_handler;          //The blue point
Evas_Object    *rel1X_parent_handler;  //The 4 line that show the container for each side of the part
Evas_Object    *rel1Y_parent_handler;  //
Evas_Object    *rel2X_parent_handler;  //
Evas_Object    *rel2Y_parent_handler;  //
Ecore_Hash     *hash;                  //Associate the engrave objects with the tree_rows

/* FUNCTION PROTOTYPES*/
void           DebugInfo   (int full);

#if TEST_DIRECT_EDJE

//This define is copied from edje_private.h
#define EDJE_PART_TYPE_NONE      0
#define EDJE_PART_TYPE_RECTANGLE 1
#define EDJE_PART_TYPE_TEXT      2
#define EDJE_PART_TYPE_IMAGE     3
#define EDJE_PART_TYPE_SWALLOW   4
#define EDJE_PART_TYPE_TEXTBLOCK 5
#define EDJE_PART_TYPE_GRADIENT  6
#define EDJE_PART_TYPE_GROUP     7
#define EDJE_PART_TYPE_LAST      8

Evas_Object *edje_o;
Evas_Hash   *Parts_Hash;


void ChangeGroup(const char *group);
#endif

#endif // INCLUSION_GUARD
