void menu_show(void);
void menu_hide(void);
void menu_push(const char *context, const char *name, void (*free_func) (void *data), void *data);
void menu_pop(void);
void menu_context_pop(void);
void menu_pop_until(const char *context, const char *name);
void menu_go(void);
void menu_item_add(const char *icon, const char *label,
		   const char *desc, const char *info,
		   void (*func) (void *data), void *data,
		   void (*free_func) (void *data),
		   void (*over_func) (void *data),
		   void (*out_func) (void *data)
		   );
void menu_item_enabled_set(const char *name, const char *label, int enabled);
void menu_item_select(const char *label);
void menu_item_select_jump(int jump);
void menu_item_select_go(void);
void menu_key(Evas_Event_Key_Down *ev);
void menu_info_show(const char *info);
const void *menu_data_get(void);
