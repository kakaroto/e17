#ifndef _ENITY_H
#define _ENITY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <Etk.h>
#include <Evas.h>

typedef struct _Enity_Tree_Col Enity_Tree_Col;

struct _Enity_Tree_Col
{
   Etk_Tree_Col *col;
   enum {
      ENITY_COL_MODEL_TEXT,
      ENITY_COL_MODEL_CHECK,
      ENITY_COL_MODEL_RADIO
   } model;
};

static Evas_List *_en_arg_data_get(Etk_Argument *args, char *key);
static void _en_tree_checkbox_toggled_cb(Etk_Object *object, Etk_Tree_Row *row, void *data);
static void _en_ok_print_stdout_cb(Etk_Object *obj, int response_id, void *data);
static void _en_entry_cb(Etk_Argument *args, int index);
static void _en_error_cb(Etk_Argument *args, int index);
static void _en_question_cb(Etk_Argument *args, int index);
static void _en_info_cb(Etk_Argument *args, int index);
static void _en_warning_cb(Etk_Argument *args, int index);
static void _en_list_column_cb(Etk_Argument *args, int index);
static void _en_list_cb(Etk_Argument *args, int index);
static void _en_dialog_text_cb(Etk_Argument *args, int index);
static void _en_entry_entry_text_cb(Etk_Argument *args, int index);

  

#endif
