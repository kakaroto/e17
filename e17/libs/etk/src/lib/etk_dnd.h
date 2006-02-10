/** @file etk_dnd.h */
#ifndef _ETK_DND_H_
#define _ETK_DND_H_

#include "etk_widget.h"

#define ETK_SELECTION_TARGET_TARGETS "TARGETS"
#define ETK_SELECTION_TARGET_TEXT "TEXT"
#define ETK_SELECTION_TARGET_COMPOUND_TEXT "COMPOUND_TEXT"
#define ETK_SELECTION_TARGET_STRING "STRING"
#define ETK_SELECTION_TARGET_UTF8_STRING "UTF8_STRING"
#define ETK_SELECTION_TARGET_FILENAME "FILENAME"

struct _Etk_Event_Selection_Get
{   
   void *data;
   
   enum {
      ETK_SELECTION_CONTENT_NONE,
      ETK_SELECTION_CONTENT_TEXT,
      ETK_SELECTION_CONTENT_FILES,
      ETK_SELECTION_CONTENT_TARGETS,
      ETK_SELECTION_CONTENT_CUSTOM
   } content;   
};

struct _Etk_Selection_Data_Targets
{
   char     **targets;
   int        num_targets;
};

int  etk_dnd_init();  
int  etk_dnd_shutdown();
void etk_selection_text_request(Etk_Widget *widget);
void etk_selection_text_set(Etk_Widget *widget, char *data);
  
/** @} */

#endif
