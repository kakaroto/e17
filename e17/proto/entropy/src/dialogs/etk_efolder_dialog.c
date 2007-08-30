#include <Etk.h>
#include "entropy.h"

typedef struct _Entropy_Etk_Efolder_Object {
	char* name;
	char* value;
} Entropy_Etk_Efolder_Object;

typedef struct _Entropy_Etk_Efolder_Dialog {
	Etk_Widget* tree;
	Etk_Widget* dialog;
	Etk_Widget* radio_tag;
	Etk_Widget* radio_meta;
	Etk_Widget* cmb_meta;
	Etk_Widget* text_value;
	Etk_Widget* button_add;
	Etk_Widget* button_ok;
	Etk_Widget* button_cancel;
} Entropy_Etk_Efolder_Dialog;
static Entropy_Etk_Efolder_Dialog* eeed = NULL;

void entropy_etk_efolder_dialog_close(Etk_Object* obj, void* data)
{
	//if (!((int)data ==1)) etk_widget_hide(_entropy_etk_efolder_dialog);
	etk_widget_hide(eeed->dialog);
}

void entropy_etk_efolder_dialog_create()
{
	Etk_Tree_Col* col;
	Etk_Widget* vbox;
	Etk_Widget* hbox;
	
	if (!eeed) {
		eeed = entropy_malloc(sizeof(Entropy_Etk_Efolder_Dialog));
		eeed->dialog = etk_window_new();

		eeed->tree = etk_tree_new();
		etk_tree_mode_set(ETK_TREE(eeed->tree), ETK_TREE_MODE_LIST);

		col = etk_tree_col_new(ETK_TREE(eeed->tree), _("Type"), 150,0.0);
		etk_tree_col_model_add(col, etk_tree_model_text_new());

		col = etk_tree_col_new(ETK_TREE(eeed->tree), _("Key"), 150,0.0);
		etk_tree_col_model_add(col, etk_tree_model_text_new());

		col = etk_tree_col_new(ETK_TREE(eeed->tree), _("Value (if applicable)"), 150,0.0);
		etk_tree_col_model_add(col, etk_tree_model_text_new());

		vbox = etk_vbox_new(ETK_FALSE,0);
		etk_container_add(ETK_CONTAINER(eeed->dialog), vbox);
		etk_box_append(ETK_BOX(vbox), eeed->tree, ETK_BOX_START,ETK_BOX_EXPAND_FILL,0);
		
		hbox = etk_hbox_new(ETK_TRUE,1);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END,ETK_BOX_NONE,0);
		eeed->radio_meta = etk_radio_button_new_with_label("Metadata", NULL);
		etk_box_append(ETK_BOX(hbox), eeed->radio_meta, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->radio_tag = etk_radio_button_new_with_label_from_widget("Tag", ETK_RADIO_BUTTON(eeed->radio_meta));
		etk_box_append(ETK_BOX(hbox), eeed->radio_tag, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->text_value = etk_entry_new();
		etk_box_append(ETK_BOX(hbox), eeed->text_value, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->cmb_meta = etk_combobox_new();
		etk_box_append(ETK_BOX(hbox), eeed->cmb_meta, ETK_BOX_START,ETK_BOX_NONE,0);
		
		hbox = etk_hbox_new(ETK_TRUE,1);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END,ETK_BOX_NONE,0);

		eeed->button_ok = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
		etk_box_append(ETK_BOX(hbox), eeed->button_ok, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->button_cancel = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
		etk_box_append(ETK_BOX(hbox), eeed->button_cancel, ETK_BOX_START,ETK_BOX_NONE,0);
	}
	
}


void entropy_etk_efolder_dialog_show()
{
	if (!eeed) {
		entropy_etk_efolder_dialog_create();
	}

	etk_widget_show_all(eeed->dialog);
}
