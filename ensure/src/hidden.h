void view_set_hidden(void *ensurev, Evas_Object *button, void *event_info);
int  on_hide(void);

int  hidden_object_add(struct ensure *ensure, uintptr_t o);
int  hidden_object_remove(struct ensure *ensure, uintptr_t o);

bool hidden_get(struct ensure *ensure, uintptr_t objid);
