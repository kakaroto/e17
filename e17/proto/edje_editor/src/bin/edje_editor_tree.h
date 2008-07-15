#ifndef _EDJE_EDITOR_TREE_H_
#define _EDJE_EDITOR_TREE_H_


/* Etk_Tree Helper */
#define TREE_COL_NAME 0
#define TREE_COL_VIS 1
#define TREE_COL_TYPE 2
#define TREE_COL_PARENT 3
#define COL_NAME   etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_NAME)
#define COL_VIS    etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_VIS)
#define COL_TYPE   etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_TYPE)
#define COL_PARENT etk_tree_nth_col_get(ETK_TREE(UI_PartsTree), TREE_COL_PARENT)


Etk_Widget *UI_PartsTree;
Etk_Widget *UI_GroupsComboBox;


Etk_Widget*   tree_create            (void);
void          tree_populate          (void);
void          tree_combobox_populate (void);
Etk_Tree_Row* tree_part_add          (const char *part_name, Etk_Tree_Row *after);
Etk_Tree_Row* tree_state_add         (const char *part_name, const char *state_name);
Etk_Tree_Row* tree_program_add       (const char* prog);


Etk_Bool _tree_row_selected_cb                 (Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool _tree_click_cb                        (Etk_Tree *tree, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
Etk_Bool _tree_combobox_active_item_changed_cb (Etk_Combobox_Entry *combobox_entry, void *data);

#endif
