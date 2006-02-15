/** @file etk_dnd.h */
#ifndef _ETK_DND_H_
#define _ETK_DND_H_

#include "etk_window.h"
#include "etk_types.h"

#define ETK_SELECTION_TARGET_TARGETS "TARGETS"
#define ETK_SELECTION_TARGET_TEXT "TEXT"
#define ETK_SELECTION_TARGET_COMPOUND_TEXT "COMPOUND_TEXT"
#define ETK_SELECTION_TARGET_STRING "STRING"
#define ETK_SELECTION_TARGET_UTF8_STRING "UTF8_STRING"
#define ETK_SELECTION_TARGET_FILENAME "FILENAME"

enum _Etk_Selection_Content
{
   ETK_SELECTION_CONTENT_NONE,
   ETK_SELECTION_CONTENT_TEXT,
   ETK_SELECTION_CONTENT_FILES,
   ETK_SELECTION_CONTENT_TARGETS,
   ETK_SELECTION_CONTENT_CUSTOM
};

struct _Etk_Event_Selection_Request
{    
   void *data;
   Etk_Selection_Content content;
};

struct _Etk_Selection_Data
{
   unsigned char    *data;
   int               length;
   
   int             (*free)(void *data);
};

struct _Etk_Selection_Data_Files
{
   Etk_Selection_Data data;
   char     **files;
   int        num_files;
};

struct _Etk_Selection_Data_Text
{
   Etk_Selection_Data data;
   char     *text;
};


struct _Etk_Selection_Data_Targets
{
   Etk_Selection_Data data;   
   char **targets;
   int num_targets;
};


#define ETK_DRAG_TYPE       (etk_drag_type_get())
#define ETK_DRAG(obj)       (ETK_OBJECT_CAST((obj), ETK_DRAG_TYPE, Etk_Drag))
#define ETK_IS_DRAG(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_DRAG_TYPE))

struct _Etk_Drag
{
   /* private: */
   /* Inherit from Etk_Window */
   Etk_Window window;   
   
   char         **types;
   unsigned int   num_types;
   void          *data;
   int            data_size;   
};

Etk_Bool etk_dnd_init();  
void etk_dnd_shutdown();
  
/** @} */

#endif
