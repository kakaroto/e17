#ifndef INCLUSION_GUARD
#define INCLUSION_GUARD

#include <Engrave.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>

/* DEFINES */
#define FAKEWIN_BORDER_TOP 16
#define FAKEWIN_BORDER_LEFT 7
#define FAKEWIN_BORDER_RIGHT 4
#define FAKEWIN_BORDER_BOTTOM 4

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
   FILECHOOSER_NEW,
   FILECHOOSER_IMAGE,
   FILECHOOSER_FONT,
   FILECHOOSER_SAVE_AS,
   TOOLBAR_NEW,
   TOOLBAR_OPEN,
   TOOLBAR_ADD,
   TOOLBAR_REMOVE,
   TOOLBAR_DEBUG,
   TOOLBAR_SAVE,
   TOOLBAR_SAVE_AS,
   TOOLBAR_PLAY,
   TOOLBAR_MOVE_UP,
   TOOLBAR_MOVE_DOWN,
   TOOLBAR_IMAGE_FILE_ADD,
   TOOLBAR_FONT_FILE_ADD,
   COLOR_OBJECT_RECT,
   COLOR_OBJECT_TEXT,
   COLOR_OBJECT_SHADOW,
   COLOR_OBJECT_OUTLINE,
   NEW_IMAGE,
   NEW_RECT,
   NEW_TEXT,
   NEW_DESC,
   NEW_GROUP,
   REMOVE_DESCRIPTION,
   REMOVE_PART,
   REMOVE_GROUP,
   REL1X_SPINNER,
   REL1Y_SPINNER,
   REL2X_SPINNER,
   REL2Y_SPINNER,
   MINW_SPINNER,
   MAXW_SPINNER,
   MINH_SPINNER,
   MAXH_SPINNER,
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
   REL_COMBO_INTERFACE
};

struct Current_State
{
   Engrave_File  *ef;
   Engrave_Group *eg;
   Engrave_Part  *ep;
   Engrave_Program *epr;
   Engrave_Part_State *eps;
}Cur;

/* GLOBALS */
int            FileChooserOperation;   //The current file chooser operation (FILECHOOSER_OPEN,FILECHOOSER_NEW etc)
char           *EdjeFile;              //The filename of the edje_editor.edj file (witch contain all the graphics used by the program)

Evas_Object    *ecanvas;               //The engrave canvas
Etk_Widget     *ETK_canvas;	         //Designer canvas
Evas_Object    *EV_canvas_bg;          //
Evas_Object    *EV_canvas_shadow;      //
Evas_Object	   *EV_fakewin;	         //The simple window implementation
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

#endif // INCLUSION_GUARD
