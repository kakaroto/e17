#include <Etk.h>
#include "entropy.h"

static Etk_Widget* _entropy_etk_efolder_dialog = NULL;
static Ecore_Hash* _entropy_global_efolder_hash;
static Etk_Widget* _entropy_etk_efolder_local_box;

typedef struct _Entropy_Etk_Efolder_Object {
	char* name;
	char* value;
} Entropy_Etk_Efolder_Object;

void entropy_etk_efolder_dialog_close(Etk_Object* obj, void* data)
{
	Entropy_Etk_Efolder_Object* c_obj;
	Ecore_List* keys;
	char* key;
	
	if (!((int)data ==1)) etk_widget_hide(_entropy_etk_efolder_dialog);

}

void entropy_etk_efolder_dialog_create()
{
	_entropy_etk_efolder_dialog = etk_window_new();

}


void entropy_etk_efolder_dialog_show()
{
	if (!_entropy_etk_efolder_dialog) {
		entropy_etk_efolder_dialog_create();
	}

	etk_widget_show_all(_entropy_etk_efolder_dialog);
}
