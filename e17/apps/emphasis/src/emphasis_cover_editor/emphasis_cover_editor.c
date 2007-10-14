#include <Enhance.h>
#include <Etk.h>
#include <Eet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _Cover_Entry Cover_Entry;
struct _Cover_Entry
{
  char *key;
  char *path;
};

Enhance *en;
Etk_Widget *tree;
Etk_Widget *cover;
Etk_Widget *filechooser_dialog = NULL;
Etk_Widget *filechooser;
Etk_Widget *add_dialog = NULL;
Etk_Widget *entry_add_artist;
Etk_Widget *entry_add_album;

Evas_List *cover_changed = NULL;

void on_window_destroy(Etk_Object *object, void *data);
void fill_tree_with_db(Etk_Tree *tree);
void on_tree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data);
void change_cover_path(char *cover_path);
void filechooser_close(Etk_Object *object, void *data);
void filechooser_dialog_open(Etk_Dialog *filechooser_dialog, int response_id, void *data);
void on_btn_open_clicked(Etk_Object *object, void *data);
void dialog_close(Etk_Object *object, void *data);

/*********/
int
tree_sort(Etk_Tree *tree,
          Etk_Tree_Row *r1, Etk_Tree_Row *r2,
          Etk_Tree_Col *col,
          void *data)
{
  char *k1 = NULL;
  char *k2 = NULL;
  etk_tree_row_fields_get(r1, col, &k1, NULL);
  etk_tree_row_fields_get(r2, col, &k2, NULL);
  return strcoll(k1, k2);
}

void
on_window_destroy(Etk_Object *object, void *data)
{
  eet_shutdown();
  enhance_shutdown();
  etk_main_quit();
}

void
on_btn_cancel_clicked(Etk_Object *object, void *data)
{
  eet_shutdown();
  enhance_shutdown();
  etk_main_quit();
}

void
on_add_dialog_response(Etk_Dialog *dialog, int response_id, void *data)
{
  const char *artist;
  const char *album;
  char *key;
  Etk_Tree_Row *row;

  if (response_id == ETK_RESPONSE_OK)
    {
      artist = etk_entry_text_get(ETK_ENTRY(entry_add_artist));
      album = etk_entry_text_get(ETK_ENTRY(entry_add_album));

      if (artist && strcmp(artist, "")
          && album && strcmp(album, ""))
        {
          asprintf(&key, "/%s/%s", artist, album);
          row = etk_tree_row_append(ETK_TREE(tree), NULL,
                                     etk_tree_nth_col_get(ETK_TREE(tree), 0),
                                     key, NULL);
        }
    }
  etk_entry_text_set(ETK_ENTRY(entry_add_artist), "");
  etk_entry_text_set(ETK_ENTRY(entry_add_album), "");
  etk_widget_hide_all(ETK_WIDGET(dialog));
}


void
on_btn_add_clicked(Etk_Object *object, void *data)
{
  if (add_dialog)
    {
      etk_widget_show_all(ETK_WIDGET(add_dialog));
      return;
    }
  else
    {
      enhance_var_get(en, "add_dialog");
    }
}

void
on_btn_save_clicked(Etk_Object *object, void *data)
{
  Cover_Entry *ce;
  Eet_File *ef;
  Evas_List *list;
  char *cover_file;

  list = cover_changed;
  asprintf(&cover_file, "%s/.e/apps/emphasis/cover.eet", getenv("HOME"));
  ef = eet_open(cover_file, EET_FILE_MODE_READ_WRITE);
  if (!ef)
    {
      fprintf(stderr, "can't open %s\n", cover_file);
      free(cover_file);
      return;
    }

  while (cover_changed)
    {
      ce = evas_list_data(cover_changed);
      eet_write(ef, ce->key, ce->path, strlen(ce->path)+1, 0);
      eet_clearcache();
      free(ce->key);
      free(ce->path);
      free(ce);
      cover_changed = evas_list_next(cover_changed);
    }
  evas_list_free(list);
  eet_close(ef);
  free(cover_file);
}

void
on_tree_row_selected(Etk_Object *object, Etk_Tree_Row *row, void *data)
{
  char *path;
  path = etk_tree_row_data_get(row);

  if(path)
    {
      etk_image_set_from_file(ETK_IMAGE(cover), path, NULL);
    }
  else
    {
      etk_image_set_from_stock(ETK_IMAGE(cover),
                               ETK_STOCK_IMAGE_MISSING,
                               ETK_STOCK_BIG);
    }
}

void
change_cover_path(char *cover_path)
{
  Etk_Tree_Row *row;
  Cover_Entry *ce;
  char *key;

  row = etk_tree_selected_row_get(ETK_TREE(tree));
  if (!row)
    {
      return;
    }

  ce = malloc(sizeof(Cover_Entry));
  etk_tree_row_fields_get(row, etk_tree_nth_col_get(ETK_TREE(tree), 0), 
                          &key, NULL);
  ce->key = strdup(key);
  ce->path = strdup(cover_path);

  cover_changed = evas_list_append(cover_changed, ce);
}

Etk_Bool
on_dialog_destroy(Etk_Object *object, void *data)
{
  etk_widget_hide_all(ETK_WIDGET(object));
  return ETK_FALSE;
}

void
on_filechooser_dialog_response(Etk_Dialog *filechooser_dialog,
                               int response_id,
                               void *data)
{
  const char *cover_file_name;
  const char *cover_file_folder;
  char *cover_path;
  Etk_Tree_Row *row;

  if (response_id == ETK_RESPONSE_OK)
    {
      cover_file_name =
       etk_filechooser_widget_selected_file_get
       (ETK_FILECHOOSER_WIDGET(filechooser));
      
      cover_file_folder =
       etk_filechooser_widget_current_folder_get
       (ETK_FILECHOOSER_WIDGET(filechooser));

      asprintf(&cover_path, "%s/%s", cover_file_folder, cover_file_name);

      change_cover_path(cover_path);

      row = etk_tree_selected_row_get(ETK_TREE(tree));
      free(etk_tree_row_data_get(row));
      etk_tree_row_data_set(row, cover_path);
      etk_image_set_from_file(ETK_IMAGE(cover), cover_path, NULL);
    }
  etk_widget_hide_all(ETK_WIDGET(filechooser_dialog));
}

void
on_btn_open_clicked(Etk_Object *object, void *data)
{
  if (!etk_tree_selected_row_get(ETK_TREE(tree)))
    {
      return;
    }
  
  if (filechooser_dialog)
    {
      etk_widget_show_all(ETK_WIDGET(filechooser_dialog));
      return;
    }
}

int main(int argc, char **argv)
{
  eet_init();
  enhance_init();
  etk_init(argc, argv);
   
  en = enhance_new();      

  /* Load the main window */
  enhance_file_load(en, "window", "emphasis_cover_editor.glade");
  tree  = enhance_var_get(en, "tree");
  cover  = enhance_var_get(en, "cover");
  filechooser = enhance_var_get(en, "filechooserwidget");
  filechooser_dialog  = enhance_var_get(en, "filechooser_dialog");
  add_dialog  = enhance_var_get(en, "add_dialog");
  entry_add_artist = enhance_var_get(en, "entry_add_artist");
  entry_add_album = enhance_var_get(en, "entry_add_album");

  fill_tree_with_db(ETK_TREE(tree));
   
  etk_main();
   
  return 0;
}

void
fill_tree_with_db(Etk_Tree *tree)
{
  int num, i;
  char **entries;
  char *cover_path;
  Etk_Tree_Col *col;
  Etk_Tree_Row *row;
  Eet_File *ef;
  char *cover_db_path;

  asprintf(&cover_db_path, "%s/.e/apps/emphasis/cover.eet", getenv("HOME"));
  ef = eet_open(cover_db_path, EET_FILE_MODE_READ);
  if (!ef)
    {
      fprintf(stderr, "Can't open %s\n", cover_db_path);
      free(cover_db_path);
      exit(-1);
    }

  col = etk_tree_col_new(tree, "/artist/album", 0, 0.0);
  etk_tree_col_model_add(col, etk_tree_model_text_new());
  etk_tree_build(tree);
  etk_tree_freeze(tree);

  entries = eet_list(ef, "*", &num);

  for (i=0; i<num; i++)
    {
      cover_path = eet_read(ef, entries[i], NULL);
      row = etk_tree_row_append(tree, NULL, col, entries[i], NULL);
      if (strcmp("not found", cover_path))
        {
          etk_tree_row_data_set(row, cover_path);
        }
      else
        {
          etk_tree_row_data_set(row, NULL);
        }
    }

  eet_close(ef);
  free(cover_db_path);

  etk_tree_col_sort_set(etk_tree_nth_col_get(tree, 0), tree_sort, NULL);
  etk_tree_thaw(tree);
}
