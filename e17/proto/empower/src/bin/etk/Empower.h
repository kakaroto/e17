#ifndef EMPOWER_H
#define EMPOWER_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <Etk.h>

static Etk_Bool _em_centered = ETK_FALSE;

typedef struct _Empower Empower;

struct _Empower
{
   Etk_Widget *dialog;
   Etk_Widget *hbox;
   Etk_Widget *entry;
   Etk_Widget *label;
   Etk_Widget *icon;
   
   int time_to_pipe;   
   char buf[1024];   
   char password[1024];
};

Empower *em;

/* empower_cb_etk prototypes */
void _em_run_cmd(Empower *em);
void _em_dialog_size_request_cb(Etk_Object *object, Etk_Size *size_request, void *data);
Etk_Bool _em_dialog_delete_event_cb(void *data);
void _em_entry_key_down(Etk_Object *object, Etk_Event_Key_Down *ev, void *data);
void _em_dialog_response_cb(Etk_Object *object, int response_id, void *data);
/* end empower_cb_etk prototypes */

/* empower_gui_etk prototypes */
void display_window();
/* end empower_gui_etk_prototypes */

#endif
