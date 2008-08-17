#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Ewl.h>

#include "define.h"
#include "enum.h"
#include "function.h"
#include "hash.h"
#include "util.h"


static Ewl_Widget *tree;
static Ewl_Widget *list;
static Ewl_Widget *text;
static Ewl_Widget *entry;
static Ecore_List *sorted_keys;
static Ecore_List *list_rows = NULL;

void ecrin_ewl_list_fill_package (char *aiguille);

static void
_main_window_close (Ewl_Widget *w, void *ev_data, void *user_data)
{
  ewl_widget_destroy (w);
  ewl_main_quit ();
}


static Ecore_List *
_list_keys_order (Ecore_List *keys)
{
  Ecore_List *l;
  char       *key;

  l = ecore_list_new ();
  ecore_list_first_goto(keys);
  while ((key = ecore_list_next(keys)))
    {
      char *str;

      ecore_list_first_goto (l);
      while ((str = ecore_list_next(l)) &&
             (strcasecmp (key, str) >= 0)) { }

      if (!str)
        ecore_list_append (l, key);
      else
        {
          ecore_list_index_goto (l, ecore_list_index (l) - 1);
          ecore_list_insert (l, key);
        }
    }

  ecore_list_destroy (keys);

  return l;
}

static void
_list_display_cb (Ewl_Widget *w, void *ev_data, void *user_data)
{
  Ewl_Row *row;
  char    *aiguille;

  aiguille = ewl_text_text_get (EWL_TEXT (w));

  /* We clear the list */
  ecore_list_first_goto(list_rows);
  while ((row = ecore_list_next(list_rows)))
    {
      ewl_tree_row_destroy (EWL_TREE (list), row);
    }

  ecrin_ewl_list_fill_package (aiguille);

/*   ecore_list_first_goto(list_rows); */
/*   while ((row = ecore_list_next(list_rows))) */
/*     { */
/*       Ewl_Widget *child; */
/*       char       *motte; */

/*       ewl_container_child_iterate_begin (EWL_CONTAINER (row)); */
/*       child = ewl_container_child_next (EWL_CONTAINER (row)); */
/*       child = ewl_container_child_next (EWL_CONTAINER (child)); */
/*       motte = ewl_text_text_get (EWL_TEXT (child)); */

/*       printf ("motte : %s\n", motte); */

/*       if (strstr (motte, aiguille)) */
/*         ewl_widget_show (EWL_WIDGET (row)); */
/*       else */
/*         ewl_widget_hide (EWL_WIDGET (row)); */
/*     } */
}

static void
_enum_display (Ewl_Widget *w, void *ev_data, void *user_data)
{
  Ecrin_Hash_Data *data;
  Ecrin_Enum      *e;
  Ecrin_Enum_Item *item;
  char             str[4096];

  data = (Ecrin_Hash_Data *)user_data;
  e = (Ecrin_Enum *)data->data;

  ewl_text_text_set (EWL_TEXT (text), data->efl_name);
  ewl_text_text_append (EWL_TEXT (text), "\n");
  ewl_text_text_append (EWL_TEXT (text), "\n");

  snprintf (str, 4096, "enum %s\n", e->name);
  ewl_text_text_append (EWL_TEXT (text), str);
  ewl_text_text_append (EWL_TEXT (text), "{\n");
  ecore_list_first_goto (e->items);
  while ((item = ecore_list_next (e->items)))
    {
      snprintf (str, 4096, "  %s", item->name);
      ewl_text_text_append (EWL_TEXT (text), str);
      if (item->value)
	snprintf (str, 4096, " = %s", item->value);
      ewl_text_text_append (EWL_TEXT (text), str);
      if (ecore_list_index (e->items) != ecore_list_count (e->items))
        ewl_text_text_append (EWL_TEXT (text), ",");
      ewl_text_text_append (EWL_TEXT (text), "\n");
    }
  ewl_text_text_append (EWL_TEXT (text), "};\n");
}

static void
_define_display (Ewl_Widget *w, void *ev_data, void *user_data)
{
  Ecrin_Hash_Data *data;
  Ecrin_Define    *define;
  char             str[4096];
  char            *p;
  char            *iter;
  
  data = (Ecrin_Hash_Data *)user_data;
  define = (Ecrin_Define *)data->data;

  ewl_text_text_set (EWL_TEXT (text), data->efl_name);
  ewl_text_text_append (EWL_TEXT (text), "\n");
  ewl_text_text_append (EWL_TEXT (text), "\n");

  snprintf (str, 4096, "#define %s", define->name);
  ewl_text_text_append (EWL_TEXT (text), str);
  iter = define->value;
  if (define->value)
    {
      while ((p = strchr (iter, '\\')) != NULL)
        {
          char *string;
          int   l;
          
          l = p - iter;
          string = strndup (iter, l);
          snprintf (str, 4096, " %s\n", string);
          ewl_text_text_append (EWL_TEXT (text), str);
          free (string);
          iter = p + 1;
        } 
    }
  snprintf (str, 4096, " %s\n", iter);
  ewl_text_text_append (EWL_TEXT (text), str);
  remove_description (define->value);
}

static void
_function_display (Ewl_Widget *w, void *ev_data, void *user_data)
{
  Ecrin_Hash_Data      *data;
  Ecrin_Function       *function;
  Ecrin_Function_Param *param;
  char                  str[4096];
  char                  space[4096];
  
  data = (Ecrin_Hash_Data *)user_data;
  function = (Ecrin_Function *)data->data;

  ewl_text_text_set (EWL_TEXT (text), data->efl_name);
  ewl_text_text_append (EWL_TEXT (text), "\n");
  ewl_text_text_append (EWL_TEXT (text), "\n");

  snprintf (str, 4096, "%s %s (", function->return_type, function->name);
  ewl_text_text_append (EWL_TEXT (text), str);
  memset (space, ' ', 4096);
  space[strlen (str)] = '\0';
  ecore_list_first_goto (function->params);
  while ((param = ecore_list_next (function->params)))
    {
      if (ecore_list_index (function->params) != 1)
        ewl_text_text_append (EWL_TEXT (text), space);
      snprintf (str, 4096, "%s", param->type);
      ewl_text_text_append (EWL_TEXT (text), str);
      if (param->name)
        {
          snprintf (str, 4096, " %s", param->name);
          ewl_text_text_append (EWL_TEXT (text), str);
        }
      if (ecore_list_index (function->params) != ecore_list_count (function->params))
        {
          snprintf (str, 4096, ",\n");
          ewl_text_text_append (EWL_TEXT (text), str);
        }
      else
        {
          snprintf (str, 4096, ")\n");
          ewl_text_text_append (EWL_TEXT (text), str);
        }
    }
}


void
ecrin_ewl_tree_fill_package (void)
{
  Ewl_Widget *row;
  Ewl_Widget *row_enum;
  Ewl_Widget *row_define;
  Ewl_Widget *row_function;
  char       *key;
  char       *label;

  label = "Eet";
  row = ewl_tree_text_row_add (EWL_TREE (tree), NULL,
                               &label);
  label = "Enum";
  row_enum = ewl_tree_text_row_add (EWL_TREE (tree), EWL_ROW (row),
                                    &label);
  label = "Define";
  row_define = ewl_tree_text_row_add (EWL_TREE (tree), EWL_ROW (row),
                                      &label);
  label = "Function";
  row_function = ewl_tree_text_row_add (EWL_TREE (tree), EWL_ROW (row),
                                        &label);


  ecore_list_first_goto(sorted_keys);
  while ((key = ecore_list_next(sorted_keys)))
    {
      Ewl_Widget      *prow;
      Ecrin_Hash_Data *data;
      
      data = ecrin_hash_data_get (key);
      switch (data->type)
        {
        case HASH_DATA_ENUM:
          prow = ewl_tree_text_row_add (EWL_TREE (tree),
                                        EWL_ROW (row_enum),
                                        &data->data_name);
          ewl_callback_append (prow, EWL_CALLBACK_CLICKED, _enum_display, data);
          break;
        case HASH_DATA_DEFINE:
          prow = ewl_tree_text_row_add (EWL_TREE (tree),
                                        EWL_ROW (row_define),
                                        &data->data_name);
          ewl_callback_append (prow, EWL_CALLBACK_CLICKED, _define_display, data);
          break;
        case HASH_DATA_FUNCTION:
          prow = ewl_tree_text_row_add (EWL_TREE (tree),
                                        EWL_ROW (row_function),
                                        &data->data_name);
          ewl_callback_append (prow, EWL_CALLBACK_CLICKED, _function_display, data);
          break;
        default:
          break;
        }
    }
}

void
ecrin_ewl_list_fill_package (char *aiguille)
{
  Ewl_Widget *row;
  char       *key;

  if (list_rows)
    ecore_list_destroy (list_rows);

  list_rows = ecore_list_new ();

  if (!aiguille)
    aiguille = "";

  ecore_list_first_goto(sorted_keys);
  while ((key = ecore_list_next(sorted_keys)))
    {
      Ecrin_Hash_Data *data;
      
      data = ecrin_hash_data_get (key);

      if (strstr (key, aiguille))
        {
          row = ewl_tree_text_row_add (EWL_TREE (list), NULL,
                                       &data->data_name);

          ecore_list_append (list_rows, row);

          switch (data->type)
            {
            case HASH_DATA_ENUM:
              ewl_callback_append (row, EWL_CALLBACK_CLICKED, _enum_display, data);
              break;
            case HASH_DATA_DEFINE:
              ewl_callback_append (row, EWL_CALLBACK_CLICKED, _define_display, data);
              break;
            case HASH_DATA_FUNCTION:
              ewl_callback_append (row, EWL_CALLBACK_CLICKED, _function_display, data);
              break;
            default:
              break;
            }
        }
    }

}

void
ecrin_ewl_window_create (void)
{
  Ewl_Widget *window;
  Ewl_Widget *hbox;
  Ewl_Widget *notebook;
  Ewl_Widget *tab;
  Ewl_Widget *vbox;
  Ewl_Widget *separator;

  /* We order the list */
  sorted_keys = _list_keys_order (ecrin_hash_keys_get ());

  window = ewl_window_new ();
  ewl_object_size_request (EWL_OBJECT (window), 400, 400);
  ewl_window_title_set (EWL_WINDOW (window),
                       "Ecrin");
  ewl_window_name_set (EWL_WINDOW (window), "Ecrin");
  ewl_window_class_set (EWL_WINDOW (window), "Ecrin");
  ewl_callback_append (window, EWL_CALLBACK_DELETE_WINDOW,
                      _main_window_close, NULL);

  hbox = ewl_hbox_new ();
  ewl_container_child_append (EWL_CONTAINER (window), hbox);
  ewl_widget_show (hbox);

  notebook = ewl_notebook_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox), notebook);
  ewl_widget_show (notebook);

  tab = ewl_label_new ("EFL package");
  ewl_widget_show (tab);
  
  tree = ewl_tree_new (1);
  ewl_tree_headers_visible_set (EWL_TREE (tree), 0);
  ewl_notebook_page_append (EWL_NOTEBOOK (notebook), tab, tree);
  ewl_widget_show (tree);

  tab = ewl_label_new ("Search");
  ewl_widget_show (tab);

  vbox = ewl_vbox_new ();
  ewl_box_homogeneous_set (EWL_BOX (vbox), 0);
  ewl_notebook_page_append (EWL_NOTEBOOK (notebook), tab, vbox);
  ewl_widget_show (vbox);

  entry = ewl_entry_new ("");
  ewl_object_fill_policy_set(EWL_OBJECT(entry),
                             EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
  ewl_container_child_append (EWL_CONTAINER (vbox), entry);
  ewl_callback_append (entry, EWL_CALLBACK_VALUE_CHANGED, _list_display_cb, NULL);
  ewl_widget_show (entry);

  separator = ewl_hseparator_new ();
  ewl_container_child_append (EWL_CONTAINER (vbox), separator);
  ewl_widget_show (separator);

  list = ewl_tree_new (1);
  ewl_tree_headers_visible_set (EWL_TREE (list), 0);
  ewl_container_child_append (EWL_CONTAINER (vbox), list);
  ewl_widget_show (list);

  separator = ewl_vseparator_new ();
  ewl_container_child_append (EWL_CONTAINER (hbox), separator);
  ewl_widget_show (separator);

  text = ewl_text_new ("");
  ewl_container_child_append (EWL_CONTAINER (hbox), text);
  ewl_widget_show (text);

  ecrin_ewl_tree_fill_package ();
  ecrin_ewl_list_fill_package ("");

  ewl_widget_show (window);
}
