#include <Etk.h>
#include "entropy.h"
#include <evfs/evfs.h>

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
	Etk_Widget* text_name;
	Etk_Widget* text_value;
	Etk_Widget* button_add;
	Etk_Widget* button_ok;
	Etk_Widget* button_cancel;

	Etk_Tree_Col* col1;
	Etk_Tree_Col* col2;
	Etk_Tree_Col* col3;
} Entropy_Etk_Efolder_Dialog;
static Entropy_Etk_Efolder_Dialog* eeed = NULL;

void entropy_etk_efolder_dialog_close(Etk_Object* obj, void* data)
{
	Eina_List* l = NULL;
	EvfsVfolderEntry* v;
	Etk_Tree_Row* iter;

	char* type;
	char* key;
	char* value;
	
	if (((int)data ==1))  {
		char* name = (char*)etk_entry_text_get(ETK_ENTRY(eeed->text_name));
		
		for (iter = etk_tree_first_row_get(ETK_TREE(eeed->tree)); iter; 
		 	iter = etk_tree_row_walk_next(iter, ETK_TRUE)) {

			etk_tree_row_fields_get(iter, 
					etk_tree_nth_col_get(ETK_TREE(eeed->tree),0), &type,
					etk_tree_nth_col_get(ETK_TREE(eeed->tree),1),&key,
					etk_tree_nth_col_get(ETK_TREE(eeed->tree),2),&value,
					NULL);

			v= calloc(1,sizeof(EvfsVfolderEntry));
			v->type = type[0];
			v->name = key;
			v->value = value;

			l = eina_list_append(l,v);
		}
		entropy_plugin_filesystem_vfolder_create(name,l);
	}
	etk_widget_hide(eeed->dialog);
}

void _etk_efolder_dialog_add_cb(Etk_Object* obj, void* data)
{
	char* type;
	if (etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(eeed->radio_meta)) == ETK_TRUE) {
		type = "Metadata";
	} else {
		type = "Tag";
	}
	
	etk_tree_row_append(ETK_TREE(eeed->tree), NULL, 
		  eeed->col1, type,
		  eeed->col2, 
		  	(char*)etk_combobox_item_field_get(etk_combobox_active_item_get(ETK_COMBOBOX(eeed->cmb_meta)),0),
		  eeed->col3, etk_entry_text_get(ETK_ENTRY(eeed->text_value)),
		  NULL
	);	

	etk_entry_clear(ETK_ENTRY(eeed->text_value));
}

void entropy_etk_efolder_dialog_create()
{
	Etk_Tree_Col* col;
	Etk_Widget* vbox;
	Etk_Widget* hbox;
	Etk_Widget* label;
	
	if (!eeed) {
		eeed = entropy_malloc(sizeof(Entropy_Etk_Efolder_Dialog));
		eeed->dialog = etk_window_new();

		eeed->tree = etk_tree_new();
		etk_tree_mode_set(ETK_TREE(eeed->tree), ETK_TREE_MODE_LIST);

		eeed->col1 = etk_tree_col_new(ETK_TREE(eeed->tree), _("Type"), 150,0.0);
		etk_tree_col_model_add(eeed->col1, etk_tree_model_text_new());

		eeed->col2 = etk_tree_col_new(ETK_TREE(eeed->tree), _("Key"), 150,0.0);
		etk_tree_col_model_add(eeed->col2, etk_tree_model_text_new());

		eeed->col3 = etk_tree_col_new(ETK_TREE(eeed->tree), _("Value (if applicable)"), 150,0.0);
		etk_tree_col_model_add(eeed->col3, etk_tree_model_text_new());

		vbox = etk_vbox_new(ETK_FALSE,0);
		etk_container_add(ETK_CONTAINER(eeed->dialog), vbox);
		etk_box_append(ETK_BOX(vbox), eeed->tree, ETK_BOX_START,ETK_BOX_EXPAND_FILL,0);

		hbox = etk_hbox_new(ETK_TRUE,1);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END,ETK_BOX_NONE,0);

		label = etk_label_new("eFolder Name");
		etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START,ETK_BOX_NONE,0);
		eeed->text_name = etk_entry_new();	
		etk_box_append(ETK_BOX(hbox), eeed->text_name, ETK_BOX_START,ETK_BOX_NONE,0);
		
		hbox = etk_hbox_new(ETK_TRUE,1);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END,ETK_BOX_NONE,0);
		eeed->radio_meta = etk_radio_button_new_with_label("Metadata", NULL);
		etk_box_append(ETK_BOX(hbox), eeed->radio_meta, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->radio_tag = etk_radio_button_new_with_label_from_widget("Tag", ETK_RADIO_BUTTON(eeed->radio_meta));
		etk_box_append(ETK_BOX(hbox), eeed->radio_tag, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->cmb_meta = etk_combobox_new_default();
		etk_box_append(ETK_BOX(hbox), eeed->cmb_meta, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->text_value = etk_entry_new();
		etk_box_append(ETK_BOX(hbox), eeed->text_value, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->button_add = etk_button_new_with_label("Add");
		etk_box_append(ETK_BOX(hbox), eeed->button_add, ETK_BOX_START,ETK_BOX_NONE,0);
		etk_signal_connect("pressed", ETK_OBJECT(eeed->button_add), _etk_efolder_dialog_add_cb,NULL);
		
		hbox = etk_hbox_new(ETK_TRUE,1);
		etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_END,ETK_BOX_NONE,0);

		eeed->button_ok = etk_button_new_from_stock(ETK_STOCK_DIALOG_OK);
		etk_signal_connect("pressed", ETK_OBJECT(eeed->button_ok), entropy_etk_efolder_dialog_close,(int*)1);
		etk_box_append(ETK_BOX(hbox), eeed->button_ok, ETK_BOX_START,ETK_BOX_NONE,0);

		eeed->button_cancel = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
		etk_box_append(ETK_BOX(hbox), eeed->button_cancel, ETK_BOX_START,ETK_BOX_NONE,0);

		etk_tree_build(ETK_TREE(eeed->tree));
	}
	
}


void entropy_etk_efolder_dialog_show(Eina_List* l)
{
	EvfsMetaObject* o;
	Eina_List* i;
	
	if (!eeed) {
		entropy_etk_efolder_dialog_create();
	}

	etk_combobox_clear(ETK_COMBOBOX(eeed->cmb_meta));
	for (i=l;i;) {
		o = i->data;
		etk_combobox_item_append(ETK_COMBOBOX(eeed->cmb_meta), o->key);
		i=i->next;
	}

	etk_widget_show_all(eeed->dialog);
}
