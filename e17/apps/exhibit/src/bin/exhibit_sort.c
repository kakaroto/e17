#include "exhibit.h"

void
_ex_sort_label_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   etk_menu_popup(ETK_MENU(data));   
}

