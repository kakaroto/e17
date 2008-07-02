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

Etk_Widget* create_tree(void);
void PopulateTree(void);
void PopulateGroupsComboBox(void);
Etk_Tree_Row *AddPartToTree(const char *part_name, Etk_Tree_Row *after);
Etk_Tree_Row *AddStateToTree(const char *part_name, const char *state_name);
Etk_Tree_Row *AddProgramToTree(const char* prog);



Etk_Bool on_GroupsComboBox_activated(Etk_Combobox *combobox, Etk_Combobox_Item *item, void *data);
Etk_Bool on_PartsTree_row_selected         (Etk_Object *object, Etk_Tree_Row *row, void *data);
Etk_Bool on_PartsTree_click                (Etk_Tree *tree, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);


#endif
