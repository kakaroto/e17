
void                layer_create(void);
void                layer_load(int);
Layer              *_layer_create_copy(Layer *);
void                layer_set(char *);
void                layer_redraw(Layer *);
void                layer_sync(Layer *);
void                layer_destroy(Layer *);

void                layer_hide(Layer *);
void                layer_show(Layer *);
void                layer_show_all(void);
void                layer_hide_all(void);
